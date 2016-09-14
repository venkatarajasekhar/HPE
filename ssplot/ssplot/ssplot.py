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

# Python 3 compatibility
from __future__ import (absolute_import, division,
                        print_function, unicode_literals)
import copy
import gzip
import math
import numpy
import percentile


def maxNoInfinity(v):
  m = float('inf')
  for t in v:
    if not math.isinf(t):
      if math.isinf(m):
        m = t
      else:
        m = max(t, m)
  return m


# a class to represent a stats file in a 2d grid in CSV format
class GridStats(object):

  def __init__(self, filename):
    self.filename = filename
    opener = gzip.open if filename.endswith('.gz') else open
    with opener(filename, 'rb') as fd:
      lines = fd.readlines()
    lines = [line.decode('utf-8') for line in lines]
    rows = []
    for line in lines:
      cols = line.split(',')
      cols = [x.strip() for x in cols]
      rows.append(cols)

    self._map = {}
    headerRow = None
    for ridx, row in enumerate(rows):
      if ridx == 0:
        headerRow = row
        continue
      rowType = None
      for cidx, col in enumerate(row):
        if cidx == 0:
          rowType = col
          self._map[rowType] = {}
        else:
          try:
            val = int(col)
          except ValueError:
            val = float(col)
          self._map[rowType][headerRow[cidx]] = val

  def get(self, row, col):
    try:
      return self._map[row][col]
    except:
      return float('inf')


