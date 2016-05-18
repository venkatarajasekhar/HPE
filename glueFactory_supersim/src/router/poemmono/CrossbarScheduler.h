/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef ROUTER_POEMMONO_CROSSBARSCHEDULER_H_
#define ROUTER_POEMMONO_CROSSBARSCHEDULER_H_

#include <json/json.h>
#include <prim/prim.h>

#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "allocator/Allocator.h"
#include "event/Component.h"

namespace PoemMono {

class CrossbarScheduler : public Component {
 public:
  /*
   * This class defines the interface required to interact with the
   *  CrossbarScheduler component. Clients will receive a call from the
   *  CrossbarScheduler using the crossbarSchedulerResponse() function.
   *  If a crossbar output port was allocated, it will be given. If no
   *  port was allocated, U32_MAX is returned. If the client will use the
   *  port allocated, then decrementCreditCount() should be called during
   *  the same cycle after/during crossbarSchedulerResponse().
   */
  class Client {
   public:
    Client();
    virtual ~Client();
    virtual void crossbarSchedulerResponse(u32 _port, u32 _vc) = 0;
  };

  // constructor and destructor
  CrossbarScheduler(const std::string& _name, const Component* _parent,
                    u32 _numClients, u32 _numVcs, u32 _numPorts,
                    Json::Value _settings);
  virtual ~CrossbarScheduler();

  // constant attributes
  u32 numClients() const;
  u32 numVcs() const;
  u32 numPorts() const;
  u32 latency() const;

  // links a client to the scheduler
  void setClient(u32 _id, Client* _client);

  // requests to send a flit to a VC
  virtual void request(u32 _client, u32 _port, u32 _vc, u32 _metadata) = 0;

  // credit counts
  virtual void initCreditCount(u32 _vc, u32 _credits) = 0;
  virtual void incrementCreditCount(u32 _vc) = 0;
  virtual u32 getCreditCount(u32 _vc) const = 0;

 protected:
  const u32 numClients_;
  const u32 numVcs_;
  const u32 numPorts_;
  const u32 latency_;
  std::vector<Client*> clients_;
};

}  // namespace PoemMono

#endif  // ROUTER_POEMMONO_CROSSBARSCHEDULER_H_
