/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef ROUTER_COMMON_VCSCHEDULER_H_
#define ROUTER_COMMON_VCSCHEDULER_H_

#include <json/json.h>
#include <prim/prim.h>

#include <string>
#include <unordered_set>
#include <vector>

#include "allocator/Allocator.h"
#include "event/Component.h"

class VcScheduler : public Component {
 public:
  /*
   * This class defines the interface required to interact with the VcScheduler
   *  component. Clients will receive a call from the VcScheduler using the
   *  vcSchedulerResponse() function. If a VC was allocated, it will be given.
   *  If no VC was allocated, U32_MAX is returned.
   */
  class Client {
   public:
    Client();
    virtual ~Client();
    virtual void vcSchedulerResponse(u32 _vc) = 0;
  };

  // constructor and destructor
  VcScheduler(const std::string& _name, const Component* _parent,
              u32 _numClients, u32 _numVcs, Json::Value _settings);
  ~VcScheduler();

  // constant attributes
  u32 numClients() const;
  u32 numVcs() const;

  // links a client to the scheduler
  void setClient(u32 _id, Client* _client);

  // requesting and releasing VCs
  void request(u32 _client, u32 _vc, u64 _metadata);
  void releaseVc(u32 _vc);

  // event processing
  void processEvent(void* _event, s32 _type);

 private:
  const u32 numClients_;
  const u32 numVcs_;

  std::vector<Client*> clients_;
  std::vector<bool> clientRequested_;

  std::vector<bool> vcTaken_;

  bool* requests_;
  u64* metadatas_;
  bool* grants_;
  Allocator* allocator_;
  bool allocEventSet_;

  // this creates an index for requests_, metadatas_, and grants_
  u64 index(u64 _client, u64 _vc) const;
};

#endif  // ROUTER_COMMON_VCSCHEDULER_H_
