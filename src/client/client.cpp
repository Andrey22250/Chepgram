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

void Authorization(tcp::socket& socket)
{
    std::cout << "Введите телефон: ";
    std::string phone;
    std::getline(std::cin, phone);

    std::string message = "LOGIN phone=" + phone;
    send_message(socket, message);
    std::string response = read_response(socket);
    if (response == "NEW")
    {
        std::cout << "Создание аккаунта. Введите пароль:";
        std::string password;
        std::getline(std::cin, password);
        send_message(socket, password);

        std::string nickname;
        std::cout << "Никнейм: ";
        std::getline(std::cin, nickname);
        send_message(socket, nickname);

        std::cout << read_response(socket);
    }
    else
    {
        std::cout << "Введите пароль:";
        std::string password;
        std::getline(std::cin, password);
        send_message(socket, password);
        std::cout << read_response(socket);
    }
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
        
        Authorization(socket);
    }
    catch (std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << "\n";
    }

    return 0;
}