# a class to represent latency stats
class LatencyStats(object):

  class PlotBounds(object):
    def __init__(self):
      # defaults
      self.spxmin = 0
      self.spxmax = 1
      self.spymin = 0
      self.spymax = 1
      self.ppxmin = 0
      self.ppxmax = 1
      self.ppymin = 0
      self.ppymax = 1
      self.cpxmin = 0
      self.cpxmax = 1
      self.cpymin = 0
      self.cpymax = 1
      self.lpxmin = 0
      self.lpxmax = 1
      self.setmid()
      self.default = True

    def readFile(self, filename):
      grid = GridStats(filename)
      self.spymin = grid.get('spy', 'min')
      self.spymax = grid.get('spy', 'max')
      self.ppxmin = grid.get('ppx', 'min')
      self.ppxmax = grid.get('ppx', 'max')
      self.ppymin = grid.get('ppy', 'min')
      self.ppymax = grid.get('ppy', 'max')
      self.cpxmin = grid.get('cpx', 'min')
      self.cpxmax = grid.get('cpx', 'max')
      self.cpymin = grid.get('cpy', 'min')
      self.cpymax = grid.get('cpy', 'max')
      self.lpxmin = grid.get('lpx', 'min')
      self.lpxmax = grid.get('lpx', 'max')
      self.setmid()
      self.default = False

    def writeFile(self, filename):
      opener = gzip.open if filename.endswith('.gz') else open
      with opener(filename, 'w') as fd:
        print('axis,min,max\n'
              'spy,{0},{1}\n'
              'ppx,{2},{3}\n'
              'ppy,{4},{5}\n'
              'cpx,{6},{7}\n'
              'cpy,{8},{9}\n'
              'lpx,{10},{11}\n'
              .format(self.spymin, self.spymax,
                      self.ppxmin, self.ppxmax, self.ppymin, self.ppymax,
                      self.cpxmin, self.cpxmax, self.cpymin, self.cpymax,
                      self.lpxmin, self.lpxmax),
              file=fd)

    def setmid(self):
      self.spxmid = (self.spxmax - self.spxmin) / 2
      self.spymid = (self.spymax - self.spymin) / 2
      self.ppxmid = (self.ppxmax - self.ppxmin) / 2
      self.ppymid = (self.ppymax - self.ppymin) / 2
      self.cpxmid = (self.cpxmax - self.cpxmin) / 2
      self.cpymid = (self.cpymax - self.cpymin) / 2
      self.lpxmid = (self.lpxmax - self.lpxmin) / 2
      self.default = False

    def greater(self, other):
      # detect defaults
      if self.default == True and other.default == True:
        return LatencyStats.PlotBounds()
      elif self.default == False and other.default == True:
        return copy.deepcopy(self)
      elif self.default == True and other.default == False:
        return copy.deepcopy(other)

      # both are not defaults, do comparison
      new = LatencyStats.PlotBounds()
      new.spymin = min(self.spymin, other.spymin)
      new.spymax = max(self.spymax, other.spymax)
      new.ppxmin = min(self.ppxmin, other.ppxmin)
      new.ppxmax = max(self.ppxmax, other.ppxmax)
      new.ppymin = min(self.ppymin, other.ppymin)
      new.ppymax = max(self.ppymax, other.ppymax)
      new.cpxmin = min(self.cpxmin, other.cpxmin)
      new.cpxmax = max(self.cpxmax, other.cpxmax)
      new.cpymin = min(self.cpymin, other.cpymin)
      new.cpymax = max(self.cpymax, other.cpymax)
      new.lpxmin = min(self.lpxmin, other.lpxmin)
      new.lpxmax = max(self.lpxmax, other.lpxmax)
      new.setmid()
      return new

  def __init__(self, filename):
    # read in raw data
    self.times = []
    self.latencies = []
    opener = gzip.open if filename.endswith('.gz') else open
    with opener(filename, 'rb') as fd:
      while (True):
        line = fd.readline().decode('utf-8')
        delim = line.find(',')
        if (delim >= 0):
          startTime = int(line[:delim])
          endTime = int(line[delim+1:])
          self.times.append(startTime)
          self.latencies.append(endTime - startTime)
        else:
          break
      self.times = numpy.array(self.times)
      self.latencies = numpy.array(self.latencies)

    # size
    self.size = len(self.times)
    if self.size > 0:
      # min and max
      self.tmin = min(self.times)
      self.tmax = max(self.times)
      self.smin = min(self.latencies)
      self.smax = max(self.latencies)
      if self.smin < 0:
        raise Exception('latencies can\'t be negative!')

      # compute the probability density function
      self.pdfBins = 50
      hist, self.pdfx = numpy.histogram(self.latencies, bins=self.pdfBins)
      self.pdfy = hist.astype(float) / hist.sum()

      # compute the cumulative distribution function
      self.cdfx = numpy.sort(self.latencies)
      self.cdfy = numpy.linspace(1.0 / self.size, 1.0, self.size)

      # find percentiles
      self.p50 = self.percentile(0.50)
      self.p90 = self.percentile(0.90)
      self.p99 = self.percentile(0.99)
      self.p999 = self.percentile(0.999)
      self.p9999 = self.percentile(0.9999)

    # set plot boundaries
    self.bounds = LatencyStats.PlotBounds()
    if self.size > 0:
      self.bounds.spxmin = self.tmin
      self.bounds.spxmax = self.tmax
      self.bounds.spymin = max(self.smin, 0)
      self.bounds.spymax = self.smax * 1.01
      self.bounds.ppxmin = self.smin
      self.bounds.ppxmax = self.smax
      self.bounds.ppymin = 0
      self.bounds.ppymax = max(self.pdfy) * 1.01
      self.bounds.cpxmin = self.smin
      self.bounds.cpxmax = self.smax
      self.bounds.cpymin = 0
      self.bounds.cpymax = 1
      self.bounds.lpxmin = self.smin * 0.999
      self.bounds.lpxmax = self.smax * 1.001
      self.bounds.setmid()

  def percentile(self, percent):
    if percent < 0 or percent > 1:
      raise Exception('percent must be between 0 and 1')
    return self.cdfx[int(round(percent * len(self.cdfx)))]

  def nines(self):
    if self.size > 0:
      nines = int(math.ceil(math.log10(len(self.cdfx))))
    else:
      nines = 5
    return nines

  def emptyPlot(self, axes, x, y):
    axes.text(x, y, 'Saturated :(', clip_on=False, color='red',
              verticalalignment='center',
              horizontalalignment='center')

  def scatterPlot(self, axes, showPercentiles=False, randomColors=False):
    # format axes
    axes.set_title('Latency scatter')
    axes.set_xlabel('Time')
    axes.set_ylabel('Latency')
    axes.set_xlim(self.bounds.spxmin, self.bounds.spxmax)
    axes.set_ylim(self.bounds.spymin, self.bounds.spymax)
    axes.grid(True)

    # detect non-empty data set
    if self.size > 0:
      # create plot
      if randomColors:
        colors = numpy.random.rand(len(self.times))
      else:
        colors = 'b'
      axes.scatter(self.times, self.latencies, color=colors, s=2)
      if showPercentiles:
        l50, = axes.plot([self.tmin, self.tmax], [self.p50, self.p50],
                         c='r', linewidth=2)
        l90, = axes.plot([self.tmin, self.tmax], [self.p90, self.p90],
                         c='g', linewidth=2)
        l99, = axes.plot([self.tmin, self.tmax], [self.p99, self.p99],
                         c='c', linewidth=2)
        l999, = axes.plot([self.tmin, self.tmax], [self.p999, self.p999],
                          c='m', linewidth=2)
        l9999, = axes.plot([self.tmin, self.tmax], [self.p9999, self.p9999],
                           c='y', linewidth=2)
    else:
      self.emptyPlot(axes, self.bounds.spxmid, self.bounds.spymid)

  def pdfPlot(self, axes, showPercentiles=False):
    # format axes
    axes.set_title('Probability density function')
    axes.set_xlabel('Latency')
    axes.set_ylabel('Probability')
    axes.set_xlim(self.bounds.ppxmin, self.bounds.ppxmax)
    axes.set_ylim(self.bounds.ppymin, self.bounds.ppymax)
    axes.grid(True)

    # detect non-empty data set
    if self.size > 0:
      # create plot
      axes.plot(self.pdfx[:-1], self.pdfy)
      if showPercentiles:
        l50, = axes.plot([self.p50, self.p50], [0, 1], c='r')
        l90, = axes.plot([self.p90, self.p90], [0, 1], c='g')
        l99, = axes.plot([self.p99, self.p99], [0, 1], c='c')
        l999, = axes.plot([self.p999, self.p999], [0, 1], c='m')
        l9999, = axes.plot([self.p9999, self.p9999], [0, 1], c='y')
        axes.legend((l50, l90, l99, l999, l9999),
                    ('50th %ile    ({0})'.format(self.p50),
                     '90th %ile    ({0})'.format(self.p90),
                     '99th %ile    ({0})'.format(self.p99),
                     '99.9th %ile  ({0})'.format(self.p999),
                     '99.99th %ile ({0})'.format(self.p9999)),
                    fontsize=12)
    else:
      self.emptyPlot(axes, self.bounds.ppxmid, self.bounds.ppymid)

  def cdfPlot(self, axes, showPercentiles=False):
    # format axes
    axes.set_title('Cumulative distribution function')
    axes.set_xlabel('Latency')
    axes.set_ylabel('Probability')
    axes.set_xlim(self.bounds.cpxmin, self.bounds.cpxmax)
    axes.set_ylim(self.bounds.cpymin, self.bounds.cpymax)
    axes.grid(True)

    # detect non-empty data set
    if self.size > 0:
      # create plot
      axes.plot(self.cdfx, self.cdfy)
      if showPercentiles:
        axes.plot([self.p50, self.p50], [0, 0.50], c='r')
        axes.plot([self.bounds.cpxmin, self.p50], [0.50, 0.50], c='r')
        axes.plot([self.p90, self.p90], [0, 0.90], c='g')
        axes.plot([self.bounds.cpxmin, self.p90], [0.90, 0.90], c='g')
        axes.plot([self.p99, self.p99], [0, 0.99], c='c')
        axes.plot([self.bounds.cpxmin, self.p99], [0.99, 0.99], c='c')
        axes.plot([self.p999, self.p999], [0, 0.999], c='m')
        axes.plot([self.bounds.cpxmin, self.p999], [0.999, 0.999], c='m')
        axes.plot([self.p9999, self.p9999], [0, 0.9999], c='y')
        axes.plot([self.bounds.cpxmin, self.p9999], [0.9999, 0.9999], c='y')
    else:
      self.emptyPlot(axes, self.bounds.cpxmid, self.bounds.cpymid)

  def cdfLogPlot(self, axes, xlog=False):
    # format axes
    axes.set_title('Logarithmic cumulative distribution function')
    axes.set_xlabel('Latency')
    axes.set_ylabel('Percentile')
    axes.set_xlim(self.bounds.lpxmin, self.bounds.lpxmax)
    axes.set_yscale('percentile', nines=self.nines())
    axes.grid(True)
    if xlog:
      axes.set_xscale('log')

    # detect non-empty data set
    if self.size > 0:
      # create the plot
      axes.scatter(self.cdfx, self.cdfy, color='b', s=2)
    else:
      self.emptyPlot(axes, self.bounds.lpxmid, 0.999)

  def quadPlot(self, plt, filename, title='',
               spxmin=float('Nan'), spxmax=float('NaN'),
               spymin=float('Nan'), spymax=float('NaN'),
               ppxmin=float('Nan'), ppxmax=float('NaN'),
               ppymin=float('Nan'), ppymax=float('NaN'),
               cpxmin=float('Nan'), cpxmax=float('NaN'),
               cpymin=float('Nan'), cpymax=float('NaN'),
               lpxmin=float('Nan'), lpxmax=float('NaN')):
    if not math.isnan(spxmin):
      self.bounds.spxmin = spxmin
    if not math.isnan(spxmax):
      self.bounds.spxmax = spxmax
    if not math.isnan(spymin):
      self.bounds.spymin = spymin
    if not math.isnan(spymax):
      self.bounds.spymax = spymax

    if not math.isnan(ppxmin):
      self.bounds.ppxmin = ppxmin
    if not math.isnan(ppxmax):
      self.bounds.ppxmax = ppxmax
    if not math.isnan(ppymin):
      self.bounds.ppymin = ppymin
    if not math.isnan(ppymax):
      self.bounds.ppymax = ppymax

    if not math.isnan(cpxmin):
      self.bounds.cpxmin = cpxmin
    if not math.isnan(cpxmax):
      self.bounds.cpxmax = cpxmax
    if not math.isnan(cpymin):
      self.bounds.cpymin = cpymin
    if not math.isnan(cpymax):
      self.bounds.cpymax = cpymax

    if not math.isnan(lpxmin):
      self.bounds.lpxmin = lpxmin
    if not math.isnan(lpxmax):
      self.bounds.lpxmax = lpxmax

    self.bounds.setmid()

    fig = plt.figure(figsize=(16, 10))
    ax1 = fig.add_subplot(2, 2, 1)
    ax2 = fig.add_subplot(2, 2, 2)
    ax3 = fig.add_subplot(2, 2, 3)
    ax4 = fig.add_subplot(2, 2, 4)

    self.scatterPlot(ax1, showPercentiles=True, randomColors=False)
    self.pdfPlot(ax2, showPercentiles=True)
    self.cdfPlot(ax3, showPercentiles=True)
    self.cdfLogPlot(ax4, xlog=False)

    fig.tight_layout()
    if title:
      fig.suptitle(title, fontsize=20)
      fig.subplots_adjust(top=0.92)
    fig.savefig(filename)


 # a class to represent load vs. latency stats
