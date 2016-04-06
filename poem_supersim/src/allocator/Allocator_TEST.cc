/*
 * Copyright (c) 2013-2015, Hewlett-Packard Laboratories, Nic McDonald
 * See LICENSE file for details
 */
#include "allocator/Allocator_TEST.h"

#include <gtest/gtest.h>

#include <sstream>
#include <string>

#include "allocator/Allocator.h"
#include "allocator/AllocatorFactory.h"
#include "random/Random.h"

#include "test/TestSetup_TEST.h"

u64 AllocatorIndex(u64 _numClients, u64 _client, u64 _resource) {
  return _numClients * _resource + _client;
}

static std::string ppp(bool* _bools, u64 _len) {
  std::stringstream ss;
  ss << '[';
  while (_len > 2) {
    ss << (*_bools ? '1' : '0');
    _len--;
    _bools++;
  }
  ss << (*_bools ? '1' : '0') << ']';
  return ss.str();
}

void AllocatorTest(Json::Value _settings, AllocatorVerifier _verifier,
                   bool _singleRequest) {
  for (u32 C = 1; C < 16; C++) {
    for (u32 R = 1; R < 16; R++) {
      TestSetup testSetup(1, 123);

      // printf("C=%u R=%u\n", C, R);
      bool* request = new bool[C * R];
      bool* requestDup = new bool[C * R];
      u64* metadata = new u64[C * R];
      bool* grant = new bool[C * R];

      // create the allocator
      Allocator* alloc = AllocatorFactory::createAllocator(
          "Alloc", nullptr, C, R, _settings);

      // map I/O to the allocator
      for (u32 c = 0; c < C; c++) {
        for (u32 r = 0; r < R; r++) {
          u64 idx = AllocatorIndex(C, c, r);
          alloc->setRequest(c, r, &request[idx]);
          alloc->setMetadata(c, r, &metadata[idx]);
          alloc->setGrant(c, r, &grant[idx]);
        }
      }

      // run the allocator with random inputs
      for (u32 run = 0; run < 100; run++) {
        // randomize the inputs
        for (u32 c = 0; c < C; c++) {
          u32 reqCount = 0;
          do {
            u32 single = gRandom->randomU64(0, R);
            for (u32 r = 0; r < R; r++) {
              u64 idx = AllocatorIndex(C, c, r);
              metadata[idx] = 10000 + c;
              if (_singleRequest) {
                request[idx] = idx == single;
              } else {
                request[idx] = gRandom->randomBool();
              }
              requestDup[idx] = request[idx];
              if (request[idx]) {
                reqCount++;
              }
            }
          } while (!_singleRequest && reqCount <= R/3);
        }

        // clear the grants
        for (u32 c = 0; c < C; c++) {
          for (u32 r = 0; r < R; r++) {
            grant[AllocatorIndex(C, c, r)] = false;
          }
        }

        // allocate
        if (false) {
          printf("r %s\n", ppp(request, C*R).c_str());
        }
        alloc->allocate();
        if (false) {
          printf("g %s\n", ppp(grant, C*R).c_str());
        }

        // verify that if something was granted, it was requested
        for (u32 c = 0; c < C; c++) {
          for (u32 r = 0; r < R; r++) {
            u64 idx = AllocatorIndex(C, c, r);
            if (grant[idx]) {
              ASSERT_TRUE(requestDup[idx]);
            }
          }
        }

        // verify
        if (_verifier) {
          _verifier(C, R, request, metadata, grant);
        }
      }

      // cleanup
      delete[] request;
      delete[] requestDup;
      delete[] metadata;
      delete[] grant;
      delete alloc;
    }
  }
}
