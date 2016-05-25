/*
 * Copyright (c) 2016, Hewlett-Packard Laboratories, Qi Li
 * See LICENSE file for details
 */
#ifndef TYPES_FLIT_H_
#define TYPES_FLIT_H_

#include <prim/prim.h>
#include <vector>

class Packet;

class Flit {
 public:
  Flit(u32 _id, bool _isHead, bool _isTail, Packet* _packet);
  virtual ~Flit();

  u32 getId() const;
  bool isHead() const;
  bool isTail() const;
  Packet* getPacket() const;

  u32 getVc() const;
  void setVc(u32 vc);

  void setSendTime(u64 time);
  u64 getSendTime() const;
  void setReceiveTime(u64 time);
  u64 getReceiveTime() const;

  void incrementGlobalHopCount();
  u32 getGlobalHopCount() const;
  void recordHop(std::vector<u32> hop);
  std::vector<std::vector<u32> > getHops();
  void setIntermediate(bool _intermediateDone);
  bool getIntermediateDone();

 private:
  u32 id_;
  bool head_;
  bool tail_;
  Packet* packet_;
  u32 vc_;

  u64 sendTime_;
  u64 receiveTime_;

  u32 globalHopCount_;
  std::vector<std::vector<u32> > hops;
  bool intermediateDone_;
};

#endif  // TYPES_FLIT_H_
