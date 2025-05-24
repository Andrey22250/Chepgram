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
    std::cout << "������� �������: ";
    std::string phone;
    std::getline(std::cin, phone);

    std::string message = "LOGIN phone=" + phone;
    send_message(socket, message);
    std::string response = read_response(socket);
    if (response == "NEW")
    {
        std::cout << "�������� ��������. ������� ������: ";
        std::string password;
        std::getline(std::cin, password);
        send_message(socket, password);

        std::string nickname;
        std::cout << "�������: ";
        std::getline(std::cin, nickname);
        send_message(socket, nickname);
        message = read_response(socket);

        if (message == "��� ��� ������������ ��� ������. ������� �����: ")
        {
            do {
                std::cout << "��� ��� ������������ ��� ������. ������� �����: ";
                std::getline(std::cin, nickname);
                send_message(socket, nickname);
                message = read_response(socket);
            } while (message == "��� ��� ������������ ��� ������. ������� �����: ");
        }
    }
    else
    {
        std::cout << "������� ������: ";
        std::string password;
        std::getline(std::cin, password);
        send_message(socket, password);
        message = read_response(socket);
        if (message == "�������� ������")
        {
            do {
                std::cout << "�������� ������. ��������� ������� �����: ";
                std::getline(std::cin, password);
                send_message(socket, password);
                message = read_response(socket);
            } while (message == "�������� ������");
        }
    }
    std::cout << message << std::endl;
}

void getChats(tcp::socket& socket)
{
    send_message(socket, "CHAT ");
    std::string response = read_response(socket);
    if (response == "NO_CHATS") {
        std::cout << "� ��� ��� �����.\n";
        return;
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

void createChat(tcp::socket& socket) {
    std::cout << "������� ��� ������������, � ������� ������ ������� ���: ";
    std::string nickname;
    std::getline(std::cin, nickname);

    std::string message = "CREATE CHAT nickname=" + nickname;
    send_message(socket, message);
    message = read_response(socket);
    std::cout << message << std::endl;
}

int openChat(tcp::socket& socket) {
    int chatID = -1;
    std::cout << "������� ������� �����������: ";
    std::string peer;
    std::getline(std::cin, peer);

    std::string command = "OPEN CHAT " + peer;
    send_message(socket, command);

    std::string response = read_response(socket);
    if (response.starts_with("OK ")) {
        chatID = std::stoi(response.substr(3));
        std::cout << "��� ������." << "\n";
    }
    else {
        std::cout << "������: " << response.substr(6) << "\n";
    }
    return chatID;
}

void getChatStory(tcp::socket& socket, int chatID) {
    send_message(socket, "GET MESSAGES " + std::to_string(chatID));
    std::string messages = read_response(socket);
    std::cout << "������� ����:\n" << messages << "\n";
}

void sendMsgToUser(tcp::socket& socket, int chat_id) {
    std::cout << "������� ���������: ";
    std::string message;
    std::getline(std::cin, message);

    std::string command = "SEND_MESSAGE " + std::to_string(chat_id) + "\n" + message;
    send_message(socket, command);

    std::string response = read_response(socket);
    if (response == "OK") {
        std::cout << "��������� ����������" << std::endl;
    }
    else {
        std::cout << "������: " << response.substr(6) << std::endl;
    }
}

int main() {
#ifdef _WIN32
    SetConsoleCP(1251); // ��������� ��������� �������
    SetConsoleOutputCP(1251); // ��������� ��������� ������ �������
#endif
    try {
        boost::asio::io_context io_context;

        tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve("79.136.138.121", "12345");

        tcp::socket socket(io_context);
        boost::asio::connect(socket, endpoints);
        
        authorization(socket);
        getChats(socket);
        /*createChat(socket);*/
        int chatID = openChat(socket);
        getChatStory(socket, chatID);
        while (true) sendMsgToUser(socket, chatID);
    }
    catch (std::exception& e) {
        std::cerr << "������: " << e.what() << "\n";
    }

    return 0;
}