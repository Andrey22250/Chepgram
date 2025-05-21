#include "Database.h"
#include <iostream>
#include <ctime>

Database::Database(const std::string& conninfo) {
    conn = PQconnectdb(conninfo.c_str());
    if (PQstatus(conn) != CONNECTION_OK) {
        std::cerr << "Соединение с базой данных провалено: " << PQerrorMessage(conn);
        PQfinish(conn);
        conn = nullptr;
    }
}

Database::~Database() {
    if (conn) PQfinish(conn);
}

bool Database::isConnected() const {
    return conn != nullptr && PQstatus(conn) == CONNECTION_OK;
}

std::optional<int> Database::authenticateUser(const std::string& phone, const std::string& password) {
    if (!conn) return std::nullopt;

    std::string query = "SELECT id FROM \"Users\" WHERE phonenumber = '" + phone + "' AND password = '" + password + "';";
    PGresult* res = PQexec(conn, query.c_str());

    if (PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) == 0) {
        PQclear(res);
        return std::nullopt;
    }

    int userId = std::stoi(PQgetvalue(res, 0, 0));
    PQclear(res);
    return userId;
}

std::string Database::getNickname(int userId)
{
    std::string query = "SELECT nickname FROM \"Users\" WHERE id = " + std::to_string(userId) + ";";
    PGresult* res = PQexec(conn, query.c_str());

    if (PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) == 0) {
        PQclear(res);
        return "";
    }

    std::string nickname = PQgetvalue(res, 0, 0);
    PQclear(res);
    return nickname;
}

bool Database::sendMessage(int senderId, int chatId, const std::string& content) {
    if (!conn) return false;

    std::string query = "INSERT INTO \"Messages\" (content, senter, chats_id, timestamp) VALUES ('" +
        content + "', " + std::to_string(senderId) + ", " + std::to_string(chatId) + ", now());";

    PGresult* res = PQexec(conn, query.c_str());
    bool success = PQresultStatus(res) == PGRES_COMMAND_OK;
    PQclear(res);
    return success;
}