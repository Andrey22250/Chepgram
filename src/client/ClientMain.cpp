/***************************************************************
 * Name:      ClientMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    Andrey ()
 * Created:   2025-05-26
 * Copyright: Andrey ()
 * License:
 **************************************************************/

#include "ClientMain.h"

//(*InternalHeaders(ClientFrame)
#include <wx/font.h>
#include <wx/intl.h>
#include <wx/settings.h>
#include <wx/string.h>
//*)

//helper functions
enum wxbuildinfoformat {
    short_f, long_f };

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}


//(*IdInit(ClientFrame)
const wxWindowID ClientFrame::ID_BUTTON3 = wxNewId();
const wxWindowID ClientFrame::ID_BUTTON2 = wxNewId();
const wxWindowID ClientFrame::ID_STATICTEXT1 = wxNewId();
const wxWindowID ClientFrame::ID_PANEL1 = wxNewId();
const wxWindowID ClientFrame::ID_STATICTEXT2 = wxNewId();
const wxWindowID ClientFrame::ID_STATICTEXT3 = wxNewId();
const wxWindowID ClientFrame::ID_TEXTCTRL1 = wxNewId();
const wxWindowID ClientFrame::ID_BUTTON1 = wxNewId();
const wxWindowID ClientFrame::ID_BUTTON4 = wxNewId();
const wxWindowID ClientFrame::ID_TEXTCTRL2 = wxNewId();
const wxWindowID ClientFrame::ID_TEXTCTRL3 = wxNewId();
const wxWindowID ClientFrame::ID_BUTTON5 = wxNewId();
const wxWindowID ClientFrame::ID_LISTCTRL1 = wxNewId();
const wxWindowID ClientFrame::ID_RICHTEXTCTRL1 = wxNewId();
const wxWindowID ClientFrame::ID_BUTTON6 = wxNewId();
const wxWindowID ClientFrame::ID_TEXTCTRL4 = wxNewId();
const wxWindowID ClientFrame::ID_PANEL3 = wxNewId();
const wxWindowID ClientFrame::ID_PANEL2 = wxNewId();
//*)

BEGIN_EVENT_TABLE(ClientFrame,wxFrame)
    EVT_LEFT_DOWN(ClientFrame::OnPanel1LeftDown)
    EVT_LEFT_UP(ClientFrame::OnPanel1LeftUp)
    EVT_MOTION(ClientFrame::OnPanel1MouseMove)
END_EVENT_TABLE()

awaitable<void> ClientFrame::send_message(const std::string& message)
{
    co_await async_write(socket_, boost::asio::buffer(message + "\0"), use_awaitable);
}

awaitable<std::string> ClientFrame::read_response() {
    streambuf buf;
    co_await async_read_until(socket_, buf, "\0", use_awaitable);
    std::string resp((std::istreambuf_iterator<char>(&buf)), std::istreambuf_iterator<char>());
    co_return resp;
}

awaitable<void> ClientFrame::main_session() {
    tcp::resolver resolver(io_context_);
    auto endpoints = co_await resolver.async_resolve("79.136.138.121", "12345", use_awaitable);

    co_await async_connect(socket_, endpoints, use_awaitable);
}

