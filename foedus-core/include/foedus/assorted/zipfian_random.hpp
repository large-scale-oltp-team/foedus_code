/*
 * Copyright (c) 2014-2015, Hewlett-Packard Development Company, LP.
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details. You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * HP designates this particular file as subject to the "Classpath" exception
 * as provided by HP in the LICENSE.txt file that accompanied this code.
 */
#ifndef FOEDUS_ASSORTED_ZIPFIAN_RANDOM_HPP_
#define FOEDUS_ASSORTED_ZIPFIAN_RANDOM_HPP_

#include <stdint.h>

#include "uniform_random.hpp"
#include "foedus/assert_nd.hpp"
#include "foedus/memory/fwd.hpp"

namespace foedus {
namespace assorted {
/**
 * @brief A simple zipfian generator based off of YCSB's Java implementation.
 * The major user is YCSB. 0 < theta < 1, lower means more skewed.
 * Generates a random number between 0 and max_exclusive_ -1.
 * @ingroup ASSORTED
 */
class ZipfianRandom {
 private:
  double zeta(uint64_t n) {
    double sum = 0;
    for (uint64_t i = 0; i < n; i++) {
      sum += 1 / std::pow(i + 1, theta_);
    }
    return sum;
  }

  void init(uint64_t items, double theta, uint64_t urnd_seed) {
    max_exclusive_ = items - 1;
    theta_ = theta;
    zetan_ = zeta(max_exclusive_);
    alpha_ = 1.0 / (1.0 - theta_);
    eta_ = (1 - std::pow(2.0 / max_exclusive_, 1 - theta_)) / (1 - zeta(2) / zetan_);
    urnd_.set_current_seed(urnd_seed);
  }

 public:
  explicit ZipfianRandom(uint64_t items, double theta, uint64_t urnd_seed) {
    init(items, theta, urnd_seed);
  }

  explicit ZipfianRandom() {}

  uint64_t next() {
    double u = urnd_.uniform_within(0, 100) / 100.0;
    double uz = u * zetan_;
    if (uz < 1.0) {
      return 0;
    }

    if (uz < 1.0 + std::pow(0.5, theta_)) {
      return 1;
    }

    uint64_t ret = (uint64_t)(max_exclusive_ * std::pow(eta_ * u - eta_ + 1, alpha_));
    return ret;
  }

 private:
  assorted::UniformRandom urnd_;
  uint64_t max_exclusive_;
  uint64_t base_;
  double theta_;
  double zetan_;
  double alpha_;
  double eta_;
};

}  // namespace assorted
}  // namespace foedus

#endif  // FOEDUS_ASSORTED_ZIPFIAN_RANDOM_HPP_
