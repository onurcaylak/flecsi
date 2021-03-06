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

//----------------------------------------------------------------------------//
// POLICY_NAMESPACE must be defined before including storage_class.h!!!
// Using this approach allows us to have only one storage_class__
// definintion that can be used by all data policies -> code reuse...
#define POLICY_NAMESPACE legion
#include <flecsi/data/storage_class.h>
#undef POLICY_NAMESPACE
//----------------------------------------------------------------------------//

#include <flecsi/data/common/privilege.h>
#include <flecsi/data/data_client.h>
#include <flecsi/data/global_data_handle.h>
#include <flecsi/data/storage.h>
#include <flecsi/execution/context.h>
#include <flecsi/utils/const_string.h>
#include <flecsi/utils/index_space.h>

namespace flecsi {
namespace data {
namespace legion {

//----------------------------------------------------------------------------//
// Global handle.
//----------------------------------------------------------------------------//

/*!
 The global_handle__ provide an access to global variables that have
 been registered in data model

 \tparam T The type of the data variable. If this type is not
           consistent with the type used to register the data, bad things
           can happen. However, it can be useful to reinterpret the type,
           e.g., when writing raw bytes. This class is part of the
           low-level \e flecsi interface, so it is assumed that you
           know what you are doing...

 @tparam PERMISSIONS The permissions to the handle.
 */

template<typename T, size_t PERMISSIONS>
struct global_handle__ : public global_data_handle__<T, PERMISSIONS> {

  /*!
    Type definitions.
   */

  using base_t = global_data_handle__<T, PERMISSIONS>;

  /*!
    Constructor.
   */

  global_handle__() {
    base_t::global = true;
  }

  /*!
   Destructor.
   */

  ~global_handle__() {}

  /*
    Copy constructor.
   */

  template<size_t P2>
  global_handle__(const global_handle__<T, P2> & a)
      : base_t(reinterpret_cast<const base_t &>(a)), label_(a.label()),
        size_(a.size()) {
    static_assert(P2 == 0, "passing mapped handle to task args");
  }

  //--------------------------------------------------------------------------//
  // Member data interface.
  //--------------------------------------------------------------------------//

  /*!
   \brief Return a std::string containing the label of the data variable
          reference by this handle.
   */

  const std::string & label() const {
    return label_;
  } // label

  /*!
   \brief Return the index space size of the data variable
          referenced by this handle.
   */

  size_t size() const {
    return size_;
  } // size

  /*!
   \brief Test to see if this handle is empty

   \return true if registered.
   */

  operator bool() const {
    return base_t::combined_data != nullptr;
    // return data_ != nullptr;
  } // operator bool

private:
  std::string label_ = "";
  size_t size_ = 1;
  //    T* data_ = nullptr;
  //     T* buffer = nullptr;
}; // struct global_handle__

//+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=//
// Main type definition.
//+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=//

//----------------------------------------------------------------------------//
// Global storage type.
//----------------------------------------------------------------------------//

/*!
 FIXME: Global storage type.
 */
template<>
struct storage_class__<global> {

  /*!
   Type definitions.
   */

  template<typename T, size_t PERMISSIONS>
  using handle_t = global_handle__<T, PERMISSIONS>;

  /*!
   Data handles.
   */

  template<
      typename DATA_CLIENT_TYPE,
      typename DATA_TYPE,
      size_t NAMESPACE,
      size_t NAME,
      size_t VERSION,
      size_t PERMISSIONS>
  static handle_t<DATA_TYPE, 0>
  get_handle(const data_client_handle__<DATA_CLIENT_TYPE, PERMISSIONS> &
                 client_handle) {
    handle_t<DATA_TYPE, 0> h;
    auto & context = execution::context_t::instance();

    auto & field_info = context.get_field_info_from_name(
        typeid(typename DATA_CLIENT_TYPE::type_identifier_t).hash_code(),
        utils::hash::field_hash<NAMESPACE, NAME>(VERSION));

    size_t index_space = field_info.index_space;
    auto & ism = context.index_space_data_map();
    h.data_client_hash = field_info.data_client_hash;
    h.color_region = ism[index_space].color_region;
    h.fid = field_info.fid;
    h.index_space = field_info.index_space;
    h.global = true;
    h.state = context.execution_state();

    return h;
  }

}; // struct storage_class__

} // namespace legion
} // namespace data
} // namespace flecsi
