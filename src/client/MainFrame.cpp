#include "MainFrame.h"

enum {
    ID_ChatList = 1,
    ID_SendButton = 2
};

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_LISTBOX(ID_ChatList, MainFrame::OnChatSelected)
EVT_BUTTON(ID_SendButton, MainFrame::OnSendMessage)
wxEND_EVENT_TABLE()

MainFrame::MainFrame(const wxString& title): wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(1440, 720)) {
    this->SetSizeHints(wxSize(570, 250), wxSize(1920, 1080));
    this->SetExtraStyle(wxFRAME_EX_METAL);
    this->SetBackgroundColour(wxColour(40, 40, 40));
    wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);

    // ������ �����
    chatList = new wxListBox(this, ID_ChatList);
    mainSizer->Add(chatList, 1, wxEXPAND | wxALL, 5);

    // ������ �����
    wxBoxSizer* rightSizer = new wxBoxSizer(wxVERTICAL);
    messageArea = new wxRichTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_MULTILINE);
    inputField = new wxTextCtrl(this, wxID_ANY);
    sendButton = new wxButton(this, ID_SendButton, "���������");

    rightSizer->Add(messageArea, 1, wxEXPAND | wxALL, 5);
    rightSizer->Add(inputField, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);
    rightSizer->Add(sendButton, 0, wxALIGN_RIGHT | wxRIGHT | wxBOTTOM, 5);

    mainSizer->Add(rightSizer, 3, wxEXPAND);

    m_button2 = new wxButton(this, wxID_ANY, _("X"), wxPoint(13905, 0), wxSize(25, 20), 0);
    m_button2->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));

    m_button3 = new wxButton(this, wxID_ANY, _("_"), wxPoint(1415, 0), wxSize(25, 20), 0);

    SetSizer(mainSizer);
    Centre();
}

void MainFrame::OnChatSelected(wxCommandEvent& event) {
    // �������� � ����� ����� �������� ��������� ����
    messageArea->Clear();
    messageArea->AppendText("������ ���: " + chatList->GetStringSelection());
}

void MainFrame::OnSendMessage(wxCommandEvent& event) {
    wxString message = inputField->GetValue();
    if (!message.IsEmpty()) {
        messageArea->AppendText("��: " + message + "\n");
        inputField->Clear();
        // TODO: �������� ��������� �� ������
    }
}