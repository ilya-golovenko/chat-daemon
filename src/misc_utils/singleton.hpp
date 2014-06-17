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
#ifndef _singleton_hpp
#define _singleton_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// ASIO headers
#include <asio.hpp>

// BOOST headers
#include <boost/scoped_ptr.hpp>


namespace util
{

template <typename T>
class singleton :
    private boost::noncopyable
{
public:
    static T& instance();

private:
    singleton();
    ~singleton();

    static asio::detail::mutex mutex_;
    static boost::scoped_ptr<T> instance_;
};

template <typename T>
T& singleton<T>::instance()
{
    if(!instance_)
    {
        asio::detail::mutex::scoped_lock lock(mutex_);

        if(!instance_)
            instance_.reset(new T());
    }

    return *instance_;
}

template <typename T>
asio::detail::mutex singleton<T>::mutex_;

template <typename T>
boost::scoped_ptr<T> singleton<T>::instance_;

}   // namespace util

#endif  // _singleton_hpp
