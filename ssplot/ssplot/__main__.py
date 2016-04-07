"""
 * Copyright (c) 2012-2016, Nic McDonald
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
"""

import os
if os.environ['DISPLAY'] == '':
  import matplotlib
  matplotlib.use('Agg')
import matplotlib.pyplot as plt
import random
from . import ssplot
import sys
import tempfile


def main():
  print('testing GridStats class...')

  # test a regular file
  sample = ('Type,Count,Minimum,Maximum,Median,90th%,99th%,99.9th%,99.99th%,'
            '99.999th%,Mean,StdDev\n'
            'Packet,544500,20,683,138,204,291,381,468,544,143.731,47.7077\n'
            'Message,544501,21,684,139,205,292,382,469,545,144.731,48.7077\n')
  fd, fname = tempfile.mkstemp()
  with open(fname, 'w') as fd:
    print(sample, file=fd)
  stats = ssplot.GridStats(fname)
  assert stats.get('Packet', 'Count') == 544500
  assert stats.get('Packet', 'Minimum') == 20
  assert stats.get('Packet', 'Maximum') == 683
  assert stats.get('Packet', 'Median') == 138
  assert stats.get('Packet', '90th%') == 204
  assert stats.get('Packet', '99th%') == 291
  assert stats.get('Packet', '99.9th%') == 381
  assert stats.get('Packet', '99.99th%') == 468
  assert stats.get('Packet', '99.999th%') == 544
  assert stats.get('Packet', 'Mean') == 143.731
  assert stats.get('Packet', 'StdDev') == 47.7077
  assert stats.get('Message', 'Count') == 544501
  assert stats.get('Message', 'Minimum') == 21
  assert stats.get('Message', 'Maximum') == 684
  assert stats.get('Message', 'Median') == 139
  assert stats.get('Message', '90th%') == 205
  assert stats.get('Message', '99th%') == 292
  assert stats.get('Message', '99.9th%') == 382
  assert stats.get('Message', '99.99th%') == 469
  assert stats.get('Message', '99.999th%') == 545
  assert stats.get('Message', 'Mean') == 144.731
  assert stats.get('Message', 'StdDev') == 48.7077

  os.remove(fname)

  # test an empty file
  fd, fname = tempfile.mkstemp()
  stats = ssplot.GridStats(fname)
  assert stats.get('Packet', 'Count') == float('inf')
  assert stats.get('Packet', 'Minimum') == float('inf')
  assert stats.get('Packet', 'Maximum') == float('inf')
  assert stats.get('Packet', 'Median') == float('inf')
  assert stats.get('Packet', '90th%') == float('inf')
  assert stats.get('Packet', '99th%') == float('inf')
  assert stats.get('Packet', '99.9th%') == float('inf')
  assert stats.get('Packet', '99.99th%') == float('inf')
  assert stats.get('Packet', '99.999th%') == float('inf')
  assert stats.get('Packet', 'Mean') == float('inf')
  assert stats.get('Packet', 'StdDev') == float('inf')
  assert stats.get('Message', 'Count') == float('inf')
  assert stats.get('Message', 'Minimum') == float('inf')
  assert stats.get('Message', 'Maximum') == float('inf')
  assert stats.get('Message', 'Median') == float('inf')
  assert stats.get('Message', '90th%') == float('inf')
  assert stats.get('Message', '99th%') == float('inf')
  assert stats.get('Message', '99.9th%') == float('inf')
  assert stats.get('Message', '99.99th%') == float('inf')
  assert stats.get('Message', '99.999th%') == float('inf')
  assert stats.get('Message', 'Mean') == float('inf')
  assert stats.get('Message', 'StdDev') == float('inf')

  os.remove(fname)

  print('ok')

if __name__ == '__main__':
  sys.exit(main())
