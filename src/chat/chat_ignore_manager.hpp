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
#ifndef _chat_ignore_manager_hpp
#define _chat_ignore_manager_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// STL headers
#include <string>

// BOOST headers
#include <boost/unordered_set.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>


namespace chat
{

class ignore_manager :
    private boost::noncopyable
{
public:
    void add_ignore(const std::string& session_id);
    void remove_ignore(const std::string& session_id);
    void clear_ignores();

    bool is_ignored(const std::string& session_id) const;
    std::size_t ignore_count() const;

private:
    void check_create_set();

private:
    typedef boost::unordered_set<std::string> ignore_set_type;
    typedef boost::scoped_ptr<ignore_set_type> ignore_set_pointer;

private:
    ignore_set_pointer ignores_;
};

}   // namespace chat

#endif  // _chat_ignore_manager_hpp
