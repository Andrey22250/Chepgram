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
const wxWindowID ClientFrame::ID_BUTTON6 = wxNewId();
const wxWindowID ClientFrame::ID_TEXTCTRL4 = wxNewId();
const wxWindowID ClientFrame::ID_PANEL3 = wxNewId();
const wxWindowID ClientFrame::ID_STATICTEXT4 = wxNewId();
const wxWindowID ClientFrame::ID_BUTTON8 = wxNewId();
const wxWindowID ClientFrame::ID_BUTTON9 = wxNewId();
const wxWindowID ClientFrame::ID_PANEL2 = wxNewId();
const wxWindowID ClientFrame::ID_BUTTON7 = wxNewId();
//*)

BEGIN_EVENT_TABLE(ClientFrame,wxFrame)
    EVT_LEFT_DOWN(ClientFrame::OnPanel1LeftDown)
    EVT_LEFT_UP(ClientFrame::OnPanel1LeftUp)
    EVT_MOTION(ClientFrame::OnPanel1MouseMove)
END_EVENT_TABLE()

awaitable<void> ClientFrame::send_message(const std::string& message, tcp::socket& socket)
{
    co_await async_write(socket, boost::asio::buffer(message + "\0"), use_awaitable);
}

awaitable<std::string> ClientFrame::read_response(tcp::socket& socket) {
    streambuf buf;
    buf.prepare(50<<20);
    co_await async_read_until(socket, buf, "\0", use_awaitable);
    std::istream is(&buf);
    std::string result;
    std::getline(is, result, '\0');
    co_return result;
}

awaitable<void> ClientFrame::notify_session() {
    tcp::resolver resolver(io_context_);
    auto endpoints = co_await resolver.async_resolve("79.136.138.121", "12345", use_awaitable);

    co_await async_connect(notify_socket_, endpoints, use_awaitable);

    // Уведомляем сервер, что это notify-соединение
    co_await send_message("NOTIFY", notify_socket_);
    co_await send_message(std::to_string(userID), notify_socket_);
    while (true) {
        auto message = co_await read_response(notify_socket_);

        if (message.starts_with("UPDATE CHAT ")) {
            int chatId = std::stoi(message.substr(strlen("UPDATE CHAT ")));

            // Обновляем список чатов и текущее окно через GUI-поток
            CallAfter([this, chatId]() {
                co_spawn(io_context_, [this, chatId]() -> awaitable<void> {
                    co_await LoadChats();
                    if (chatId == activeChatId_) {
                        co_await GetMsg();  // тут GUI-обновления должны быть через CallAfter!
                    }
                    }, detached);
            });
        }
    }
}

awaitable<void> ClientFrame::main_session() {
    tcp::resolver resolver(io_context_);
    auto endpoints = co_await resolver.async_resolve("79.136.138.121", "12345", use_awaitable);

    co_await async_connect(socket_, endpoints, use_awaitable);

    co_await send_message("MAIN", socket_);
}

