#pragma once
#include <wx/wx.h>
#include <wx/scrolwin.h>

class MessagePanel : public wxScrolledWindow {
public:
    MessagePanel(wxWindow* parent)
        : wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL | wxHSCROLL)
    {
        SetBackgroundColour(wxColour(28, 28, 28));
        SetScrollRate(0, 10);
    }

    void AddMessage(const wxString& nickname, const wxString& timestamp, const wxString& body, bool isOwn)
    {
        wxPanel* bubble = CreateBubble(nickname, timestamp, body, isOwn);

        // Вычисляем размеры надписей
        wxStaticText* nickLbl = new wxStaticText(bubble, wxID_ANY, nickname);
        nickLbl->SetForegroundColour(wxColour(255, 255, 255));
        nickLbl->SetFont(wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

        wxStaticText* timeLbl = new wxStaticText(bubble, wxID_ANY, timestamp);
        timeLbl->SetForegroundColour(wxColour(200, 200, 200));
        timeLbl->SetFont(wxFont(7, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL));

        wxStaticText* bodyLbl = new wxStaticText(bubble, wxID_ANY, body);
        bodyLbl->Wrap(600);
        bodyLbl->SetForegroundColour(wxColour(255, 255, 255));
        bodyLbl->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

        // Получаем размеры
        wxSize nickSz = nickLbl->GetBestSize();
        wxSize timeSz = timeLbl->GetBestSize();
        wxSize bodySz = bodyLbl->GetBestSize();

        int bubbleW = std::max({ nickSz.x, timeSz.x, bodySz.x }) + 20;
        int bubbleH = nickSz.y + timeSz.y + bodySz.y + 20;

        // Позиция пузырька
        int x = isOwn ? (GetClientSize().x - bubbleW - 10) : 10;

        bubble->SetSize(wxSize(bubbleW, bubbleH));
        bubble->SetPosition(wxPoint(x, next_y_));

        // Располагаем надписи внутри пузырька
        nickLbl->SetPosition(wxPoint(10, 5));
        timeLbl->SetPosition(wxPoint(10, 5 + nickSz.y + 2));
        bodyLbl->SetPosition(wxPoint(10, 5 + nickSz.y + 2 + timeSz.y + 4));

        next_y_ += bubbleH + 10;

        SetVirtualSize(GetClientSize().x, next_y_ + 10);  // реальная накопленная высота
        SetScrollRate(0, 10);
        Refresh();
    }


    wxPanel* CreateBubble(const wxString& nickname, const wxString& timestamp, const wxString& body, bool isOwn)
    {
        wxPanel* bubble = new wxPanel(this, wxID_ANY);
        bubble->SetBackgroundColour(isOwn ? wxColour(0, 64, 128) : wxColour(64, 64, 64));
        bubble->SetWindowStyle(wxBORDER_NONE);
        return bubble;
    }

    void ClearMessages()
    {
        DestroyChildren();
        next_y_ = 10;
        // Обновляем виртуальный размер и возвращаемся наверх
        SetVirtualSize(GetClientSize().x, 100);
        Scroll(0, 0); // очень важно
        Refresh();
    }
private:
    int next_y_= 10;
};