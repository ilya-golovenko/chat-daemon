//---------------------------------------------------------------------------
//
//    Copyright (C) 2008 - 2016 Ilya Golovenko
//    This file is part of Chat.Daemon project
//
//    spchatd is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    spchatd is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with spchatd. If not, see <http://www.gnu.org/licenses/>.
//
//---------------------------------------------------------------------------

// Application headers
#include <file/win_file_event.hpp>

#if defined(CHAT_PLATFORM_WINDOWS)

// Application headers
#include <misc/path_utils.hpp>
#include <misc/win_utils.hpp>

// MISSIO headers
#include <missio/logging/common.hpp>


namespace chat
{
namespace util
{

win_file_event::win_file_event(file_handle& file) :
    file_(file),
    change_handle_(nullptr),
    terminated_(false)
{
}

win_file_event::~win_file_event()
{
    shutdown();
}

void win_file_event::initialize()
{
    LOG_COMP_TRACE_FUNCTION(win_file_event);

    if(!is_valid_handle(change_handle_))
    {
        LOG_COMP_NOTICE(win_file_event, "initializing event notifications");

        terminated_ = false;

        std::string const path = util::path::remove_filespec(file_.filename());
        change_handle_ = ::FindFirstChangeNotificationA(path.c_str(), FALSE, FILE_NOTIFY_CHANGE_SIZE);

        if(INVALID_HANDLE_VALUE == change_handle_)
            throw std::runtime_error(util::win::error_to_string("FindFirstChangeNotification", ::GetLastError()));
    }
}

void win_file_event::shutdown()
{
    LOG_COMP_TRACE_FUNCTION(win_file_event);

    if(is_valid_handle(change_handle_))
    {
        LOG_COMP_NOTICE(win_file_event, "shutting down event notifications");

        terminated_ = true;

        ::FindCloseChangeNotification(change_handle_);
        change_handle_ = INVALID_HANDLE_VALUE;
    }
}

bool win_file_event::wait()
{
    LOG_COMP_TRACE_FUNCTION(win_file_event);

    if(WAIT_OBJECT_0 == ::WaitForSingleObject(change_handle_, INFINITE))
    {
        if(!terminated_)
        {
            if(!::FindNextChangeNotification(change_handle_))
                throw std::runtime_error(util::win::error_to_string("FindNextChangeNotification", GetLastError()));

            return true;
        }
    }

    return false;
}

bool win_file_event::is_valid_handle(HANDLE handle)
{
    return nullptr != handle && INVALID_HANDLE_VALUE != handle;
}

}   // namespace util
}   // namespace chat

#endif  // defined(CHAT_PLATFORM_WINDOWS)
