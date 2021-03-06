#include <asio-fs/File.hh>

#if defined(INFINIT_WINDOWS)
# include <io.h>
# include <fcntl.h>
# include <sys/types.h>
# include <sys/stat.h>
#endif


namespace boost
{
  namespace asio
  {
    namespace fs
    {
      static
      int
      native_open(char const* filename, int opflags, int flags)
      {
#if defined(INFINIT_WINDOWS)
        return _open(filename, opflags, flags);
#else
        return open(filename, opflags, flags);
#endif
      }

      static
      int
      native_close(int fd)
      {
#if defined(INFINIT_WINDOWS)
        return _close(fd);
#else
        return close(fd);
#endif
      }

      File::handle_type const File::invalid_handle = -1;
      Flag const File::default_flags = Flag::read_write | Flag::create;
      File::Access const File::default_permissions = {
        Permission::rw,
        Permission::rw,
        Permission::rw,
      };

      File::File(io_service& owner,
                 boost::filesystem::path path,
                 Flag const flags,
                 Permission const user,
                 Permission const group,
                 Permission const other):
        _owner(owner),
        _path{std::move(path)},
        _handle{native_open(path.string().c_str(),
                     File::_native_flags(flags),
                     File::_native_mode(user, group, other))}
      {
        if (this->_handle == File::invalid_handle)
          // XXX throw system error instead.
        {
#if defined(INFINIT_WINDOWS)
          int err = GetLastError();
#else
          int err = errno;
#endif
          std::stringstream ss;
          ss << "Invalid handle: error " << err;
          throw std::runtime_error(ss.str());
        }
      }

      File::File(io_service& owner):
        _owner(owner),
        _path{},
        _handle{File::invalid_handle}
      {}

      File::~File()
      {
        try
        {
          if (this->_handle != File::invalid_handle)
            this->close();
        }
        catch (...)
        {}
      }

      void
      File::close()
      {
        if (this->_handle == File::invalid_handle)
          throw std::runtime_error{"File already closed"};
        if (native_close(this->_handle) == -1)
          // XXX throw system error instead.
          throw std::runtime_error{"Couldn't close the file"};
        this->_handle = File::invalid_handle;
        this->_path.clear();
      }

      int
      File::_native_flags(Flag const flags)
      {
        int res = 0;

        if (flags & Flag::read_only)
          res |= O_RDONLY;
        else if (flags & Flag::write_only)
          res |= O_WRONLY;
        else if (flags & Flag::read_write)
          res |= O_RDWR;
        else
          throw std::runtime_error("Invalid access mode");

        if (flags & Flag::create)
          res |= O_CREAT;
        if (flags & Flag::append)
          res |= O_APPEND;
#if !defined(INFINIT_WINDOWS)
        if (flags & Flag::close_on_execute)
          res |= O_CLOEXEC;
#endif
        if (flags & Flag::truncate)
          res |= O_TRUNC;
        if (flags & Flag::force_create)
          res |= O_EXCL;

        return res;
      }

      mode_t
      File::_native_mode(Permission const user,
                         Permission const group,
                         Permission const other)
      {
        mode_t res = 0;
#if !defined(INFINIT_WINDOWS)
        if (user & Permission::read)
          res |= S_IRUSR;
        if (user & Permission::write)
          res |= S_IWUSR;
        if (user & Permission::execute)
          res |= S_IXUSR;

        if (group & Permission::read)
          res |= S_IRGRP;
        if (group & Permission::write)
          res |= S_IWGRP;
        if (group & Permission::execute)
          res |= S_IXGRP;

        if (other & Permission::read)
          res |= S_IROTH;
        if (other & Permission::write)
          res |= S_IWOTH;
        if (other & Permission::execute)
          res |= S_IXOTH;
#endif
        return res;
      }
    }
  }
}