ClientFrame::ClientFrame(wxWindow* parent,wxWindowID id) :io_context_(), socket_(io_context_), notify_socket_(io_context_),
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
    Create(parent, wxID_ANY, _("Chepgram"), wxDefaultPosition, wxDefaultSize, wxBORDER_NONE, _T("wxID_ANY"));
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
    PhoneCtrl->SetFocus();
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
    PswdCtrl->SetMaxLength(31);
    PswdCtrl->Hide();
    PswdCtrl->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
    PswdCtrl->SetBackgroundColour(wxColour(40,40,40));
    wxFont PswdCtrlFont(22,wxFONTFAMILY_DEFAULT,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL,false,_T("Roboto"),wxFONTENCODING_DEFAULT);
    PswdCtrl->SetFont(PswdCtrlFont);
    NickCtrl = new wxTextCtrl(this, ID_TEXTCTRL3, wxEmptyString, wxPoint(584,352), wxSize(264,48), wxTE_LEFT|wxTE_CENTRE|wxBORDER_NONE, wxDefaultValidator, _T("ID_TEXTCTRL3"));
    NickCtrl->SetMaxLength(35);
    NickCtrl->Hide();
    NickCtrl->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
    NickCtrl->SetBackgroundColour(wxColour(40,40,40));
    wxFont NickCtrlFont(22,wxFONTFAMILY_DEFAULT,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL,false,_T("Roboto"),wxFONTENCODING_DEFAULT);
    NickCtrl->SetFont(NickCtrlFont);
    NickBut = new wxButton(this, ID_BUTTON5, _T("->"), wxPoint(856,360), wxSize(40,32), wxBORDER_NONE, wxDefaultValidator, _T("ID_BUTTON5"));
    NickBut->Disable();
    NickBut->Hide();
    NickBut->SetForegroundColour(wxColour(255,255,255));
    NickBut->SetBackgroundColour(wxColour(0,64,128));
    chatListCtrl = new wxListCtrl(this, ID_LISTCTRL1, wxPoint(8,32), wxSize(328,648), wxLC_REPORT|wxLC_NO_HEADER|wxLC_SINGLE_SEL|wxBORDER_NONE, wxDefaultValidator, _T("ID_LISTCTRL1"));
    chatListCtrl->Hide();
    chatListCtrl->SetForegroundColour(wxColour(255,255,255));
    chatListCtrl->SetBackgroundColour(wxColour(20,20,20));
    wxFont chatListCtrlFont(14,wxFONTFAMILY_DEFAULT,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL,false,_T("Roboto"),wxFONTENCODING_DEFAULT);
    chatListCtrl->SetFont(chatListCtrlFont);
    Panel2 = new wxPanel(this, ID_PANEL2, wxPoint(344,32), wxSize(1088,680), wxTAB_TRAVERSAL, _T("ID_PANEL2"));
    Panel2->Hide();
    Panel2->SetBackgroundColour(wxColour(28,28,28));
    Panel3 = new wxPanel(Panel2, ID_PANEL3, wxPoint(0,656), wxSize(1088,20), wxTAB_TRAVERSAL, _T("ID_PANEL3"));
    Panel3->SetBackgroundColour(wxColour(20,20,20));
    sendButton = new wxButton(Panel3, ID_BUTTON6, _T("->"), wxPoint(1040,0), wxSize(35,20), wxBORDER_NONE, wxDefaultValidator, _T("ID_BUTTON6"));
    sendButton->Disable();
    sendButton->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));
    sendButton->SetBackgroundColour(wxColour(0,64,128));
    sendButton->Raise();
    inputField = new wxTextCtrl(Panel3, ID_TEXTCTRL4, wxEmptyString, wxDefaultPosition, wxSize(1032,23), wxTE_PROCESS_ENTER|wxBORDER_NONE, wxDefaultValidator, _T("ID_TEXTCTRL4"));
    inputField->SetMaxLength(4999);
    inputField->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));
    inputField->SetBackgroundColour(wxColour(20,20,20));
    wxFont inputFieldFont(13,wxFONTFAMILY_DEFAULT,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL,false,_T("Roboto"),wxFONTENCODING_DEFAULT);
    inputField->SetFont(inputFieldFont);
    NameUserLbl = new wxStaticText(Panel2, ID_STATICTEXT4, wxEmptyString, wxPoint(0,0), wxDefaultSize, wxBORDER_NONE, _T("ID_STATICTEXT4"));
    NameUserLbl->SetForegroundColour(wxColour(255,255,255));
    wxFont NameUserLblFont(14,wxFONTFAMILY_DEFAULT,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL,false,_T("Roboto Medium"),wxFONTENCODING_DEFAULT);
    NameUserLbl->SetFont(NameUserLblFont);
    AddToChat = new wxButton(Panel2, ID_BUTTON8, _T("+"), wxPoint(1032,0), wxSize(24,23), wxBORDER_NONE, wxDefaultValidator, _T("ID_BUTTON8"));
    AddToChat->Hide();
    AddToChat->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));
    AddToChat->SetBackgroundColour(wxColour(0,64,128));
    wxFont AddToChatFont(16,wxFONTFAMILY_DEFAULT,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,_T("Roboto"),wxFONTENCODING_DEFAULT);
    AddToChat->SetFont(AddToChatFont);
    AddToChat->SetToolTip(_T("12"));
    ListMembers = new wxButton(Panel2, ID_BUTTON9, _T("⋮"), wxPoint(1064,0), wxSize(24,23), wxBORDER_NONE, wxDefaultValidator, _T("ID_BUTTON9"));
    ListMembers->Hide();
    ListMembers->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));
    ListMembers->SetBackgroundColour(wxColour(0,64,128));
    wxFont ListMembersFont(16,wxFONTFAMILY_DEFAULT,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,_T("Roboto"),wxFONTENCODING_DEFAULT);
    ListMembers->SetFont(ListMembersFont);
    ListMembers->SetToolTip(_T("12"));
    AddChatBut = new wxButton(this, ID_BUTTON7, _T("+"), wxPoint(8,688), wxSize(24,23), wxBORDER_NONE, wxDefaultValidator, _T("ID_BUTTON7"));
    AddChatBut->Hide();
    AddChatBut->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));
    AddChatBut->SetBackgroundColour(wxColour(0,64,128));
    wxFont AddChatButFont(16,wxFONTFAMILY_DEFAULT,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,_T("Roboto"),wxFONTENCODING_DEFAULT);
    AddChatBut->SetFont(AddChatButFont);
    AddChatBut->SetToolTip(_T("12"));
    Center();

    Connect(ID_BUTTON3, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&ClientFrame::OnminButtonClick);
    Connect(ID_BUTTON2, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&ClientFrame::OnCloseButtonClick);
    Panel1->Connect(wxEVT_LEFT_DOWN, (wxObjectEventFunction)&ClientFrame::OnPanel1LeftDown, NULL, this);
    Panel1->Connect(wxEVT_LEFT_UP, (wxObjectEventFunction)&ClientFrame::OnPanel1LeftUp, NULL, this);
    Panel1->Connect(wxEVT_MOTION, (wxObjectEventFunction)&ClientFrame::OnPanel1MouseMove, NULL, this);
    Connect(ID_TEXTCTRL1, wxEVT_COMMAND_TEXT_UPDATED, (wxObjectEventFunction)&ClientFrame::OnPhoneCtrlText);
    Connect(ID_TEXTCTRL1, wxEVT_COMMAND_TEXT_ENTER, (wxObjectEventFunction)&ClientFrame::OnNextButClick);
    Connect(ID_BUTTON1, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&ClientFrame::OnNextButClick);
    Connect(ID_BUTTON4, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&ClientFrame::OnPswdButClick);
    Connect(ID_TEXTCTRL2, wxEVT_COMMAND_TEXT_UPDATED, (wxObjectEventFunction)&ClientFrame::OnPswdCtrlText);
    Connect(ID_TEXTCTRL2, wxEVT_COMMAND_TEXT_ENTER, (wxObjectEventFunction)&ClientFrame::OnPswdButClick);
    Connect(ID_TEXTCTRL3, wxEVT_COMMAND_TEXT_UPDATED, (wxObjectEventFunction)&ClientFrame::OnNickCtrlText);
    Connect(ID_TEXTCTRL3, wxEVT_COMMAND_TEXT_ENTER, (wxObjectEventFunction)&ClientFrame::OnNickButClick);
    Connect(ID_BUTTON5, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&ClientFrame::OnNickButClick);
    Connect(ID_LISTCTRL1, wxEVT_COMMAND_LIST_ITEM_SELECTED, (wxObjectEventFunction)&ClientFrame::OnchatListCtrlItemSelect);
    Connect(ID_BUTTON6, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&ClientFrame::OnsendButtonClick);
    Connect(ID_TEXTCTRL4, wxEVT_COMMAND_TEXT_UPDATED, (wxObjectEventFunction)&ClientFrame::OninputFieldText);
    Connect(ID_TEXTCTRL4, wxEVT_COMMAND_TEXT_ENTER, (wxObjectEventFunction)&ClientFrame::OnsendButtonClick);
    Connect(ID_BUTTON8, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&ClientFrame::OnAddToChatClick);
    Connect(ID_BUTTON9, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&ClientFrame::OnAddToChatClick);
    Connect(ID_BUTTON7, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&ClientFrame::OnAddChatButClick);
    Connect(wxID_ANY, wxEVT_ACTIVATE, (wxObjectEventFunction)&ClientFrame::OnActivate);
    //*)
    messagePanel_ = new MessagePanel(Panel2);
    messagePanel_->SetSize(0, 24, 1088, 624);
    AddChatBut->SetToolTip(wxString::FromUTF8("Новый чат"));
    minButton->Bind(wxEVT_ENTER_WINDOW, &ClientFrame::OnButtonHoverEnter, this);
    minButton->Bind(wxEVT_LEAVE_WINDOW, &ClientFrame::OnButtonHoverLeave, this);
    CloseButton->Bind(wxEVT_ENTER_WINDOW, &ClientFrame::OnButtonHoverEnter, this);
    CloseButton->Bind(wxEVT_LEAVE_WINDOW, &ClientFrame::OnButtonHoverLeave, this);
    AuthBut->Bind(wxEVT_ENTER_WINDOW, &ClientFrame::RegButtonHoverEnter, this);
    AuthBut->Bind(wxEVT_LEAVE_WINDOW, &ClientFrame::RegButtonHoverLeave, this);
    AddToChat->Bind(wxEVT_ENTER_WINDOW, &ClientFrame::RegButtonHoverEnter, this);
    AddToChat->Bind(wxEVT_LEAVE_WINDOW, &ClientFrame::RegButtonHoverLeave, this);
    PswdBut->Bind(wxEVT_ENTER_WINDOW, &ClientFrame::RegButtonHoverEnter, this);
    PswdBut->Bind(wxEVT_LEAVE_WINDOW, &ClientFrame::RegButtonHoverLeave, this);
    NickBut->Bind(wxEVT_ENTER_WINDOW, &ClientFrame::RegButtonHoverEnter, this);
    NickBut->Bind(wxEVT_LEAVE_WINDOW, &ClientFrame::RegButtonHoverLeave, this);
    AddChatBut->Bind(wxEVT_LEAVE_WINDOW, &ClientFrame::RegButtonHoverLeave, this);
    AddChatBut->Bind(wxEVT_ENTER_WINDOW, &ClientFrame::RegButtonHoverEnter, this);
    sendButton->Bind(wxEVT_LEAVE_WINDOW, &ClientFrame::RegButtonHoverLeave, this);
    sendButton->Bind(wxEVT_ENTER_WINDOW, &ClientFrame::RegButtonHoverEnter, this);
    AuthText->SetLabelText(wxString::FromUTF8("Авторизация"));
    ListMembers->SetLabelText(wxString::FromUTF8("⋮"));
    PhoneNumText->SetLabelText(wxString::FromUTF8("Введите номер телефона:"));
    chatListCtrl->InsertColumn(0, wxString::FromUTF8("Собеседник"), wxLIST_FORMAT_LEFT, 208);
    chatListCtrl->InsertColumn(1, wxString::FromUTF8("Последнее"), wxLIST_FORMAT_RIGHT, 120);
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
    // Проверка на символы
    if (phone[0] == '+') {
        // + допустим только в начале — проверим, что остальные символы цифры
        if (!std::all_of(phone.begin() + 1, phone.end(), ::isdigit)) {
            wxMessageBox(wxString::FromUTF8("Телефон должен содержать только цифры после знака '+'."));
            PhoneCtrl->SetLabelText(wxString::FromUTF8(""));
            co_return;
        }
    }
    else {
        wxMessageBox(wxString::FromUTF8("Телефон должен начинаться с +."));
        PhoneCtrl->SetLabelText(wxString::FromUTF8(""));
        co_return;
    }
    std::string message = "LOGIN phone=" + phone;
    co_await send_message(message, socket_);
    std::string response = co_await read_response(socket_);
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
        PswdCtrl->SetFocus();
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
    wxString password = PswdCtrl->GetValue().ToUTF8();
    std::string labelStr = std::string(AuthText->GetLabel().ToUTF8());
    co_await send_message(password.ToStdString(), socket_);
    if(labelStr == "Вход в аккаунт")
    {
        std::string response = co_await read_response(socket_);
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
                    AddChatBut->Show();
                    co_spawn(io_context_, LoadChats(), boost::asio::detached);
                    // Уведомления
                    co_spawn(io_context_, notify_session(), [this](std::exception_ptr ep) {
                        if (ep) {
                            CallAfter([this]() {
                                wxMessageBox(wxString::FromUTF8("Ошибка соединения (notify)"));
                                Close();
                                });
                        }
                        });
                }
            }
            });
    }
    else
    {
        PhoneNumText->SetLabel(wxString::FromUTF8("Ваш никнейм:"));
        PswdBut->Hide();
        PswdCtrl->Hide();
        NickBut->Show();
        NickCtrl->Show();
        NickCtrl->SetFocus();
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
    wxString password = NickCtrl->GetValue().ToUTF8();
    std::string labelStr = std::string(AuthText->GetLabel().ToUTF8());
    co_await send_message(password.ToStdString(), socket_);
    std::string response = co_await read_response(socket_);
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
            co_spawn(io_context_, LoadChats(), boost::asio::detached);
            // Уведомления
            CallAfter([this](){
                co_spawn(io_context_, notify_session(), [this](std::exception_ptr ep) {
                    if (ep) {
                        CallAfter([this]() {
                            wxMessageBox(wxString::FromUTF8("ошибка соединения (notify)"));
                            Close();
                            });
                    }
                    });
            });
            AddChatBut->Show();
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
    co_await send_message("CHAT ", socket_);

    // 2) получаем ответ
    std::string response = co_await read_response(socket_);
    if (response == "NO_CHATS") {
        chatListCtrl->SetItem(0, 1, wxString::FromUTF8("Начните общение"));
        co_return;
    }

    // 3) парсим строки
    std::istringstream stream(response);
    std::string line;
    struct ChatInfo { std::string name, last; bool is_group = false;};
    std::vector<ChatInfo> chats;
    std::vector<ChatIdInfo> newChatIds;
    while (std::getline(stream, line)) {
        ChatInfo info{};
        size_t id_pos = line.find("chat_id=");
        size_t nickname_pos = line.find("nickname=");
        size_t last_pos = line.find("last=");
        size_t is_group_pos = line.find("is_group=");

        if (id_pos == std::string::npos || nickname_pos == std::string::npos ||
            last_pos == std::string::npos || is_group_pos == std::string::npos)
            continue;

        std::string nickname = line.substr(nickname_pos + 9, last_pos - (nickname_pos + 9) - 1);
        info.name = nickname;
        std::string last = line.substr(last_pos + 5, is_group_pos - (last_pos + 5) - 1);
        info.last = last;
        std::string is_group_str = line.substr(is_group_pos + 9);
        info.is_group = (is_group_str == "1" || is_group_str == "true" || is_group_str == "t");
        ChatIdInfo chatInfo{};
        int chat_id = std::stoi(line.substr(id_pos + 8, nickname_pos - (id_pos + 8) - 1));
        chatInfo.id = chat_id;
        chatInfo.is_group = info.is_group;
        newChatIds.push_back(std::move(chatInfo));
        chats.push_back(std::move(info));
    }

    // 4) обновляем GUI
    CallAfter([this, chats = std::move(chats), newChatIds = std::move(newChatIds)]() {
        chatListCtrl->DeleteAllItems();

        long idx = 0;
        for (auto& ci : chats) {
            // Вставляем новую строку и заполняем колонки
            idx = chatListCtrl->InsertItem(idx, wxString::FromUTF8(ci.name));
            chatListCtrl->SetItem(idx, 1, wxString::FromUTF8(ci.last));
            ++idx;
        }
        chatIds_ = std::move(newChatIds);
        chatListCtrl->Refresh();
        chatListCtrl->Update();
        });
}

