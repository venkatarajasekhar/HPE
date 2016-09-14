/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef APPLICATION_SIMPLEMEM_APPLICATION_H_
#define APPLICATION_SIMPLEMEM_APPLICATION_H_

#include <json/json.h>
#include <prim/prim.h>

#include <string>

#include "event/Component.h"
#include "application/Application.h"

namespace SimpleMem {

class Application : public ::Application {
 public:
  Application(const std::string& _name, const Component* _parent,
              Json::Value _settings);
  ~Application();
  f64 percentComplete() const override;
  u32 numVcs() const;
  u32 totalMemory() const;
  u32 memorySlice() const;
  u32 blockSize() const;
  u32 bytesPerFlit() const;
  u32 headerOverhead() const;
  u32 maxPacketSize() const;
  void processorComplete(u32 _id);
  void processEvent(void* _event, s32 _type) override;

 private:
  u32 numVcs_;
  u32 totalMemory_;
  u32 memorySlice_;
  u32 blockSize_;
  u32 bytesPerFlit_;
  u32 headerOverhead_;
  u32 maxPacketSize_;

  u32 remainingProcessors_;
};

}  // namespace SimpleMem

#endif  // APPLICATION_SIMPLEMEM_APPLICATION_H_
