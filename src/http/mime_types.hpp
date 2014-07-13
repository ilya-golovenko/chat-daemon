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
#ifndef _http_mime_types_hpp
#define _http_mime_types_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// STL headers
#include <string>


namespace http
{
namespace mime_types
{

std::string const text_html                     = "text/html";
std::string const text_plain                    = "text/plain";
std::string const text_xml                      = "text/xml";
std::string const text_css                      = "text/css";

std::string const image_jpeg                    = "image/jpeg";
std::string const image_gif                     = "image/gif";
std::string const image_tiff                    = "image/tiff";
std::string const image_png                     = "image/png";

std::string const application_rtf               = "application/rtf";
std::string const application_pdf               = "application/pdf";
std::string const application_javascript        = "application/javascript";
std::string const application_octet_stream      = "application/octet-stream";
std::string const application_form_urlencoded   = "application/x-www-form-urlencoded";

std::string const& from_extension(std::string const& str);

}   // namespace mime_types
}   // namespace http

#endif  // _http_mime_types_hpp