awaitable<void> ClientFrame::GetMsg()
{
    // Запрос на сервер
    co_await send_message("GET MESSAGES " + std::to_string(activeChatId_), socket_);
    std::string messages = co_await read_response(socket_);

    if (messages == "ERROR Не удалось получить сообщения") {
        CallAfter([this]() {
            messagePanel_->Freeze();
            messagePanel_->ClearMessages();
            messagePanel_->AddMessage(wxString::FromUTF8("From Chepgram"), wxString::FromUTF8("Now"), wxString::FromUTF8("Ошибка получения сообщений"), false);
            messagePanel_->Thaw();
            int w, h;
            messagePanel_->GetVirtualSize(&w, &h);
            messagePanel_->SetVirtualSize(GetClientSize().x, h);
            messagePanel_->Scroll(0, h);
            });
        co_return;
    }
    else if (messages == "Нет сообщений") {
        CallAfter([this]() {
            messagePanel_->Freeze();
            messagePanel_->ClearMessages();
            messagePanel_->AddMessage(wxString::FromUTF8("From Chepgram"), wxString::FromUTF8("Now"), wxString::FromUTF8("Начните общение!"), false);
            messagePanel_->Thaw();
            int w, h;
            messagePanel_->GetVirtualSize(&w, &h);
            messagePanel_->SetVirtualSize(GetClientSize().x, h);
            messagePanel_->Scroll(0, h);
            });
        co_return;
    }
    else
    {
        CallAfter([this, messages]() {
            messagePanel_->Freeze();
            messagePanel_->ClearMessages();

            std::istringstream stream(messages);
            std::string line;

            while (std::getline(stream, line)) {
                // Ожидаем формат: <senderId>|<nickname>[HH:MM] сообщение
                auto sep1 = line.find('|');
                if (sep1 == std::string::npos)
                {
                    continue;
                }
                int senderId = std::stoi(line.substr(0, sep1));
                std::string rest = line.substr(sep1 + 1);

                auto t1 = rest.find('[');
                auto t2 = rest.find(']');
                if (t1 == std::string::npos || t2 == std::string::npos) continue;

                std::string nickname = rest.substr(0, t1);
                std::string timestamp = rest.substr(t1 + 1, t2 - t1 - 1);
                std::string body = rest.substr(t2 + 2); // skip "] "

                bool isMine = (senderId == userID);

                messagePanel_->AddMessage(
                    wxString::FromUTF8(nickname),
                    wxString::FromUTF8(timestamp),
                    wxString::FromUTF8(body),
                    isMine);
            }
            messagePanel_->Thaw();

            // Принудительно обновляем виртуальный размер (особенно если не было сообщений)
            int w, h;
            messagePanel_->GetVirtualSize(&w, &h);
            messagePanel_->SetVirtualSize(GetClientSize().x, h);

            // Сбрасываем скролл в начало (или в конец, как хочешь)
            messagePanel_->Scroll(0, h);  // или Scroll(0, h);
            });
    }

    co_return;
}

