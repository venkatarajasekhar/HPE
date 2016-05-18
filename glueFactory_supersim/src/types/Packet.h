/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef TYPES_PACKET_H_
#define TYPES_PACKET_H_

#include <prim/prim.h>

#include <vector>

class Flit;
class Message;

class Packet {
 public:
  Packet(u32 _id, u32 _numFlits, Message* _message);

  // this deletes all flit data as well (if they aren't nullptr)
  virtual ~Packet();

  u32 getId() const;

  u32 numFlits() const;
  Flit* getFlit(u32 _index) const;
  void setFlit(u32 _index, Flit* _flit);

  Message* getMessage() const;

  void incrementHopCount();
  u32 getHopCount() const;

  u64 headLatency() const;
  u64 serializationLatency() const;
  u64 totalLatency() const;

  u64 getDeadline() const;
  void setDeadline(u64 _deadline);

 private:
  u32 id_;
  std::vector<Flit*> flits_;
  Message* message_;

  u32 hopCount_;
  u64 deadline_;  // TODO(nic) is this too specific?
};

#endif  // TYPES_PACKET_H_
