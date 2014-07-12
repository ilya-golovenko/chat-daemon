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
#include <http/url.hpp>
#include <http/common.hpp>
#include <http/url_builder.hpp>

// STL headers
#include <stdexcept>


namespace http
{

url url::from_string(std::string const& str)
{
    url url;

    std::size_t length = 0u;
    char const* pstr = str.c_str();

    // protocol

    if(std::strstr(pstr, "://"))
    {
        length = std::strcspn(pstr, ":");
        url.protocol_.assign(pstr, length);
        pstr += length + 3;
    }

    // user_info

    length = std::strcspn(pstr, "@:[/?#");

    if('@' == pstr[length])
    {
        url.user_info_.assign(pstr, pstr + length);
        pstr += length + 1;
    }
    else if(':' == pstr[length])
    {
        length += std::strcspn(pstr + length, "@/?#");

        if('@' == pstr[length])
        {
            url.user_info_.assign(pstr, length);
            pstr += length + 1;
        }
    }

    // hostname

    length = std::strcspn(pstr, ":/?#");
    url.hostname_.assign(pstr, length);
    pstr += length;

    // port

    if(':' == *pstr)
    {
        length = std::strcspn(++pstr, "/?#");

        if(0 == length)
            throw std::runtime_error("cannot parse url: " + str);

        url.port_.assign(pstr, length);

        for(std::size_t i = 0; i < url.port_.size(); ++i)
        {
            if(!std::isdigit(url.port_[i]))
                throw std::runtime_error("cannot parse url: " + str);
        }

        pstr += length;
    }

    // path

    if('/' == *pstr)
    {
        length = std::strcspn(pstr, "?#");
        url.path_.assign(pstr, length);
        pstr += length;
    }
    else
    {
        url.path_ = "/";
    }

    // query

    if('?' == *pstr)
    {
        length = std::strcspn(++pstr, "#");
        url.query_.assign(pstr, length);
        pstr += length;
    }

    // fragment

    if('#' == *pstr)
        url.fragment_.assign(++pstr);

    return url;
}

url::url() :
    protocol_(protocol_name),
    path_(strings::slash)
{
}

url::url(std::string const& str)
{
    *this = from_string(str);
}

std::string const& url::get_protocol() const
{
    return protocol_;
}

std::string const& url::get_user_info() const
{
    return user_info_;
}

std::string const& url::get_hostname() const
{
    return hostname_;
}

std::uint16_t url::get_port() const
{
    return port_.empty() ? protocol_port : std::stoi(port_);
}

std::string const& url::get_path() const
{
    return path_;
}

std::string const& url::get_query() const
{
    return query_;
}

std::string const& url::get_fragment() const
{
    return fragment_;
}

std::string url::get_host_info(bool escape) const
{
    return to_string(host_info_components, escape);
}

std::string url::get_relative_url(bool escape) const
{
    return to_string(relative_url_components, escape);
}

std::string url::to_string(int components, bool escape) const
{
    return url_builder(*this).to_string(components, escape);
}

bool operator==(url const& lhs, url const& rhs)
{
    return lhs.protocol_ == rhs.protocol_ &&
           lhs.user_info_ == rhs.user_info_ &&
           lhs.hostname_ == rhs.hostname_ &&
           lhs.port_ == rhs.port_ &&
           lhs.path_ == rhs.path_ &&
           lhs.query_ == rhs.query_ &&
           lhs.fragment_ == rhs.fragment_;
}

bool operator<(url const& lhs, url const& rhs)
{
    if(lhs.protocol_ != rhs.protocol_)
        return lhs.protocol_ < rhs.protocol_;

    if(lhs.user_info_ != rhs.user_info_)
        return lhs.user_info_ < rhs.user_info_;

    if(lhs.hostname_ != rhs.hostname_)
        return lhs.hostname_ < rhs.hostname_;

    if(lhs.port_ != rhs.port_)
        return lhs.port_ < rhs.port_;

    if(lhs.path_ != rhs.path_)
        return lhs.path_ < rhs.path_;

    if(lhs.query_ != rhs.query_)
        return lhs.query_ < rhs.query_;

    return lhs.fragment_ < rhs.fragment_;
}

}   // namespace http
