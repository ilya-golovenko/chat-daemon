//---------------------------------------------------------------------------
//
//    Copyright (C) 2008, 2009, 2014 Ilya Golovenko
//    This file is part of Chat.Daemon project
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
#include <app/detail/posix/daemon_helper.hpp>

#if defined(CHAT_PLATFORM_POSIX)

// Application headers
#include <misc/error_utils.hpp>

// STL headers
#include <stdexcept>

// POSIX headers
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>


namespace chat
{
namespace detail
{

bool posix_daemon_helper::daemonize()
{
    pid_t const pid = ::fork();

    if(pid < 0) // fork failed
        throw std::runtime_error(util::errno_to_string("fork", errno));

    if(pid > 0) // parent process
        return true;

    // Change file mode mask
    ::umask(0);

    // Set working directory
    ::chdir("/");

    // Create new session
    if(::setsid() < 0)
      throw std::runtime_error(util::errno_to_string("setsid", errno));

    // Close standard descriptors
    ::close(STDIN_FILENO);
    ::close(STDOUT_FILENO);
    ::close(STDERR_FILENO);

    return false;
}

}   // namespace detail
}   // namespace chat

#endif  // defined(CHAT_PLATFORM_POSIX)