ClientFrame::ClientFrame(wxWindow* parent,wxWindowID id) :io_context_(), socket_(io_context_),
work_guard_(std::make_unique<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>>(boost::asio::make_work_guard(io_context_)))
{
    // Запускаем io_context в отдельном потоке
    io_thread = std::thread([this]() {
        io_context_.run();
        });

    // Запускаем main_session, как в консоли
    co_spawn(io_context_, main_session(), [this](std::exception_ptr ep) {
        if (ep) {
            CallAfter([this]() {
                wxMessageBox(wxString::FromUTF8("Ошибка при подключении или передаче данных"));
                Close();
                });
        }
        });
    wxFont font(13, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);

    //(*Initialize(ClientFrame)
    Create(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE, _T("wxID_ANY"));
    SetClientSize(wxSize(1440,720));
    SetBackgroundColour(wxColour(32,32,32));
    SetHelpText(_("Password"));
    Panel1 = new wxPanel(this, ID_PANEL1, wxPoint(0,0), wxSize(1440,28), wxBORDER_NONE|wxCLIP_CHILDREN, _T("ID_PANEL1"));
    Panel1->SetForegroundColour(wxColour(32,32,32));
    Panel1->SetBackgroundColour(wxColour(35,35,35));
    minButton = new wxButton(Panel1, ID_BUTTON3, _T("_"), wxPoint(1360,0), wxSize(40,25), wxBORDER_NONE, wxDefaultValidator, _T("ID_BUTTON3"));
    minButton->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
    minButton->SetBackgroundColour(wxColour(45,45,45));
    CloseButton = new wxButton(Panel1, ID_BUTTON2, _("X"), wxPoint(1400,0), wxSize(40,25), wxBORDER_NONE, wxDefaultValidator, _T("ID_BUTTON2"));
    CloseButton->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
    CloseButton->SetBackgroundColour(wxColour(45,45,45));
    LabelMain = new wxStaticText(Panel1, ID_STATICTEXT1, _("Chepgram"), wxPoint(1,5), wxSize(70,25), wxALIGN_CENTRE, _T("ID_STATICTEXT1"));
    LabelMain->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
    LabelMain->SetFont(font);
    AuthText = new wxStaticText(this, ID_STATICTEXT2, _("Авторизация"), wxPoint(0,160), wxSize(1440,16), wxALIGN_CENTRE, _T("ID_STATICTEXT2"));
    AuthText->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
    wxFont authfont(32, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    AuthText->SetFont(authfont);
    PhoneNumText = new wxStaticText(this, ID_STATICTEXT3, _("Введите номер телефона:"), wxPoint(472,280), wxSize(512,32), wxALIGN_CENTRE, _T("ID_STATICTEXT3"));
    PhoneNumText->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
    wxFont phonefont(24, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    PhoneNumText->SetFont(phonefont);
    PhoneCtrl = new wxTextCtrl(this, ID_TEXTCTRL1, wxEmptyString, wxPoint(584,352), wxSize(264,48), wxTE_CENTRE|wxBORDER_NONE, wxDefaultValidator, _T("ID_TEXTCTRL1"));
    PhoneCtrl->SetMaxLength(12);
    PhoneCtrl->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
    PhoneCtrl->SetBackgroundColour(wxColour(40,40,40));
    wxFont PhoneCtrlFont(28,wxFONTFAMILY_DEFAULT,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL,false,_T("Roboto"),wxFONTENCODING_DEFAULT);
    PhoneCtrl->SetFont(PhoneCtrlFont);
    PhoneCtrl->SetHelpText(_T("+79000000000"));
    AuthBut = new wxButton(this, ID_BUTTON1, _T("->"), wxPoint(856,360), wxSize(40,32), wxBORDER_NONE, wxDefaultValidator, _T("ID_BUTTON1"));
    AuthBut->Disable();
    AuthBut->SetForegroundColour(wxColour(255,255,255));
    AuthBut->SetBackgroundColour(wxColour(0,64,128));
    PswdBut = new wxButton(this, ID_BUTTON4, _T("->"), wxPoint(856,360), wxSize(40,32), wxBORDER_NONE, wxDefaultValidator, _T("ID_BUTTON4"));
    PswdBut->Disable();
    PswdBut->Hide();
    PswdBut->SetForegroundColour(wxColour(255,255,255));
    PswdBut->SetBackgroundColour(wxColour(0,64,128));
    PswdCtrl = new wxTextCtrl(this, ID_TEXTCTRL2, wxEmptyString, wxPoint(584,352), wxSize(264,48), wxTE_PASSWORD|wxTE_LEFT|wxBORDER_NONE, wxDefaultValidator, _T("ID_TEXTCTRL2"));
    PswdCtrl->SetMaxLength(12);
    PswdCtrl->Hide();
    PswdCtrl->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
    PswdCtrl->SetBackgroundColour(wxColour(40,40,40));
    wxFont PswdCtrlFont(22,wxFONTFAMILY_DEFAULT,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL,false,_T("Roboto"),wxFONTENCODING_DEFAULT);
    PswdCtrl->SetFont(PswdCtrlFont);
    PswdCtrl->SetHelpText(_T("+79000000000"));
    NickCtrl = new wxTextCtrl(this, ID_TEXTCTRL3, wxEmptyString, wxPoint(584,352), wxSize(264,48), wxTE_LEFT|wxTE_CENTRE|wxBORDER_NONE, wxDefaultValidator, _T("ID_TEXTCTRL3"));
    NickCtrl->SetMaxLength(12);
    NickCtrl->Hide();
    NickCtrl->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
    NickCtrl->SetBackgroundColour(wxColour(40,40,40));
    wxFont NickCtrlFont(22,wxFONTFAMILY_DEFAULT,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL,false,_T("Roboto"),wxFONTENCODING_DEFAULT);
    NickCtrl->SetFont(NickCtrlFont);
    NickCtrl->SetHelpText(_T("+79000000000"));
    NickBut = new wxButton(this, ID_BUTTON5, _T("->"), wxPoint(856,360), wxSize(40,32), wxBORDER_NONE, wxDefaultValidator, _T("ID_BUTTON5"));
    NickBut->Disable();
    NickBut->Hide();
    NickBut->SetForegroundColour(wxColour(255,255,255));
    NickBut->SetBackgroundColour(wxColour(0,64,128));
    chatListCtrl = new wxListCtrl(this, ID_LISTCTRL1, wxPoint(8,32), wxSize(328,688), wxLC_REPORT|wxLC_NO_HEADER|wxLC_SINGLE_SEL|wxBORDER_NONE, wxDefaultValidator, _T("ID_LISTCTRL1"));
    chatListCtrl->Hide();
    chatListCtrl->SetForegroundColour(wxColour(255,255,255));
    chatListCtrl->SetBackgroundColour(wxColour(20,20,20));
    wxFont chatListCtrlFont(14,wxFONTFAMILY_DEFAULT,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL,false,_T("Roboto"),wxFONTENCODING_DEFAULT);
    chatListCtrl->SetFont(chatListCtrlFont);
    Panel2 = new wxPanel(this, ID_PANEL2, wxPoint(344,32), wxSize(1088,680), wxTAB_TRAVERSAL, _T("ID_PANEL2"));
    Panel2->Hide();
    Panel2->SetBackgroundColour(wxColour(28,28,28));
    messageArea = new wxRichTextCtrl(Panel2, ID_RICHTEXTCTRL1, _("Text"), wxPoint(1,0), wxSize(1088,648), wxRE_MULTILINE|wxBORDER_NONE, wxDefaultValidator, _T("ID_RICHTEXTCTRL1"));
    wxRichTextAttr rchtxtAttr_1;
    rchtxtAttr_1.SetBulletStyle(wxTEXT_ATTR_BULLET_STYLE_ALIGN_LEFT);
    rchtxtAttr_1.SetBackgroundColour(wxColour(20,20,20));
    messageArea->SetBasicStyle(rchtxtAttr_1);
    messageArea->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));
    messageArea->SetBackgroundColour(wxColour(20,20,20));
    Panel3 = new wxPanel(Panel2, ID_PANEL3, wxPoint(0,656), wxSize(1088,20), wxTAB_TRAVERSAL, _T("ID_PANEL3"));
    Panel3->SetBackgroundColour(wxColour(20,20,20));
    sendButton = new wxButton(Panel3, ID_BUTTON6, _T("->"), wxPoint(1056,0), wxSize(35,20), wxBORDER_NONE, wxDefaultValidator, _T("ID_BUTTON6"));
    sendButton->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));
    sendButton->SetBackgroundColour(wxColour(0,64,128));
    sendButton->Raise();
    inputField = new wxTextCtrl(Panel3, ID_TEXTCTRL4, wxEmptyString, wxDefaultPosition, wxSize(1048,23), wxTE_PROCESS_ENTER|wxBORDER_NONE, wxDefaultValidator, _T("ID_TEXTCTRL4"));
    inputField->SetMaxLength(5000);
    inputField->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));
    inputField->SetBackgroundColour(wxColour(20,20,20));
    Center();

    Connect(ID_BUTTON3, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&ClientFrame::OnminButtonClick);
    Connect(ID_BUTTON2, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&ClientFrame::OnCloseButtonClick);
    Panel1->Connect(wxEVT_LEFT_DOWN, (wxObjectEventFunction)&ClientFrame::OnPanel1LeftDown, NULL, this);
    Panel1->Connect(wxEVT_LEFT_UP, (wxObjectEventFunction)&ClientFrame::OnPanel1LeftUp, NULL, this);
    Panel1->Connect(wxEVT_MOTION, (wxObjectEventFunction)&ClientFrame::OnPanel1MouseMove, NULL, this);
    Connect(ID_TEXTCTRL1, wxEVT_COMMAND_TEXT_UPDATED, (wxObjectEventFunction)&ClientFrame::OnPhoneCtrlText);
    Connect(ID_BUTTON1, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&ClientFrame::OnNextButClick);
    Connect(ID_BUTTON4, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&ClientFrame::OnPswdButClick);
    Connect(ID_TEXTCTRL2, wxEVT_COMMAND_TEXT_UPDATED, (wxObjectEventFunction)&ClientFrame::OnPswdCtrlText);
    Connect(ID_TEXTCTRL3, wxEVT_COMMAND_TEXT_UPDATED, (wxObjectEventFunction)&ClientFrame::OnNickCtrlText);
    Connect(ID_BUTTON5, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&ClientFrame::OnNickButClick);
    Connect(ID_LISTCTRL1, wxEVT_COMMAND_LIST_BEGIN_DRAG, (wxObjectEventFunction)&ClientFrame::OnchatListCtrlBeginDrag);
    Connect(ID_BUTTON6, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&ClientFrame::OnsendButtonClick);
    Connect(wxID_ANY, wxEVT_ACTIVATE, (wxObjectEventFunction)&ClientFrame::OnActivate);
    //*)
    minButton->Bind(wxEVT_ENTER_WINDOW, &ClientFrame::OnButtonHoverEnter, this);
    minButton->Bind(wxEVT_LEAVE_WINDOW, &ClientFrame::OnButtonHoverLeave, this);
    CloseButton->Bind(wxEVT_ENTER_WINDOW, &ClientFrame::OnButtonHoverEnter, this);
    CloseButton->Bind(wxEVT_LEAVE_WINDOW, &ClientFrame::OnButtonHoverLeave, this);
    AuthBut->Bind(wxEVT_ENTER_WINDOW, &ClientFrame::RegButtonHoverEnter, this);
    AuthBut->Bind(wxEVT_LEAVE_WINDOW, &ClientFrame::RegButtonHoverLeave, this);
    PswdBut->Bind(wxEVT_ENTER_WINDOW, &ClientFrame::RegButtonHoverEnter, this);
    PswdBut->Bind(wxEVT_LEAVE_WINDOW, &ClientFrame::RegButtonHoverLeave, this);
    NickBut->Bind(wxEVT_ENTER_WINDOW, &ClientFrame::RegButtonHoverEnter, this);
    NickBut->Bind(wxEVT_LEAVE_WINDOW, &ClientFrame::RegButtonHoverLeave, this);
    AuthText->SetLabelText(wxString::FromUTF8("Авторизация"));
    PhoneNumText->SetLabelText(wxString::FromUTF8("Введите номер телефона:"));
}

