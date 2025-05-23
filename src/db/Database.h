#pragma once
#include <string>
#include <iostream>
#include <libpq-fe.h>

class Database {
public:
    Database();
    ~Database();

    bool connect();
    std::pair<bool, std::string> checkPhoneExists(const std::string& phone);
    std::pair<bool, std::string> verifyPassword(int userId, const std::string& password);
    std::pair<bool, std::string> registerNewUser(const std::string& phone, const std::string& password, const std::string& nickname);
private:
    PGconn* conn;
};