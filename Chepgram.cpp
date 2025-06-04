#include "Chepgram.h"

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

awaitable<int> auth(tcp::socket& socket, Database& db, const std::string& request)
{
    int userID = -1;
    auto phone_pos = request.find("phone=") + 6;
    std::string phone = request.substr(phone_pos);
    auto [ok, response] = db.checkPhoneExists(phone);
    co_await send_message(socket, response);
    if (!ok) {
        auto password = co_await read_response(socket);
        std::cout << "Получено: " << password << "\n";
        auto nickname = co_await read_response(socket);
        std::cout << "Получено: " << nickname << "\n";
        auto [ok, finalname, newID] = db.registerNewUser(phone, password, nickname);
        if (!ok)
        {
            std::cout << "Получено: " << finalname << "\n";
            co_await send_message(socket, finalname);
            do {
                auto nickname = co_await read_response(socket);
                std::cout << "Получено: " << nickname << "\n";
                auto [ok1, finalname, newID] = db.registerNewUser(phone, password, nickname);
                ok = ok1;
                std::cout << "Получено: " << finalname << "\n";
                co_await send_message(socket, finalname);
            } while (!ok);
        }
        else {
            co_await send_message(socket, finalname);
            std::cout << "Получено: " << finalname << "\n";
        }
        userID = newID;
    }
    else {
        userID = std::stoi(response);
        auto password = co_await read_response(socket);
        std::cout << "Получено: " << password << "\n";
        auto [ok, finalname] = db.verifyPassword(std::stoi(response), password);
        if (!ok)
        {
            std::cout << "Получено: " << finalname << "\n";
            co_await send_message(socket, finalname);
            do {
                password = co_await read_response(socket);
                std::cout << "Получено: " << password << "\n";
                auto [ok1, finalname] = db.verifyPassword(std::stoi(response), password);
                std::cout << "Получено: " << finalname << "\n";
                ok = ok1;
                co_await send_message(socket, finalname);
            } while (!ok);
        }
        else {
            co_await send_message(socket, finalname);
            std::cout << "Получено: " << finalname << "\n";
        }
    }
    co_return userID;
}

awaitable<void> getUserChats(tcp::socket& socket, Database& db, int userId)
{
    auto chats = db.getUserChatsAndNames(userId);
    std::string response;
    for (const auto& chat : chats) {
        response += "chat_id=" + std::to_string(chat.chat_id) +
            ";nickname=" + chat.nickname +
            ";last=" + chat.last_message + 
            ";is_group=" + std::to_string(chat.is_group) + "\n";
    }
    if (response.empty()) {
        response = "NO_CHATS\n";
        co_return;
    }
    response += "\0";
    co_await send_message(socket, response);
}

awaitable<void> createChat(tcp::socket& socket, Database& db, const std::string& request, int userID)
{
    std::string nickname = request.substr(std::string("CREATE CHAT nickname=").length());
    auto [ok, response] = db.createChatWithUser(userID, nickname);
    co_await send_message(socket, response);
}

awaitable<void> sendMessage(tcp::socket& socket, Database& db, const std::string& request, int userID)
{
    // Формат: SEND MESSAGE <chat_id>\n<сообщение>
    size_t newline_pos = request.find('\n');
    int chat_id = std::stoi(request.substr(13, newline_pos - 13));
    std::string content = request.substr(newline_pos + 1);

    bool ok = db.sendMessage(chat_id, userID, content);
    if (ok) {
        co_await send_message(socket, "OK");
    }
    else {
        co_await send_message(socket, "ERROR Не удалось отправить сообщение");
    }
}

awaitable<void> getMessages(tcp::socket& socket, Database& db, const std::string& request) {
    int chatId = std::stoi(request.substr(std::string("GET MESSAGES ").length()));
    std::string messages = db.getMessagesForChat(chatId);
    co_await send_message(socket, messages);
}

awaitable<void> createGroupChat(tcp::socket& socket, Database& db, const std::string& request, int userID)
{
    std::string name = request.substr(std::string("CREATE GROUP CHAT name=").length());
    auto [ok, response] = db.createGroupChat(userID, name);
    co_await send_message(socket, response);
}

awaitable<void> addUserToChat(tcp::socket& socket, Database& db, const std::string& request, int userID)
{
    size_t newline_pos = request.find('\n');
    std::string nickname = request.substr(21, newline_pos - 21);
    int chat_id = std::stoi(request.substr(newline_pos + 9));
    auto [ok, response] = db.addUserToGroupChat(chat_id, nickname);
    co_await send_message(socket, response);
    if (ok)
    {
        std::string systemMessage = "User " + nickname + " added to chat.";
        db.sendMessage(chat_id, userID, systemMessage);
    }
}

