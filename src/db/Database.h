#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <libpq-fe.h>

struct ChatsInfo {
    int chat_id;
    std::string nickname;
    std::string last_message;
};

class Database {
public:
    Database();
    ~Database();

    bool connect(); //Соединение с базой данных

    //Функции для авторизации
    std::pair<bool, std::string> checkPhoneExists(const std::string& phone);
    std::pair<bool, std::string> verifyPassword(int userId, const std::string& password);
    std::tuple<bool, std::string, int> registerNewUser(const std::string& phone, const std::string& password, const std::string& nickname);

    //Работа с чатами
    std::vector<ChatsInfo> getUserChatsAndNames(int userID);
    std::pair<bool, std::string> createChatWithUser(int currentUserId, const std::string& nickname);
private:
    PGconn* conn;
};