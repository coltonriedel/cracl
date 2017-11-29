#ifndef CRACL_RECEIVER_UBLOX_HPP
#define CRACL_RECEIVER_UBLOX_HPP

#include <array>
#include <string>

namespace cracl
{

enum msg_class {
  ACK              = 0x05,
  AID              = 0x0b,
  CFG              = 0x06,
  ESF              = 0x10,
  HNR              = 0x28,
  INF              = 0x04,
  LOG              = 0x21,
  MGA              = 0x13,
  MON              = 0x0a,
  NAV              = 0x01,
  RXM              = 0x02,
  SEC              = 0x27,
  TIM              = 0x0d,
  UPD              = 0x09
};

enum msg_id {
  /* ACK */
  ACK              = 0x01,
  NAK              = 0x00,
  /* AID */
  ALM              = 0x30,
  AOP              = 0x30,
  EPH              = 0x31,
  HUI              = 0x02,
  INI              = 0x01,
  /* CFG */
  ANT              = 0x13,
  CFG              = 0x09,
  DAT              = 0x06,
  DGNSS            = 0x70,
  DOSC             = 0x61,
  DYNSEED          = 0x85,
  ESRC             = 0x60,
  FIXSEED          = 0x84,
  GEOFENCE         = 0x69,
  GNSS             = 0x3E,
  HNR              = 0x5c,
  INF              = 0x02,
  IFTM             = 0x39,
  LOGFILTER        = 0x47,
  MSG              = 0x01,
  NAV5             = 0x24,
  NAVX5            = 0x23,
  NMEA             = 0x17,
  ODO              = 0x1e,
  PM2              = 0x3b,
  PRT              = 0x00,
  PWR              = 0x57,
  RATE             = 0x08,
  RINV             = 0x34,
  RST              = 0x04,
  RXM              = 0x11,
  SBAS             = 0x16,
  SMGR             = 0x62,
  TMODE2           = 0x3d,
  TMODE3           = 0x71,
  TP5              = 0x31,
  TXSLOT           = 0x53,
  USB              = 0x1b,
  /* ESF */
  INS              = 0x15,
  MEAS             = 0x02,
  RAW              = 0x03,
  STATUS           = 0x10,
  /* HNR */
  PVT              = 0x00,
  /* INF */
  DEBUG            = 0x04,
  ERROR            = 0x00,
  NOTICE           = 0x02,
  TEST             = 0x03,
  WARNING          = 0x01,
  /* LOG */
  CREATE           = 0x07,
  ERASE            = 0x03,
  FINDTIME         = 0x0e,
  INFO             = 0x08,
  RETRIEVEPOSEXTRA = 0x0f,
  RETRIEVEPOS      = 0x0b,
  RETRIEVESTRING   = 0x0d,
  RETRIEVE         = 0x09,
  STRING           = 0x04,
  /* MGA */
  ACK              = 0x60,
  ANO              = 0x20,
  BDS              = 0x03,
  DBD              = 0x80,
  FLASH            = 0x21,
  GAL              = 0x02,
  GLO              = 0x06,
  GPS              = 0x00,
  INI              = 0x40,
  QZSS             = 0x05,
  /* MON */
  GNSS             = 0x28,
  HW2              = 0x0b,
  HW               = 0x09,
  IO               = 0x02,
  MSGPP            = 0x06,
  PATCH            = 0x27,
  RXBUF            = 0x07,
  RXR              = 0x21,
  SMGR             = 0x2e,
  TXBUF            = 0x08,
  VER              = 0x04,
  /* NAV */
  AOPSTATUS        = 0x60,
  ATT              = 0x05,
  CLOCK            = 0x22,
  DGPS             = 0x31,
  DOP              = 0x04,
  EOE              = 0x61,
  GEOFENCE         = 0x39,
  HPPOSECEF        = 0x13,
  HPPOSLLH         = 0x14,
  ODO              = 0x09,
  ORB              = 0x34,
  POSECEF          = 0x01,
  POSLLH           = 0x02,
  PVT              = 0x07,
  RELPOSNED        = 0x3c,
  RESETODO         = 0x10,
  SAT              = 0x35,
  SBAS             = 0x32,
  SOL              = 0x06,
  STATUS           = 0x03,
  SVINFO           = 0x30,
  SVIN             = 0x3b,
  TIMEBDS          = 0x24,
  TIMEGAL          = 0x25,
  TIMEGLO          = 0x23,
  TIMEGPS          = 0x20,
  TIMELS           = 0x26,
  TIMEUTC          = 0x21,
  VELECEF          = 0x11,
  VELNED           = 0x12,
  /* RXM */
  IMES             = 0x61,
  MEASX            = 0x14,
  PMREQ            = 0x41,
  RAWX             = 0x15,
  RLM              = 0x59,
  RTCM             = 0x32,
  SFRBX            = 0x13,
  SVSI             = 0x20,
  /* SEC */
  SIGN             = 0x01,
  UNIQID           = 0x03,
  /* TIM */
  DOSC             = 0x11,
  FCHG             = 0x16,
  HOC              = 0x17,
  SMEAS            = 0x13,
  SVIN             = 0x04,
  TM2              = 0x03,
  TOS              = 0x12,
  TP               = 0x01,
  VCOCAL           = 0x15,
  VRFY             = 0x06,
  /* UPD */
  SOS              = 0x14
};



