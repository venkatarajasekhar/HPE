/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "network/Network.h"

#include <cassert>

Network::Network(const std::string& _name, const Component* _parent,
                 Json::Value _settings)
    : Component(_name, _parent),
      numVcs_(_settings["num_vcs"].asUInt()) {
  // check settings
  assert(numVcs_ > 0);

  // create a channel log object
  channelLog_ = new ChannelLog(_settings["channel_log"]);
}

Network::~Network() {
  delete channelLog_;
}

u32 Network::numVcs() const {
  return numVcs_;
}

void Network::startMonitoring() {
  std::vector<Channel*> channels;
  collectChannels(&channels);
  for (auto it = channels.begin(); it != channels.end(); ++it) {
    Channel* c = *it;
    c->startMonitoring();
  }
}

void Network::endMonitoring() {
  std::vector<Channel*> channels;
  collectChannels(&channels);
  for (auto it = channels.begin(); it != channels.end(); ++it) {
    Channel* c = *it;
    c->endMonitoring();
    channelLog_->logChannel(c);
  }
}
