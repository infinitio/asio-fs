#include <asio-fs/Service.hh>
#include <asio-fs/posix.hh>

#if defined(INFINIT_WINDOWS)
# include <io.h>
# define open _open
# define read _read
# define write _write
# define close _close
#endif

namespace boost
{
  namespace asio
  {
    namespace fs
    {
      template <typename T>
      struct Call
      {};

      template <>
      struct Call<std::function<void (system::error_code const&)>>
      {
        template <typename A>
          static
          void
          call(std::function<void (system::error_code const&)> const& handler,
               system::error_code const& e,
               A res)
        {
          handler(e);
        }
      };

      template <typename A>
      struct Call<std::function<void (system::error_code const&, A)>>
      {
        static
          void
          call(std::function<void (system::error_code const&, A)> const& handler,
               system::error_code const& e,
               A res)
        {
          handler(e, res);
        }
      };

      template <typename Handler, typename ... Args>
      static
      void
      _async_run(io_service& io_service,
                 Handler const& handler,
                 std::function<int (Args...)> const& syscall,
                 Args... args)
      {
        // FIXME: GCC >= 4.9 does not support parameters packs in lambda captures -
        // AFAIK only ICC does.
        auto call = std::bind(syscall, args...);
        auto operation =
          [call, handler] () -> std::function<void ()>
          {
            auto res = call();
            system::error_code e;
            if (res == -1)
              e = system::error_code(errno, system::system_category());
            return [handler, e, res]
            {
              Call<Handler>::call(handler, e, res);
            };
          };
        auto& service =
          use_service<Service>(io_service);
        service.run(operation);
      }

      typedef std::function<void (system::error_code const&)> CloseHandler;
      void
      async_close(io_service& io_service,
                  int fd,
                  CloseHandler const& handler)
      {
        _async_run<CloseHandler, int>(
          io_service,
          handler,
          std::function<int (int)>(close),
          fd);
      }

      typedef std::function<void (system::error_code const&, int)> OpenHandler;
      void
      async_open(io_service& io_service,
                 char const* pathname,
                 int flags,
                 mode_t mode,
                 OpenHandler const& handler)
      {
        _async_run<OpenHandler, char const*, int, mode_t>(
          io_service,
          handler,
          std::function<int (char const*, int, mode_t)>(open),
          pathname,
          flags,
          mode);
      }

      typedef std::function<void (system::error_code const&, size_t)> ReadHandler;
      void
      async_read(io_service& io_service,
                 int fd,
                 void* buf,
                 size_t count,
                 ReadHandler handler)
      {
        _async_run<ReadHandler>(
          io_service,
          handler,
          std::function<int (int fd, void *buf, size_t)>(read),
          fd,
          buf,
          count);
      }

      typedef std::function<void (system::error_code const&, size_t)> WriteHandler;
      void
      async_write(io_service& io_service,
                  int fd,
                  void const* buf,
                  size_t count,
                  WriteHandler handler)
      {
        _async_run<WriteHandler>(
          io_service,
          handler,
          std::function<int (int fd, void const* buf, size_t)>(write),
          fd,
          buf,
          count);
      }
    }
  }
}
