/***************************************************************
 * Name:      ClientMain.h
 * Purpose:   Defines Application Frame
 * Author:    Andrey ()
 * Created:   2025-05-26
 * Copyright: Andrey ()
 * License:
 **************************************************************/

#ifndef CLIENTMAIN_H
#define CLIENTMAIN_H

//(*Headers(ClientFrame)
#include <wx/button.h>
#include <wx/frame.h>
#include <wx/listctrl.h>
#include <wx/panel.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
//*)

#include <wx/textdlg.h>
#include <wx/timer.h>
#include <wx/msgdlg.h>
#include <iostream>
#include <string>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;
using namespace boost::asio;

class ClientFrame: public wxFrame
{
    public:
        ClientFrame(wxWindow* parent,wxWindowID id = -1);
        virtual ~ClientFrame();

    private:
        boost::asio::io_context io_context_;
        std::unique_ptr<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>> work_guard_;
        boost::asio::ip::tcp::socket socket_;
        std::thread io_thread;

        bool m_dragging = false;
        wxPoint m_dragStartPos;

        int userID;
        std::vector<int> chatIds_;
        int activeChatId_ = -1;
        //(*Handlers(ClientFrame)
        void OnCloseButtonClick(wxCommandEvent& event);
        void OnminButtonClick(wxCommandEvent& event);
        void OnActivate(wxActivateEvent& event);
        void OnPanel1LeftDown(wxMouseEvent& event);
        void OnPanel1LeftUp(wxMouseEvent& event);
        void OnPanel1MouseMove(wxMouseEvent& event);
        void OnButtonHoverLeave(wxMouseEvent& event);
        void OnButtonHoverEnter(wxMouseEvent& event);
        void RegButtonHoverLeave(wxMouseEvent& event);
        void RegButtonHoverEnter(wxMouseEvent& event);
        void OnPhoneCtrlText(wxCommandEvent& event);
        void OnNextButClick(wxCommandEvent& event);
        void OnPswdCtrlText(wxCommandEvent& event);
        void OnPswdButClick(wxCommandEvent& event);
        void OnNickButClick(wxCommandEvent& event);
        void OnNickCtrlText(wxCommandEvent& event);
        void OnsendButtonClick(wxCommandEvent& event);
        void OnchatListCtrlItemSelect(wxListEvent& event);
        void OnButton1Click(wxCommandEvent& event);
        void OninputFieldText(wxCommandEvent& event);
        //*)
        void OnTimer1Trigger(wxTimerEvent& event);
        awaitable<void> send_message(const std::string& message);
        awaitable<std::string> read_response();
        awaitable<void> main_session();
        awaitable<void> PhoneEnter();
        awaitable<void> Password();
        awaitable<void> Nickname();
        awaitable<void> LoadChats();
        awaitable<void> AddNewChat();
        awaitable<void> SendMsg();
        awaitable<void> GetMsg();

        //(*Identifiers(ClientFrame)
        static const wxWindowID ID_BUTTON3;
        static const wxWindowID ID_BUTTON2;
        static const wxWindowID ID_STATICTEXT1;
        static const wxWindowID ID_PANEL1;
        static const wxWindowID ID_STATICTEXT2;
        static const wxWindowID ID_STATICTEXT3;
        static const wxWindowID ID_TEXTCTRL1;
        static const wxWindowID ID_BUTTON1;
        static const wxWindowID ID_BUTTON4;
        static const wxWindowID ID_TEXTCTRL2;
        static const wxWindowID ID_TEXTCTRL3;
        static const wxWindowID ID_BUTTON5;
        static const wxWindowID ID_LISTCTRL1;
        static const wxWindowID ID_RICHTEXTCTRL1;
        static const wxWindowID ID_BUTTON6;
        static const wxWindowID ID_TEXTCTRL4;
        static const wxWindowID ID_PANEL3;
        static const wxWindowID ID_STATICTEXT4;
        static const wxWindowID ID_PANEL2;
        static const wxWindowID ID_BUTTON7;
        //*)
        static const wxWindowID ID_TEXTENTRYDIALOG1;
        static const wxWindowID ID_TIMER1;

        //(*Declarations(ClientFrame)
        wxButton* AddChatBut;
        wxButton* AuthBut;
        wxButton* CloseButton;
        wxButton* NickBut;
        wxButton* PswdBut;
        wxButton* minButton;
        wxButton* sendButton;
        wxListCtrl* chatListCtrl;
        wxPanel* Panel1;
        wxPanel* Panel2;
        wxPanel* Panel3;
        wxRichTextCtrl* messageArea;
        wxStaticText* AuthText;
        wxStaticText* LabelMain;
        wxStaticText* NameUserLbl;
        wxStaticText* PhoneNumText;
        wxTextCtrl* NickCtrl;
        wxTextCtrl* PhoneCtrl;
        wxTextCtrl* PswdCtrl;
        wxTextCtrl* inputField;
        //*)
        wxTextEntryDialog* TextEntryDialog1;
        wxTimer Timer1;

        DECLARE_EVENT_TABLE()
};

#endif // CLIENTMAIN_H
