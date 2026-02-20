#pragma once

#include <boost/asio/io_service.hpp>

namespace appbase {

/// Lightweight wrapper around boost::asio::io_service.
/// Allows forward-declaring the type in application.hpp,
/// removing the heavy Boost.Asio include from the public header.
class io_service_wrapper
{
public:
    explicit io_service_wrapper( boost::asio::io_service& io ) noexcept
        : io_service_( io )
    {}

    io_service_wrapper( const io_service_wrapper& ) = delete;
    io_service_wrapper& operator=( const io_service_wrapper& ) = delete;

    boost::asio::io_service& get() noexcept { return io_service_; }
    const boost::asio::io_service& get() const noexcept { return io_service_; }

private:
    boost::asio::io_service& io_service_;
};

} // namespace appbase
