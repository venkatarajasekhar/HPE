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

import numpy as np
from numpy import ma
from matplotlib import scale as mscale
from matplotlib import transforms as mtransforms
from matplotlib.ticker import FixedFormatter, FixedLocator


class PercentileTransform(mscale.ScaleBase):
  name = 'percentile'

  def __init__(self, axis, **kwargs):
    mscale.ScaleBase.__init__(self)
    self.nines = kwargs.get('nines', 5)

  def get_transform(self):
    return self.Transform(self.nines)

  def set_default_locators_and_formatters(self, axis):
    axis.set_major_locator(FixedLocator(
      np.array([1-10**(-k) for k in range(1+self.nines)])))
    axis.set_major_formatter(FixedFormatter(
      [str(1-10**(-k)) for k in range(1+self.nines)]))

  def limit_range_for_scale(self, vmin, vmax, minpos):
    return vmin, min(1 - 10**(-self.nines), vmax)

  class Transform(mtransforms.Transform):
    input_dims = 1
    output_dims = 1
    is_separable = True

    def __init__(self, nines):
      mtransforms.Transform.__init__(self)
      self.nines = nines

    def transform_non_affine(self, a):
      masked = ma.masked_where(a > 1-10**(-1-self.nines), a)
      if masked.mask.any():
        return -ma.log10(1-a)
      else:
        return -np.log10(1-a)

    def inverted(self):
      return PercentileTransform.InvertedTransform(self.nines)

  class InvertedTransform(mtransforms.Transform):
    input_dims = 1
    output_dims = 1
    is_separable = True

    def __init__(self, nines):
      mtransforms.Transform.__init__(self)
      self.nines = nines

    def transform_non_affine(self, a):
      return 1. - 10**(-a)

    def inverted(self):
      return PercentileTransform.Transform(self.nines)

mscale.register_scale(PercentileTransform)
