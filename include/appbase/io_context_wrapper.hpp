#pragma once

#include <boost/asio/io_context.hpp>

namespace appbase {

/// Lightweight wrapper around boost::asio::io_context.
/// Allows forward-declaring the type in application.hpp,
/// removing the heavy Boost.Asio include from the public header.
class io_context_wrapper
{
public:
    explicit io_context_wrapper( boost::asio::io_context& io ) noexcept
        : io_context_( io )
    {}

    io_context_wrapper( const io_context_wrapper& ) = delete;
    io_context_wrapper& operator=( const io_context_wrapper& ) = delete;

    boost::asio::io_context& get() noexcept { return io_context_; }
    const boost::asio::io_context& get() const noexcept { return io_context_; }

private:
    boost::asio::io_context& io_context_;
};

} // namespace appbase
