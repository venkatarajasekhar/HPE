/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef APPLICATION_TERMINAL_H_
#define APPLICATION_TERMINAL_H_

#include <prim/prim.h>

#include <unordered_set>
#include <string>
#include <vector>

#include "event/Component.h"
#include "types/Message.h"
#include "types/MessageReceiver.h"

class Application;

class Terminal : public Component, public MessageReceiver {
 public:
  Terminal(const std::string& _name, const Component* _parent, u32 _id,
           std::vector<u32> _address);
  virtual ~Terminal();
  u32 getId() const;
  const std::vector<u32>* address() const;
  u32 messagesSent() const;
  u32 messagesReceived() const;
  void setMessageReceiver(MessageReceiver* _receiver);

  /*
   * This message is called by the network when a message is being given
   * to this terminal.
   */
  void receiveMessage(Message* _message) override;

  /*
   * This informs this terminal that the message has entered
   * the network interface. This allows the owner to generate messages
   * back-to-back without being concerned about message queuing.
   */
  virtual void messageEnteredInterface(Message* _message) = 0;

  /*
   * This informs this terminal that the message has exited the network and
   * that this terminal doesn't need to worry about memory deallocation.
   * Overriding implementation must call this!
   */
  virtual void messageExitedNetwork(Message* _message);

  /*
   * This returns the number of messages, packet, and flits that have been send
   * by this terminal but have not yet been received at the destination.
   */
  void enrouteCount(u32* _messages, u32* _packets, u32* _flits) const;

 protected:
  /*
   * This function is used by subclasses when they send a message.
   * This function performs the following (in order):
   *  1. Sets the message's owner (to this).
   *  2. Sets the message's Id.
   *  3. Sets the message's source Id.
   *  4. Sets the message's source address.
   *  5. Sets the message's destination Id.
   *  6. Sets the message's destination address.
   *  7. Sends the message.
   *  8. Adds the message to the internal outstanding messages set.
   * This returns the message Id.
   */
  u32 sendMessage(Message* _message, u32 _destinationId);

  /*
   * This function must be implemented by the subclass. This function is called
   *  right after the terminal receives a message with receiveMessage(). This
   *  class will perform some receive logic then call this subclass function to
   *  handle the message.
   */
  virtual void handleMessage(Message* _message) = 0;

 private:
  u32 id_;
  std::vector<u32> address_;
  MessageReceiver* messageReceiver_;
  u32 messagesSent_;
  u32 messagesReceived_;
  std::unordered_set<Message*> outstandingMessages_;
};

#endif  // APPLICATION_TERMINAL_H_
