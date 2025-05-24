#include "MainFrame.h"

enum {
    ID_ChatList = 1,
    ID_SendButton = 2
};

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_LISTBOX(ID_ChatList, MainFrame::OnChatSelected)
EVT_BUTTON(ID_SendButton, MainFrame::OnSendMessage)
wxEND_EVENT_TABLE()

MainFrame::MainFrame(const wxString& title)
    : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(800, 600)) {

    wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);

    // Список чатов
    chatList = new wxListBox(this, ID_ChatList);
    mainSizer->Add(chatList, 1, wxEXPAND | wxALL, 5);

    // Правая часть
    wxBoxSizer* rightSizer = new wxBoxSizer(wxVERTICAL);
    messageArea = new wxRichTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_MULTILINE);
    inputField = new wxTextCtrl(this, wxID_ANY);
    sendButton = new wxButton(this, ID_SendButton, "Отправить");

    rightSizer->Add(messageArea, 1, wxEXPAND | wxALL, 5);
    rightSizer->Add(inputField, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);
    rightSizer->Add(sendButton, 0, wxALIGN_RIGHT | wxRIGHT | wxBOTTOM, 5);

    mainSizer->Add(rightSizer, 3, wxEXPAND);

    SetSizer(mainSizer);
    Centre();
}

void MainFrame::OnChatSelected(wxCommandEvent& event) {
    // Заглушка — здесь будет загрузка сообщений чата
    messageArea->Clear();
    messageArea->AppendText("Выбран чат: " + chatList->GetStringSelection());
}

void MainFrame::OnSendMessage(wxCommandEvent& event) {
    wxString message = inputField->GetValue();
    if (!message.IsEmpty()) {
        messageArea->AppendText("Вы: " + message + "\n");
        inputField->Clear();
        // TODO: отправка сообщения на сервер
    }
}