void ClientFrame::OnchatListCtrlItemSelect(wxListEvent& event)
{
    int itemIndex = event.GetIndex();
    std::string nickname = std::string(chatListCtrl->GetItemText(itemIndex).ToUTF8());
    if (itemIndex >= 0 && itemIndex < chatIds_.size())
    {
        activeChatId_ = chatIds_[itemIndex].id; // сохраняем активный чат
        if (chatIds_[itemIndex].is_group)
        {
            AddToChat->Show();
            ListMembers->Show();
        }
        else
        {
            AddToChat->Hide();
            ListMembers->Hide();
        }
        CallAfter([this, nickname]() {
            Panel2->Show();
            NameUserLbl->SetLabelText(wxString::FromUTF8(nickname));
            inputField->SetLabelText("");

            // Дать GUI время обновиться
            co_spawn(io_context_, GetMsg(), detached);
        });
    }
}

awaitable<void> ClientFrame::AddNewChat()
{
    wxArrayString choices;
    choices.Add(wxString::FromUTF8("Личный чат"));
    choices.Add(wxString::FromUTF8("Групповой чат"));

    wxSingleChoiceDialog dlg(this, wxString::FromUTF8("Выберите тип чата:"), wxString::FromUTF8("Создание чата"), choices);
    if (dlg.ShowModal() == wxID_OK) {
        wxString choice = dlg.GetStringSelection();
        if (choice == wxString::FromUTF8("Личный чат")) {
            wxTextEntryDialog dialog(this, wxString::FromUTF8("Введите никнейм пользователя для нового чата:"), wxString::FromUTF8("Новый чат"));
            if (dialog.ShowModal() == wxID_OK)
            {
                wxString nickname = dialog.GetValue().ToUTF8();
                if (!nickname.IsEmpty() && nickname.size() < 50)
                {
                    co_await send_message("CREATE CHAT nickname=" + std::string(nickname), socket_);
                }
                else
                {
                    wxMessageBox(wxString::FromUTF8("Неверный никнейм."));
                    co_return;
                }
                std::string message = co_await read_response(socket_);
                if (message == "ERROR: Ошибка при поиске существующего чата" || message == "ERROR: Не удалось вставить текущего пользователя")
                    wxMessageBox(wxString::FromUTF8("Ошибка сервера"));
                else
                    wxMessageBox(wxString::FromUTF8(message));
            }
        }
        else if (choice == wxString::FromUTF8("Групповой чат")) {
            wxTextEntryDialog dialog(this, wxString::FromUTF8("Введите название группового чата:"), wxString::FromUTF8("Новый чат"));
            if (dialog.ShowModal() == wxID_OK)
            {
                wxString name = dialog.GetValue().ToUTF8();
                if(!name.IsEmpty() && name.size() < 100)
                    co_await send_message("CREATE GROUP CHAT name=" + std::string(name), socket_);
                else
                {
                    wxMessageBox(wxString::FromUTF8("Неверное название чата."));
                    co_return;
                }
            }
            wxString message = co_await read_response(socket_);
            if (message == "ERROR: Не удалось создать чат")
                wxMessageBox(wxString::FromUTF8("Ошибка сервера"));
            else
                wxMessageBox(message);
        }
    }
    co_spawn(io_context_, LoadChats(), detached);
}

