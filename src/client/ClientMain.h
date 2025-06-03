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
#include <wx/stattext.h>
#include <wx/textctrl.h>
//*)

#include <wx/textdlg.h>
#include <wx/msgdlg.h>
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include "messagePanel.h"

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
        tcp::socket notify_socket_;
        std::thread io_thread;

        bool m_dragging = false;
        wxPoint m_dragStartPos;

        int userID;
        struct ChatIdInfo {
            int id;
            bool is_group;
        };
        std::vector<ChatIdInfo> chatIds_;
        int activeChatId_ = -1;

        MessagePanel* messagePanel_;
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
        void OninputFieldText(wxCommandEvent& event);
        void OnAddToChatClick(wxCommandEvent& event);
        void OnAddChatButClick(wxCommandEvent& event);
        void OnListMembersClick(wxCommandEvent& event);
        //*)
        awaitable<void> notify_session();
        awaitable<void> send_message(const std::string& message, tcp::socket& socket);
        awaitable<std::string> read_response(tcp::socket& socket);
        awaitable<void> main_session();
        awaitable<void> PhoneEnter();
        awaitable<void> Password();
        awaitable<void> Nickname();
        awaitable<void> LoadChats();
        awaitable<void> AddNewChat();
        awaitable<void> SendMsg();
        awaitable<void> GetMsg();
        awaitable<void> AddUserToGroupChat();
        awaitable<void> GetListMembersOfChat();

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
        static const wxWindowID ID_BUTTON6;
        static const wxWindowID ID_TEXTCTRL4;
        static const wxWindowID ID_PANEL3;
        static const wxWindowID ID_STATICTEXT4;
        static const wxWindowID ID_BUTTON8;
        static const wxWindowID ID_BUTTON9;
        static const wxWindowID ID_PANEL2;
        static const wxWindowID ID_BUTTON7;
        //*)
        static const wxWindowID ID_TEXTENTRYDIALOG1;

        //(*Declarations(ClientFrame)
        wxButton* AddChatBut;
        wxButton* AddToChat;
        wxButton* AuthBut;
        wxButton* CloseButton;
        wxButton* ListMembers;
        wxButton* NickBut;
        wxButton* PswdBut;
        wxButton* minButton;
        wxButton* sendButton;
        wxListCtrl* chatListCtrl;
        wxPanel* Panel1;
        wxPanel* Panel2;
        wxPanel* Panel3;
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

        DECLARE_EVENT_TABLE()
};

#endif // CLIENTMAIN_H
