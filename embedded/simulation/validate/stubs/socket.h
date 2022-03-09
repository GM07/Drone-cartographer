#ifndef SOCKET_H
#define SOCKET_H

#include <stdlib.h>

#include <string>
/**
 * @brief This is fake implementation of the boost::socket library
 * it's used to mock the socket calls when testing the firmware
 */
namespace boost::asio {

inline int32_t buffer(void*, std::size_t) { return 0; }

class io_service {
 public:
  io_service() = default;
};

namespace local::stream_protocol {
class socket {
 public:
  virtual ~socket() = default;
  socket(io_service){};
  socket() = default;

  inline size_t available() { return 0; }
  inline size_t receive(const int32_t) { return 0; }
  inline size_t send(const int32_t) { return 0; }
  inline void connect(const std::string&) { return; }
  inline void close() {}
  inline void shutdown() {}
};
}  // namespace local::stream_protocol

}  // namespace boost::asio

namespace boost::system {

class system_error {
  system_error() = default;
};

}  // namespace boost::system

#endif
