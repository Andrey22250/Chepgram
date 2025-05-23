#include "Chepgram.h"

using boost::asio::ip::tcp;
using namespace boost::asio;

awaitable<void> send_message(tcp::socket& socket, const std::string& message) {
    co_await async_write(socket, buffer(message + "\0"), use_awaitable);
}

awaitable<std::string> read_response(tcp::socket& socket) {
    streambuf buf;
    co_await async_read_until(socket, buf, "\0", use_awaitable);
    std::istream is(&buf);
    std::string result;
    std::getline(is, result, '\0');
    co_return result;
}

awaitable<void> handle_session(tcp::socket socket, Database& db) {
    try {
        auto request = co_await read_response(socket);
        std::cout << "Получено: " << request << "\n";

        if (request.starts_with("LOGIN ")) {
            auto phone_pos = request.find("phone=") + 6;
            std::string phone = request.substr(phone_pos);
            auto [ok, response] = db.checkPhoneExists(phone);
            co_await send_message(socket, response);

            if (!ok) {
                auto password = co_await read_response(socket);
                std::cout << "Получено: " << password << "\n";
                auto nickname = co_await read_response(socket);
                std::cout << "Получено: " << nickname << "\n";
                auto [ok2, finalname] = db.registerNewUser(phone, password, nickname);
                std::cout << "Получено: " << finalname << "\n";
                if (!ok2)
                    do {
                        co_await send_message(socket, finalname);
                        auto nickname = co_await read_response(socket);
                        std::cout << "Получено: " << nickname << "\n";
                        auto [ok2, finalname] = db.registerNewUser(phone, password, nickname);
                        std::cout << "Получено: " << finalname << "\n";
                    } while (ok2);
                else{
                        co_await send_message(socket, finalname);
                        std::cout << "Получено: " << finalname << "\n";
                }
            }
            else {
                auto password = co_await read_response(socket);
                std::cout << "Получено: " << password << "\n";
                auto [ok3, finalname] = db.verifyPassword(std::stoi(response), password);
                if (!ok3)
                    do {
                        co_await send_message(socket, finalname);
                        auto password = co_await read_response(socket);
                        std::cout << "Получено: " << password << "\n";
                        auto [ok2, finalname] = db.verifyPassword(std::stoi(response), password);
                        std::cout << "Получено: " << finalname << "\n";
                    } while (ok3);
                else {
                    co_await send_message(socket, finalname);
                    std::cout << "Получено: " << finalname << "\n";
                }
            }
        }
        else if (request.starts_with("CHAT ")) {

        }
        else {
            co_await send_message(socket, "UNKNOWN COMMAND\n");
        }
        while (true);
    }
    catch (std::exception& e) {
        std::cerr << "Ошибка сессии: " << e.what() << "\n";
    }
}

int main() {
#ifdef _WIN32
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
#endif
    try {
        io_context io;
        tcp::acceptor acceptor(io, tcp::endpoint(tcp::v4(), 12345));
        std::cout << "[Сервер] Запущен на порту 12345...\n";

        Database db;
        if (!db.connect()) return 1;

        co_spawn(io, [&]() -> awaitable<void> {
            for (;;) {
                tcp::socket socket = co_await acceptor.async_accept(use_awaitable);
                co_spawn(io, handle_session(std::move(socket), db), detached);
            }
            }, detached);

        io.run();
    }
    catch (std::exception& e) {
        std::cerr << "Ошибка сервера: " << e.what() << "\n";
    }

    return 0;
}