ClientFrame::~ClientFrame()
{
    work_guard_->reset();
    io_context_.stop();
    if (io_thread.joinable())
        io_thread.join();
    //(*Destroy(ClientFrame)
    //*)
}

void ClientFrame::OnCloseButtonClick(wxCommandEvent& event)
{
    Close();
}

void ClientFrame::OnminButtonClick(wxCommandEvent& event)
{
    this->Iconize(true);
}

void ClientFrame::OnActivate(wxActivateEvent& event)
{
}

void ClientFrame::OnPanel1LeftDown(wxMouseEvent& event)
{
    if (event.GetEventObject() != Panel1) {
        event.Skip();
        return;
    }
    m_dragging = true;
    m_dragStartPos = Panel1->ClientToScreen(event.GetPosition());
    CaptureMouse();
}

void ClientFrame::OnPanel1LeftUp(wxMouseEvent& event)
{
    m_dragging = false;
    if (HasCapture()) ReleaseMouse();
}

void ClientFrame::OnPanel1MouseMove(wxMouseEvent& event)
{
    if (m_dragging) {
        wxPoint currentPos = Panel1->ClientToScreen(event.GetPosition());
        wxPoint diff = currentPos - m_dragStartPos;
        wxPoint newPos = GetPosition() + diff;
        Move(newPos);
        m_dragStartPos = currentPos;
    }
}

