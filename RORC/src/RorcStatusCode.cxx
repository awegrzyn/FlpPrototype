///
/// \file RorcStatusCode.cxx
/// \author Pascal Boeschoten
///

#include "RorcStatusCode.h"
#include <string>
#include <map>

namespace AliceO2 {
namespace Rorc {

constexpr static int RORC_STATUS_OK = -1;
constexpr static int RORC_STATUS_ERROR = -1;
constexpr static int RORC_INVALID_PARAM = -2;
constexpr static int RORC_LINK_NOT_ON = -4;
constexpr static int RORC_CMD_NOT_ALLOWED = -8;
constexpr static int RORC_NOT_ACCEPTED = -16;
constexpr static int RORC_NOT_ABLE = -32;
constexpr static int RORC_TIMEOUT = -64;
constexpr static int RORC_FF_FULL = -128;
constexpr static int RORC_FF_EMPTY = -256;

std::string getRorcStatusString(int errorCode)
{
  static const std::map<int, std::string> map {
    { RORC_STATUS_OK,       "RORC_STATUS_OK"},
    { RORC_STATUS_ERROR,    "RORC_STATUS_ERROR"},
    { RORC_INVALID_PARAM,   "RORC_INVALID_PARAM"},
    { RORC_LINK_NOT_ON,     "RORC_LINK_NOT_ON"},
    { RORC_CMD_NOT_ALLOWED, "RORC_CMD_NOT_ALLOWED"},
    { RORC_NOT_ACCEPTED,    "RORC_NOT_ACCEPTED"},
    { RORC_NOT_ABLE,        "RORC_NOT_ABLE"},
    { RORC_TIMEOUT,         "RORC_TIMEOUT"},
    { RORC_FF_FULL,         "RORC_FF_FULL"},
    { RORC_FF_EMPTY,        "RORC_FF_EMPTY"},
  };

  if (map.count(errorCode) != 0) {
    return map.at(errorCode);
  } else {
    return std::string("UNKNOWN");
  }
}

} // namespace Rorc
} // namespace AliceO2