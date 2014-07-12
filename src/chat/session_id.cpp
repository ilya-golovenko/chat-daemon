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
#include <chat/session_id.hpp>

// STL headers
#include <iostream>
#include <utility>


namespace chat
{

session_id const& session_id::any()
{
  static session_id const id("*");
  return id;
}

session_id::session_id(std::string const& str) :
  str_(str)
{
}

std::string const& session_id::str() const
{
  return str_;
}

bool session_id::is_any() const
{
  return str_ == any().str_;
}

bool operator<(session_id const& lhs, session_id const& rhs)
{
  return lhs.str() < rhs.str();
}

bool operator==(session_id const& lhs, session_id const& rhs)
{
  return lhs.str() == rhs.str() || lhs.is_any() || rhs.is_any();
}

std::ostream& operator<<(std::ostream& os, session_id const& id)
{
  os << id.str();
  return os;
}

}   // namespace chat
