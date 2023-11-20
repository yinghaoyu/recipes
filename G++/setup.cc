#include <iostream>
#include <memory>

class resource
{
 public:
  virtual bool is_usable() = 0;

  virtual bool setup() = 0;
};

class network_resource
{
 public:
  virtual bool is_usable(const std::string& url) = 0;

  virtual bool setup(const std::string& url) = 0;
};

class publisher : public resource, public network_resource
{
 public:
  // 基类的成员不构成重载，需要手动 using
  using network_resource::is_usable;
  using network_resource::setup;
  using resource::is_usable;
  using resource::setup;
};
class nanomsg_publisher_master : public publisher
{
 public:
  nanomsg_publisher_master() {}

  bool setup() override
  {
    std::cout << "nanomsg_publisher_master setup" << std::endl;
    return true;
  }

  bool setup(const std::string& url) override
  {
    std::cout << "nanomsg_publisher_master setup with URL: " << url
              << std::endl;
    return true;
  }

  bool is_usable() override { return true; }

  bool is_usable(const std::string& url) override
  {
    std::cout << "nanomsg_publisher_master is_usable with URL: " << url
              << std::endl;
    return true;
  }
};

int main()
{
  std::shared_ptr<publisher> publisherMasterPtr =
      std::make_shared<nanomsg_publisher_master>();
  publisherMasterPtr->setup();       // Calls the version with no parameters
  publisherMasterPtr->setup("111");  // Calls the version with a parameter
}
