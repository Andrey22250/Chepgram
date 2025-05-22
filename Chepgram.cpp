#include "Chepgram.h"

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

void session(tcp::socket socket, Database& db) {
    try {
        std::string request = read_response(socket);
        std::cout << "Получено: " << request << "\n";

        if (request.starts_with("LOGIN ")) {
            auto phone_pos = request.find("phone=") + 6;
            std::string phone = request.substr(phone_pos);

            auto [ok, response] = db.checkPhoneExists(phone);
            std::string reply = ok ? "OK " + response + "\n" : "ERROR\n";
            send_message(socket, response);
            if(ok == 0)
            {
                std::string password = read_response(socket);
                std::string nickname = read_response(socket);
                auto [ok, finalname] = db.registerNewUser(phone, password, nickname);
                send_message(socket, finalname);
            }
            else
            {
                std::string password = read_response(socket);
                auto [ok, finalname] = db.verifyPassword(stoi(response), password);
                send_message(socket, finalname);
            }

        }
        else if (request.starts_with(" "))
        {

        }
        else {
            boost::asio::write(socket, boost::asio::buffer("UNKNOWN COMMAND\n"));
        }

    }
    catch (std::exception& e) {
        std::cerr << "Ошибка сессии: " << e.what() << "\n";
    }
}

int main() {
#ifdef _WIN32
    SetConsoleCP(1251); // Установка кодировки консоли
    SetConsoleOutputCP(1251); // Установка кодировки вывода консоли
#endif

    try {
        boost::asio::io_context io_context;
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 12345));

        Database db;
        if (!db.connect()) {
            return 1;
        }

        std::cout << "Сервер запущен на порту 12345...\n";

        while (true) {
            tcp::socket socket(io_context);
            acceptor.accept(socket);
            std::thread(session, std::move(socket), std::ref(db)).detach();
        }

    }
    catch (std::exception& e) {
        std::cerr << "Ошибка сервера: " << e.what() << "\n";
    }

    return 0;
}
