/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "application/simplemem/MemoryTerminal.h"

#include <cassert>
#include <cstring>

#include "event/Simulator.h"
#include "types/Message.h"
#include "types/Packet.h"
#include "types/Flit.h"
#include "random/Random.h"
#include "application/simplemem/Application.h"
#include "application/simplemem/MemoryOp.h"

namespace SimpleMem {

MemoryTerminal::MemoryTerminal(
    const std::string& _name, const Component* _parent, u32 _id,
    std::vector<u32> _address, u32 _memorySlice, Json::Value _settings)
    : ::Terminal(_name, _parent, _id, _address),
      fsm_(eState::kWaiting) {
  memoryOffset_ = (_id / 2) * _memorySlice;
  memory_ = new u8[_memorySlice];
  latency_ = _settings["latency"].asUInt();
  assert(latency_ > 0);
}

MemoryTerminal::~MemoryTerminal() {
  delete [] memory_;
}

void MemoryTerminal::processEvent(void* _event, s32 _type) {
  sendMemoryResponse();
}

void MemoryTerminal::handleMessage(Message* _message) {
  dbgprintf("received message");

  // log the message
  Application* app = reinterpret_cast<Application*>(gSim->getApplication());
  app->getMessageLog()->logMessage(_message);

  // verify the message
  MemoryOp* memOp = reinterpret_cast<MemoryOp*>(_message->getData());
  assert(memOp != nullptr);
  assert(memOp->blockSize() == app->blockSize());

  // perform memory access
  u32 address = memOp->address();
  if ((memOp->op() == MemoryOp::eOp::kReadReq) ||
      (memOp->op() == MemoryOp::eOp::kWriteReq)) {
    if ((address >= memoryOffset_) &&
        (address < (memoryOffset_ + app->memorySlice()))) {
      messages_.push(_message);
    } else {
      assert(false);  // not good address
    }
  } else {
    assert(false);  // not read or write
  }

  if (fsm_ == eState::kWaiting) {
    startMemoryAccess();
  }
}

void MemoryTerminal::messageEnteredInterface(Message* _message) {
}

void MemoryTerminal::messageExitedNetwork(Message* _message) {
  // any override of this function must call the base class's function
  ::Terminal::messageExitedNetwork(_message);
}

void MemoryTerminal::startMemoryAccess() {
  addEvent(gSim->futureCycle(latency_), 0, nullptr, 0);
  fsm_ = eState::kAccessing;
}

void MemoryTerminal::sendMemoryResponse() {
  // get the next request
  Message* request = messages_.front();
  messages_.pop();
  MemoryOp* memOpReq = reinterpret_cast<MemoryOp*>(request->getData());
  assert(memOpReq != nullptr);
  MemoryOp::eOp reqOp = memOpReq->op();

  Application* app = reinterpret_cast<Application*>(gSim->getApplication());
  u32 numVcs = app->numVcs();
  u32 blockSize = app->blockSize();
  u32 bytesPerFlit = app->bytesPerFlit();
  u32 headerOverhead = app->headerOverhead();
  u32 maxPacketSize = app->maxPacketSize();

  // get a pointer into the memory array
  u32 address = memOpReq->address();
  address &= ~(blockSize - 1);  // align to blockSize
  u8* memoryData = memory_ + (address - memoryOffset_);

  // create the response
  MemoryOp::eOp respOp = reqOp == MemoryOp::eOp::kReadReq ?
                         MemoryOp::eOp::kReadResp : MemoryOp::eOp::kWriteResp;
  MemoryOp* memOpResp = new MemoryOp(respOp, address,
                                     (reqOp == MemoryOp::eOp::kReadReq ?
                                      blockSize : 0));

  // determine the message length
  //  perform memory operation
  u32 messageLength = headerOverhead + 1 + sizeof(u32);
  if (reqOp == MemoryOp::eOp::kReadReq) {
    messageLength += blockSize;
    memcpy(memOpResp->block(), memoryData, blockSize);
  } else {
    memcpy(memoryData, memOpReq->block(), blockSize);
  }
  messageLength /= bytesPerFlit;
  u32 numPackets = messageLength / maxPacketSize;
  if ((messageLength % maxPacketSize) > 0) {
    numPackets++;
  }

  // create the outgoing message, packets, and flits
  Message* response = new Message(numPackets, memOpResp);
  u32 flitsLeft = messageLength;
  for (u32 p = 0; p < numPackets; p++) {
    u32 packetLength = flitsLeft > maxPacketSize ? maxPacketSize : flitsLeft;
    Packet* packet = new Packet(p, packetLength, response);
    response->setPacket(p, packet);

    packet->setDeadline(gSim->time() + 10000);

    u32 vc = gRandom->randomU64(0, numVcs - 1);
    for (u32 f = 0; f < packetLength; f++) {
      bool headFlit = f == 0;
      bool tailFlit = f == (packetLength - 1);
      Flit* flit = new Flit(f, headFlit, tailFlit, packet);
      flit->setVc(vc);
      packet->setFlit(f, flit);
    }
    flitsLeft -= packetLength;
  }

  // send the response to the requester
  u32 requesterId = request->getSourceId();
  assert((requesterId & 0x1) == 1);
  dbgprintf("sending %s response to %u (address %u)",
            (respOp == MemoryOp::eOp::kWriteResp) ?
            "write" : "read", requesterId, address);
  sendMessage(response, requesterId);

  // delete the request
  delete memOpReq;
  delete request;

  // if more memory requests are outstanding, continue accessing memory
  if (messages_.size() > 0) {
    startMemoryAccess();
  } else {
    fsm_ = eState::kWaiting;
  }
}

}  // namespace SimpleMem
