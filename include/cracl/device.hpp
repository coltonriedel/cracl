#ifndef CRACL_DEVICE_HPP
#define CRACL_DEVICE_HPP

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/asio/use_future.hpp>
#include <boost/asio/serial_port.hpp>

#include <array>
#include <future>
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
      const size_t size_transferred)
  {
    if (!error)
    {
      m_timer.cancel();

      std::cout << "\033[32mcracl::device::read_size_callback() called without error\033[0m" << std::endl;
      m_read_status = read_status::finalized;
      m_read_size = size_transferred;

      std::cout << "\033[35m" << size_transferred << " bytes read\033[0m" << std::endl;
    }
    else
    {
#ifdef __APPLE__
      if (error.value() == 45)
#elif _WIN32
      if (error.value() == 995)
#else
      if (error.value() == 125)
#endif
      {
        std::cout << "\033[33mcracl::device::read_size_callback() called with timeout error\033[0m" << std::endl;
        m_read_status = read_status::timeout;
      }
      else
      {
        std::cout << "\033[31mcracl::device::read_size_callback() called with error: "
          << error.value() << "\033[0m" << std::endl;
        m_read_status = read_status::error;
      }
    }
  }

  void read_delim_callback(const boost::system::error_code& error,
      const size_t size_transferred)
  {
    if (!error)
    {
      m_timer.cancel();

      std::cout << "cracl::device::read_delim_callback() called without error" << std::endl;
      m_read_status = read_status::finalized;
      m_read_size = size_transferred;
    }
    else
    {
#ifdef __APPLE__
      if (error.value() == 45)
#elif _WIN32
      if (error.value() == 995)
#else
      if (error.value() == 125)
#endif
      {
        std::cout << "cracl::device::read_delim_callback() called with timeout error" << std::endl;
        m_read_status = read_status::timeout;
      }
      else
      {
        std::cout << "cracl::device::read_delim_callback() called with error: "
          << error.value() << std::endl;
        m_read_status = read_status::error;
      }
    }
  }

  void timeout_callback(const boost::system::error_code& error)
  {
    if (!error && m_read_status == ongoing)
    {
      std::cout << "\033[33mcracl::device::timeout_callback() called without error\033[0m" << std::endl;
      //m_read_status = read_status::timeout;
      m_read_status = read_status::finalized;
    }
#ifdef __APPLE__
    else if (error.value() == 45)
#elif _WIN32
    else if (error.value() == 995)
#else
    else if (error.value() == 125)
#endif
    {
      if (m_read_status == read_status::timeout)
        std::cout << "\ttimeout callback called with read status set to timeout" << std::endl;
      if (m_read_status == finalized)
        std::cout << "\ttimeout callback called with read status set to finalized" << std::endl;
      m_read_status = read_status::timeout;
      std::cout << "\033[33mcracl::device::timeout_callback() called with timeout error\033[0m" << std::endl;
    }
    else if (error)
    {
      m_read_status = read_status::error;
      std::cout << "\033[31mcracl::device::timeout_callback() called with error: "
        << error.value() << "\033[0m" << std::endl;
    }
    else
    {
      std::cout << "\033[36mcracl::device::timeout_callback() called without error but read-status not ongoing\033[0m" << std::endl;
    }
  }

