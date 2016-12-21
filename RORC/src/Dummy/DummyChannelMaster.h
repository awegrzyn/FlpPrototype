/// \file DummyChannelMaster.h
/// \brief Definition of the DummyChannelMaster class.
///
/// \author Pascal Boeschoten (pascal.boeschoten@cern.ch)

#pragma once

#include <array>
#include <boost/scoped_ptr.hpp>
#include "ChannelUtilityInterface.h"
#include "PageManager.h"
#include "ChannelMasterBase.h"

namespace AliceO2 {
namespace Rorc {

/// A dummy implementation of the ChannelMasterInterface.
/// This exists so that the RORC module may be built even if the all the dependencies of the 'real' card
/// implementation are not met (this mainly concerns the PDA driver library).
/// It provides some basic simulation of page pushing and output.
class DummyChannelMaster final : public ChannelMasterBase
{
  public:

    DummyChannelMaster(const Parameters& parameters);
    virtual ~DummyChannelMaster();
    virtual void resetChannel(ResetLevel::type resetLevel) override;
    virtual void startDma() override;
    virtual void stopDma() override;
    virtual uint32_t readRegister(int index) override;
    virtual void writeRegister(int index, uint32_t value) override;
    virtual CardType::type getCardType() override;

    virtual int fillFifo(int maxFill) override;
    virtual int getAvailableCount() override;
    virtual std::shared_ptr<Page> popPageInternal(const MasterSharedPtr& channel) override;
    virtual void freePage(const Page& page) override;

    virtual std::vector<uint32_t> utilityCopyFifo() override;
    virtual void utilityPrintFifo(std::ostream& os) override;
    virtual void utilitySetLedState(bool state) override;
    virtual void utilitySanityCheck(std::ostream& os) override;
    virtual void utilityCleanupState() override;
    virtual int utilityGetFirmwareVersion() override;

  private:
    static constexpr size_t FIFO_CAPACITY = 128;

    /// Lock that guards against both inter- and intra-process ownership
    boost::scoped_ptr<Interprocess::Lock> mInterprocessLock;

    PageManager<FIFO_CAPACITY> mPageManager;

    size_t mBufferSize;
    size_t mPageSize;
    size_t mMaxPages;
    int mPageCounter;
    std::vector<char> mPageBuffer;
};

} // namespace Rorc
} // namespace AliceO2
