//---------------------------------------------------------------------------
//
//    Copyright (C) 2009 Ilya Golovenko
//    This file is part of libsphttp.
//
//    libsphttp is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    libsphttp is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with libsphttp. If not, see <http://www.gnu.org/licenses/>.
//
//---------------------------------------------------------------------------

// Application headers
#include <http/url_builder.hpp>
#include <http/utilities.hpp>


namespace http
{

url_builder::url_builder() :
    protocol_(protocol_name),
    path_(strings::slash)
{
}

url_builder::url_builder(url const& url)
{
    initialize_from_url(url);
}

url_builder::url_builder(std::string const& str)
{
    initialize_from_url(url(str));
}

std::string const& url_builder::get_protocol() const
{
    return protocol_;
}

void url_builder::set_protocol(std::string const& protocol)
{
    protocol_ = protocol;
}

std::string const& url_builder::get_user_info() const
{
    return user_info_;
}

void url_builder::set_user_info(std::string const& user_info)
{
    user_info_ = user_info;
}

std::string const& url_builder::get_hostname() const
{
    return hostname_;
}

void url_builder::set_hostname(std::string const& hostname)
{
    hostname_ = hostname;
}

std::uint16_t url_builder::get_port() const
{
    return port_.empty() ? protocol_port : std::stoi(port_);
}

void url_builder::set_port(std::uint16_t port)
{
    port_ = protocol_port == port ? std::string() : std::to_string(port);
}

std::string const& url_builder::get_path() const
{
    return path_;
}

void url_builder::set_path(std::string const& path)
{
    path_ = path.empty() ? std::string("/") : path;
}

std::string const& url_builder::get_query() const
{
    return query_;
}

void url_builder::set_query(std::string const& query)
{
    query_ = query;
}

std::string const& url_builder::get_fragment() const
{
    return fragment_;
}

void url_builder::set_fragment(std::string const& fragment)
{
    fragment_ = fragment;
}

url url_builder::get_url(int components, bool escape) const
{
    return url(to_string(components, escape));
}

std::string url_builder::to_string(int components, bool escape) const
{
    std::ostringstream buffer;

    if(components & url::protocol_component)
    {
        if(!protocol_.empty())
        {
            buffer << protocol_ << "://";
        }
    }

    if(components & url::user_info_component)
    {
        if(!user_info_.empty())
        {
            buffer << user_info_ << "@";
        }
    }

    if(components & url::hostname_component)
    {
        buffer << hostname_;
    }

    if(components & url::port_component)
    {
        if(!port_.empty())
        {
            buffer << ":" << port_;
        }
    }

    if(components & url::path_component)
    {
        buffer << (escape ? escape_string(path_) : path_);
    }

    if(components & url::query_component)
    {
        if(!query_.empty())
        {
            buffer << "?" << (escape ? escape_string(query_) : query_);
        }
    }

    if(components & url::fragment_component)
    {
        if(!fragment_.empty())
        {
            buffer << "#" << fragment_;
        }
    }

    return buffer.str();
}

void url_builder::initialize_from_url(url const& url)
{
    set_protocol(url.get_protocol());
    set_user_info(url.get_user_info());
    set_hostname(url.get_hostname());
    set_port(url.get_port());
    set_path(url.get_path());
    set_query(url.get_query());
    set_fragment(url.get_fragment());
}

}   // namespace http
