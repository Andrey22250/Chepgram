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

//Проверка номера телефона+
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

//Получение пароля++
std::pair<bool, std::string> Database::verifyPassword(int userId, const std::string& password) {
    std::string query = "SELECT password, nickname FROM \"Users\" WHERE id = " + std::to_string(userId) + ";";
    PGresult* res = PQexec(conn, query.c_str());

    if (PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) == 0) {
        PQclear(res);
        return { false, "1Wrong" };
    }

    std::string dbPassword = PQgetvalue(res, 0, 0);
    std::string nickname = PQgetvalue(res, 0, 1);
    PQclear(res);

    if (dbPassword == password) {
        return { true, "Успешный вход. Ник: " + nickname };
    }
    else {
        return { false, "1Wrong" };
    }
}

//Регистрация нового пользователя+-
std::tuple<bool, std::string, int> Database::registerNewUser(const std::string& phone, const std::string& password, const std::string& nickname) {
    // Проверка ника
    std::string checkNickQuery = "SELECT id FROM \"Users\" WHERE nickname = '" + nickname + "';";
    PGresult* nickRes = PQexec(conn, checkNickQuery.c_str());

    if (PQntuples(nickRes) > 0) {
        PQclear(nickRes);
        return { false, "Used", -1 };
    }
    PQclear(nickRes);

    std::string insertQuery = "INSERT INTO \"Users\" (nickname, phonenumber, password) "
        "VALUES ('" + nickname + "', '" + phone + "', '" + password + "') RETURNING id;";
    PGresult* insertRes = PQexec(conn, insertQuery.c_str());

    if (PQresultStatus(insertRes) != PGRES_TUPLES_OK || PQntuples(insertRes) == 0) {
        std::string err = PQresultErrorMessage(insertRes);
        PQclear(insertRes);
        return { false, "Error" + err, -1 };
    }

    std::string newId = PQgetvalue(insertRes, 0, 0);
    PQclear(insertRes);
    return { true, "Ваш телефон = " + phone + ", ваше имя пользователя = " + nickname + " (зарегистрирован)", std::stoi(newId)};
}

//Получение списка чатов+
std::vector<ChatsInfo> Database::getUserChatsAndNames(int userId) {
    std::vector<ChatsInfo> chats;

    std::string query =
        "SELECT uc.chat_id, "
        "  CASE "
        "    WHEN c.is_group THEN c.name "
        "    ELSE u.nickname "
        "  END AS display_name, "
        "  CASE "
        "    WHEN now() - c.last_message < INTERVAL '2 days' "
        "      THEN to_char(c.last_message, 'HH24:MI') "
        "    ELSE to_char(c.last_message, 'DD.MM.YYYY') "
        "  END AS last_message_display, c.is_group "
        "FROM \"Users_Chats\" uc "
        "JOIN \"Chats\" c ON c.id = uc.chat_id "
        "LEFT JOIN LATERAL ( "
        "  SELECT u.nickname "
        "  FROM \"Users_Chats\" uc2 "
        "  JOIN \"Users\" u ON u.id = uc2.user_id "
        "  WHERE uc2.chat_id = uc.chat_id AND uc2.user_id != " + std::to_string(userId) + " "
        "  LIMIT 1 "
        ") u ON NOT c.is_group "
        "WHERE uc.user_id = " + std::to_string(userId) + " "
        "ORDER BY c.last_message DESC NULLS LAST;";

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
        info.is_group = PQgetvalue(res, i, 3);
        chats.push_back(info);
    }

    PQclear(res);
    return chats;
}

//Создание чата с пользователем+
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

//Отправка сообщения+
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
        "SELECT sender, nickname, content, formatted_time "
        "FROM ( "
        "    SELECT m.sender, u.nickname, m.content, m.timestamp, "
        "           CASE "
        "               WHEN now() - m.timestamp < INTERVAL '2 days' "
        "                   THEN TO_CHAR(m.timestamp AT TIME ZONE 'Asia/Barnaul', 'HH24:MI') "
        "               ELSE TO_CHAR(m.timestamp AT TIME ZONE 'Asia/Barnaul', 'DD.MM.YYYY HH24:MI') "
        "           END AS formatted_time "
        "    FROM \"Messages\" m "
        "    JOIN \"Users\" u ON m.sender = u.id "
        "    WHERE m.chats_id = " + std::to_string(chatId) + " "
        "    ORDER BY m.timestamp DESC "
        "    LIMIT 8 "
        ") sub "
        "ORDER BY timestamp ASC;";

    PGresult* res = PQexec(conn, query.c_str());

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        PQclear(res);
        return "ERROR Не удалось получить сообщения";
    }

    int rows = PQntuples(res);
    for (int i = 0; i < rows; ++i) {
        std::string senderId = PQgetvalue(res, i, 0);
        std::string nickname = PQgetvalue(res, i, 1);
        std::string content = PQgetvalue(res, i, 2);
        std::string timestamp = PQgetvalue(res, i, 3);

        result += senderId + "|" + nickname + "[" + timestamp + "] " + content + "\n";
    }

    PQclear(res);
    return result.empty() ? "Нет сообщений" : result;
}

std::vector<int> Database::getUpdatedChats(int userId) {
    std::vector<int> updated;

    std::string query = 
        "SELECT c.id FROM \"Chats\" c "
        "JOIN \"Users_Chats\" uc ON c.id = uc.chat_id "
        "JOIN \"Messages\" m ON c.id = m.chats_id "
        "WHERE uc.user_id = " + std::to_string(userId) +
         " AND c.last_message > NOW() - INTERVAL '2 seconds' "
        "AND m.timestamp > NOW() - INTERVAL '10 seconds' "
        "AND m.sender != "+ std::to_string(userId) + ";";

    PGresult* res = PQexec(conn, query.c_str());
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "Ошибка в getUpdatedChatsForUser: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        return updated;
    }

    int rows = PQntuples(res);
    for (int i = 0; i < rows; ++i) {
        updated.push_back(std::stoi(PQgetvalue(res, i, 0)));
    }

    PQclear(res);
    return updated;
}

std::pair<bool, std::string> Database::createGroupChat(int currentUserId, const std::string& name) {
    // 1. Создание группового чата
    std::string query1 =
        "INSERT INTO \"Chats\" (name, is_group) VALUES ('" + name + "', TRUE) RETURNING id;";
    PGresult* res1 = PQexec(conn, query1.c_str());

    if (PQresultStatus(res1) != PGRES_TUPLES_OK || PQntuples(res1) == 0) {
        PQclear(res1);
        return { false, "ERROR: Не удалось создать чат" };
    }

    int chatId = std::stoi(PQgetvalue(res1, 0, 0));
    PQclear(res1);

    // 2. Добавление текущего пользователя в Users_Chats
    std::string insertUserQuery =
        "INSERT INTO \"Users_Chats\" (user_id, chat_id) VALUES (" +
        std::to_string(currentUserId) + ", " + std::to_string(chatId) + ");";
    PGresult* res2 = PQexec(conn, insertUserQuery.c_str());

    bool success = PQresultStatus(res2) == PGRES_COMMAND_OK;
    PQclear(res2);

    if (!success) {
        PQclear(res2);
        return { false, "ERROR: Не удалось создать чат" };
    }

    return { true, "OK: Создан групповой чат:  " + name };
}