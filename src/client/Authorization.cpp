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
    Create(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("id"));
    //*)
}

Authorization::~Authorization()
{
    //(*Destroy(Authorization)
    //*)
}

