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
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
//*)

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
        boost::asio::io_context io_context;
        std::unique_ptr<boost::asio::ip::tcp::socket> socket;
        std::thread io_thread;
        bool m_dragging = false;
        wxPoint m_dragStartPos;
        //(*Handlers(ClientFrame)
        void OnQuit(wxCommandEvent& event);
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
        //*)

        //(*Identifiers(ClientFrame)
        static const wxWindowID ID_BUTTON3;
        static const wxWindowID ID_BUTTON2;
        static const wxWindowID ID_STATICTEXT1;
        static const wxWindowID ID_PANEL1;
        static const wxWindowID ID_PANEL2;
        static const wxWindowID ID_STATICTEXT2;
        static const wxWindowID ID_STATICTEXT3;
        static const wxWindowID ID_TEXTCTRL1;
        static const wxWindowID ID_BUTTON1;
        static const wxWindowID ID_BUTTON4;
        static const wxWindowID ID_TEXTCTRL2;
        //*)

        //(*Declarations(ClientFrame)
        wxButton* AuthBut;
        wxButton* CloseButton;
        wxButton* PswdBut;
        wxButton* minButton;
        wxPanel* Panel1;
        wxPanel* Panel2;
        wxStaticText* AuthText;
        wxStaticText* LabelMain;
        wxStaticText* PhoneNumText;
        wxTextCtrl* PhoneCtrl;
        wxTextCtrl* PswdCtrl;
        //*)

        DECLARE_EVENT_TABLE()
};

#endif // CLIENTMAIN_H
