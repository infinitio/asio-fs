#ifndef BOOST_ASIO_FS_FILE_HXX
# define BOOST_ASIO_FS_FILE_HXX

# include <asio-fs/File.hh>
# include <asio-fs/posix.hh>

namespace boost
{
  namespace asio
  {
    namespace fs
    {
      template<typename CompletionHandler>
      void
      File::async_open(boost::filesystem::path path,
                       CompletionHandler handler,
                       Flag const flags,
                       Permission const user,
                       Permission const group,
                       Permission const other)
      {
        if (this->_handle != File::invalid_handle)
          throw std::runtime_error{"Already opened file"};
        else if (this->_path != boost::filesystem::path{})
          throw std::runtime_error{"Open currently in process"};
        if (path == boost::filesystem::path{})
          throw std::runtime_error{"Cannot open a file with null path"};

        // Path stored as a marker of open operation
        this->_path = std::move(path);

        // Ensure string is stored in path instance
        std::string const& path_str = path.string();

        boost::asio::fs::async_open(
          this->_owner,
          path_str.c_str(),
          File::_native_flags(flags),
          File::_native_mode(user, group, other),
          [this, handler] (system::error_code const& err, handle_type fd) {
            if (err)
              this->_path = boost::filesystem::path{};
            else
              this->_handle = fd;
            handler(err);
          });
      }

      template<typename CompletionHandler>
      void
      File::async_close(CompletionHandler handler)
      {
        if (this->_handle == invalid_handle)
          throw std::runtime_error{"Not opened file"};
        boost::asio::fs::async_close(
          this->_owner,
          this->_handle,
          [this, handler] (system::error_code const& err) {
            if (!err)
            {
              this->_path = boost::filesystem::path{};
              this->_handle = File::invalid_handle;
            }
            handler(err);
          });
      }

      template<typename MutableBufferSequence,
               typename ReadCompletionHandler>
      void
      File::async_read_some(MutableBufferSequence const& buffer,
                            ReadCompletionHandler handler)
      {
        if (this->_handle == invalid_handle)
          throw std::runtime_error("File not opened");
        boost::asio::fs::async_read(
          this->_owner,
          this->_handle,
          buffer_cast<void*>(buffer),
          buffer_size(buffer),
          handler);
      }

      template<typename ConstBufferSequence,
               typename WriteCompletionHandler>
      void
      File::async_write_some(ConstBufferSequence const& buffer,
                             WriteCompletionHandler handler)
      {
        if (this->_handle == invalid_handle)
          throw std::runtime_error("File not opened");
        boost::asio::fs::async_write(
          this->_owner,
          this->_handle,
          buffer_cast<void const*>(buffer),
          buffer_size(buffer),
          handler);
      }
    }
  }
}

#endif
