#pragma once
#include <wx/wx.h>
#include <wx/listbox.h>
#include <wx/richtext/richtextctrl.h>

class MainFrame : public wxFrame {
public:
    MainFrame(const wxString& title);

private:
    wxListBox* chatList;
    wxRichTextCtrl* messageArea;
    wxTextCtrl* inputField;
    wxButton* sendButton;

    void OnChatSelected(wxCommandEvent& event);
    void OnSendMessage(wxCommandEvent& event);

    wxDECLARE_EVENT_TABLE();
};