void ClientFrame::OnAddChatButClick(wxCommandEvent& event)
{
    co_spawn(io_context_, AddNewChat(), boost::asio::detached);
}

void ClientFrame::OninputFieldText(wxCommandEvent& event)
{
    std::string text = inputField->GetValue().ToStdString();
    int length = text.size();  // Количество символов
    if (length > 0)
        sendButton->Enable();
    else
        sendButton->Disable(); // Если нужно отключать кнопку
}

awaitable<void> ClientFrame::SendMsg()
{
    wxString message = inputField->GetValue().ToUTF8();
    co_await send_message("SEND_MESSAGE " + std::to_string(activeChatId_) + "\n" + message.ToStdString(), socket_);
    std::string response = co_await read_response(socket_);
    inputField->SetLabelText("");
    CallAfter([this]() {
        co_spawn(io_context_, [this]() -> awaitable<void> {
            co_await LoadChats();
            co_await GetMsg();  // тут GUI-обновления должны быть через CallAfter!
            }, detached);
        CallAfter([this]() {
            int w, h;
            messagePanel_->GetVirtualSize(&w, &h);
            messagePanel_->Scroll(0, h); // прокрутка вниз
            });
        });
}

void ClientFrame::OnsendButtonClick(wxCommandEvent& event)
{
    co_spawn(io_context_, SendMsg(), detached);
}

