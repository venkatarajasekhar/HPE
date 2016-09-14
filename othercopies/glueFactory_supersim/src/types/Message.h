/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef TYPES_MESSAGE_H_
#define TYPES_MESSAGE_H_

#include <prim/prim.h>

#include <vector>

class Packet;
class Terminal;

class Message {
 public:
  Message(u32 _numPackets, void* _data);

  // this deletes all packet data as well (as long as packets aren't nullptr)
  virtual ~Message();

  Terminal* getOwner() const;
  void setOwner(Terminal* _owner);

  u32 getId() const;
  void setId(u32 _id);

  u32 numPackets() const;
  u32 numFlits() const;
  Packet* getPacket(u32 _index) const;
  void setPacket(u32 _index, Packet* _packet);

  void* getData() const;
  void setData(void* _data);

  u32 getSourceId() const;
  void setSourceId(u32 _sourceId);
  u32 getDestinationId() const;
  void setDestinationId(u32 _destinationId);

  void setSourceAddress(const std::vector<u32>* _address);
  const std::vector<u32>* getSourceAddress() const;

  void setDestinationAddress(const std::vector<u32>* _address);
  const std::vector<u32>* getDestinationAddress() const;

 private:
  Terminal* owner_;
  u32 id_;
  std::vector<Packet*> packets_;
  void* data_;

  u32 sourceId_;
  u32 destinationId_;

  const std::vector<u32>* sourceAddress_;
  const std::vector<u32>* destinationAddress_;
};

#endif  // TYPES_MESSAGE_H_
