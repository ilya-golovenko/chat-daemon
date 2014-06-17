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
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// STL headers
#include <string>


namespace http
{
namespace mime_types
{

const std::string text_html                     = "text/html";
const std::string text_plain                    = "text/plain";
const std::string text_xml                      = "text/xml";
const std::string text_css                      = "text/css";

const std::string image_jpeg                    = "image/jpeg";
const std::string image_gif                     = "image/gif";
const std::string image_tiff                    = "image/tiff";
const std::string image_png                     = "image/png";

const std::string application_rtf               = "application/rtf";
const std::string application_pdf               = "application/pdf";
const std::string application_javascript        = "application/javascript";
const std::string application_octet_stream      = "application/octet-stream";
const std::string application_form_urlencoded   = "application/x-www-form-urlencoded";

const std::string& from_extension(const std::string& extension);

}   // namespace mime_types
}   // namespace http

#endif  // _http_mime_types_hpp
