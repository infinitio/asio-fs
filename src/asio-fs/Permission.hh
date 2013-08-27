#ifndef BOOST_ASIO_FS_PERMISSION_HH
# define BOOST_ASIO_FS_PERMISSION_HH

namespace boost
{
  namespace asio
  {
    namespace fs
    {
      enum class Permission
      {
        none = 0,
        read = 1 << 0,
        write = 1 << 1,
        execute = 1 << 2,
        rw = (read | write),
        rwx = (read | write | execute),
      };

      inline
      Permission
      operator |(Permission const lhs,
                 Permission const rhs)
      {
        return static_cast<Permission>(
          static_cast<int>(lhs) | static_cast<int>(rhs)
        );
      }

      inline
      bool
      operator &(Permission const lhs,
                 Permission const rhs)
      {
        return static_cast<int>(lhs) & static_cast<int>(rhs);
      }

      inline
      Permission&
      operator |=(Permission& lhs,
                  Permission const rhs)
      {
        return (lhs = lhs | rhs);
      }
    }
  }
}

#endif
