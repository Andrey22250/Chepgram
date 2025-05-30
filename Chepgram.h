#include "src/db/Database.h"
#include <libpq-fe.h>
#include <iostream>
#include <boost/asio.hpp>
#ifdef _WIN32
#include <Windows.h>
#include <conio.h>
#endif

using boost::asio::ip::tcp;
using namespace boost::asio;

io_context io;