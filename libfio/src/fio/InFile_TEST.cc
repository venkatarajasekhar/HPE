/*
 * Copyright (c) 2016, Nic McDonald
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * - Neither the name of prim nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include "fio/InFile.h"

#include <gtest/gtest.h>

#include "fio/util_TEST.h"

TEST(InFile, compressedTest) {
  std::string fr = "/tmp/myoutfile.txt";
  std::string fc = fr + ".gz";
  fio::InFile* infile;

  // regular
  std::remove(fr.c_str());
  ASSERT_FALSE(exists(fr.c_str()));
  stringToFile("hello\nworld\n\n\ntail", fr.c_str());
  ASSERT_TRUE(exists(fr.c_str()));
  infile = new fio::InFile(fr.c_str());
  ASSERT_FALSE(infile->compressed());
  ASSERT_TRUE(exists(fr.c_str()));
  delete infile;
  ASSERT_TRUE(exists(fr.c_str()));
  std::remove(fr.c_str());
  ASSERT_FALSE(exists(fr.c_str()));

  // compressed
  std::remove(fr.c_str());
  std::remove(fc.c_str());
  ASSERT_FALSE(exists(fr.c_str()));
  ASSERT_FALSE(exists(fc.c_str()));
  stringToFile("hello\nworld\n\n\ntail", fr.c_str());
  ASSERT_TRUE(exists(fr.c_str()));
  int res = std::system(std::string("gzip -f " + fr).c_str());
  ASSERT_EQ(res, 0);
  ASSERT_TRUE(exists(fc.c_str()));
  infile = new fio::InFile(fc.c_str());
  ASSERT_TRUE(infile->compressed());
  ASSERT_TRUE(exists(fc.c_str()));
  delete infile;
  ASSERT_TRUE(exists(fc.c_str()));
  std::remove(fr.c_str());
  std::remove(fc.c_str());
  ASSERT_FALSE(exists(fr.c_str()));
  ASSERT_FALSE(exists(fc.c_str()));
}

void infileComparisionTest(const char* _regName, bool _compress,
                           const std::string& _contents, u64 _blockSize,
                           const std::vector<std::string>& _lines,
                           bool _verbose = false) {
  std::string fr = _regName;
  std::string fc = fr + ".gz";
  fio::InFile* infile;

  std::remove(fr.c_str());
  ASSERT_FALSE(exists(fr.c_str()));
  stringToFile(_contents, fr.c_str());
  ASSERT_TRUE(exists(fr.c_str()));

  int res = std::system(std::string("gzip -k -f " + fr).c_str());
  ASSERT_EQ(res, 0);
  ASSERT_TRUE(exists(fc.c_str()));

  std::string fi = _compress ? fc : fr;
  infile = new fio::InFile(fi.c_str(), '\n', _blockSize);
  ASSERT_EQ(infile->compressed(), _compress);
  ASSERT_TRUE(exists(fr.c_str()));
  ASSERT_TRUE(exists(fc.c_str()));

  std::vector<std::string> lines;
  while (true) {
    std::string line;
    fio::InFile::Status sts = infile->getLine(&line);
    ASSERT_NE(sts, fio::InFile::Status::ERROR);
    if (sts == fio::InFile::Status::END) {
      ASSERT_EQ(line, "");
      break;
    }
    if (sts == fio::InFile::Status::OK) {
      lines.push_back(line);
      if (_verbose) {
        printf("LINE: '%s'\n", line.c_str());
      }
    }
  }

  ASSERT_EQ(lines.size(), _lines.size());
  assert(lines.size() == _lines.size());
  for (u32 idx = 0; idx < lines.size(); idx++) {
    ASSERT_EQ(lines.at(idx), _lines.at(idx));
  }

  delete infile;
  ASSERT_TRUE(exists(fr.c_str()));
  std::remove(fr.c_str());
  ASSERT_FALSE(exists(fr.c_str()));
  ASSERT_TRUE(exists(fc.c_str()));
  std::remove(fc.c_str());
  ASSERT_FALSE(exists(fc.c_str()));
}

TEST(InFile, single) {
  for (u8 compress = 0; compress < 1; compress++) {
    for (u64 blockSize = 10; blockSize < 11; blockSize++) {
      infileComparisionTest(
          "/tmp/myoutfile.txt",
          compress,
          "hello\nworld\n\n\ntail",
          blockSize,
          std::vector<std::string>({"hello", "world", "", "", "tail"}),
          false);
    }
  }
}

TEST(InFile, simple) {
  for (u8 compress = 0; compress < 2; compress++) {
    for (u64 blockSize = 10; blockSize < 123; blockSize++) {
      infileComparisionTest(
          "/tmp/myoutfile.txt",
          compress,
          "hello\nworld\n\n\ntail",
          blockSize,
          std::vector<std::string>({"hello", "world", "", "", "tail"}));
    }
  }
}

TEST(InFile, complex) {
  for (u8 compress = 0; compress < 2; compress++) {
    for (u64 blockSize = 100; blockSize < 231; blockSize++) {
      infileComparisionTest(
          "/tmp/myoutfile.txt",
          compress,
          "this is a bigger example        \n"
          "      I love white    space in weird locations    \n"
          "\n"
          "   \n"
          "\n"
          "    ",
          blockSize,
          std::vector<std::string>({"this is a bigger example        ",
                  "      I love white    space in weird locations    ",
                  "", "   ", "", "    "}));
    }
  }
}
