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
import os
from .Condition import Condition


class FileModificationCondition(Condition):
  """
  This class uses lists of files to determine if a task should run. This
  condition takes a set of input files that are file based dependencies. This
  condition will have the task run if any of the input files are newer than the
  output files. This condition takes a set of output files which the task would
  create or modify if it ran. This condition will have the task run if any of
  the input or output files do not exist.
  """

  def __init__(self, inputs=[], outputs=[]):
    """
    This constructs a FileModificationCondition object.

    Args:
      inputs (list<str>)           : a list of filenames for the input files
      outputs (list<str>)          : a list of filenames for the output files
    """
    super(FileModificationCondition, self).__init__()
    self._inputs = inputs
    self._outputs = outputs

  def addInput(self, filename):
    """
    This adds a new file to the input file list

    Args:
      filename (str) : the filename to be added
    """
    self._inputs.append(filename)

  def addOutput(self, filename):
    """
    This adds a new file to the output file list

    Args:
      filename (str) : the filename to be added
    """
    self._outputs.append(filename)

  def check(self):
    """
    See Condition.check()
    This implementation will return True if any of the output files do not
    exist or if the input files are newer than the outputs
    """

    # check for non-existent output files
    #  get minimum modification time of output files
    mtime = float('INF')
    for ofile in self._outputs:
      if not os.path.isfile(ofile):
        return True
      else:
        mtime = min(mtime, os.path.getmtime(ofile))

    # check whether any input file is newer than the minimum output file
    #  modifcation time
    for ifile in self._inputs:
      if not os.path.isfile(ifile):
        return True
      if os.path.getmtime(ifile) >= mtime:
        return True

    # all tests passed, don't need to run task
    return False
