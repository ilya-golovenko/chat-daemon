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
#include "chat_session.hpp"
#include <config_common.hpp>
#include <string_utils.hpp>

// BOOST headers
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/make_shared.hpp>

// STL headers
#include <sstream>


namespace chat
{
namespace field
{

const char simple_field     = 0;
const char admin_field      = 1;
const char quoted_field     = 2;

static const char field_map[count] =
{
    quoted_field,   // nickname,
    admin_field,    // access
    admin_field,    // address
    admin_field,    // hostname
    simple_field,   // sex
    simple_field,   // auth
    quoted_field,   // skin
    admin_field,    // browser
    quoted_field,   // status
    admin_field,    // usernum
    quoted_field,   // away
    simple_field,   // say
    quoted_field,   // nick_css
    quoted_field,   // nick_html
    simple_field,   // time
    admin_field,    // ignores
    quoted_field    // room
};

bool is_admin(type field_type)
{
    return (field_map[field_type] == admin_field);
}

bool is_quoted(type field_type)
{
    return (field_map[field_type] == quoted_field);
}

}   // namespace field

session_ptr session::create(const std::string& id, const std::string& info)
{
    return boost::make_shared<session>(id, info);
}

const std::string& session::id() const
{
    return id_;
}

const std::string& session::nickname() const
{
    return get(field::nickname);
}

const std::string& session::skin() const
{
    return get(field::skin);
}

const std::string& session::room() const
{
    return (fields_[field::room].empty() ?
        default_room : fields_[field::room]);
}

std::size_t session::access() const
{
    return access_;
}

sex::type session::sex() const
{
    return sex_;
}

const std::string& session::get(field::type field_type) const
{
    return fields_[field_type];
}

void session::set(field::type field_type, const std::string& value)
{
    if(fields_[field_type] != value)
    {
        changed_ = true;
        fields_[field_type] = value;

        if(field_type == field::access)
            access_ = util::to_type<std::size_t>(value);
        else if(field_type == field::sex)
            sex_ = util::to_type<sex::type>(value);
    }
}

const std::string& session::to_string()
{
    if(changed_)
        rebuild_session_string();

    return session_;
}

session::session(const std::string& id, const std::string& info) :
    id_(id),
    changed_(true)
{
    std::vector<std::string> fields;
    fields.reserve(field::count);

    boost::split(fields, info, boost::is_any_of("\x02"));
    std::copy(fields.begin(), fields.end(), fields_.begin());

    access_ = util::to_type<std::size_t>(get(field::access));
    sex_ = util::to_type<sex::type>(get(field::sex));
}

void session::rebuild_session_string()
{
    std::ostringstream buffer;

    for(std::size_t index = 0; index < fields_.size(); ++index)
    {
        bool admin_field = field::is_admin(index);
        bool quoted_field = field::is_quoted(index);

        if(fields_[index].empty())
            quoted_field = true;

        if(index > 0)
            buffer << ',';

        if(quoted_field)
            buffer << '\"';
        else if(admin_field)
            buffer << "\"<\x07";

        buffer << fields_[index];

        if(quoted_field)
            buffer << '\"';
        else if(admin_field)
            buffer << "\x07>\"";
    }

    session_ = buffer.str();
    changed_ = false;
}

}   // namespace chat
