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
#ifndef _handler_alloc_hpp
#define _handler_alloc_hpp

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif  // defined(_MSC_VER) && (_MSC_VER >= 1200)

// ASIO headers
#include <asio.hpp>

// BOOST headers
#include <boost/aligned_storage.hpp>
#include <boost/noncopyable.hpp>
#include <boost/array.hpp>


namespace util
{

template
<
    std::size_t count_,
    std::size_t size_
>
class handler_allocator :
    private boost::noncopyable
{
public:
    handler_allocator()
    {
        for(std::size_t i = 0; i < count_; ++i)
            occupied_[i] = false;
    }

    void* allocate(std::size_t size)
    {
        if(size < size_)
        {
            for(std::size_t i = 0; i < count_; ++i)
            {
                if(!occupied_[i])
                {
                    occupied_[i] = true;
                    return storage_[i].address();
                }
            }
        }

        return operator new(size);
    }

    void deallocate(void* pointer, std::size_t size)
    {
        if(size < size_)
        {
            for(std::size_t i = 0; i < count_; ++i)
            {
                if(pointer == storage_[i].address())
                {
                    occupied_[i] = false;
                    return;
                }
            }
        }

        operator delete(pointer);
    }

private:
    typedef boost::aligned_storage<size_> storage_item_type;
    typedef boost::array<storage_item_type, count_> storage_type;

private:
    bool occupied_[count_];
    storage_type storage_;
};

template
<
    typename Allocator,
    typename Handler
>
class custom_alloc_handler
{
public:
    custom_alloc_handler(Allocator& allocator, const Handler& handler) :
        allocator_(allocator),
        handler_(handler)
    {
    }

    template <typename Arg1>
    void operator()(const Arg1& arg1)
    {
        handler_(arg1);
    }

    template <typename Arg1, typename Arg2>
    void operator()(const Arg1& arg1, const Arg2& arg2)
    {
        handler_(arg1, arg2);
    }

    Allocator& get_allocator()
    {
        return allocator_;
    }

private:
    Allocator& allocator_;
    Handler handler_;
};

template <typename Allocator, typename Handler>
inline void* asio_handler_allocate(std::size_t size, custom_alloc_handler<Allocator, Handler>* handler)
{
    return handler->get_allocator().allocate(size);
}

template <typename Allocator, typename Handler>
inline void asio_handler_deallocate(void* pointer, std::size_t size, custom_alloc_handler<Allocator, Handler>* handler)
{
    handler->get_allocator().deallocate(pointer, size);
}

template <typename Allocator, typename Handler>
inline custom_alloc_handler<Allocator, Handler> make_custom_alloc_handler(Allocator& allocator, const Handler& handler)
{
    return custom_alloc_handler<Allocator, Handler>(allocator, handler);
}

}   // namespace util

#endif  // _handler_alloc_hpp
