///
/// \file   AlfaPublisherBackend.cxx
/// \author Barthelemy von Haller
///

#include "QualityControl/AlfaPublisherBackend.h"
#include "FairMQTransportFactoryZMQ.h"
#include "TMessage.h"
#include "TH1F.h"
#include "FairMQProgOptions.h"
#include "FairMQParser.h"

using namespace std;

namespace AliceO2 {
namespace QualityControl {
namespace Core {

AlfaPublisherBackend::AlfaPublisherBackend()
  : mText("asdf")
{

  FairMQProgOptions config;

  // set up communication layout and properties
  FairMQChannel histoChannel;
  histoChannel.UpdateType("push");
  histoChannel.UpdateMethod("bind");
  histoChannel.UpdateAddress("tcp://*:5556");
  histoChannel.UpdateSndBufSize(10000);
  histoChannel.UpdateRcvBufSize(10000);
  histoChannel.UpdateRateLogging(0);
  fChannels["data-out"].push_back(histoChannel);

  // Get the transport layer
#ifdef NANOMSG
  FairMQTransportFactory* transportFactory = new FairMQTransportFactoryNN();
#else
  FairMQTransportFactory *transportFactory = new FairMQTransportFactoryZMQ();
#endif
  SetTransport(transportFactory);

  ChangeState(INIT_DEVICE);
  WaitForEndOfState(INIT_DEVICE);

  ChangeState(INIT_TASK);
  WaitForEndOfState(INIT_TASK);

  th1 = new TH1F("test", "test", 100, 0, 99);
}

AlfaPublisherBackend::~AlfaPublisherBackend()
{
  ChangeState(RESET_TASK);
  WaitForEndOfState(RESET_TASK);

  ChangeState(RESET_DEVICE);
  WaitForEndOfState(RESET_DEVICE);

  ChangeState(END);
}

// helper function to clean up the object holding the data after it is transported.
void AlfaPublisherBackend::CustomCleanup(void *data, void *object)
{
//  delete static_cast<TMessage*>(object); // Does it delete the TH1F ?
}

void AlfaPublisherBackend::CustomCleanupTMessage(void *data, void *object)
{
  delete (TMessage *) object;
}

void AlfaPublisherBackend::publish(MonitorObject *mo)
{
  // from Mohammad
//  TH1F * th1 = new TH1F("test", "test", 100, 0, 99);
//  TMessage *message = new TMessage(kMESS_OBJECT);
//  message->WriteObject(th1);
//  FairMQMessage *msg = fTransportFactory->CreateMessage(message->Buffer(), message->BufferSize(), CustomCleanup,
//                                                        message);
//  fChannels.at("data-out").at(0).Send(msg);

  ChangeState(RUN);
  WaitForEndOfState(RUN);
}

void AlfaPublisherBackend::Init()
{
  cout << "Init()" << endl;
}

void AlfaPublisherBackend::Run()
{

  // this is called when the state change to RUN, i.e. when we call publish

  th1->FillRandom("gaus", 10);
  TMessage *message = new TMessage(kMESS_OBJECT);
  message->WriteObject(th1);
  unique_ptr<FairMQMessage> msg(
    fTransportFactory->CreateMessage(message->Buffer(), message->BufferSize(), CustomCleanupTMessage, message));

//  FairMQMessage *msg = fTransportFactory->CreateMessage(message->Buffer(), message->BufferSize(), CustomCleanup,
//                                                        message);

  std::cout << "Sending \"" << th1->GetName() << "\"" << std::endl;

  fChannels.at("data-out").at(0).Send(msg);

  cout << "Run()" << endl;
}

} // namespace Core
} // namespace QualityControl
} // namespace AliceO2

