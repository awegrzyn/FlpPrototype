/// \file CrorcChannelMaster.h
/// \brief Definition of the CrorcChannelMaster class.
///
/// \author Pascal Boeschoten (pascal.boeschoten@cern.ch)

#pragma once

#include <mutex>
#include <unordered_map>
#include <boost/circular_buffer_fwd.hpp>
#include <boost/scoped_ptr.hpp>
#include "ChannelMasterPdaBase.h"
#include "RORC/Parameters.h"
#include "PageManager.h"
#include "ReadyFifo.h"
#include "SuperpageQueue.h"

namespace AliceO2 {
namespace Rorc {

/// Extends ChannelMaster object, and provides device-specific functionality
class CrorcChannelMaster final : public ChannelMasterPdaBase
{
  public:

    CrorcChannelMaster(const Parameters& parameters);
    virtual ~CrorcChannelMaster() override;

    virtual CardType::type getCardType() override;

    virtual std::vector<uint32_t> utilityCopyFifo() override;
    virtual void utilityPrintFifo(std::ostream& os) override;
    virtual void utilitySetLedState(bool state) override;
    virtual void utilitySanityCheck(std::ostream& os) override;
    virtual void utilityCleanupState() override;
    virtual int utilityGetFirmwareVersion() override;

    virtual void enqueueSuperpage(size_t offset, size_t size) override;
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

    /// Name for the CRORC shared data object in the shared state file
    static std::string getCrorcSharedDataName()
    {
      return "CrorcChannelMasterSharedData";
    }

  private:

    static constexpr size_t MAX_SUPERPAGES = 32;
    static constexpr size_t FIFO_QUEUE_MAX = READYFIFO_ENTRIES; // Firmware FIFO Size

    int mFifoBack = 0; ///< Back index of the firmware FIFO
    int mFifoSize = 0; ///< Amount of elements in the firmware FIFO

    int getFifoFront()
    {
      return (mFifoBack + mFifoSize) % READYFIFO_ENTRIES;
    };

    using SuperpageQueueType = SuperpageQueue<MAX_SUPERPAGES>;
    using SuperpageQueueEntry = SuperpageQueueType::SuperpageQueueEntry;
    SuperpageQueueType mSuperpageQueue;

    volatile void* getNextSuperpageBusAddress(const SuperpageQueueEntry& superpage);

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

    ReadyFifo* getFifoUser()
    {
      return reinterpret_cast<ReadyFifo*>(getFifoAddressUser());
    }

    ReadyFifo* getFifoBus()
    {
      return reinterpret_cast<ReadyFifo*>(getFifoAddressBus());
    }

    /// Enables data receiving in the RORC
    void startDataReceiving();

    /// Initializes and starts the data generator with the given parameters
    /// \param generatorParameters The parameters for the data generator.
    void startDataGenerator(const GeneratorParameters& generatorParameters);

    /// Pushes the initial 128 pages to the CRORC's Free FIFO
    void initializeFreeFifo();

    /// Pushes a page to the CRORC's Free FIFO
    /// \param readyFifoIndex Index of the Ready FIFO to write the page's transfer status to
    /// \param pageBusAddress Address on the bus to push the page to
    void pushFreeFifoPage(int readyFifoIndex, volatile void* pageBusAddress);

    /// Check if data has arrived
    DataArrivalStatus::type dataArrived(int index);

    /// Arms C-RORC data generator
    void crorcArmDataGenerator();

    /// Arms DDL
    /// \param resetMask The reset mask. See the RORC_RESET_* macros in rorc.h
    void crorcArmDdl(int resetMask);

    /// Find and store DIU version
    void crorcInitDiuVersion();

    /// Checks if link is up
    void crorcCheckLink();

    /// Send a command to the SIU
    /// \param command The command to send to the SIU. These are probably the macros under 'interface commands' in
    ///   the header ddl_def.h
    void crorcSiuCommand(int command);

    /// Send a command to the DIU
    /// \param command The command to send to the SIU. These are probably the macros under 'interface commands' in
    ///   the header ddl_def.h
    void crorcDiuCommand(int command);

    /// Reset the C-RORC
    void crorcReset(int command);

    /// Checks if the C-RORC's Free FIFO is empty
    void crorcCheckFreeFifoEmpty();

    /// Starts data receiving
    void crorcStartDataReceiver();

    /// Starts the trigger
    void crorcStartTrigger();

    /// Stops the trigger
    void crorcStopTrigger();

    /// Set SIU loopback
    void crorcSetSiuLoopback();

    long long int mLoopPerUsec; ///< Some timing parameter used during communications with the card
    double mPciLoopPerUsec; ///< Some timing parameters used during communications with the card
    int mRorcRevision;
    int mSiuVersion;
    int mDiuVersion;

    static constexpr CardType::type CARD_TYPE = CardType::Crorc;

    /// Indicates deviceStartDma() was called, but DMA was not actually started yet. We do this because we need a
    /// superpage to actually start.
    bool mPendingDmaStart = false;

    /// Starts pending DMA with given superpage for the initial pages
    void startPendingDma(SuperpageQueueEntry& superpage);

    void pushIntoSuperpage(SuperpageQueueEntry& superpage);
};

} // namespace Rorc
} // namespace AliceO2
