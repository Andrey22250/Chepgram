#pragma once
#include <libpq-fe.h>
#include <string>
#include <optional>

class Database {
public:
    Database(const std::string& conninfo);
    ~Database();

    bool isConnected() const;

    // Аутентификация
    std::optional<int> authenticateUser(const std::string& phone, const std::string& password);
    // Получение никнейма
    std::string getNickname(int userIdOpt);
    // Отправка сообщения
    bool sendMessage(int senderId, int chatId, const std::string& content);

private:
    PGconn* conn;
};