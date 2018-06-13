#ifndef CRACL_DEVICE_HPP
#define CRACL_DEVICE_HPP

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/asio/serial_port.hpp>

#include <array>
#include <string>

using port_base = boost::asio::serial_port_base;

namespace cracl
{

enum read_status { ongoing, finalized, error, timeout };

class device
{
  size_t m_timeout;
  size_t m_read_size;
  read_status m_read_status;

  std::string m_location;
  std::string m_delim;

  boost::asio::io_service m_io;
  boost::asio::serial_port m_port;
  boost::asio::streambuf m_buf;
  boost::asio::deadline_timer m_timer;

  void read_size_callback(const boost::system::error_code& error,
      const size_t size_transferred);

  void read_delim_callback(const boost::system::error_code& error,
      const size_t size_transferred);

  void timeout_callback(const boost::system::error_code& error);

public:
  device(const std::string& location, size_t baud_rate,
      size_t timeout, size_t char_size, std::string delim,
      port_base::parity::type parity,
      port_base::flow_control::type flow_control,
      port_base::stop_bits::type stop_bits);

  void reset();

  void baud_rate(size_t baud_rate);

  size_t timeout();

  void timeout(size_t timeout);

  void write(const char* data, size_t size);

  void write(const std::vector<uint8_t>& data);

  void write(const std::vector<char>& data);

  void write(const std::string& data);

  std::vector<uint8_t> read();

  std::vector<uint8_t> read(size_t size);

  uint8_t read_byte();
};

} // namespace cracl

#endif // CRACL_DEVICE_HPP
