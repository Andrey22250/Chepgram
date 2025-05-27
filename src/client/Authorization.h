#ifndef AUTHORIZATION_H
#define AUTHORIZATION_H

//(*Headers(Authorization)
#include <wx/frame.h>
#include <wx/sizer.h>
//*)

class Authorization: public wxFrame
{
    public:

        Authorization(wxWindow* parent,wxWindowID id=wxID_ANY);
        virtual ~Authorization();

        //(*Declarations(Authorization)
        //*)

    protected:

        //(*Identifiers(Authorization)
        //*)

    private:

        //(*Handlers(Authorization)
        //*)

        DECLARE_EVENT_TABLE()
};

#endif
