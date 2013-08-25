#ifndef BOOST_ASIO_FS_POSIX_HH
# define BOOST_ASIO_FS_POSIX_HH

# include <functional>

# include <boost/asio.hpp>
# include <boost/system/error_code.hpp>

namespace boost
{
  namespace asio
  {
    namespace fs
    {
      typedef
        std::function<void (system::error_code const&)>
        CloseHandler;
      void
      async_close(io_service& io_service,
                  int fd,
                  CloseHandler const& handler);
      typedef
        std::function<void (system::error_code const&, int)>
        OpenHandler;
      void
      async_open(io_service& io_service,
                 char const* pathname,
                 int flags,
                 mode_t mode,
                 OpenHandler const& handler);
      typedef
        std::function<void (system::error_code const&, size_t)>
        ReadHandler;
      void
      async_read(io_service& io_service,
                 int fd,
                 void* buf,
                 size_t count,
                 ReadHandler handler);
      typedef
        std::function<void (system::error_code const&, size_t)>
        ReadHandler;
      void
      async_write(io_service& io_service,
                 int fd,
                 void* buf,
                 size_t count,
                 ReadHandler handler);
    }
  }
}

#endif
