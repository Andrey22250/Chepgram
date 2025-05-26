/***************************************************************
 * Name:      ClientApp.cpp
 * Purpose:   Code for Application Class
 * Author:    Andrey ()
 * Created:   2025-05-26
 * Copyright: Andrey ()
 * License:
 **************************************************************/

#include "ClientApp.h"

//(*AppHeaders
#include "ClientMain.h"
#include <wx/image.h>
//*)

IMPLEMENT_APP(ClientApp);

bool ClientApp::OnInit()
{
    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    if ( wxsOK )
    {
        ClientFrame* Frame = new ClientFrame(0);
        Frame->Show();
        SetTopWindow(Frame);
    }
    //*)
    return wxsOK;

}