awaitable<void> getChatMembers(tcp::socket& socket, Database& db, const std::string& request)
{
    int chatId = std::stoi(request.substr(std::string("GET CHAT MEMBERS ").length()));
    std::string response = db.getListMemberOfChat(chatId);
    co_await send_message(socket, response);
}

awaitable<void> handle_session_notify(tcp::socket socket, Database& db) {
    try {
        std::string message = co_await read_response(socket);
        int userID = std::stoi(message);
        auto executor = co_await boost::asio::this_coro::executor;
        while (true) {
            // Периодически проверяем изменения в last_message (можно улучшить через кэш или очередь)
            auto updatedChats = db.getUpdatedChats(userID);  // вернёт список chat_id, где обновился last_message
            steady_timer timer(executor);
            for (int chat_id : updatedChats) {
                auto ep = socket.remote_endpoint();
                std::cout << "To: " << ep.address().to_string() << ":" << ep.port() << std::endl;
                std::string notify = "UPDATE CHAT " + std::to_string(chat_id);
                co_await send_message(socket, notify);
            }
            timer.expires_after(std::chrono::milliseconds(750));
            co_await timer.async_wait(use_awaitable); //Чтобы опрос был только раз в секунду
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Ошибка соединения уведомлений: пользователь разорвал соединение" << std::endl;
    }
}

awaitable<void> handle_session(tcp::socket socket, Database& db) {
    try {
        int userID = -1;
        while (true) {
            auto request = co_await read_response(socket);
            auto ep = socket.remote_endpoint();
            std::cout << "From: " << ep.address().to_string() << ":" << ep.port() << "    ";
            std::cout << "Получено: " << request << "\n";
            if (request.starts_with("LOGIN ")) {
                userID = co_await auth(socket, db, request);
            }
            else if (request.starts_with("CHAT ")) {
                co_await getUserChats(socket, db, userID);
            }
            else if (request.starts_with("CREATE CHAT "))
            {
                co_await createChat(socket, db, request, userID);
            }
            else if(request.starts_with("SEND MESSAGE ")) {
                co_await sendMessage(socket, db, request, userID);
            }
            else if (request.starts_with("GET MESSAGES ")) {
                co_await getMessages(socket, db, request);
            }
            else if (request.starts_with("CREATE GROUP CHAT "))
            {
                co_await createGroupChat(socket, db, request, userID);
            }
            else if (request.starts_with("ADD TO CHAT "))
            {
                co_await addUserToChat(socket, db, request, userID);
            }
            else if (request.starts_with("GET CHAT MEMBERS "))
            {
                co_await getChatMembers(socket, db, request);
            }
            else {
                co_await send_message(socket, "UNKNOWN COMMAND\n");
            }
        }
    }
    catch (std::exception& e) {
        std::cerr << "Ошибка сессии: Пользователь разорвал соединение"  << "\n";
    }
}

int main() {
#ifdef _WIN32
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
#endif
    try {
        boost::asio::io_context io;
        tcp::acceptor acceptor(io, tcp::endpoint(tcp::v4(), 12345));
        std::cout << "[Сервер] Запущен на порту 12345...\n";

        co_spawn(io, [&]() -> awaitable<void> {
            for (;;) {
                
                tcp::socket socket = co_await acceptor.async_accept(use_awaitable);
                
                // Прочитаем первую строку после соединения, чтобы узнать роль
                std::string role = co_await read_response(socket);
                std::cout << role << std::endl;
                if (role == "MAIN") {
                    std::cout << "[Сервер] Принято MAIN соединение\n";
                    co_spawn(io, [s = std::move(socket)]() mutable -> awaitable<void> {
                        Database db;
                        if (!db.connect()) co_return;
                        co_await handle_session(std::move(s), db);
                        }, detached);
                }
                else if (role == "NOTIFY") {
                    std::cout << "[Сервер] Принято NOTIFY соединение\n";
                    co_spawn(io, [s = std::move(socket)]() mutable -> awaitable<void> {
                        Database db;
                        if (!db.connect()) co_return;
                        co_await handle_session_notify(std::move(s), db);
                        }, detached);
                }
                else {
                    std::cerr << "[Сервер] Неизвестная роль соединения: " << role << "\n";
                    socket.close();
                }
            }
            }, detached);

        io.run();
    }
    catch (std::exception& e) {
        std::cerr << "Ошибка сервера: пользователь разорвал соединение"  << "\n";
    }

    return 0;
}