void ClientFrame::OnButtonHoverEnter(wxMouseEvent& event) {
    wxWindow* btn = dynamic_cast<wxWindow*>(event.GetEventObject());
    if (btn) {
        btn->SetBackgroundColour(wxColour(60, 60, 60)); // Цвет при наведении
        btn->Refresh();
    }
}

void ClientFrame::OnButtonHoverLeave(wxMouseEvent& event) {
    wxWindow* btn = dynamic_cast<wxWindow*>(event.GetEventObject());
    if (btn) {
        btn->SetBackgroundColour(wxColour(45, 45, 45)); // Цвет по умолчанию
        btn->Refresh();
    }
}

void ClientFrame::RegButtonHoverEnter(wxMouseEvent& event) {
    wxWindow* btn = dynamic_cast<wxWindow*>(event.GetEventObject());
    if (btn) {
        btn->SetBackgroundColour(wxColour(0, 98, 196)); // Цвет при наведении
        btn->Refresh();
    }
}

void ClientFrame::RegButtonHoverLeave(wxMouseEvent& event) {
    wxWindow* btn = dynamic_cast<wxWindow*>(event.GetEventObject());
    if (btn) {
        btn->SetBackgroundColour(wxColour(0, 66, 132)); // Цвет по умолчанию
        btn->Refresh();
    }
}

