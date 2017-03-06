/// \file ChannelMasterBase.h
/// \brief Definition of the ChannelMasterBase class.
///
/// \author Pascal Boeschoten (pascal.boeschoten@cern.ch)

#pragma once

#include <set>
#include <vector>
#include <memory>
#include <mutex>
#include "BufferProvider.h"
#include "ChannelBase.h"
#include "ChannelPaths.h"
#include "ChannelUtilityInterface.h"
#include "InterprocessLock.h"
#include "RORC/ChannelMasterInterface.h"
#include "RORC/Exception.h"
#include "RORC/Parameters.h"
#include "Utilities/Util.h"

//#define ALICEO2_RORC_CHANNEL_MASTER_DISABLE_LOCKGUARDS
#ifdef ALICEO2_RORC_CHANNEL_MASTER_DISABLE_LOCKGUARDS
#define CHANNELMASTER_LOCKGUARD()
#else
#define CHANNELMASTER_LOCKGUARD() LockGuard _lockGuard(getMutex());
#endif

namespace AliceO2 {
namespace Rorc {

/// Partially implements the ChannelMasterInterface. It provides:
/// - Interprocess synchronization
/// - Creation of files and directories related to the channel
class ChannelMasterBase: public ChannelBase, public ChannelMasterInterface, public ChannelUtilityInterface
{
  public:
    using AllowedChannels = std::set<int>;

    /// Constructor for the ChannelMaster object
    /// \param cardType Type of the card
    /// \param parameters Parameters of the channel
    /// \param serialNumber Serial number of the card
    /// \param allowedChannels Channels allowed by this card type
    ChannelMasterBase(CardType::type cardType, const Parameters& parameters, int serialNumber,
        const AllowedChannels& allowedChannels);

    virtual ~ChannelMasterBase();

    virtual void pushSuperpage(size_t, size_t) override
    {
      BOOST_THROW_EXCEPTION(Exception() << ErrorInfo::Message("not yet implemented"));
    }

    virtual int getSuperpageQueueCount() override
    {
      BOOST_THROW_EXCEPTION(Exception() << ErrorInfo::Message("not yet implemented"));
    }

    virtual int getSuperpageQueueAvailable() override
    {
      BOOST_THROW_EXCEPTION(Exception() << ErrorInfo::Message("not yet implemented"));
    }

    virtual int getSuperpageQueueCapacity() override
    {
      BOOST_THROW_EXCEPTION(Exception() << ErrorInfo::Message("not yet implemented"));
    }

    virtual SuperpageStatus getSuperpageStatus() override
    {
      BOOST_THROW_EXCEPTION(Exception() << ErrorInfo::Message("not yet implemented"));
    }

    virtual SuperpageStatus popSuperpage() override
    {
      BOOST_THROW_EXCEPTION(Exception() << ErrorInfo::Message("not yet implemented"));
    }

    virtual void fillSuperpages() override
    {
      BOOST_THROW_EXCEPTION(Exception() << ErrorInfo::Message("not yet implemented"));
    }

  protected:
    using Mutex = std::mutex;
    using LockGuard = std::lock_guard<Mutex>;

    /// Namespace for enum describing the initialization state of the shared data
    struct InitializationState
    {
        enum type
        {
          UNKNOWN = 0, UNINITIALIZED = 1, INITIALIZED = 2
        };
    };

    Mutex& getMutex()
    {
      return mMutex;
    }

    int getChannelNumber() const
    {
      return mChannelNumber;
    }

    int getSerialNumber() const
    {
      return mSerialNumber;
    }

    virtual void setLogLevel(InfoLogger::InfoLogger::Severity severity) final override
    {
      ChannelBase::setLogLevel(severity);
    }

    void log(const std::string& message, boost::optional<InfoLogger::InfoLogger::Severity> severity = boost::none)
    {
      ChannelBase::log(mSerialNumber, mChannelNumber, message, severity);
    }

    ChannelPaths getPaths()
    {
      return {mCardType, mSerialNumber, mChannelNumber};
    }

    const BufferProvider& getBufferProvider()
    {
      return *mBufferProvider;
    }

  private:

    /// Validate ChannelParameters
    static void validateParameters(const Parameters& parameters);

    /// Check if the channel number is valid
    void checkChannelNumber(const AllowedChannels& allowedChannels);

    /// Mutex to lock thread-unsafe operations
    Mutex mMutex;

    /// Type of the card
    CardType::type mCardType;

    /// Serial number of the device
    int mSerialNumber;

    /// DMA channel number
    const int mChannelNumber;

    /// Lock that guards against both inter- and intra-process ownership
    std::unique_ptr<Interprocess::Lock> mInterprocessLock;

    /// Contains addresses & size of the buffer
    std::unique_ptr<BufferProvider> mBufferProvider;
};

} // namespace Rorc
} // namespace AliceO2
