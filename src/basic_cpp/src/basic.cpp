#include <iostream>

#include <serial/serial.h>
#include <cstring>
#include "basic_cpp/arduino_comms.h"
#include <unistd.h>

void ArduinoComms::setup(const std::string &serial_device, int32_t baud_rate, int32_t timeout_ms)
{  
    serial_conn_.setPort(serial_device);
    serial_conn_.setBaudrate(baud_rate);
    serial::Timeout tt = serial::Timeout::simpleTimeout(timeout_ms);
    serial_conn_.setTimeout(tt); // This should be inline except setTimeout takes a reference and so needs a variable
    serial_conn_.open();
    // serial_conn_.(serial_device, baud_rate, serial::Timeout::simpleTimeout(timeout_ms));

}


void ArduinoComms::sendEmptyMsg()
{
    std::string response = sendMsg("\n");
}

void ArduinoComms::readEncoderValues(int &val_1, int &val_2)
{
    std::string response = sendMsg("e\n");
    std::cout << "Response from readEncoderValues <" << response << ">\n";
    std::string delimiter = " ";
    size_t del_pos = response.find(delimiter);
    std::string token_1 = response.substr(0, del_pos);
    std::string token_2 = response.substr(del_pos + delimiter.length());

    val_1 = std::atoi(token_1.c_str());
    val_2 = std::atoi(token_2.c_str());
}

void ArduinoComms::setMotorValues(int val_1, int val_2)
{
    std::stringstream ss;
    ss << "m " << val_1 << " " << val_2 << "\n";
    std::string response = sendMsg(ss.str());
    std::cout << "setMotorValues " << ss.str() << ", response <" << response << ">\n";
}

void ArduinoComms::setPidValues(float k_p, float k_d, float k_i, float k_o)
{
    std::stringstream ss;
    ss << "u " << k_p << ":" << k_d << ":" << k_i << ":" << k_o << "\n";
    sendMsg(ss.str());
}

std::string ArduinoComms::sendMsg(const std::string &msg_to_send, bool print_output)
{
    serial_conn_.write(msg_to_send);
    std::string response = serial_conn_.readline();

    if (print_output)
    {
      std::cout << "response " << response << "\n";
        // RCLCPP_INFO_STREAM(logger_,"Sent: " << msg_to_send);
        // RCLCPP_INFO_STREAM(logger_,"Received: " << response);
    }

    return response;
}

int main(int argc, char **argv) {
  
  std::cout << "Hello World!\n";

  serial::Serial serial_conn_;
  std::cout << "SerialPort created!\n";

  ArduinoComms arduino_;
  std::cout << "arduino_ created!\n";

  arduino_.setup("/dev/ttyACM0", 57600, 2000);
  std::cout << "arduino_.setup done!\n";

  arduino_.sendEmptyMsg();

  arduino_.setMotorValues(100, 200);
  sleep(2);

  int val1 = -10;
  int val2 = -20;
  arduino_.readEncoderValues(val1, val2);
  // std::cout << "readEncoderValues done!\n";
  std::stringstream ss;
  ss << "m " << val1 << " " << val2 << "\n";;
  std::cout << "Motor Values " << ss.str();
  
  sleep(2);
  arduino_.readEncoderValues(val1, val2);
  ss << "m " << val1 << " " << val2 << "\n";;
  std::cout << "Motor Values " << ss.str();
  serial_conn_.close();
  return 0;
}
