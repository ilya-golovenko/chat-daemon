//---------------------------------------------------------------------------
//
//    Copyright (C) 2008, 2009 Ilya Golovenko
//    This file is part of spdaemon.
//
//    spdaemon is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    spdaemon is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with spdaemon. If not, see <http://www.gnu.org/licenses/>.
//
//---------------------------------------------------------------------------
#ifndef _log_file_handler_hpp
#define _log_file_handler_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include "log_handler.hpp"
#include <common.hpp>

// BOOST headers
#include <boost/date_time.hpp>

// STL headers
#include <fstream>
#include <sstream>
#include <string>


class log_file_handler :
    public log_handler
{
public:
    static log_handler_ptr create(const std::string& filename,
        std::ostream::pos_type max_size, bool backup_log);

public:
    log_file_handler(const std::string& filename,
        std::ostream::pos_type max_size, bool backup_log);

    virtual ~log_file_handler();

    virtual void add_message(const log_message& message);

private:
    void initialize(const std::string& backup_filename);

    const std::string get_backup_filename() const;
    bool is_log_file_backup_required();
    void backup_old_log_file();

    void flush_to_file();
    void finalize();

private:
    bool backup_log_;
    std::string filename_;
    std::ostream::pos_type max_size_;

    std::ofstream file_;
    std::ostringstream buffer_;
};

#endif  // _log_file_handler_hpp
