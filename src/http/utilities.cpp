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
#include <http/utilities.hpp>
#include <http/common.hpp>

// BOOST headers
#include <boost/algorithm/string.hpp>

// STL headers
#include <stdexcept>
#include <algorithm>


namespace http
{

static char const hex_chars[] = "0123456789ABCDEF";

static std::uint8_t const url_escape_table[] =
{
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1
};

std::uint8_t hex_to_int(std::uint8_t hex)
{
    hex = hex - '0';

    if(hex > 9)
    {
        hex = (hex + '0' - 1) | 0x20;
        hex = hex - 'a' + 11;
    }

    if(hex > 15)
        hex = 0xFF;

    return hex;
}

std::string escape_string(std::string const& input)
{
    std::ostringstream output;

    for(std::uint8_t const c : input)
    {
        if(c > 127 || url_escape_table[c])
        {
            output << '%' << hex_chars[c >> 4];
            output << hex_chars[c & 0x0F]; 
        }
        else
        {
            output << static_cast<char>(c);
        }
    }

    return output.str();
}

bool unescape_string(std::string const& input, std::string& output)
{
    std::ostringstream temp;

    for(std::size_t index = 0; index < input.size(); ++index)
    {
        if('%' == input[index])
        {
            if(index + 2 < input.size())
            {
                std::uint8_t high = hex_to_int(input[index + 1]);
                std::uint8_t low = hex_to_int(input[index + 2]);

                if(high != 0xFF && low != 0xFF)
                {
                    std::uint8_t result = (high << 4) | low;

                    if(result >= 32 && result != 127)
                    {
                        temp << static_cast<char>(result);

                        index += 2;
                        continue;
                    }
                }
            }

            return false;
        }
        else if('+' == input[index])
        {
            temp << ' ';
        }
        else
        {
            temp << input[index];
        }
    }

    output = temp.str();

    return true;
}

std::string combine_url_path(std::string const& path1, std::string const& path2)
{
    if(!boost::starts_with(path1, strings::slash))
        return combine_url_path(strings::slash + path1, path2);

    bool has_end_slash = boost::ends_with(path1, strings::slash);
    bool has_start_slash = boost::starts_with(path2, strings::slash);

    if(!(has_end_slash || has_start_slash))
        return path1 + strings::slash + path2;

    if(has_end_slash && has_start_slash)
        return path1 + path2.substr(1);

    return path1 + path2;
}

}   // namespace http
