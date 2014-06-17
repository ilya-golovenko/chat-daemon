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
#include "http_url.hpp"
#include "http_common.hpp"
#include "http_url_builder.hpp"

// BOOST headers
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>


namespace http
{

const url url::from_string(const char* string)
{
    http::url url;
    std::size_t length;

    // protocol

    if(std::strstr(string, "://"))
    {
        std::size_t length = std::strcspn(string, ":");
        url.protocol_.assign(string, string + length);
        boost::to_lower(url.protocol_);
        string += length + 3;
    }

    // user_info

    length = std::strcspn(string, "@:[/?#");

    if('@' == string[length])
    {
        url.user_info_.assign(string, string + length);
        string += length + 1;
    }
    else if(':' == string[length])
    {
        length += std::strcspn(string + length, "@/?#");

        if('@' == string[length])
        {
            url.user_info_.assign(string, string + length);
            string += length + 1;
        }
    }

    // hostname

    length = std::strcspn(string, ":/?#");
    url.hostname_.assign(string, string + length);
    string += length;

    // port

    if(':' == *string)
    {
        length = std::strcspn(++string, "/?#");

        if(0 == length)
            throw invalid_url_error(string);

        url.port_.assign(string, string + length);

        for(std::size_t i = 0; i < url.port_.size(); ++i)
        {
            if(!std::isdigit(url.port_[i]))
                throw invalid_url_error(string);
        }

        string += length;
    }

    // path

    if('/' == *string)
    {
        length = std::strcspn(string, "?#");
        url.path_.assign(string, string + length);
        string += length;
    }
    else
    {
        url.path_ = "/";
    }

    // query

    if('?' == *string)
    {
        length = std::strcspn(++string, "#");
        url.query_.assign(string, string + length);
        string += length;
    }

    // fragment

    if('#' == *string)
        url.fragment_.assign(++string);

    return url;
}

const url url::from_string(const std::string& string)
{
    return from_string(string.c_str());
}

url::url() :
    protocol_(protocol_name),
    path_(strings::slash)
{
}

url::url(const char* string)
{
    *this = from_string(string);
}

url::url(const std::string& string)
{
    *this = from_string(string);
}

url::url(const url& other) :
    protocol_(other.protocol_),
    user_info_(other.user_info_),
    hostname_(other.hostname_),
    port_(other.port_),
    path_(other.path_),
    query_(other.query_),
    fragment_(other.fragment_)
{
}

url& url::operator=(const url& other)
{
    if(&other != this)
    {
        protocol_ = other.protocol_;
        user_info_ = other.user_info_;
        hostname_ = other.hostname_;
        port_ = other.port_;
        path_ = other.path_;
        query_ = other.query_;
        fragment_ = other.fragment_;
    }

    return *this;
}

const std::string& url::protocol() const
{
    return protocol_;
}

const std::string& url::user_info() const
{
    return user_info_;
}

const std::string& url::hostname() const
{
    return hostname_;
}

unsigned short url::port() const
{
    if(port_.empty())
        return protocol_port;
    return boost::lexical_cast<unsigned short>(port_);
}

const std::string& url::path() const
{
    return path_;
}

const std::string& url::query() const
{
    return query_;
}

const std::string& url::fragment() const
{
    return fragment_;
}

const std::string url::host_info(bool escape) const
{
    return to_string(host_info_components, escape);
}

const std::string url::relative_url(bool escape) const
{
    return to_string(relative_url_components, escape);
}

const std::string url::to_string(int components, bool escape) const
{
    return http::url_builder(*this).to_string(components, escape);
}

bool operator==(const url& lhs, const url& rhs)
{
    return (lhs.protocol_ == rhs.protocol_
        && lhs.user_info_ == rhs.user_info_
        && lhs.hostname_ == rhs.hostname_
        && lhs.port_ == rhs.port_
        && lhs.path_ == rhs.path_
        && lhs.query_ == rhs.query_
        && lhs.fragment_ == rhs.fragment_);
}

bool operator<(const url& lhs, const url& rhs)
{
    if(lhs.protocol_ != rhs.protocol_)
        return (lhs.protocol_ < rhs.protocol_);

    if(lhs.user_info_ != rhs.user_info_)
        return (lhs.user_info_ < rhs.user_info_);

    if(lhs.hostname_ != rhs.hostname_)
        return (lhs.hostname_ < rhs.hostname_);

    if(lhs.port_ != rhs.port_)
        return (lhs.port_ < rhs.port_);

    if(lhs.path_ != rhs.path_)
        return (lhs.path_ < rhs.path_);

    if(lhs.query_ != rhs.query_)
        return (lhs.query_ < rhs.query_);

    return (lhs.fragment_ < rhs.fragment_);
}

}   // namespace http
