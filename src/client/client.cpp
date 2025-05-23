#include "client.h"

using boost::asio::ip::tcp;

void send_message(tcp::socket& socket, const std::string& message)
{
    boost::asio::write(socket, boost::asio::buffer(message + "\0"));
}

std::string read_response(tcp::socket& socket) {
    boost::asio::streambuf buf;
    boost::asio::read_until(socket, buf, "\0");
    std::string resp((std::istreambuf_iterator<char>(&buf)), std::istreambuf_iterator<char>());
    return resp;
}

void authorization(tcp::socket& socket)
{
    std::cout << "Введите телефон: ";
    std::string phone;
    std::getline(std::cin, phone);

    std::string message = "LOGIN phone=" + phone;
    send_message(socket, message);
    std::string response = read_response(socket);
    if (response == "NEW")
    {
        std::cout << "Создание аккаунта. Введите пароль: ";
        std::string password;
        std::getline(std::cin, password);
        send_message(socket, password);

        std::string nickname;
        std::cout << "Никнейм: ";
        std::getline(std::cin, nickname);
        send_message(socket, nickname);
        message = read_response(socket);

        if (message == "Это имя пользователя уже занято. Введите новое: ")
        {
            do {
                std::cout << "Это имя пользователя уже занято. Введите новое: ";
                std::getline(std::cin, nickname);
                send_message(socket, nickname);
            } while (read_response(socket) != "Это имя пользователя уже занято. Введите новое: ");
        }
    }
    else
    {
        std::cout << "Введите пароль: ";
        std::string password;
        std::getline(std::cin, password);
        send_message(socket, password);
        message = read_response(socket);
        if (message == "Неверный пароль")
        {
            do {
                std::cout << "Неверный пароль. Повторите попытку ввода: ";
                std::getline(std::cin, password);
                send_message(socket, password);
            } while (read_response(socket) != "Неверный пароль");
        }
    }
    std::cout << message << std::endl;
}

void getChats(tcp::socket& socket)
{
    send_message(socket, "CHAT ");
    std::string response = read_response(socket);
    if (response == "NO_CHATS") {
        std::cout << "У вас нет чатов.\n";
        return;
    }
    std::istringstream stream(response);
    std::string line;
    std::cout << "Ваши чаты:\n";
    while (std::getline(stream, line)) {
        size_t id_pos = line.find("chat_id=");
        size_t nickname_pos = line.find("nickname=");
        size_t last_pos = line.find("last=");

        if (id_pos == std::string::npos || nickname_pos == std::string::npos || last_pos == std::string::npos)
            continue;

        int chat_id = std::stoi(line.substr(id_pos + 8, nickname_pos - id_pos - 9));
        std::string nickname = line.substr(nickname_pos + 9, last_pos - nickname_pos - 10);
        std::string last_time = line.substr(last_pos + 5);

        std::cout << "Чат с " << nickname << " — последнее сообщение: " << last_time << "\n";
    }
}

void createChat(tcp::socket& socket) {
    std::cout << "Введите ник пользователя, с которым хотите создать чат: ";
    std::string nickname;
    std::getline(std::cin, nickname);

    std::string message = "CREATE CHAT nickname=" + nickname;
    send_message(socket, message);
    message = read_response(socket);
    std::cout << message << std::endl;
}

int main() {
#ifdef _WIN32
    SetConsoleCP(1251); // Установка кодировки консоли
    SetConsoleOutputCP(1251); // Установка кодировки вывода консоли
#endif
    try {
        boost::asio::io_context io_context;

        tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve("127.0.0.1", "12345");

        tcp::socket socket(io_context);
        boost::asio::connect(socket, endpoints);
        
        authorization(socket);
        getChats(socket);
        createChat(socket);
        while (true);
    }
    catch (std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << "\n";
    }

    return 0;
}