void ClientFrame::OnPhoneCtrlText(wxCommandEvent& event)
{
    std::string phone = PhoneCtrl->GetValue().ToStdString();
    int length = phone.size();  // Количество символов
    if (length == 12)
        AuthBut->Enable();
    else
        AuthBut->Disable(); // Если нужно отключать кнопку
}

awaitable<void> ClientFrame::PhoneEnter()
{
    std::string phone = PhoneCtrl->GetValue().ToStdString();
    std::string message = "LOGIN phone=" + phone;
    co_await send_message(message);
    std::string response = co_await read_response();
    CallAfter([this, response]() {
        if (response == "NEW") {
            AuthText->SetLabelText(wxString::FromUTF8("Создание аккаунта"));
        }
        else {
            AuthText->SetLabelText(wxString::FromUTF8("Вход в аккаунт"));
            userID = stoi(response);
        }
        PhoneNumText->SetLabelText(wxString::FromUTF8("Введите пароль:"));
        PhoneCtrl->Hide();
        PswdCtrl->Show();
        AuthBut->Hide();
        PswdBut->Show();
        });
}

void ClientFrame::OnNextButClick(wxCommandEvent& event)
{
    co_spawn(io_context_, PhoneEnter(), boost::asio::detached);
}

void ClientFrame::OnPswdCtrlText(wxCommandEvent& event)
{
    std::string pwd = PswdCtrl->GetValue().ToStdString();
    if (pwd.length() >= 8 && pwd.length() <= 31)
        PswdBut->Enable();
    else
        PswdBut->Disable();
}

