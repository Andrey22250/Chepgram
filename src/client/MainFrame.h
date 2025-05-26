#pragma once
#include <wx/wx.h>
#include <wx/listbox.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/button.h>
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/frame.h>

class MainFrame : public wxFrame {
public:
    MainFrame(const wxString& title);

private:
    wxListBox* chatList;
    wxRichTextCtrl* messageArea;
    wxTextCtrl* inputField;
    wxButton* sendButton;
    wxButton* m_button2;
    wxButton* m_button3;

    void OnChatSelected(wxCommandEvent& event);
    void OnSendMessage(wxCommandEvent& event);

    wxDECLARE_EVENT_TABLE();
};