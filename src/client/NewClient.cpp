#include <wx/wx.h>
#include "MainFrame.h"

class ChatApp : public wxApp {
public:
    virtual bool OnInit() {
        MainFrame* frame = new MainFrame("����������");
        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(ChatApp);