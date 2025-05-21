#include "Chepgram.h"

int main() {
#ifdef _WIN32
    SetConsoleCP(1251); // Установка кодировки консоли
    SetConsoleOutputCP(1251); // Установка кодировки вывода консоли
#endif

    Database db("host=localhost port=5432 dbname=chepgram user=postgres password=24071977F_x");

    if (!db.isConnected()) {
        std::cerr << "DB connection failed\n";
        return 1;
    }

    std::string phone, password;
    std::cout << "Phone: ";
    std::cin >> phone;
    std::cout << "Password: ";
    std::cin >> password;

    auto userIdOpt = db.authenticateUser(phone, password);
    if (!userIdOpt) {
        std::cout << "Login failed\n";
        return 0;
    }
    std::string Nickname = db.getNickname(*userIdOpt);
    std::cout << "Login success! Your nickname is: " << Nickname << "\n";

    int chatId;
    std::string message;
    std::cout << "Enter chat id: ";
    std::cin >> chatId;
    std::cin.seekg(std::cin.eof());
    std::cout << "Enter message: ";
    std::getline(std::cin, message);

    if (db.sendMessage(*userIdOpt, chatId, message)) {
        std::cout << "Message sent\n";
    }
    else {
        std::cout << "Failed to send\n";
    }
}
