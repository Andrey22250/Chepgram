#pragma once
#include <libpq-fe.h>
#include <string>
#include <optional>

class Database {
public:
    Database(const std::string& conninfo);
    ~Database();

    bool isConnected() const;

    // ��������������
    std::optional<int> authenticateUser(const std::string& phone, const std::string& password);
    // ��������� ��������
    std::string getNickname(int userIdOpt);
    // �������� ���������
    bool sendMessage(int senderId, int chatId, const std::string& content);

private:
    PGconn* conn;
};