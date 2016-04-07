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

import math
import matplotlib.pyplot as plt
import numpy as np
import percentile


def auto_nines(v):
  return int(math.ceil(math.log10(len(v))))


# let's pretend we have a bunch of latency samples
mu = 50  # mean
sigma = 5  # standard deviation
latencies = np.random.normal(mu, sigma, 12345)

# compute the CDF
cdfx = np.sort(latencies)
cdfy = np.linspace(1 / len(latencies), 1.0, len(latencies))

# make a figure
fig = plt.figure(figsize=(16,10))

# percentile line plot
ax = fig.add_subplot(2, 2, 1)
ax.plot(cdfx, cdfy, color='b', linewidth=2)
ax.set_xlabel('latency (seconds)')
ax.set_ylabel('percentile')
ax.set_yscale('percentile', nines=auto_nines(cdfy))
ax.grid(True)
ax.set_title('Percentiles (line)')

# percentile scatter plot
ax = fig.add_subplot(2, 2, 2)
ax.scatter(cdfx, cdfy, color='b', s=2)
ax.set_xlabel('latency (seconds)')
ax.set_ylabel('percentile')
ax.set_yscale('percentile', nines=auto_nines(cdfy))
ax.grid(True)
ax.set_title('Percentiles (scatter)')

# histogram
ax = fig.add_subplot(2, 2, 3)
count, bins, ignored = ax.hist(latencies, 30, normed=True)
ax.set_xlabel('latency (seconds)')
ax.set_ylabel('probability')
ax.grid(True)
ax.set_title('Histogram')

# PDF
ax = fig.add_subplot(2, 2, 4)
vals = (1 / (sigma * np.sqrt(2 * np.pi)) *
        np.exp( -(bins - mu)**2 / (2 * sigma**2)))
ax.plot(bins, vals, linewidth=2, color='r')
ax.set_xlabel('latency (seconds)')
ax.set_ylabel('probability')
ax.grid(True)
ax.set_title('Probabilty Density Function (PDF)')

# show the plots
plt.show()
