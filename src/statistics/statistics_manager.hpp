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
#ifndef _statistics_manager_hpp
#define _statistics_manager_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// Application headers
#include <config_common.hpp>
#include <chat_forwards.hpp>
#include <async_timer.hpp>

// ASIO headers
#include <asio.hpp>

// BOOST headers
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>

// STL headers
#include <cstddef>
#include <string>
#include <ctime>


class statistics_manager :
    private boost::noncopyable
{
public:
    statistics_manager(chat::server& server);

    void configure();
    void start();
    void stop();

    void add_connection();
    void add_chat_connection();
    void add_ignored_connection();

    void add_input_message(std::size_t size);
    void add_output_message(std::size_t size);

private:
    enum fields_type
    {
        total_connections,

        chat_connections,
        error_connections,
        ignored_connections,

        output_bytes,
        input_bytes,

        output_messages,
        input_messages,

        fields_count
    };

private:
    void start_status_timer();
    void start_statistics_timer();

    void update_run_status();

    void update_all_statistics();
    void update_simple_statistics();
    void update_detail_statistics();

    std::size_t get_current_detail_point() const;

    void add_simple_statistics(fields_type field, std::size_t count);
    void add_detail_statistics(fields_type field, std::size_t count);

    std::size_t get_error_connections() const;
    std::size_t get_error_connections(std::size_t point) const;

    const std::string get_simple_statistics() const;
    const std::string get_detail_statistics() const;

    void handle_status_timer(const asio::error_code& error);
    void handle_statistics_timer(const asio::error_code& error);

private:
    enum { detail_points = 60 / 5 * 24 };
    enum { detail_point_duration = 60 * 5 };

    typedef boost::array<std::size_t, fields_count> simple_statistics_type;
    typedef boost::array<simple_statistics_type, detail_points> detail_statistics_type;

private:
    chat::server& server_;

    std::string os_name_;
    std::string host_name_;

    static const std::time_t start_time_;

    simple_statistics_type simple_statistics_;
    detail_statistics_type detail_statistics_;

    util::async_timer status_timer_;
    util::async_timer statistics_timer_;
};

typedef boost::shared_ptr<statistics_manager> statistics_manager_ptr;

#endif  // _statistics_manager_hpp
