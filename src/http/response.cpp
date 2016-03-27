//---------------------------------------------------------------------------
//
//    Copyright (C) 2009 - 2016 Ilya Golovenko
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
#include <http/response.hpp>
#include <http/mime_types.hpp>
#include <http/common.hpp>

// MISSIO headers
#include <missio/format/format.hpp>


namespace http
{

static missio::format::string const format
{
    "<html>\r\n"
    "<head><title>{0}</title></head>\r\n"
    "<body>\r\n"
    "<center><h1>{0}</h1><center>\r\n"
    "<hr>\r\n"
    "<center>{1}</center>\r\n"
    "</body>\r\n"
    "</html>\r\n"
};

static std::string const padding
{
    "<!-- The padding to disable MSIE's friendly error page -->\r\n"
    "<!-- The padding to disable MSIE's friendly error page -->\r\n"
    "<!-- The padding to disable MSIE's friendly error page -->\r\n"
    "<!-- The padding to disable MSIE's friendly error page -->\r\n"
    "<!-- The padding to disable MSIE's friendly error page -->\r\n"
    "<!-- The padding to disable MSIE's friendly error page -->\r\n"
    "<!-- The padding to disable MSIE's friendly error page -->\r\n"
};

response::response() :
    status_(status::ok)
{
}

response::response(status const& status) :
    status_(status)
{
}

response::response(version const& version, status const& status) :
    message(version),
    status_(status)
{
}

status const& response::get_status() const
{
    return status_;
}

void response::set_status(status const& status)
{
    status_ = status;
}

void response::set_server(std::string const& server)
{
    set(header_names::server, server);
}

boost::optional<std::string> response::get_server() const
{
    return get(header_names::server);
}

void response::set_cookie(std::string const& cookie)
{
    set(header_names::set_cookie, cookie);
}

boost::optional<std::string> response::get_cookie() const
{
    return get(header_names::set_cookie);
}

void response::to_buffers(buffers& buffers) const
{
    buffers.push_back(asio::buffer(version_.to_string()));
    buffers.push_back(asio::buffer(strings::space));
    buffers.push_back(asio::buffer(status_.to_string()));
    buffers.push_back(asio::buffer(strings::crlf));

    message::to_buffers(buffers);
}

void response::dump(std::ostream& os) const
{
    os << version_;
    os << strings::space;
    os << status_;
    os << std::endl;

    message::dump(os);
}

response response::make_stock_response(status const& status, std::string const& server)
{
    std::string content;

    missio::format::print(content, format, status, server);

    response response(status);

    response.add_content(content);
    response.add_content(padding);

    response.set_connection(connection_tokens::close);
    response.set_content_type(mime::text_html);
    response.set_content_length();
    response.set_server(server);

    return response;
}

}   // namespace http
