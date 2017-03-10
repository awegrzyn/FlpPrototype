/// \file CruChannelMaster.h
/// \brief Definition of the CruChannelMaster class.
///
/// \author Pascal Boeschoten (pascal.boeschoten@cern.ch)

#pragma once

#include "ChannelMasterPdaBase.h"
#include <memory>
#include <queue>
#include <boost/circular_buffer_fwd.hpp>
#include "CruFifoTable.h"
#include "CruBarAccessor.h"
#include "PageManager.h"
#include "RORC/Parameters.h"
#include "SuperpageQueue.h"
#include "Utilities/GuardFunction.h"

namespace AliceO2 {
namespace Rorc {

/// Extends ChannelMaster object, and provides device-specific functionality
class CruChannelMaster final : public ChannelMasterPdaBase
{
  public:

    CruChannelMaster(const Parameters& parameters);
    virtual ~CruChannelMaster() override;

    virtual CardType::type getCardType() override;

    virtual std::vector<uint32_t> utilityCopyFifo() override;
    virtual void utilityPrintFifo(std::ostream& os) override;
    virtual void utilitySetLedState(bool state) override;
    virtual void utilitySanityCheck(std::ostream& os) override;
    virtual void utilityCleanupState() override;
    virtual int utilityGetFirmwareVersion() override;

    virtual void pushSuperpage(Superpage) override;
    virtual int getSuperpageQueueCount() override;
    virtual int getSuperpageQueueAvailable() override;
    virtual int getSuperpageQueueCapacity() override;
    virtual SuperpageStatus getSuperpageStatus() override;
    virtual SuperpageStatus popSuperpage() override;
    virtual void fillSuperpages() override;

    AllowedChannels allowedChannels();

  protected:

    virtual void deviceStartDma() override;
    virtual void deviceStopDma() override;
    virtual void deviceResetChannel(ResetLevel::type resetLevel) override;

    /// Name for the CRU shared data object in the shared state file
    static std::string getCruSharedDataName()
    {
      return "CruChannelMasterSharedData";
    }

  private:

    /// Limits the number of superpages allowed in the queue
    static constexpr size_t MAX_SUPERPAGES = 32;

    /// Firmware FIFO Size
    static constexpr size_t FIFO_QUEUE_MAX = CRU_DESCRIPTOR_ENTRIES;

    using SuperpageQueueType = SuperpageQueue<MAX_SUPERPAGES>;
    using SuperpageQueueEntry = SuperpageQueueType::SuperpageQueueEntry;

    /// Namespace for enum describing the status of a page's arrival
    struct DataArrivalStatus
    {
        enum type
        {
          NoneArrived,
          PartArrived,
          WholeArrived,
        };
    };

    void initFifo();
    void initCru();
    void resetCru();
    void setBufferReady();
    void setBufferNonReady();
    void pushIntoSuperpage(SuperpageQueueEntry& superpage);
    uintptr_t getNextSuperpageBusAddress(const SuperpageQueueEntry& superpage);

    CruBarAccessor getBar()
    {
      return CruBarAccessor(getPdaBarPtr());
    }

    static constexpr CardType::type CARD_TYPE = CardType::Cru;

    CruFifoTable* getFifoUser()
    {
      return reinterpret_cast<CruFifoTable*>(getFifoAddressUser());
    }

    CruFifoTable* getFifoBus()
    {
      return reinterpret_cast<CruFifoTable*>(getFifoAddressBus());
    }

    /// Get front index of FIFO
    int getFifoFront()
    {
      return (mFifoBack + mFifoSize) % FIFO_QUEUE_MAX;
    };

    /// Back index of the firmware FIFO
    int mFifoBack = 0;

    /// Amount of elements in the firmware FIFO
    int mFifoSize = 0;

    SuperpageQueueType mSuperpageQueue;

    /// Acks that "should've" been issued before buffer was ready, but have to be postponed until after that
    int mPendingAcks = 0;

    /// Buffer readiness state. True means page descriptors have been filled, so the card can start transferring
    bool mBufferReady = false;

    int mInitialAcks = 0;

    // These variables are configuration parameters

    /// Reset level on initialization of channel
    ResetLevel::type mInitialResetLevel;

    /// Gives the type of loopback
    LoopbackMode::type mLoopbackMode;

    /// Enables the data generator
    bool mGeneratorEnabled;

    /// Data pattern for the data generator
    GeneratorPattern::type mGeneratorPattern;

    /// Maximum number of events
    int mGeneratorMaximumEvents;

    /// Initial value of the first data in a data block
    uint32_t mGeneratorInitialValue;

    /// Sets the second word of each fragment when the data generator is used
    uint32_t mGeneratorInitialWord;

    /// Random seed parameter in case the data generator is set to produce random data
    int mGeneratorSeed;

    /// Length of data written to each page
    size_t mGeneratorDataSize;
};

} // namespace Rorc
} // namespace AliceO2
