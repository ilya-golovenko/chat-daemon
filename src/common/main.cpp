//---------------------------------------------------------------------------
//
//    Copyright (C) 2008, 2009 Ilya Golovenko
//    This file is part of spdaemon.
//
//    spdaemon is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    spdaemon is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with spdaemon. If not, see <http://www.gnu.org/licenses/>.
//
//---------------------------------------------------------------------------

// Application headers
#include "platform.hpp"
#include <chat_daemon.hpp>
#include <log_common.hpp>
#include <log_manager.hpp>
#include <utilities.hpp>
#include <win_utils.hpp>

// BOOST headers
#include <boost/function.hpp>
#include <boost/bind.hpp>


void run_daemon(int argc, char* argv[])
{
    chat::daemon::instance().run(argc, argv);
}

void restart_daemon()
{
    chat::daemon::instance().restart();
}

void stop_daemon()
{
    chat::daemon::instance().stop();
}

#if defined(_WINDOWS)
BOOL WINAPI console_ctrl_handler(DWORD ctrl_type)
{
    LOGNOT("control handler invoked: type = %1%")
        << util::win32_ctrl_name(ctrl_type);

    switch(ctrl_type)
    {
        case CTRL_C_EVENT:
        case CTRL_BREAK_EVENT:
        case CTRL_CLOSE_EVENT:
            stop_daemon();
            break;

        default:
            return FALSE;
    }

    return TRUE;
}

void init_signal_handler()
{
    SetConsoleCtrlHandler(&console_ctrl_handler, TRUE);
}
#endif  // defined(_WINDOWS)

#if defined(_UNIX)
void signal_handler(int signal)
{
    LOGNOT("signal handler invoked: signal = %1%") << strsignal(signal);

    if(SIGHUP == signal)
        restart_daemon();
    else
        stop_daemon();
}

void init_signal_handler()
{
    // Register signals handler
    signal(SIGHUP, &signal_handler);
    signal(SIGINT, &signal_handler);
    signal(SIGTERM, &signal_handler);
    signal(SIGQUIT, &signal_handler);
}
#endif  // defined(_UNIX)

int main(int argc, char* argv[])
{
    try
    {
        init_signal_handler();
        run_daemon(argc, argv);
    }
    catch(std::exception& e)
    {
        LOGERR(e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
