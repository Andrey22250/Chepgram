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

//(*IdInit(ClientFrame)
const wxWindowID ClientFrame::ID_BUTTON3 = wxNewId();
const wxWindowID ClientFrame::ID_BUTTON2 = wxNewId();
const wxWindowID ClientFrame::ID_STATICTEXT1 = wxNewId();
const wxWindowID ClientFrame::ID_PANEL1 = wxNewId();
const wxWindowID ClientFrame::ID_PANEL2 = wxNewId();
const wxWindowID ClientFrame::ID_STATICTEXT2 = wxNewId();
const wxWindowID ClientFrame::ID_STATICTEXT3 = wxNewId();
const wxWindowID ClientFrame::ID_TEXTCTRL1 = wxNewId();
const wxWindowID ClientFrame::ID_BUTTON1 = wxNewId();
const wxWindowID ClientFrame::ID_BUTTON4 = wxNewId();
const wxWindowID ClientFrame::ID_TEXTCTRL2 = wxNewId();
//*)

BEGIN_EVENT_TABLE(ClientFrame,wxFrame)
    EVT_LEFT_DOWN(ClientFrame::OnPanel1LeftDown)
    EVT_LEFT_UP(ClientFrame::OnPanel1LeftUp)
    EVT_MOTION(ClientFrame::OnPanel1MouseMove)
END_EVENT_TABLE()

awaitable<void> connect_to_server(std::unique_ptr<tcp::socket>& socket, const std::string& host, const std::string& port) {
    auto executor = co_await boost::asio::this_coro::executor;
    tcp::resolver resolver(executor);

    auto endpoints = co_await resolver.async_resolve(host, port, use_awaitable);

    socket = std::make_unique<tcp::socket>(executor);
    co_await async_connect(*socket, endpoints);

    co_return;
}

ClientFrame::ClientFrame(wxWindow* parent,wxWindowID id)
{
    co_spawn(io_context, connect_to_server(socket, "79.136.138.121", "12345"), [this](std::exception_ptr ep) {
            if (ep) {
                wxMessageBox("Не удалось подключиться к серверу. Попробуйте позднее.");
                Close();
            }
            else {
                wxMessageBox(wxString::FromUTF8("Подключение к серверу успешно!"));
                // можно продолжать с авторизацией или загрузкой чатов
            }
        });
    // Запуск io_context в фоне
    io_thread = std::thread([this]() {
        try {
            io_context.run();
        }
        catch (std::exception& e) {
            wxMessageBox(wxString::FromUTF8("Ошибка: ") + wxString(e.what()));
        }
        });
    wxFont font(13, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);

    //(*Initialize(ClientFrame)
    wxBoxSizer* FakeBoxSizer;

    Create(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE, _T("wxID_ANY"));
    SetClientSize(wxSize(1440,720));
    SetBackgroundColour(wxColour(32,32,32));
    SetHelpText(_("Password"));
    Panel1 = new wxPanel(this, ID_PANEL1, wxPoint(1,0), wxSize(1440,28), wxTAB_TRAVERSAL|wxCLIP_CHILDREN, _T("ID_PANEL1"));
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
    Panel2 = new wxPanel(this, ID_PANEL2, wxPoint(1500,488), wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL2"));
    FakeBoxSizer = new wxBoxSizer(wxHORIZONTAL);
    Panel2->SetSizer(FakeBoxSizer);
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
    wxFont PhoneCtrlFont(28,wxFONTFAMILY_DEFAULT,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL,false,_T("Roboto Condensed"),wxFONTENCODING_DEFAULT);
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
    PswdCtrl = new wxTextCtrl(this, ID_TEXTCTRL2, wxEmptyString, wxPoint(584,352), wxSize(264,48), wxTE_PASSWORD|wxTE_CENTRE|wxBORDER_NONE, wxDefaultValidator, _T("ID_TEXTCTRL2"));
    PswdCtrl->SetMaxLength(12);
    PswdCtrl->Hide();
    PswdCtrl->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
    PswdCtrl->SetBackgroundColour(wxColour(40,40,40));
    wxFont PswdCtrlFont(22,wxFONTFAMILY_DEFAULT,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL,false,_T("Roboto Condensed"),wxFONTENCODING_DEFAULT);
    PswdCtrl->SetFont(PswdCtrlFont);
    PswdCtrl->SetHelpText(_T("+79000000000"));
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
    AuthText->SetLabelText(wxString::FromUTF8("Авторизация"));
    PhoneNumText->SetLabelText(wxString::FromUTF8("Введите номер телефона:"));
}

ClientFrame::~ClientFrame()
{
    //(*Destroy(ClientFrame)
    //*)
    io_context.stop();
    if (io_thread.joinable()) io_thread.join();
}

void ClientFrame::OnQuit(wxCommandEvent& event)
{
    Close();
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
    m_dragging = true;
    m_dragStartPos = ClientToScreen(event.GetPosition());
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
        wxPoint currentPos = ClientToScreen(event.GetPosition());
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

void ClientFrame::OnNextButClick(wxCommandEvent& event)
{
    std::string phone = PhoneCtrl->GetValue().ToStdString();
    std::string message = "LOGIN phone=" + phone;
    send_message(*socket, message);
    std::string response = read_response(*socket);
    if (response == "NEW")
    {
        AuthText->SetLabelText(wxString::FromUTF8("Создание аккаунта"));
    }
    else
    {
        AuthText->SetLabelText(wxString::FromUTF8("Вход в аккаунт"));
    }
    PhoneNumText->SetLabelText(wxString::FromUTF8("Введите пароль:"));
    PhoneCtrl->Hide();
    PswdCtrl->Show();
    AuthBut->Hide();
    PswdBut->Show();
}

void ClientFrame::OnPswdCtrlText(wxCommandEvent& event)
{
    std::string phone = PswdCtrl->GetValue().ToStdString();
    int length = phone.size();  // Количество символов
    if (length >= 8 && length <=31)
        PswdBut->Enable();
    else
        PswdBut->Disable(); // Если нужно отключать кнопку
}

void ClientFrame::OnPswdButClick(wxCommandEvent& event)
{
    if(AuthText->GetLabel() == "Создание аккаунта")
        send_message(*socket, PswdCtrl->GetValue().ToStdString());
    else
    {
        send_message(*socket, PswdCtrl->GetValue().ToStdString());
        std::string message = read_response(*socket);
        if(message == "Неверный пароль")
        {
            wxMessageBox(wxString::FromUTF8("Неверный пароль. Повторите попытку ввода"));
            PswdCtrl->SetLabelText("");
        }
    }
}
