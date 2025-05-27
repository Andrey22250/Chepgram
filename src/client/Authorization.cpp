#include "Authorization.h"

//(*InternalHeaders(Authorization)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(Authorization)
//*)

BEGIN_EVENT_TABLE(Authorization,wxFrame)
    //(*EventTable(Authorization)
    //*)
END_EVENT_TABLE()

Authorization::Authorization(wxWindow* parent,wxWindowID id)
{
    //(*Initialize(Authorization)
    wxBoxSizer* BoxSizer1;

    Create(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("id"));
    SetClientSize(wxSize(1052,519));
    Move(wxPoint(-1,-1));
    BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
    SetSizer(BoxSizer1);
    Layout();
    //*)
}

Authorization::~Authorization()
{
    //(*Destroy(Authorization)
    //*)
}

