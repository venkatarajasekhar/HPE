/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "stats/MessageLog.h"

#include <string>
#include <sstream>

#include "types/Packet.h"
#include "types/Flit.h"

MessageLog::MessageLog(Json::Value _settings)
    : FileLog(_settings) {}

MessageLog::~MessageLog() {}

void MessageLog::logMessage(const Message* _message) {
  std::stringstream ss;
  ss << "+M" << ',';
  ss << _message->getId() << ',';
  ss << _message->getSourceId() << ',';
  ss << _message->getDestinationId() << '\n';
  for (u32 p = 0; p < _message->numPackets(); p++) {
    Packet* packet = _message->getPacket(p);
    ss << " +P" << ',';
    ss << packet->getId() << ',';
    ss << packet->getHopCount() << '\n';
    for (u32 f = 0; f < packet->numFlits(); f++) {
      Flit* flit = packet->getFlit(f);
      ss << "   F" << ',';
      ss << flit->getId() << ',';
      ss << flit->getSendTime() << ',';
      ss << flit->getReceiveTime() << '\n';
    }
    ss << " -P\n";
  }
  ss << "-M\n";

  write(ss.str());
}
