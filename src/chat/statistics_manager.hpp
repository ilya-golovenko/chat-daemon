//---------------------------------------------------------------------------
//
//    Copyright (C) 2008, 2009, 2015 Ilya Golovenko
//    This file is part of Chat.Daemon project
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
#ifndef _chat_statistics_manager_hpp
#define _chat_statistics_manager_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// ASIO headers
#include <asio/steady_timer.hpp>
#include <asio.hpp>

// STL headers
#include <cstddef>
#include <string>
#include <array>
#include <ctime>


namespace chat
{

class server_context;
struct server_config;

class statistics_manager
{
public:
    explicit statistics_manager(server_context& context);

    statistics_manager(statistics_manager const&) = delete;
    statistics_manager& operator=(statistics_manager const&) = delete;

    void configure(server_config const& config);

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

    std::string get_simple_statistics() const;
    std::string get_detail_statistics() const;

    void handle_status_timer(asio::error_code const& error);
    void handle_statistics_timer(asio::error_code const& error);

    static std::string get_operating_system_info();

private:
    static std::size_t const detail_points = 60 / 5 * 24;
    static std::size_t const detail_point_duration = 60 * 5;

    typedef std::array<std::size_t, fields_count> simple_statistics_array;
    typedef std::array<simple_statistics_array, detail_points> detail_statistics_array;

private:
    server_context& context_;

    std::string server_path_;

    std::chrono::seconds status_interval_;
    std::chrono::seconds statistics_interval_;

    simple_statistics_array simple_statistics_;
    detail_statistics_array detail_statistics_;

    asio::steady_timer status_timer_;
    asio::steady_timer statistics_timer_;
};

}   // namespace chat

#endif  // _chat_statistics_manager_hpp
