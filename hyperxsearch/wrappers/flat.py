#!/usr/bin/env python3

import argparse
import os
import subprocess
import taskrun
import threading

import search_util

def largestNetwork(exe, maxradix, minbandwidth, mindimensions, maxdimensions,
                   minconcentration, maxconcentration, lock, results,
                   verbose):
  info = search_util.findLargestNetwork(
    exe, maxradix, minbandwidth, mindimensions, maxdimensions, minconcentration,
    maxconcentration, verbose)

  lock.acquire()
  results[maxradix] = info
  lock.release()


def main(args):
  lock = threading.Lock()
  results = {}

  if args.verbose and args.cores > 1:
    print('verbose mode forces cores to 1')
    args.cores = 1
  if args.verbose:
    print('using {0} cores for execution'.format(args.cores))

  rm = taskrun.ResourceManager(
    taskrun.CounterResource('cores', 1, args.cores))
  tm = taskrun.TaskManager(resource_manager=rm, observers=[])

  for radix in range(args.minradix, args.maxradix+1, 1):
    task = taskrun.FunctionTask(
      tm, 'radix_{0}'.format(radix), largestNetwork,
      args.hyperxsearch, radix, args.minbandwidth, None, args.maxdimensions,
      args.minconcentration, args.maxconcentration, lock, results, args.verbose)
    task.priority = radix

  tm.run_tasks()

  grid = []
  grid.append(['#', 'Dimensions', 'Widths', 'Weights', 'Concentration',
               'Terminals', 'Routers', 'Radix', 'Channels', 'Bisections',
               'Cost'])
  for radix in range(args.minradix, args.maxradix+1, 1):
    grid.append(results[radix])
  print(search_util.makeGrid(grid))


if __name__ == '__main__':
  ap = argparse.ArgumentParser()
  ap.add_argument('hyperxsearch',
                  help='hyperxsearch executable')
  ap.add_argument('minradix', type=int,
                  help='minimum radix to search')
  ap.add_argument('maxradix', type=int,
                  help='maximum radix to search')
  ap.add_argument('maxdimensions', type=int,
                  help='maximum number of dimensions')
  ap.add_argument('minbandwidth', type=float,
                  help='minimum bisection bandwidth')
  ap.add_argument('--minconcentration', type=int, default=0,
                  help='minimum concentration')
  ap.add_argument('--maxconcentration', type=int, default=0,
                  help='maximum concentration')
  ap.add_argument('-c', '--cores', type=int, default=os.cpu_count(),
                  help='number of cores to use')
  ap.add_argument('-v', '--verbose', default=False, action='store_true',
                  help='turn on verbose output')
  args = ap.parse_args()

  assert args.minradix <= args.maxradix

  main(args)
