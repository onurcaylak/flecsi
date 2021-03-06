/*
    @@@@@@@@  @@           @@@@@@   @@@@@@@@ @@
   /@@/////  /@@          @@////@@ @@////// /@@
   /@@       /@@  @@@@@  @@    // /@@       /@@
   /@@@@@@@  /@@ @@///@@/@@       /@@@@@@@@@/@@
   /@@////   /@@/@@@@@@@/@@       ////////@@/@@
   /@@       /@@/@@//// //@@    @@       /@@/@@
   /@@       @@@//@@@@@@ //@@@@@@  @@@@@@@@ /@@
   //       ///  //////   //////  ////////  //

   Copyright (c) 2016, Los Alamos National Security, LLC
   All rights reserved.
                                                                              */
#pragma once

/*! @file */

#include <bitset>

#include <flecsi/utils/debruijn.h>

namespace flecsi {

/*!
  Bitmasks for launch types.

  \note This enumeration is not scoped so that users can do things
        like:
        \code
        launch_t l(single | leaf);
        \endcode
 */

enum launch_mask_t : size_t {
  single = 1 << 0,
  index = 1 << 1,
  leaf = 1 << 2,
  inner = 1 << 3,
  idempotent = 1 << 4
}; // enum launch_mask_t

namespace execution {

// This will be used by the task_hash_t type to create hash keys for
// task registration. If you add more launch flags below, you will need
// to increase the launch_bits accordingly, i.e., launch_bits must
// be greater than or equal to the number of bits in the bitset for
// launch_t below.
constexpr size_t launch_bits = 5;

/*!
  Use a std::bitset to store launch information.

  @note This will most likely use 4 bytes of data for efficiency.
 */

using launch_t = std::bitset<launch_bits>;

/*!
  Enumeration of various task launch types. Not all of these may be
  supported by all runtimes. Unsupported launch information will be
  ignored.
 */

enum class launch_type_t : size_t {
  single,
  index,
  leaf,
  inner,
  idempotent
}; // enum launch_type_t

/*!
  Convert a processor mask to a processor type.
 */

inline launch_type_t
mask_to_type(launch_mask_t m) {
  return static_cast<launch_type_t>(utils::debruijn32_t::index(m));
} // mask_to_type

  /*!
    Macro to create repetitive interfaces.
   */

#define test_boolean_interface(name)                                           \
  inline bool launch_##name(const launch_t & l) {                              \
    return l.test(static_cast<size_t>(launch_type_t::name));                   \
  }

test_boolean_interface(single) test_boolean_interface(index)
    test_boolean_interface(leaf) test_boolean_interface(inner)
        test_boolean_interface(idempotent)

#undef test_boolean_interface

    /*!
      Static launch_t creation function.
     */

    template<
        bool SINGLE = false,
        bool INDEX = false,
        bool LEAF = false,
        bool INNER = false,
        bool IDEMPOTENT = false>
    launch_t make_launch() {
  return {(SINGLE ? 1 << 0 : 0) | (INDEX ? 1 << 1 : 0) | (LEAF ? 1 << 2 : 0) |
          (INNER ? 1 << 3 : 0) | (IDEMPOTENT ? 1 << 4 : 0)};
} // make_launch

} // namespace execution
} // namespace flecsi
