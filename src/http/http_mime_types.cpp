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
#include "http_mime_types.hpp"

// BOOST headers
#include <boost/algorithm/string.hpp>


namespace http
{
namespace mime_types
{

struct entry
{
    const std::string extension;
    const std::string& mime_type;
};

const entry table[] =
{
    { ".html", text_html },
    { ".htm", text_html },
    { ".txt", text_plain },
    { ".xml", text_xml },
    { ".css", text_css },

    { ".jpeg", image_jpeg },
    { ".jpg", image_jpeg },
    { ".jpe", image_jpeg },
    { ".gif", image_gif },
    { ".tiff", image_tiff },
    { ".tif", image_tiff },
    { ".png", image_png },

    { ".rtf", application_rtf },
    { ".pdf", application_pdf },
    { ".js", application_javascript },
    { ".exe", application_octet_stream }
};

enum { table_size = sizeof(table) / sizeof(*table) };

const std::string& from_extension(const std::string& extension)
{
	for(std::size_t index = 0; index < table_size; ++index)
	{
		if(boost::iequals(extension, table[index].extension))
			return table[index].mime_type;
	}

	return application_octet_stream;
}

}	// namespace mime_types
}	// namespace http
