#ifndef BOOST_ASIO_FS_SERVICE_HH
# define BOOST_ASIO_FS_SERVICE_HH

# include <thread>

# include <boost/asio/io_service.hpp>

namespace boost
{
  namespace asio
  {
    namespace fs
    {
      class Service:
        public boost::asio::io_service::service
      {
      /*------.
      | Types |
      `------*/
      public:
        typedef Service Self;
        typedef io_service::service Super;


      /*-------------.
      | Construction |
      `-------------*/
      public:
        Service(io_service& owner);
        virtual
        ~Service();
        virtual
        void
        shutdown_service() override;
        static io_service::id id;

      /*----.
      | Run |
      `----*/
      public:
        void
        run(std::function<std::function<void ()> ()> const& action);
      private:
        io_service _pool;
        io_service::work _pool_holder;
        std::vector<std::thread> _threads;
      };
    }
  }
}

#endif