awaitable<void> ClientFrame::Password()
{
    std::string password = PswdCtrl->GetValue().ToStdString();
    std::string labelStr = std::string(AuthText->GetLabel().ToUTF8());
    if(labelStr == "Вход в аккаунт")
    {
        co_await send_message(password);
        std::string response = co_await read_response();
        CallAfter([this, response]() {
            {
                if (response == "1Wrong")
                {
                    wxMessageBox(wxString::FromUTF8("Неверный пароль. Повторите попытку ввода"));
                    PswdCtrl->SetLabelText("");
                }
                else
                {
                    wxMessageBox(response);
                    AuthText->Hide();
                    PhoneNumText->Hide();
                    PswdBut->Hide();
                    PswdCtrl->Hide();
                    chatListCtrl->Show();
                    co_spawn(io_context_, LoadChats(), boost::asio::detached);
                }
            }
            });
    }
    else
    {
        co_await send_message(password);
        PhoneNumText->SetLabel(wxString::FromUTF8("Ваш никнейм:"));
        PswdBut->Hide();
        PswdCtrl->Hide();
        NickBut->Show();
        NickCtrl->Show();
    }
}

void ClientFrame::OnPswdButClick(wxCommandEvent& event)
{
    co_spawn(io_context_, Password(), boost::asio::detached);
}

void ClientFrame::OnNickCtrlText(wxCommandEvent& event)
{
    std::string pwd = NickCtrl->GetValue().ToStdString();
    if (pwd.length() >= 4 && pwd.length() <= 249)
        NickBut->Enable();
    else
        NickBut->Disable();
}

awaitable<void> ClientFrame::Nickname()
{
    std::string password = NickCtrl->GetValue().ToStdString();
    std::string labelStr = std::string(AuthText->GetLabel().ToUTF8());
    co_await send_message(password);
    std::string response = co_await read_response();
    CallAfter([this, response]() {
        if (response == "Used")
        {
            wxMessageBox(wxString::FromUTF8("Это имя пользователя уже занято. Введите новое"));
            PswdCtrl->SetLabelText("");
        }
        else
        {
            wxMessageBox(response);
            AuthText->Hide();
            PhoneNumText->Hide();
            NickBut->Hide();
            NickCtrl->Hide();
            chatListCtrl->Show();
        }
    });
}

void ClientFrame::OnNickButClick(wxCommandEvent& event)
{
    co_spawn(io_context_, Nickname(), boost::asio::detached);
}

awaitable<void> ClientFrame::LoadChats()
{
    // 1) отправляем запрос
    co_await send_message("CHAT ");

    // 2) получаем ответ
    std::string response = co_await read_response();
    if (response == "NO_CHATS") {
        co_return;
    }

    // 3) парсим строки
    std::istringstream stream(response);
    std::string line;
    struct ChatInfo { std::string name, last; };
    std::vector<ChatInfo> chats;
    while (std::getline(stream, line)) {
        ChatInfo info{};
        size_t id_pos = line.find("chat_id=");
        size_t nickname_pos = line.find("nickname=");
        size_t last_pos = line.find("last=");

        if (id_pos == std::string::npos || nickname_pos == std::string::npos || last_pos == std::string::npos)
            continue;

        int chat_id = std::stoi(line.substr(id_pos + 8, nickname_pos - id_pos - 9));
        chatIds_.push_back(chat_id);
        std::string nickname = line.substr(nickname_pos + 9, last_pos - nickname_pos - 10);
        info.name = nickname;
        std::string last_time = line.substr(last_pos + 5);
        info.last = last_time;

        chats.push_back(std::move(info));
    }

    // 4) обновляем GUI
    CallAfter([this, chats = std::move(chats)]() {
        chatListCtrl->InsertColumn(0, wxString::FromUTF8("Собеседник"), wxLIST_FORMAT_LEFT, 178);
        chatListCtrl->InsertColumn(1, wxString::FromUTF8("Последнее"), wxLIST_FORMAT_RIGHT, 150);
        chatListCtrl->DeleteAllItems();
        chatIds_.clear();

        long idx = 0;
        for (auto& ci : chats) {
            // Вставляем новую строку и заполняем колонки
            idx = chatListCtrl->InsertItem(idx, wxString::FromUTF8(ci.name));
            chatListCtrl->SetItem(idx, 1, wxString::FromUTF8(ci.last));
            ++idx;
        }
        chatListCtrl->Refresh();
        chatListCtrl->Update();
        });
}

void ClientFrame::OnsendButtonClick(wxCommandEvent& event)
{
}

void ClientFrame::OnchatListCtrlBeginDrag(wxListEvent& event)
{
}
