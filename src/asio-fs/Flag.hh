#ifndef BOOST_ASIO_FS_FLAG_HH
# define BOOST_ASIO_FS_FLAG_HH

namespace boost
{
  namespace asio
  {
    namespace fs
    {
      enum class Flag
      {
        none =              0,
        read_only =         (1 << 0),
        write_only =        (1 << 1),
        read_write =        (1 << 2),

        create =            (1 << 3),
        append =            (1 << 4),
        close_on_execute =  (1 << 5),
        truncate =          (1 << 6),
        force_create =      (1 << 7),
      };

      inline
      Flag
      operator |(Flag const lhs,
                 Flag const rhs)
      {
        return static_cast<Flag>(
          static_cast<int>(lhs) | static_cast<int>(rhs)
        );
      }

      inline
      bool
      operator &(Flag const lhs,
                 Flag const rhs)
      {
        return static_cast<int>(lhs) & static_cast<int>(rhs);
      }

      inline
      Flag&
      operator |=(Flag& lhs,
                  Flag const rhs)
      {
        return (lhs = lhs | rhs);
      }
    }
  }
}

#endif