public:
  device(const std::string& location, size_t baud_rate=115200,
      size_t timeout=100, size_t char_size=8, std::string delim="\r\n",
      port_base::parity::type parity=port_base::parity::none,
      port_base::flow_control::type flow_control=port_base::flow_control::none,
      port_base::stop_bits::type stop_bits=port_base::stop_bits::one)
    : m_timeout(timeout), m_location(location), m_delim(std::move(delim)),
      m_io(), m_port(m_io), m_timer(m_io)
  {
    m_port.open(m_location);

    m_port.set_option(port_base::baud_rate(baud_rate));
    m_port.set_option(port_base::character_size(char_size));
    m_port.set_option(port_base::parity(parity));
    m_port.set_option(port_base::flow_control(flow_control));
    m_port.set_option(port_base::stop_bits(stop_bits));

    if (!m_port.is_open())
      throw std::runtime_error(std::string("Could not open port at: "
            + location));

    std::cout << "Timeout set to " << m_timeout << "ms" << std::endl;
  }

  void reset()
  {
    m_port.cancel();

    m_io.reset();
  }

  void baud_rate(size_t baud_rate)
  {
    m_port.set_option(port_base::baud_rate(baud_rate));
  }

  size_t timeout()
  {
    return m_timeout;
  }

  void timeout(size_t timeout)
  {
    m_timeout = timeout;
  }

  void write(const char* data, size_t size)
  {
    boost::asio::write(m_port, boost::asio::buffer(data, size));
  }

  void write(const std::vector<uint8_t>& data)
  {
    boost::asio::write(m_port, boost::asio::buffer(&data[0], data.size()));
  }

  void write(const std::vector<char>& data)
  {
    boost::asio::write(m_port, boost::asio::buffer(&data[0], data.size()));
  }

  void write(const std::string& data)
  {
    boost::asio::write(m_port, boost::asio::buffer(data.c_str(), data.size()));
  }

  std::vector<uint8_t> read()
  {
    std::vector<uint8_t> result;

    boost::asio::async_read_until(m_port, m_buf, m_delim,
        boost::bind(&device::read_delim_callback, this,
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred)
        );

    m_timer.expires_from_now(boost::posix_time::millisec(m_timeout));

    m_timer.async_wait(boost::bind(&device::timeout_callback, this,
          boost::asio::placeholders::error));

    m_read_status = read_status::ongoing;
    m_read_size = 0;

    while (true)
    {
      m_io.run_one();

      if (m_read_status == finalized)
      {
        m_timer.cancel();

        std::istream is(&m_buf);

        result.resize(m_read_size);

        char* data = reinterpret_cast<char*> (result.data());

        is.read(data, m_read_size);

        break;
      }
      else if (m_read_status == read_status::timeout)
      {
        m_port.cancel();

        break;
      }
      else if (m_read_status == error)
      {
        m_timer.cancel();

        m_port.cancel();

        break;
      }
    }

    m_io.reset();

    return result;
  }

  std::vector<uint8_t> read(size_t size)
  {
    std::vector<uint8_t> result(size, 0x00);
    char* data = reinterpret_cast<char*> (result.data());

    std::cout << "\033[35mattempting to read " << size << " bytes\033[0m" << std::endl;
    if (m_buf.size() > 0)
    {
      std::cout << "\033[35m" << m_buf.size() << " bytes on buffer at start of read\033[0m" << std::endl;
      std::istream is(&m_buf);

      size_t toRead = std::min(m_buf.size(), size);

      is.read(data, toRead);

      data += toRead;
      size -= toRead;
    }

    if (size != 0)
    {
      std::cout << "\033[35m" << size << " bytes remaining to read \033[0m" << std::endl;
      boost::asio::async_read(m_port, boost::asio::buffer(data, size),
          boost::bind(&device::read_size_callback, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred)
          );

      m_timer.expires_from_now(boost::posix_time::millisec(m_timeout));

      m_timer.async_wait(boost::bind(&device::timeout_callback, this,
            boost::asio::placeholders::error));

      m_read_status = read_status::ongoing;
      m_read_size = 0;

      while (true)
      {
        m_io.run_one();

        if (m_read_status == finalized)
        {
          m_timer.cancel();

          break;
        }
        else if (m_read_status == read_status::timeout)
        {
          m_port.cancel();

          break;
        }
        else if (m_read_status == error)
        {
          m_timer.cancel();

          m_port.cancel();

          break;
        }
        else
          std::cout << "Spinning for " << size << std::endl;
      }
    }

    m_io.reset();

    return result;
  }

  uint8_t read_byte()
  {
    uint8_t result = 0x00;
    std::cout << "\033[35mattempting to read one byte \033[0m" << std::endl;

    if (m_buf.size() > 0)
    {
      std::cout << "\033[35m" << m_buf.size() << " bytes on buffer at start of read\033[0m" << std::endl;
      std::istream is(&m_buf);

      is.read(reinterpret_cast<char*> (&result), 1);
    }
    else
    {
      std::cout << "\033[35mone byte remaining to read \033[0m" << std::endl;
      boost::asio::async_read(m_port, boost::asio::buffer(&result, 1),
          boost::bind(&device::read_size_callback, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred)
          );

      m_timer.expires_from_now(boost::posix_time::millisec(m_timeout));

      m_timer.async_wait(boost::bind(&device::timeout_callback, this,
            boost::asio::placeholders::error));

      m_read_status = read_status::ongoing;
      m_read_size = 0;

      while (true)
      {
        m_io.run_one();

        if (m_read_status == finalized)
        {
          m_timer.cancel();

          break;
        }
        else if (m_read_status == read_status::timeout)
        {
          m_port.cancel();

          break;
        }
        else if (m_read_status == error)
        {
          m_timer.cancel();

          m_port.cancel();

          break;
        }
        else
          std::cout << "Spinning for one byte" << std::endl;
      }
    }

    m_io.reset();

    return result;
  }
};

} // namespace cracl

#endif // CRACL_DEVICE_HPP
