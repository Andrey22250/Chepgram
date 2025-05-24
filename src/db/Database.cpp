#include "database.h"

Database::Database() : conn(nullptr) {}

Database::~Database() {
    if (conn) PQfinish(conn);
}

bool Database::connect() {
    conn = PQconnectdb("host=localhost dbname=chepgram user=postgres password=24071977F_x");    //Передаём все параметры для подключения к базе
    if (PQstatus(conn) != CONNECTION_OK) {  //Если что-то пошло не так(база на другом адресе или пароль другой)
        std::cerr << "Ошибка подключения к БД: " << PQerrorMessage(conn);
        return false;
    }
    return true;
}

std::pair<bool, std::string> Database::checkPhoneExists(const std::string& phone) {
    std::string query = "SELECT id FROM \"Users\" WHERE phonenumber = '" + phone + "';";
    PGresult* res = PQexec(conn, query.c_str());

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        PQclear(res);
        return { false, "Ошибка при проверке телефона" };
    }

    if (PQntuples(res) > 0) {
        std::string userId = PQgetvalue(res, 0, 0);
        PQclear(res);
        return { true, userId };  // пользователь найден
    }

    PQclear(res);
    return { false, "NEW" }; // нет такого номера
}

std::pair<bool, std::string> Database::verifyPassword(int userId, const std::string& password) {
    std::string query = "SELECT password, nickname FROM \"Users\" WHERE id = " + std::to_string(userId) + ";";
    PGresult* res = PQexec(conn, query.c_str());

    if (PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) == 0) {
        PQclear(res);
        return { false, "Ошибка при проверке пароля" };
    }

    std::string dbPassword = PQgetvalue(res, 0, 0);
    std::string nickname = PQgetvalue(res, 0, 1);
    PQclear(res);

    if (dbPassword == password) {
        return { true, "Успешный вход. Ник: " + nickname };
    }
    else {
        return { false, "Неверный пароль" };
    }
}

std::tuple<bool, std::string, int> Database::registerNewUser(const std::string& phone, const std::string& password, const std::string& nickname) {
    // Проверка ника
    std::string checkNickQuery = "SELECT id FROM \"Users\" WHERE nickname = '" + nickname + "';";
    PGresult* nickRes = PQexec(conn, checkNickQuery.c_str());

    if (PQntuples(nickRes) > 0) {
        PQclear(nickRes);
        return { false, "Это имя пользователя уже занято. Введите новое: ", -1 };
    }
    PQclear(nickRes);

    std::string insertQuery = "INSERT INTO \"Users\" (nickname, phonenumber, password) "
        "VALUES ('" + nickname + "', '" + phone + "', '" + password + "') RETURNING id;";
    PGresult* insertRes = PQexec(conn, insertQuery.c_str());

    if (PQresultStatus(insertRes) != PGRES_TUPLES_OK || PQntuples(insertRes) == 0) {
        std::string err = PQresultErrorMessage(insertRes);
        PQclear(insertRes);
        return { false, "Ошибка регистрации: " + err, -1 };
    }

    std::string newId = PQgetvalue(insertRes, 0, 0);
    PQclear(insertRes);
    return { true, "Ваш телефон = " + phone + ", ваше имя пользователя = " + nickname + " (зарегистрирован)", std::stoi(newId)};
}

std::vector<ChatsInfo> Database::getUserChatsAndNames(int userId) {
    std::vector<ChatsInfo> chats;

    std::string query = "SELECT uc.chat_id, u.nickname, MAX(Ct.last_message) AS last_message_time "
        "FROM \"Users_Chats\" uc "
        "JOIN \"Users_Chats\" uc2 ON uc.chat_id = uc2.chat_id AND uc2.user_id != " + std::to_string(userId) + 
        " JOIN \"Users\" u ON u.id = uc2.user_id "
        "LEFT JOIN \"Chats\" Ct ON Ct.id = uc.chat_id "
        "WHERE uc.user_id = " + std::to_string(userId) +
        " GROUP BY uc.chat_id, u.nickname "
        "ORDER BY last_message_time DESC NULLS LAST;";

    PGresult* res = PQexec(conn,query.c_str());

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "Ошибка выполнения запроса getUserChatsWithNames: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        return chats;
    }

    int rows = PQntuples(res);
    for (int i = 0; i < rows; ++i) {
        ChatsInfo info;
        info.chat_id = std::stoi(PQgetvalue(res, i, 0));
        info.nickname = PQgetvalue(res, i, 1);
        char* ts = PQgetvalue(res, i, 2);
        info.last_message = ts ? ts : "";
        chats.push_back(info);
    }

    PQclear(res);
    return chats;
}

