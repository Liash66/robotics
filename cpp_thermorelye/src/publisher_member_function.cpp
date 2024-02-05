#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <asio.hpp>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "serial_connector.hpp"

using namespace std::chrono_literals;

class ThermorelyePublisher : public rclcpp::Node
{
public:
  ThermorelyePublisher(): Node("thermorelye_publisher"), count_(0)
  {
    publisher_ = this->create_publisher<std_msgs::msg::String>("topic", 10);
    timer_ = this->create_wall_timer(1000ms, std::bind(&ThermorelyePublisher::timer_callback, this));

    // Создать последовательный порт
    m_sc = new robot::io::SerialConnector("/dev/ttyUSB0", 9600);
    // Открыть
    m_sc->Open();
  }

private:
  void timer_callback()
  {
    // Вызвать метод Receicve(buffer)
    char buffer[350];
    char sendbuf1[1] = {'a'};
    char sendbuf2[1] = {'s'};
    m_sc->Receive(buffer, 350);

    if (m_flag)
    {
      m_flag = false;
      m_sc->Send(sendbuf1, 1);
    }    
    else if (m_flag == false)
    {
      m_flag = true;
      m_sc->Send(sendbuf2, 1);
    }
    auto message = std_msgs::msg::String();
    message.data = buffer;
    RCLCPP_INFO(this->get_logger(), "Publishing: '%s'", message.data.c_str());
    publisher_->publish(message);
  }

  bool m_flag = true;
  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
  size_t count_;
  robot::io::SerialConnector* m_sc;
};

int main(int argc, char* argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<ThermorelyePublisher>());
  rclcpp::shutdown();

  return 0;
}