  LGOFILTER =
class MON { static const uint8_t value = <F3<F6><F7><F7><F7><> };

template <typename T> const uint8_t msg_class<T>::value = 0x00;
template <> const uint8_t msg_class<T>::value = 0x00;


class ublox
{
  uint8_t s_mu = 0xb5; // μ sync character
  uint8_t s_b  = 0x62; // b sync character

  uint8_t c_mon = 0x0a; // MON message class

  /* @bring Function to compute the checksum (XOR) of NMEA messages
   *
   * @param The message to compute the checksum on, containing both '$' and '*'
   */
  void add_pubx_checksum(std::string &msg)
  {
    uint8_t check = 0x00;
    std::ostringstream checksum;

    // Start at 1 to skip '$', stop at (n - 1) to skip '*'
    for (size_t i = 1; i < msg.length() - 1; ++i)
      check ^= (uint8_t)msg[i];

    checksum << std::setw(2) << std::setfill('0') << std::hex << std::uppercase
      << (int)check;

    msg += checksum.str();
  }

  /* @brief Function to compute the Fletcher checksums of an UBX message and
   *        append them to the message before returning
   *
   * @param A vector of bytes (including sync bytes) representing an UBX message
   */
  void add_ubx_checksum(std::vector<uint8_t> &msg)
  {
    uint8_t check_a = 0;
    uint8_t check_b = 0;

    // Start at 2 to skip sync characters
    for (size_t i = 2; i < msg.size(); ++i)
      check_b += (check_a += msg[i]);

    msg.push_back(check_a);
    msg.push_back(check_b);
  }

  /* @brief Function to convert a vector of bytes representing an UBX message
   *        into a string containing hex escape characters
   *
   * @param A vector of bytes representing the contents of an UBX message
   * @return A string of escaped hex characters representing the message
   */
  std::string escape_ubx_message(std::vector<uint8_t> &msg)
  {
    std::ostringstream result;

    for (size_t i = 0; i < msg.size(); ++i)
      result << "\\x"
        << std::setw(2) << std::setfill('0') << std::hex << (int)msg[i];

    return result.str();
  }

public:
  /* @brief Constructor for UBX device
   *
   * @param A shared_ptr to a communication interface
   */
  ublox(std::shared_ptr<interface> port)
    : nmea_device(port), base_device(port)
  {
    comm->set_timeout(100);
    comm->set_baud(9600);
  }

  // PUBX Messages

  /*
   */
  void pubx_rate(std::string nmea_type, size_t rate)
  {
    std::string command = "$PUBX,40," + nmea_type + ",0," + std::to_string(rate)
      + ",0,0,0,0*";

    add_pubx_checksum(command);

    comm->write(command);
  }

  // UBX Messages

  /* @brief Function to get the results of the UBX-MON-VER command
   *
   * UBX-MON-VER returns the currently running firmware version, hardware
   * version, and any extensions to the firmware
   *
   * @return The output of the UBX-MON-VER command
   */
  std::string ubx_mon_ver()
  {
    uint8_t length_a = 0x00;  // UBX_MON_VER passes no parameters
    uint8_t length_b = 0x00;  //    so the length is always 0

    std::vector<uint8_t> packet = { s_mu, s_b, c_mon, 0x04 /* ID */,
                                    length_a, length_b };

    add_ubx_checksum(packet);

    return comm->query_raw(escape_ubx_message(packet));
  }

  template <typename Class, typename ID, typename... Args>
  std::vector<uint8_t> ubx(Args... args)
  {




  }
};

} // namespace cracl

#endif // CRACL_RECEIVER_UBLOX_HPP