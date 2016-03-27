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
#include <http/utilities.hpp>
#include <http/common.hpp>

// BOOST headers
#include <boost/algorithm/string.hpp>

// STL headers
#include <stdexcept>
#include <algorithm>
#include <iterator>


namespace http
{

static char const hex_chars[] =
{
    '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

static char const escape_table[] =
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

static int hex_to_int(int c)
{
    if(c >= '0' && c <= '9')
    {
        return c - '0';
    }

    if(c >= 'a' && c <= 'f')
    {
        return c - 'a' + 10;
    }

    if(c >= 'A' && c <= 'F')
    {
        return c - 'A' + 10;
    }

    return -1;
}

static bool should_escape_char(unsigned char c)
{
    return c < std::size(escape_table) && escape_table[c];
}

std::string escape_string(std::string const& str)
{
    std::string result;

    for(unsigned char const c : str)
    {
        if(should_escape_char(c))
        {
            result.push_back('%');
            result.push_back(hex_chars[c >> 4]);
            result.push_back(hex_chars[c & 0xF]); 
        }
        else
        {
            result.push_back(c);
        }
    }

    return result;
}

std::string unescape_string(std::string const& str)
{
    std::string result;

    for(std::size_t i = 0; i < str.size(); ++i)
    {
        switch(str[i])
        {
            case '%':
                if(i + 2 < str.size())
                {
                    int const h = hex_to_int(str[++i]);
                    int const l = hex_to_int(str[++i]);

                    if(h >= 0 && l >= 0)
                    {
                        int const v = (h << 4) | l;

                        if(v >= 32 && v != 127)
                        {
                            result.push_back(char(v));
                        }
                    }
                }
                else
                {
                    // ignore invalid sequence
                    result.push_back('%');
                }
                break;

            case '+':
                result.push_back(' ');
                break;

            default:
                result.push_back(str[i]);
        }
    }

    return result;
}

std::string combine_url_path(std::string const& path1, std::string const& path2)
{
    if(!boost::algorithm::starts_with(path1, strings::slash))
    {
        return combine_url_path(strings::slash + path1, path2);
    }

    bool has_end_slash = boost::algorithm::ends_with(path1, strings::slash);
    bool has_start_slash = boost::algorithm::starts_with(path2, strings::slash);

    if(!(has_end_slash || has_start_slash))
    {
        return path1 + strings::slash + path2;
    }

    if(has_end_slash && has_start_slash)
    {
        return path1 + path2.substr(1);
    }

    return path1 + path2;
}

}   // namespace http
