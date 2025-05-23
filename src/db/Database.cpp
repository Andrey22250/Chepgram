#include "database.h"

Database::Database() : conn(nullptr) {}

Database::~Database() {
    if (conn) {
        PQfinish(conn);
    }
}

bool Database::connect() {
    conn = PQconnectdb("host=localhost dbname=chepgram user=postgres password=24071977F_x");
    if (PQstatus(conn) != CONNECTION_OK) {
        std::cerr << "������ ����������� � ��: " << PQerrorMessage(conn);
        return false;
    }
    return true;
}

std::pair<bool, std::string> Database::checkPhoneExists(const std::string& phone) {
    std::string query = "SELECT id FROM \"Users\" WHERE phonenumber = '" + phone + "';";
    PGresult* res = PQexec(conn, query.c_str());

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        PQclear(res);
        return { false, "������ ��� �������� ��������" };
    }

    if (PQntuples(res) > 0) {
        std::string userId = PQgetvalue(res, 0, 0);
        PQclear(res);
        return { true, userId };  // ������������ ������
    }

    PQclear(res);
    return { false, "NEW" }; // ��� ������ ������
}

std::pair<bool, std::string> Database::verifyPassword(int userId, const std::string& password) {
    std::string query = "SELECT password, nickname FROM \"Users\" WHERE id = " + std::to_string(userId) + ";";
    PGresult* res = PQexec(conn, query.c_str());

    if (PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) == 0) {
        PQclear(res);
        return { false, "������ ��� �������� ������" };
    }

    std::string dbPassword = PQgetvalue(res, 0, 0);
    std::string nickname = PQgetvalue(res, 0, 1);
    PQclear(res);

    if (dbPassword == password) {
        return { true, "�������� ����. ���: " + nickname };
    }
    else {
        return { false, "�������� ������" };
    }
}

std::pair<bool, std::string> Database::registerNewUser(const std::string& phone, const std::string& password, const std::string& nickname) {
    // �������� ����
    std::string checkNickQuery = "SELECT id FROM \"Users\" WHERE nickname = '" + nickname + "';";
    PGresult* nickRes = PQexec(conn, checkNickQuery.c_str());

    if (PQntuples(nickRes) > 0) {
        PQclear(nickRes);
        return { false, "��� ��� ������������ ��� ������. ������� �����: " };
    }
    PQclear(nickRes);

    std::string insertQuery = "INSERT INTO \"Users\" (nickname, phonenumber, password) "
        "VALUES ('" + nickname + "', '" + phone + "', '" + password + "') RETURNING id;";
    PGresult* insertRes = PQexec(conn, insertQuery.c_str());

    if (PQresultStatus(insertRes) != PGRES_TUPLES_OK || PQntuples(insertRes) == 0) {
        std::string err = PQresultErrorMessage(insertRes);
        PQclear(insertRes);
        return { false, "������ �����������: " + err };
    }

    std::string newId = PQgetvalue(insertRes, 0, 0);
    PQclear(insertRes);
    return { true, "��� �������= " + phone + ", ���� ��� ������������= " + nickname + " (���������������)" };
}