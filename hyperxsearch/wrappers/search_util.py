import argparse
import os
import subprocess


def getInfo(exe, maxradix, minterminals, minbandwidth, mindimensions,
            maxdimensions, minconcentration, maxconcentration):
  cmd = ('{0} --maxradix {1} --minterminals {2} --minbandwidth {3} '
         '--maxdimensions {4} --maxresults 1').format(
           exe, maxradix, minterminals, minbandwidth, maxdimensions)
  if mindimensions:
    cmd += ' --mindimensions {0}'.format(mindimensions)
  if minconcentration:
    cmd += ' --minconcentration {0}'.format(minconcentration)
  if maxconcentration:
    cmd += ' --maxconcentration {0}'.format(maxconcentration)
  stdout = subprocess.check_output(cmd, shell=True).decode('utf-8')
  lines = stdout.split('\n')
  if len(lines) < 3:
    return None
  else:
    return lines[1].split()


def findLargestNetwork(exe, maxradix, minbandwidth, mindimensions,
                       maxdimensions, minconcentration, maxconcentration,
                       verbose):
  if verbose:
    print('exe={0} maxradix={1} minbandwidth={2} mindimensions={3} '
          'maxdimensions={4}'
          .format(exe, maxradix, minbandwidth, mindimensions, maxdimensions,
                  minconcentration, maxconcentration))

  best = None

  bot = 2
  top = maxradix**(maxdimensions+1)

  # verify top is unachievable
  info = getInfo(exe, maxradix, top, minbandwidth, mindimensions,
                 maxdimensions, minconcentration, maxconcentration)
  assert info is None, 'The programmer is an idiot!'

  # use a binary search to find the largest network possible
  while True:
    assert bot <= top
    mid = ((top - bot) // 2) + bot
    info = getInfo(exe, maxradix, mid, minbandwidth, mindimensions,
                   maxdimensions, minconcentration, maxconcentration)

    if verbose:
      print('bot={0} top={1} mid={2} solution={3}'.format(
        bot, top, mid, False if info is None else True))

    if bot == mid:
      assert info is not None
      best = info
      break

    if info is None:
      top = mid - 1
    else:
      bot = mid

  return best

def makeGrid(grid):
  # find max width for each column
  widths = [0] * len(grid[0])
  for colIdx in range(len(grid[0])):
    for rowIdx in range(len(grid)):
      if len(grid[rowIdx][colIdx]) > widths[colIdx]:
        widths[colIdx] = len(grid[rowIdx][colIdx])

  # make the string
  gstr = ''
  for rowIdx in range(len(grid)):
    for colIdx in range(len(grid[0])):
      gstr += (grid[rowIdx][colIdx] +
               (' ' * (widths[colIdx] - len(grid[rowIdx][colIdx]))) +
              ' ')
    if rowIdx < len(grid) - 1:
      gstr += '\n'
  return gstr
