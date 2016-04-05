/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#ifndef ALLOCATOR_ALLOCATOR_TEST_H_
#define ALLOCATOR_ALLOCATOR_TEST_H_

#include <json/json.h>
#include <prim/prim.h>

typedef void(*AllocatorVerifier)(u32 _numClients, u32 _numResources,
                                 const bool* _request, const u64* _metadata,
                                 const bool* _grant);

u64 AllocatorIndex(u64 _numClients, u64 _client, u64 _resource);

void AllocatorTest(Json::Value _settings, AllocatorVerifier _verifier,
                   bool _singleRequest);

#endif  // ALLOCATOR_ALLOCATOR_TEST_H_
