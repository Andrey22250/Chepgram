/***************************************************************
 * Name:      ClientApp.h
 * Purpose:   Defines Application Class
 * Author:    Andrey ()
 * Created:   2025-05-26
 * Copyright: Andrey ()
 * License:
 **************************************************************/

#ifndef CLIENTAPP_H
#define CLIENTAPP_H

#include <wx/app.h>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;
using namespace boost::asio;

class ClientApp : public wxApp
{
    public:

        virtual bool OnInit();
};

#endif // CLIENTAPP_H
