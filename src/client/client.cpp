#include "client.h"

using boost::asio::ip::tcp;
using namespace boost::asio;

awaitable<void> send_message(tcp::socket& socket, const std::string& message)
{
    co_await boost::asio::async_write(socket, boost::asio::buffer(message + "\0"));
}

awaitable<std::string>  read_response(tcp::socket& socket) {
    boost::asio::streambuf buf;
    co_await boost::asio::async_read_until(socket, buf, "\0");
    std::string resp((std::istreambuf_iterator<char>(&buf)), std::istreambuf_iterator<char>());
    co_return resp;
}

awaitable<void> authorization(tcp::socket& socket)
{
    std::cout << "������� �������: ";
    std::string phone;
    std::getline(std::cin, phone);

    std::string message = "LOGIN phone=" + phone;
    co_await send_message(socket, message);
    std::string response = co_await read_response(socket);
    if (response == "NEW")
    {
        std::cout << "�������� ��������. ������� ������: ";
        std::string password;
        std::getline(std::cin, password);
        co_await send_message(socket, password);

        std::string nickname;
        std::cout << "�������: ";
        std::getline(std::cin, nickname);
        co_await send_message(socket, nickname);
        message = co_await read_response(socket);

        if (message == "��� ��� ������������ ��� ������. ������� �����: ")
        {
            do {
                std::cout << "��� ��� ������������ ��� ������. ������� �����: ";
                std::getline(std::cin, nickname);
                co_await send_message(socket, nickname);
                message = co_await read_response(socket);
            } while (message == "��� ��� ������������ ��� ������. ������� �����: ");
        }
    }
    else
    {
        std::cout << "������� ������: ";
        std::string password;
        std::getline(std::cin, password);
        co_await send_message(socket, password);
        message = co_await read_response(socket);
        if (message == "�������� ������")
        {
            do {
                std::cout << "�������� ������. ��������� ������� �����: ";
                std::getline(std::cin, password);
                co_await send_message(socket, password);
                std::string message = co_await read_response(socket);
            } while (message == "�������� ������");
        }
    }
    std::cout << message << std::endl;
}

awaitable<void> getChats(tcp::socket& socket)
{
    send_message(socket, "CHAT ");
    std::string response = co_await read_response(socket);
    if (response == "NO_CHATS") {
        std::cout << "� ��� ��� �����.\n";
        co_return;
    }
    std::istringstream stream(response);
    std::string line;
    std::cout << "���� ����:\n";
    while (std::getline(stream, line)) {
        size_t id_pos = line.find("chat_id=");
        size_t nickname_pos = line.find("nickname=");
        size_t last_pos = line.find("last=");

        if (id_pos == std::string::npos || nickname_pos == std::string::npos || last_pos == std::string::npos)
            continue;

        int chat_id = std::stoi(line.substr(id_pos + 8, nickname_pos - id_pos - 9));
        std::string nickname = line.substr(nickname_pos + 9, last_pos - nickname_pos - 10);
        std::string last_time = line.substr(last_pos + 5);

        std::cout << "��� � " << nickname << " � ��������� ���������: " << last_time << "\n";
    }
}

awaitable<void> createChat(tcp::socket& socket) {
    std::cout << "������� ��� ������������, � ������� ������ ������� ���: ";
    std::string nickname;
    std::getline(std::cin, nickname);

    std::string message = "CREATE CHAT nickname=" + nickname;
    co_await send_message(socket, message);
    message = co_await read_response(socket);
    std::cout << message << std::endl;
}

awaitable<int> openChat(tcp::socket& socket) {
    int chatID = -1;
    std::cout << "������� ������� �����������: ";
    std::string peer;
    std::getline(std::cin, peer);

    std::string command = "OPEN CHAT " + peer;
    co_await send_message(socket, command);

    std::string response = co_await read_response(socket);
    if (response.starts_with("OK ")) {
        chatID = std::stoi(response.substr(3));
        std::cout << "��� ������." << "\n";
    }
    else {
        std::cout << "������: " << response.substr(6) << "\n";
    }
    co_return chatID;
}

awaitable<void> getChatStory(tcp::socket& socket, int chatID) {
    co_await send_message(socket, "GET MESSAGES " + std::to_string(chatID));
    std::string messages = co_await read_response(socket);
    std::cout << "������� ����:\n" << messages << "\n";
}

awaitable<void> sendMsgToUser(tcp::socket& socket, int chat_id) {
    std::cout << "������� ���������: ";
    std::string message;
    std::getline(std::cin, message);

    std::string command = "SEND_MESSAGE " + std::to_string(chat_id) + "\n" + message;
    co_await send_message(socket, command);

    std::string response = co_await read_response(socket);
    if (response == "OK") {
        std::cout << "��������� ����������" << std::endl;
    }
    else {
        std::cout << "������: " << response.substr(6) << std::endl;
    }
}

awaitable<void> main_session(io_context& io_context) {
    tcp::resolver resolver(io_context);
    auto endpoints = co_await resolver.async_resolve("79.136.138.121", "12345", use_awaitable);

    tcp::socket socket(io_context);
    co_await async_connect(socket, endpoints, use_awaitable);

    std::cout << "����������� �������\n";

    co_await authorization(socket);
    co_await getChats(socket);
    int chat_id = co_await openChat(socket);
    if (chat_id >= 0) {
        co_await getChatStory(socket, chat_id);
        co_await sendMsgToUser(socket, chat_id);
    }
    co_return;
}

int main() {
#ifdef _WIN32
    SetConsoleCP(1251); // ��������� ��������� �������
    SetConsoleOutputCP(1251); // ��������� ��������� ������ �������
#endif
    try {
        io_context io_context;
        co_spawn(io_context, main_session(io_context), detached);
        io_context.run();
    }
    catch (std::exception& e) {
        std::cerr << "������: " << e.what() << "\n";
    }

    return 0;
}