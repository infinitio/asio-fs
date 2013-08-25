#include <asio-fs/Service.hh>

namespace boost
{
  namespace asio
  {
    namespace fs
    {
      /*-------------.
      | Construction |
      `-------------*/

      Service::Service(io_service& owner):
        Super(owner),
        _pool(),
        _pool_holder(this->_pool)
      {
        for (int i = 0; i < 4; ++i)
        {
          auto action = [this]
            {
              this->_pool.run();
            };
          this->_threads.push_back(std::thread(action));
        }
      }

      Service::~Service()
      {}

      void
      Service::shutdown_service()
      {
        this->_pool.stop();
        for (auto& thread: this->_threads)
          thread.join();
      }

      io_service::id Service::id;

      /*----.
      | Run |
      `----*/

      void
      Service::run(std::function<std::function<void ()> ()> const& action)
      {
        auto& io_service = this->get_io_service();
        io_service::work work(io_service);
        this->_pool.post([&io_service, work, action]
                         {
                           io_service.post(action());
                         });
      }
    }
  }
}
