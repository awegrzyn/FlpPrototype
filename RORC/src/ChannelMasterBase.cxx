/// \file ChannelMasterBase.cxx
/// \brief Implementation of the ChannelMasterBase class.
///
/// \author Pascal Boeschoten (pascal.boeschoten@cern.ch)

#include "ChannelMasterBase.h"
#include <iostream>
#include "BufferProviderFile.h"
#include "BufferProviderMemory.h"
#include "ChannelPaths.h"
#include "Utilities/SmartPointer.h"
#include "Utilities/System.h"
#include "Visitor.h"

namespace AliceO2 {
namespace Rorc {

namespace b = boost;
namespace bfs = boost::filesystem;

void ChannelMasterBase::checkChannelNumber(const AllowedChannels& allowedChannels)
{
  if (!allowedChannels.count(mChannelNumber)) {
    std::ostringstream stream;
    stream << "Channel number not supported, must be one of: ";
    for (int c : allowedChannels) {
      stream << c << ' ';
    }
    BOOST_THROW_EXCEPTION(InvalidParameterException() << ErrorInfo::Message(stream.str())
        << ErrorInfo::ChannelNumber(mChannelNumber));
  }
}

void ChannelMasterBase::validateParameters(const Parameters& p)
{
  // TODO Implement some basic checks, but nothing device-specific
//  if (cp.dma.bufferSize % (2l * 1024l * 1024l) != 0) {
//    BOOST_THROW_EXCEPTION(InvalidParameterException()
//        << ErrorInfo::Message("Parameter 'dma.bufferSize' not a multiple of 2 mebibytes"));
//  }
//  if (cp.generator.dataSize > cp.dma.pageSize) {
//    BOOST_THROW_EXCEPTION(InvalidParameterException()
//        << ErrorInfo::Message("Parameter 'generator.dataSize' greater than 'dma.pageSize'"));
//  }
//  if ((cp.dma.bufferSize % cp.dma.pageSize) != 0) {
//    BOOST_THROW_EXCEPTION(InvalidParameterException()
//        << ErrorInfo::Message("DMA buffer size not a multiple of 'dma.pageSize'"));
//  }
}

ChannelMasterBase::ChannelMasterBase(CardDescriptor cardDescriptor, const Parameters& parameters,
    const AllowedChannels& allowedChannels)
    : mCardDescriptor(cardDescriptor), mChannelNumber(parameters.getChannelNumberRequired())
{
  using namespace Utilities;

  // Check the channel number is allowed
  checkChannelNumber(allowedChannels);

  // Check parameters for basic validity
  validateParameters(parameters);

  // Create parent directories
  auto paths = getPaths();
  for (const auto& p : {paths.fifo(), paths.lock()}) {
    makeParentDirectories(p);
  }

  // Get lock
  log("Getting master lock", InfoLogger::InfoLogger::Debug);
  resetSmartPtr(mInterprocessLock, paths.lock(), paths.namedMutex());
  log("Acquired master lock", InfoLogger::InfoLogger::Debug);

  if (auto bufferParameters = parameters.getBufferParameters()) {
    // Create appropriate BufferProvider subclass
    mBufferProvider = Visitor::apply<std::unique_ptr<BufferProvider>>(*bufferParameters,
        [&](BufferParameters::Memory parameters){ return std::make_unique<BufferProviderMemory>(parameters); },
        [&](BufferParameters::File parameters){ return std::make_unique<BufferProviderFile>(parameters); });
  } else {
    BOOST_THROW_EXCEPTION(ParameterException() << ErrorInfo::Message("ChannelMaster requires BufferParameters"));
  }
}

ChannelMasterBase::~ChannelMasterBase()
{
  log("Releasing master lock", InfoLogger::InfoLogger::Debug);
}

} // namespace Rorc
} // namespace AliceO2