class LoadLatencyStats(object):

  FIELDS = ['Minimum', 'Mean', 'Median', '90th%', '99th%', '99.9th%',
            '99.99th%', '99.999th%', 'Maximum']

  class PlotBounds(object):
    def __init__(self):
      # defaults
      self.ymin = float('inf')
      self.ymax = float('inf')
      self.default = True

    def load(ymin=float('inf'), ymax=float('inf')):
      self.ymin = ymin
      self.ymax = ymax
      self.default = ymin is not float('inf') or ymax is not float('inf')

    def readFile(self, filename):
      grid = GridStats(filename)
      self.ymin = grid.get('y', 'min')
      self.ymax = grid.get('y', 'max')
      self.default = False

    def writeFile(self, filename):
      opener = gzip.open if filename.endswith('.gz') else open
      with opener(filename, 'w') as fd:
        print('axis,min,max\n'
              'y,{0},{1}\n'
              .format(self.ymin, self.ymax),
              file=fd)

    def greater(self, other):
      # detect defaults
      if self.default == True and other.default == True:
        return LoadLatencyStats.PlotBounds()
      elif self.default == False and other.default == True:
        return copy.deepcopy(self)
      elif self.default == True and other.default == False:
        return copy.deepcopy(other)

      # both are not defaults, do comparison
      new = LoadLatencyStats.PlotBounds()
      new.ymin = min(self.ymin, other.ymin)
      assert new.ymin <= self.ymin
      assert new.ymin <= other.ymin
      new.ymax = maxNoInfinity([self.ymax, other.ymax])
      new.default = False
      return new


  def __init__(self, start, stop, step, grids, **kwargs):
    # create arrays
    load = numpy.arange(start, stop, step)
    self.data = {'Load': load}
    for field in LoadLatencyStats.FIELDS:
      self.data[field] = numpy.empty(len(load), dtype=float)

    # parse kwargs
    verbose = kwargs.get('verbose', False);
    statRow = kwargs.get('row', 'Packet')
    if verbose:
      print('load {0}'.format(self.data['Load']))
      print('analyzing {0}s'.format(statRow))

    assert len(grids) == len(self.data['Load']), "wrong number of grids"

    # load data arrays
    for idx, grid in enumerate(grids):
      assert type(grid) == GridStats, "'grid' elements must be GridStats"
      if verbose:
        print('extracting {0}'.format(grid.filename))
      for key in self.data.keys():
        if key != 'Load':
          s = grid.get(statRow, key)
          if verbose:
            print('Load {0} {1} is {2}'.format(self.data['Load'][idx], key, s))
          self.data[key][idx] = s

    self.bounds = LoadLatencyStats.PlotBounds()

    self.bounds.ymin = min(self.data['Minimum'])
    self.bounds.ymax = maxNoInfinity(self.data['Maximum'])

  def plotAll(self, plt, filename, title='',
              ymin=float('Nan'), ymax=float('NaN')):
    if not math.isnan(ymin):
      self.bounds.ymin = ymin
    if not math.isnan(ymax):
      self.bounds.ymax = ymax

    # create figure
    fig = plt.figure(figsize=(16, 10))
    ax1 = fig.add_subplot(1, 1, 1)

    # create a colors list from a colormap
    lineCount = 9
    cmap = plt.get_cmap('gist_rainbow')
    colors = [cmap(idx) for idx in numpy.linspace(0, 1, lineCount)]

    # set axis labels
    ax1.set_xlabel('Load')
    ax1.set_ylabel('Latency')

    # plot load vs. latency curves
    lines = []
    for idx, field in enumerate(reversed(LoadLatencyStats.FIELDS)):
      lines.append(ax1.plot(self.data['Load'], self.data[field],
                            color=colors[idx], lw=1, label=field)[0])

    # if given, apply title
    if title:
      ax1.set_title(title, fontsize=20)

    # create legend
    labels = [line.get_label() for line in lines]
    ax1.legend(lines, labels, loc='upper left', fancybox=True, shadow=True,
               ncol=1)

    # set plot bounds
    ax1.set_xlim(self.data['Load'][0], self.data['Load'][-1]);
    ax1.set_ylim(self.bounds.ymin, self.bounds.ymax);
    ax1.xaxis.grid(True)
    ax1.yaxis.grid(True)

    fig.tight_layout()
    fig.savefig(filename)

  @staticmethod
  def plotCompare(plt, filename, stats, field='Mean', labels=[], title='',
                  ymin=float('NaN'), ymax=float('NaN')):
    # make sure the loads are all the same
    mload = stats[0].data['Load']
    for stat in stats:
      assert len(mload) == len(set(mload).intersection(stat.data['Load'])), \
        print('{0} != {1}'.format(mload, stat.data['Load']))
    assert len(labels) == 0 or len(labels) == len(stats)
    assert field in LoadLatencyStats.FIELDS

    # create figure
    fig = plt.figure(figsize=(16, 10))
    ax1 = fig.add_subplot(1, 1, 1)

    # create a colors list from a colormap
    lineCount = len(stats)
    cmap = plt.get_cmap('gist_rainbow')
    colors = [cmap(idx) for idx in numpy.linspace(0, 1, lineCount)]

    # set axis labels
    ax1.set_xlabel('Load')
    ax1.set_ylabel('{0} Latency'.format(field))

    # plot all lines
    lines = []
    for idx, stat in enumerate(stats):
      label = None
      if len(labels) > 0:
        label = labels[idx]
      line, = ax1.plot(mload, stat.data[field], color=colors[idx], lw=1,
                       label=label)
      lines.append(line)

    # if given, apply title
    if title:
      ax1.set_title(title, fontsize=20)

    # create legend
    if len(labels) > 0:
      labels = [line.get_label() for line in lines]
      ax1.legend(lines, labels, loc='upper left', fancybox=True, shadow=True,
                 ncol=1)

    # set plot bounds
    ax1.set_xlim(stats[0].data['Load'][0], stats[0].data['Load'][-1]);
    if not math.isnan(ymin) and not math.isnan(ymax):
      ax1.set_ylim(ymin, ymax)
    elif not math.isnan(ymin):
      ax1.set_ylim(bottom=ymin)
    elif not math.isnan(ymax):
      ax1.set_ylim(top=ymax)
    ax1.grid(True)

    fig.tight_layout()
    fig.savefig(filename)
