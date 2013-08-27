#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE asio_fs

#include <asio-fs/File.hh>
#include <asio-fs/Service.hh>

#include <boost/asio.hpp>
#include <boost/test/unit_test.hpp>

#include <fstream>

namespace
{
  struct TmpFile
  {
    char* name;
    TmpFile()
      : name{mktemp(strdup("test_file.XXXXXX"))}
    {
      BOOST_CHECK(this->name != nullptr);
    }
    ~TmpFile()
    {
      (void)::unlink(this->name);
      ::free(this->name);
    }
  };
}

BOOST_AUTO_TEST_CASE(async_open)
{
  bool opened = false;
  auto on_open = [&] (boost::system::error_code const& err) {
    if (err)
      return;
    opened = true;
  };

  {
    TmpFile tmp;
    boost::asio::io_service service;
    boost::asio::fs::File file{service};
    file.async_open(tmp.name, on_open);
    service.run();
  }

  BOOST_CHECK(opened == true);
}

BOOST_AUTO_TEST_CASE(async_write)
{
  boost::asio::io_service service;
  boost::asio::fs::File file{service};

  size_t written = 0;
  auto on_write = [&] (boost::system::error_code const& err,
                       size_t const bytes_transferred)
  {
    if (err)
      return;
    written = bytes_transferred;
  };

  bool opened = false;
  char const data[] = "HELLO";
  auto on_open = [&] (boost::system::error_code const& err) {
    if (err)
      return;
    opened = true;
    file.async_write_some(boost::asio::buffer(data, sizeof(data)), on_write);
  };

  TmpFile tmp;
  {
    file.async_open(tmp.name, on_open);
    service.run();
  }

  BOOST_CHECK(opened == true);
  BOOST_CHECK_EQUAL(written, sizeof(data));
  {
    std::string content;
    std::ifstream{tmp.name} >> content;
    BOOST_CHECK_EQUAL((std::string{data, sizeof(data)}), content);
  }
}

BOOST_AUTO_TEST_CASE(async_read)
{
  boost::asio::io_service service;
  boost::asio::fs::File file{service};

  size_t read_bytes = 0;
  auto on_read = [&] (boost::system::error_code const& err,
                      size_t const bytes_transferred)
  {
    if (err)
      return;
    read_bytes = bytes_transferred;
  };

  bool opened = false;
  char dst[256];
  auto on_open = [&] (boost::system::error_code const& err) {
    if (err)
      return;
    opened = true;
    file.async_read_some(boost::asio::buffer(dst, sizeof(dst)), on_read);
  };

  TmpFile tmp;
  std::ofstream{tmp.name} << "HELLO";
  {
    file.async_open(tmp.name, on_open);
    service.run();
  }

  BOOST_CHECK(opened == true);
  BOOST_CHECK_EQUAL(read_bytes, sizeof("HELLO") - 1); // minus '\0'
  BOOST_CHECK_EQUAL((::memcmp(dst, "HELLO", read_bytes)), 0);
}
