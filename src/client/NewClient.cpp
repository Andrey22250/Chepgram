#include <wx/wx.h>
#include <boost/asio.hpp>
#include "MainFrame.h"

class ChatApp : public wxApp {
public:
    virtual bool OnInit() {
        MainFrame* frame = new MainFrame("Мессенджер");
        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(ChatApp);