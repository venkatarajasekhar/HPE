/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef TYPES_MESSAGEOWNER_H_
#define TYPES_MESSAGEOWNER_H_

class Message;

class MessageOwner {
 public:
  MessageOwner();
  virtual ~MessageOwner();

  /*
   * This informs the message owner that the message has entered
   * the network interface. This allows the owner to generate messages
   * back-to-back without being concerned about message queuing.
   */
  virtual void messageEnteredInterface(Message* _message) = 0;

  /*
   * This informs the current message owner that a new message owner
   * is being assigned and that this owner doesn't need to worry about
   * memory deallocation.
   */
  virtual void messageDelivered(Message* _message) = 0;
};

#endif  // TYPES_MESSAGEOWNER_H_
