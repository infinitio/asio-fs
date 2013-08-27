#ifndef BOOST_ASIO_FS_FILE_HH
# define BOOST_ASIO_FS_FILE_HH

# include <asio-fs/Permission.hh>
# include <asio-fs/Flag.hh>

# include <boost/asio.hpp>
# include <boost/filesystem.hpp>

namespace boost
{
  namespace asio
  {
    namespace fs
    {
      class File
      {
      public:
        typedef int handle_type;
        struct Access
        {
          Permission const user, group, other;
        };

      public:
        static handle_type const invalid_handle;
        static Flag const default_flags;
        static Access const default_permissions;

      private:
        io_service& _owner;
        boost::filesystem::path _path;
        handle_type _handle;

      public:
        /// Open synchroneously a file.
        File(io_service& owner,
             boost::filesystem::path path,
             Flag const flags = File::default_flags,
             Permission const user = File::default_permissions.user,
             Permission const group = File::default_permissions.group,
             Permission const other = File::default_permissions.other);

        /// Empty file instance.
        File(io_service& owner);

        virtual
        ~File();

        /// Evaluate to true if the file is valid.
        operator bool() const { return this->_handle != invalid_handle; }

      public:
        template<typename CompletionHandler>
        void
        async_open(boost::filesystem::path path,
                   CompletionHandler handler,
                   Flag const flags = File::default_flags,
                   Permission const user = File::default_permissions.user,
                   Permission const group = File::default_permissions.group,
                   Permission const other = File::default_permissions.other);

        template<typename CompletionHandler>
        void
        async_close(CompletionHandler handler);

        /// Close a file synchroneously.
        void
        close();

        /// Close a file synchroneously storing error code.
        //void
        //close(boost::system::error_code& err);


      public:
        /// Buffer-oriented asynchronous read stream requirements.
        /// @see http://www.boost.org/doc/libs/1_54_0/doc/html/boost_asio/reference/AsyncReadStream.html
        template<typename MutableBufferSequence,
                 typename ReadCompletionHandler>
        void
        async_read_some(MutableBufferSequence const& buffer,
                        ReadCompletionHandler handler);


      public:
        /// Buffer-oriented asynchronous write stream requirements
        /// @see http://www.boost.org/doc/libs/1_54_0/doc/html/boost_asio/reference/AsyncWriteStream.html
        template<typename ConstBufferSequence,
                 typename WriteCompletionHandler>
        void
        async_write_some(ConstBufferSequence const& buffer,
                         WriteCompletionHandler handler);

        handle_type
        native_handle() const { return this->_handle; }

        io_service&
        get_io_service() { return this->_owner; }

      private:
        static
        int
        _native_flags(Flag const flags);
        static
        mode_t
        _native_mode(Permission const user,
                     Permission const group,
                     Permission const other);
      };
    }
  }
}

# include <asio-fs/File.hxx>

#endif
