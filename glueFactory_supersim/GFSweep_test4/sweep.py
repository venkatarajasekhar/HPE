#!/usr/bin/env python3

import argparse
import os
import subprocess
import taskrun


def error(msg, code=-1):
  if msg:
    print('ERROR: {0}'.format(msg))
  exit(code)


def main(args):
  # ensure rundir exists, if not make it
  if not os.path.isdir(args.rundir):
    try:
      os.mkdir(args.rundir)
    except:
      error('couldn\'t create {0}'.format(args.rundir))

  # ensure the supersim environment exists
  if not os.path.isdir(args.ssenv):
    error('{0} does not exist'.format(args.ssenv))

  # create a task manager to handle all tasks
  rm = taskrun.ResourceManager(taskrun.CounterResource('cpus', 9999, args.cpus),
                               taskrun.MemoryResource('mem', 9999, args.mem))
  ob = taskrun.VerboseObserver(description=args.verbose)
  tm = taskrun.TaskManager(resource_manager=rm,
                           observer=ob,
                           failure_mode=taskrun.FailureMode.ACTIVE_CONTINUE)

  # sweep params
  sweepStart = 1
  sweepStop = 99
  sweepStep = 2
  loads = ['{0:.02f}'.format(x/100)
           for x in range(sweepStart, sweepStop+1, sweepStep)]

  # sweep load
  parse_tasks = []
  for load in loads:
    runId = '{0:02d}_{1}'.format(args.queue, load)

    # create file names
    supersim_bin = os.path.join(args.ssenv, 'poem_supersim', 'bin', 'supersim')
    sslatency_bin = os.path.join(args.ssenv, 'sslatency', 'bin', 'sslatency')
    settings_json = os.path.join(args.ssenv, 'poem_supersim', 'json',
                                 'hierarchyhyperx_iq_GlueFactoryTest4.json')
    simout_log = os.path.join(args.rundir, 'simout_{0}.log'.format(runId))
    messages_mpf = os.path.join(args.rundir, 'messages_{0}.mpf'.format(runId))
    rates_csv = os.path.join(args.rundir, 'rates_{0}.csv'.format(runId))
    channels_csv = os.path.join(args.rundir, 'channels_{0}.mpf'.format(runId))
    messages_csv = os.path.join(args.rundir, 'messages_{0}.csv'.format(runId))
    packets_csv = os.path.join(args.rundir, 'packets_{0}.csv'.format(runId))
    aggregate_csv = os.path.join(args.rundir, 'aggregate_{0}.csv'.format(runId))
    packets_png = os.path.join(args.rundir, 'packets_{0}.png'.format(runId))

    # create simulation task
    sim_cmd = ('{0} {1} '
               'application.max_injection_rate=float={2} '
               'application.message_log.file=string={3} '
               'application.rate_log.file=string={4} '
               'network.channel_log.file=string={5} '
               'network.interface.init_credits=uint={6} '
               'network.router.input_queue_depth=uint={6} ').format(
                 supersim_bin, settings_json, load, messages_mpf, rates_csv,
                 channels_csv, args.queue)
    sim_task = taskrun.ProcessTask(tm, 'sim_{0}'.format(runId), sim_cmd)
    sim_task.stdout_file = simout_log
    sim_task.stderr_file = simout_log
    sim_task.resources = {'cpus': 1, 'mem': 10}
    sim_task.add_condition(taskrun.FileModificationCondition(
      [settings_json], [simout_log, messages_mpf, rates_csv, channels_csv]))

    # create parser task
    parse_cmd = '{0} -m {1} -p {2} -a {3} {4}'.format(
      sslatency_bin, messages_csv, packets_csv, aggregate_csv, messages_mpf)
    parse_task = taskrun.ProcessTask(tm, 'parse_{0}'.format(runId), parse_cmd)
    parse_task.resources = {'cpus': 1, 'mem': 2}
    parse_task.add_dependency(sim_task)
    parse_task.add_condition(taskrun.FileModificationCondition(
      [messages_mpf], [messages_csv, packets_csv, aggregate_csv]))
    parse_tasks.append(parse_task)

    # create plot task
    plot_cmd = 'sslqp {0} {1}'.format(packets_csv, packets_png)
    plot_task = taskrun.ProcessTask(tm, 'plot_{0}'.format(runId), plot_cmd)
    plot_task.resources = {'cpus': 1, 'mem': 2}
    plot_task.add_dependency(parse_task)
    plot_task.add_condition(taskrun.FileModificationCondition(
      [packets_csv], [packets_png]))

  # create a task to make a load latency graph
  loadlat_file = os.path.join(args.rundir,
                              'load_latency_{0:02d}.png'.format(args.queue))
  loadlat_cmd = ('ssllp --row Packet --title "Bandwidth Test" '
                 '{1} {2} {3} {4}').format(args.queue, loadlat_file, sweepStart,
                                           sweepStop+1, sweepStep)
  agg_files = []
  for load in loads:
    runId = '{0:02d}_{1}'.format(args.queue, load)
    aggregate_csv = os.path.join(args.rundir, 'aggregate_{0}.csv'.format(runId))
    loadlat_cmd += ' {0}'.format(aggregate_csv)
    agg_files.append(aggregate_csv)
  loadlat_task = taskrun.ProcessTask(tm, 'loadlat_{0}'.format(args.queue),
                                     loadlat_cmd)
  loadlat_task.resources = {'cpus': 1, 'mem': 2}
  for dep in parse_tasks:
    loadlat_task.add_dependency(dep)
  loadlat_task.add_condition(taskrun.FileModificationCondition(
    agg_files, [loadlat_file]))

  # run all tasks
  tm.run_tasks()


if __name__ == '__main__':
  ap = argparse.ArgumentParser()
  ap.add_argument('ssenv',
                  help='location of supersim environment')
  ap.add_argument('rundir',
                  help='location of output run directory')
  ap.add_argument('-c', '--cpus', type=int, default=os.cpu_count(),
                  help='maximum number of cpus to use during run')
  ap.add_argument('-m', '--mem', type=float,
                  default=taskrun.MemoryResource.current_available_memory_gib(),
                  help='maximum amount of memory to use during run')
  ap.add_argument('-q', '--queue', type=int, default=120,
                  help='queue size of router')
  ap.add_argument('-v', '--verbose', action='store_true',
                  help='show all commands')
  args = ap.parse_args()
  exit(main(args))
