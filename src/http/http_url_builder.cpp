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
#include "http_url_builder.hpp"
#include "http_utilities.hpp"

// BOOST headers
#include <boost/lexical_cast.hpp>


namespace http
{

url_builder::url_builder() :
    protocol_(protocol_name),
    path_(strings::slash)
{
}

url_builder::url_builder(const http::url& url)
{
    initialize_from_url(url);
}

url_builder::url_builder(const std::string& url)
{
    initialize_from_url(http::url(url));
}

const std::string& url_builder::protocol() const
{
    return protocol_;
}

void url_builder::set_protocol(const std::string& protocol)
{
    protocol_ = protocol;
}

const std::string& url_builder::user_info() const
{
    return user_info_;
}

void url_builder::set_user_info(const std::string& user_info)
{
    user_info_ = user_info;
}

const std::string& url_builder::hostname() const
{
    return hostname_;
}

void url_builder::set_hostname(const std::string& hostname)
{
    hostname_ = hostname;
}

unsigned short url_builder::port() const
{
    if(port_.empty())
        return protocol_port;
    return std::atoi(port_.c_str());
}

void url_builder::set_port(unsigned short port)
{
    if(protocol_port != port)
        port_ = boost::lexical_cast<std::string>(port);
    else
        port_.clear();
}

const std::string& url_builder::path() const
{
    return path_;
}

void url_builder::set_path(const std::string& path)
{
    if(path.empty())
        path_ = strings::slash;
    else
        path_ = path;
}

const std::string& url_builder::query() const
{
    return query_;
}

void url_builder::set_query(const std::string& query)
{
    query_ = query;
}

const std::string& url_builder::fragment() const
{
    return fragment_;
}

void url_builder::set_fragment(const std::string& fragment)
{
    fragment_ = fragment;
}

const std::string url_builder::to_string(int components, bool escape) const
{
    std::ostringstream buffer;

    if(components & http::url::protocol_component)
    {
        if(!protocol_.empty())
            buffer << protocol_ << "://";
    }

    if(components & http::url::user_info_component)
    {
        if(!user_info_.empty())
            buffer << user_info_ << "@";
    }

    if(components & http::url::hostname_component)
    {
        if(!hostname_.empty())
            buffer << hostname_;
    }

    if(components & http::url::port_component)
    {
        if(!port_.empty())
            buffer << ":" << port_;
    }

    if(components & http::url::path_component)
    {
        if(!path_.empty())
        {
            if(!escape)
                buffer << path_;
            else
                buffer << http_util::escape(path_);
        }
    }

    if(components & http::url::query_component)
    {
        if(!query_.empty())
        {
            buffer << "?";

            if(!escape)
                buffer << query_;
            else
                buffer << http_util::escape(query_);
        }
    }

    if(components & http::url::fragment_component)
    {
        if(!fragment_.empty())
            buffer << "#" << fragment_;
    }

    return buffer.str();
}

const http::url url_builder::get_url(int components, bool escape) const
{
    return http::url(to_string(components, escape));
}

void url_builder::initialize_from_url(const http::url& url)
{
    set_protocol(url.protocol());
    set_user_info(url.user_info());
    set_hostname(url.hostname());
    set_port(url.port());
    set_path(url.path());
    set_query(url.query());
    fragment_ = url.fragment();
}

}   // namespace http
