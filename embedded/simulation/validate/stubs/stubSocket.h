#include "gmock/gmock.h"
#include "socket.h"

namespace boost::asio::local::stream_protocol {
class StubSocket : public socket {
 public:
  StubSocket(boost::asio::io_service){};
  virtual ~StubSocket() = default;
  MOCK_METHOD1(receive, size_t(const int32_t));
  MOCK_METHOD0(available, size_t());
};
}  // namespace boost::asio::local::stream_protocol