std::pair<bool, std::string> Database::createChatWithUser(int currentUserId, const std::string& nickname) {
    // Найдём ID пользователя по никнейму
    std::string query1 = "SELECT id FROM \"Users\" WHERE nickname = '" + nickname + "';";
    PGresult* res1 = PQexec(conn, query1.c_str());

    if (PQresultStatus(res1) != PGRES_TUPLES_OK || PQntuples(res1) == 0) {
        PQclear(res1);
        return { false, "ERROR: Пользователь не найден" };
    }

    int otherUserId = std::stoi(PQgetvalue(res1, 0, 0));
    PQclear(res1);

    if (otherUserId == currentUserId) {
        return { false, "ERROR: Нельзя создать чат с самим собой" };
    }

    // Проверим, есть ли уже чат между этими двумя пользователями
    std::string checkQuery =
        "SELECT c.id FROM \"Chats\" c "
        "JOIN \"Users_Chats\" uc1 ON uc1.chat_id = c.id AND uc1.user_id = " + std::to_string(currentUserId) + " "
        "JOIN \"Users_Chats\" uc2 ON uc2.chat_id = c.id AND uc2.user_id = " + std::to_string(otherUserId) + " "
        "WHERE c.is_group = false "
        "GROUP BY c.id "
        "HAVING COUNT(*) = 2;";
    PGresult* res2 = PQexec(conn, checkQuery.c_str());

    if (PQresultStatus(res2) != PGRES_TUPLES_OK) {
        PQclear(res2);
        return { false, "ERROR: Ошибка при поиске существующего чата" };
    }

    if (PQntuples(res2) > 0) {
        PQclear(res2);
        return { false, "ERROR: Чат уже создан" };
    }

    PQclear(res2);

    // Создаём новый чат
    std::string insertChatQuery = "INSERT INTO \"Chats\" (name, last_message, is_group) VALUES (1, NULL, false) RETURNING id;";
    PGresult* res3 = PQexec(conn, insertChatQuery.c_str());

    if (PQresultStatus(res3) != PGRES_TUPLES_OK || PQntuples(res3) == 0) {
        PQclear(res3);
        return { false, "ERROR: Ошибка при создании чата" };
    }

    int newChatId = std::stoi(PQgetvalue(res3, 0, 0));
    PQclear(res3);

    // Добавляем записи в Users_Chats
    std::string insert1 = "INSERT INTO \"Users_Chats\" (user_id, chat_id) VALUES (" +
        std::to_string(currentUserId) + ", " + std::to_string(newChatId) + ");";
    PGresult* res4 = PQexec(conn, insert1.c_str());

    if (PQresultStatus(res4) != PGRES_COMMAND_OK) {
        PQclear(res4);
        return { false, "ERROR: Не удалось вставить текущего пользователя" };
    }
    PQclear(res4);

    std::string insert2 = "INSERT INTO \"Users_Chats\" (user_id, chat_id) VALUES (" +
        std::to_string(otherUserId) + ", " + std::to_string(newChatId) + ");";
    PGresult* res5 = PQexec(conn, insert2.c_str());

    if (PQresultStatus(res5) != PGRES_COMMAND_OK) {
        PQclear(res5);
        return { false, "ERROR: Не удалось вставить текущего пользователя" };
    }
    PQclear(res5);

    return { true, "OK: Создан чат с пользователем " + nickname };
}

std::tuple<bool, std::string, int> Database::getChatIdWithUser(int user_id, const std::string& peer_nickname) {
    std::string query = "SELECT id FROM \"Users\" WHERE nickname = '" + peer_nickname + "';";
    PGresult* res = PQexec(conn, query.c_str());
    if (PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) == 0) {
        PQclear(res);
        return { false, "Пользователь не найден", -1 };
    }
    int peer_id = std::stoi(PQgetvalue(res, 0, 0));
    PQclear(res);

    query =
        "SELECT uc1.chat_id "
        "FROM \"Users_Chats\" uc1 "
        "JOIN \"Users_Chats\" uc2 ON uc1.chat_id = uc2.chat_id "
        "WHERE uc1.user_id = " + std::to_string(user_id) + " AND uc2.user_id = " + std::to_string(peer_id) + " "
        "LIMIT 1;";
    res = PQexec(conn, query.c_str());

    if (PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) == 0) {
        PQclear(res);
        return { false, "Чат с данным пользователем не найден", -1 };
    }

    int chat_id = std::stoi(PQgetvalue(res, 0, 0));
    PQclear(res);
    return { true, "OK", chat_id };
}

bool Database::sendMessage(int chat_id, int sender_id, const std::string& content) {
    std::string query =
        "INSERT INTO \"Messages\" (chats_id, sender, content, timestamp) VALUES (" +
        std::to_string(chat_id) + ", " + std::to_string(sender_id) + ", '" + content + "', 'NOW');";
    PGresult* res = PQexec(conn, query.c_str());

    bool success = PQresultStatus(res) == PGRES_COMMAND_OK;
    PQclear(res);
    if (!success) return false;
    return success;
}

std::string Database::getMessagesForChat(int chatId) {
    std::string result;

    std::string query =
        "SELECT nickname, content, timestamp AT TIME ZONE 'Asia/Barnaul' FROM ("
        "   SELECT u.nickname, m.content, m.timestamp "
        "   FROM \"Messages\" m "
        "   JOIN \"Users\" u ON m.sender = u.id "
        "   WHERE m.chats_id = " + std::to_string(chatId) + " "
        "   ORDER BY m.timestamp DESC "
        "   LIMIT 78"
        ") sub "
        "ORDER BY timestamp ASC;";

    PGresult* res = PQexec(conn, query.c_str());

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        PQclear(res);
        return "ERROR Не удалось получить сообщения";
    }

    int rows = PQntuples(res);
    for (int i = 0; i < rows; ++i) {
        std::string nickname = PQgetvalue(res, i, 0);
        std::string content = PQgetvalue(res, i, 1);
        std::string timestamp = PQgetvalue(res, i, 2);

        result += "[" + timestamp + "] " + nickname + ": " + content + "\n";
    }

    PQclear(res);
    return result.empty() ? "Нет сообщений" : result;
}