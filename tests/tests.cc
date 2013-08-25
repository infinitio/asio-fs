#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE asio_fs

#include <fstream>

#include <boost/asio.hpp>
#include <boost/test/unit_test.hpp>

#include <asio-fs/Service.hh>
#include <asio-fs/posix.hh>

BOOST_AUTO_TEST_CASE(basic_read)
{
  boost::asio::io_service service;
  auto fs_service = new boost::asio::fs::Service(service);
  boost::asio::add_service<boost::asio::fs::Service>(service, fs_service);

  char buf[32];
  int size = -1;

  bool closed = false;
  int fd = -1;
  auto on_closed =
    [&closed] (const boost::system::error_code& ec)
    {
      BOOST_CHECK(!ec);
      closed = true;
    };
  auto on_read =
    [&service, &fd, &size, on_closed] (const boost::system::error_code& ec,
                                       ssize_t read)
    {
      BOOST_CHECK(!ec);
      size = read;
      boost::asio::fs::async_close(service, fd, on_closed);
    };
  auto on_opened =
    [&service, &buf, &fd, on_read] (const boost::system::error_code& ec,
                                       int _fd)
    {
      BOOST_CHECK(!ec);
      fd = _fd;
      boost::asio::fs::async_read(service, fd, buf, sizeof(buf), on_read);
    };
  char* name = mktemp(strdup("test_file.XXXXXX"));
  {
    std::ofstream file(name);
    file << "test content\n";
  }
  boost::asio::fs::async_open(service, name, 0, O_RDONLY, on_opened);
  service.run();
  BOOST_CHECK(closed);
  BOOST_CHECK_EQUAL(size, 13);
  BOOST_CHECK_EQUAL(std::string(buf, size), "test content\n");
  unlink(name);
  free(name);
}
