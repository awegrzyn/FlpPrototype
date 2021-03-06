/// \file O2EPNex.cxx
/// \brief FLP device using FairMQ and data format specified in DataBlock.h
///
/// \author D. Klein, A. Rybalchenko, M.Al-Turany
/// \modifed by Adam Wegrzynek (21.08.2015)

#include <boost/thread.hpp>
#include <boost/bind.hpp>

#include "O2EPNex.h"
#include "FairMQLogger.h"

O2EPNex::O2EPNex()
{
}

/// Structure definied in DataBlock.h is used instead of Content from O2FLPex.h
/// Inside of infinite loop it:
///  - reads the message via FairMQ
///  - sets the header and data pointers
void O2EPNex::Run()
{
  while (GetCurrentState() == RUNNING) {
    FairMQMessage* msg = fTransportFactory->CreateMessage();
    fChannels["data-in"].at(0).Receive(msg);
	char* buffer = (char*) msg->GetData();
	//LOG(INFO) << input->header->blockType << " " << input->header->headerSize << " " << input->header->dataSize;
	delete msg;
  }
}

O2EPNex::~O2EPNex()
{
}
