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
    std::tuple<bool, std::string, int> getChatIdWithUser(int user_id, const std::string& peer_nickname);
    bool sendMessage(int chat_id, int sender_id, const std::string& content);
    std::string getMessagesForChat(int chatId);
    std::vector<int> getUpdatedChats(int userId);
private:
    PGconn* conn;
};