awaitable<void> ClientFrame::AddUserToGroupChat()
{
    wxTextEntryDialog dialog(this, wxString::FromUTF8("Введите никнейм пользователя для добавления в чат:"), wxString::FromUTF8("Добавить пользователя"));
    if (dialog.ShowModal() == wxID_OK)
    {
        wxString nickname = dialog.GetValue().ToUTF8();
        if (!nickname.IsEmpty() && nickname.size() < 50)
        {
            co_await send_message("ADD TO CHAT nickname=" + std::string(nickname) + "\nchat_id=" + std::to_string(activeChatId_), socket_);
        }
        else
        {
            wxMessageBox(wxString::FromUTF8("Неверный никнейм."));
            co_return;
        }
        wxString message = co_await read_response(socket_);
        if (message == "ERROR: Ошибка при поиске существующего чата" || message == "ERROR: Не удалось вставить текущего пользователя")
            wxMessageBox(wxString::FromUTF8("Ошибка сервера"));
        else if(message.starts_with("OK"))
        {
            wxMessageBox(message);
            CallAfter([this]() {
                co_spawn(io_context_, [this]() -> awaitable<void> {
                    co_await LoadChats();
                    co_await GetMsg();  // тут GUI-обновления должны быть через CallAfter!
                    }, detached);
            });
        }
        else
            wxMessageBox(message);
    }
}

void ClientFrame::OnAddToChatClick(wxCommandEvent& event)
{
    co_spawn(io_context_, AddUserToGroupChat(), detached);
}
