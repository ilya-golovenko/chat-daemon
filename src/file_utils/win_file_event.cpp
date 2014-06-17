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
#include "win_file_event.hpp"
#include <win_utils.hpp>
#include <path_utils.hpp>
#include <log_common.hpp>


namespace util
{

#if defined(_WINDOWS)

win_file_event::win_file_event(file& file) :
    file_(file),
    change_handle_(0),
    terminated_(false)
{
}

win_file_event::~win_file_event()
{
    shutdown();
}

void win_file_event::initialize()
{
    if(!is_valid_handle(change_handle_))
    {
        LOGNOT("initializing event notifications");

        terminated_ = false;

        const std::string path = util::path::remove_filespec(file_.filename());
        change_handle_ = FindFirstChangeNotification(path.c_str(), FALSE, FILE_NOTIFY_CHANGE_SIZE);

        if(INVALID_HANDLE_VALUE == change_handle_)
            throw std::runtime_error(win32_error_to_string("FindFirstChangeNotification", GetLastError()));
    }
}

void win_file_event::shutdown()
{
    if(is_valid_handle(change_handle_))
    {
        LOGNOT("shutting down event notifications");

        terminated_ = true;

        FindCloseChangeNotification(change_handle_);
        change_handle_ = INVALID_HANDLE_VALUE;
    }
}

bool win_file_event::wait()
{
    if(WAIT_OBJECT_0 == WaitForSingleObject(change_handle_, INFINITE))
    {
        if(!terminated_)
        {
            if(!FindNextChangeNotification(change_handle_))
                throw std::runtime_error(win32_error_to_string("FindNextChangeNotification", GetLastError()));

            return true;
        }
    }

    return false;
}

bool win_file_event::is_valid_handle(HANDLE handle)
{
    return (handle && handle != INVALID_HANDLE_VALUE);
}

#endif  // defined(_WINDOWS)

}   // namespace util
