/// \file    Version.h
/// \brief   Report the version for this package.
///
/// \author  Barthelemy von Haller, CERN

#ifndef ALICEO2_MONITORING_CORE_DATA_COLLECTOR_VERSION_H
#define ALICEO2_MONITORING_CORE_DATA_COLLECTOR_VERSION_H

#include <string>

namespace AliceO2 {
namespace Monitoring {
namespace Core {
/// The current major version.
#define DATA_COLLECTOR_VERSION_MAJOR 0

/// The current minor version.
#define DATA_COLLECTOR_VERSION_MINOR 0

/// The current patch level.
#define DATA_COLLECTOR_VERSION_PATCH 0

/// The current svn revision.
#define DATA_COLLECTOR_VCS_REVISION "4ab76bf58336b4402d5d3c819352275cf1d6e4b5"

/// True if the current version is newer than the given one.
#define DATA_COLLECTOR_VERSION_GT(MAJOR, MINOR, PATCH) \
  ((DATA_COLLECTOR_VERSION_MAJOR > MAJOR) ||           \
   (DATA_COLLECTOR_VERSION_MAJOR ==                    \
    MAJOR&&(DATA_COLLECTOR_VERSION_MINOR > MINOR || (DATA_COLLECTOR_VERSION_MINOR == MINOR&& DATA_COLLECTOR_VERSION_PATCH > PATCH))))

/// True if the current version is equal or newer to the given.
#define DATA_COLLECTOR_VERSION_GE(MAJOR, MINOR, PATCH) \
  ((DATA_COLLECTOR_VERSION_MAJOR > MAJOR) ||           \
   (DATA_COLLECTOR_VERSION_MAJOR ==                    \
    MAJOR&&(DATA_COLLECTOR_VERSION_MINOR > MINOR || (DATA_COLLECTOR_VERSION_MINOR == MINOR&& DATA_COLLECTOR_VERSION_PATCH >= PATCH))))

/// True if the current version is older than the given one.
#define DATA_COLLECTOR_VERSION_LT(MAJOR, MINOR, PATCH) \
  ((DATA_COLLECTOR_VERSION_MAJOR < MAJOR) ||           \
   (DATA_COLLECTOR_VERSION_MAJOR ==                    \
    MAJOR&&(DATA_COLLECTOR_VERSION_MINOR < MINOR || (DATA_COLLECTOR_VERSION_MINOR == MINOR&& DATA_COLLECTOR_VERSION_PATCH < PATCH))))

/// True if the current version is older or equal to the given.
#define DATA_COLLECTOR_VERSION_LE(MAJOR, MINOR, PATCH) \
  ((DATA_COLLECTOR_VERSION_MAJOR < MAJOR) ||           \
   (DATA_COLLECTOR_VERSION_MAJOR ==                    \
    MAJOR&&(DATA_COLLECTOR_VERSION_MINOR < MINOR || (DATA_COLLECTOR_VERSION_MINOR == MINOR&& DATA_COLLECTOR_VERSION_PATCH <= PATCH))))

/// Information about the current DataCollector version.
class Version {
public:
  /// @return the current major version of DataCollector.
  static int getMajor();

  /// @return the current minor version of DataCollector.
  static int getMinor();

  /// @return the current patch level of DataCollector.
  static int getPatch();

  /// @return the current DataCollector version (MM.mm.pp).
  static std::string getString();

  /// @return the SVN revision.
  static std::string getRevision();

  /// @return the current DataCollector version plus the SVN revision (MM.mm.pp.rev).
  static std::string getRevString();
};

} // namespace Core
} // namespace Monitoring
} // namespace AliceO2

#endif // ALICEO2_MONITORING_CORE_DATA_COLLECTOR_VERSION_H