/*~--------------------------------------------------------------------------~*
 * Copyright (c) 2015 Los Alamos National Security, LLC
 * All rights reserved.
 *~--------------------------------------------------------------------------~*/

#ifndef flecsi_data_legion_data_handle_policy_h
#define flecsi_data_legion_data_handle_policy_h

#include <legion.h>
#include <legion_stl.h>

///
/// \file
/// \date Initial file creation: Apr 04, 2017
///

namespace flecsi {

///
/// \class legion_data_handle_policy_t data_handle_policy.h
/// \brief legion_data_handle_policy_t provides...
///
struct legion_data_handle_policy_t
{
  using field_id_t = Legion::FieldID;

  void copy(const legion_data_handle_policy_t& p){
    fid = p.fid;
    context = p.context;
    runtime = p.runtime;
    index_space = p.index_space;
    exclusive_lr = p.exclusive_lr;
    shared_lr = p.shared_lr;
    ghost_lr = p.ghost_lr;
    pbarrier_as_owner_ptr = p.pbarrier_as_owner_ptr;
    ghost_owners_pbarriers_ptrs = p.ghost_owners_pbarriers_ptrs;
    ghost_owners_lregions = p.ghost_owners_lregions;
    global_to_local_color_map_ptr = p.global_to_local_color_map_ptr;
    color_region = p.color_region;
    exclusive_pr = p.exclusive_pr;
    shared_pr = p.shared_pr;
    ghost_pr = p.ghost_pr;
    ghost_is_readable = p.ghost_is_readable;
    exclusive_priv = p.exclusive_priv;
    shared_priv = p.shared_priv;
    ghost_priv = p.ghost_priv;
  }

  // +++ The following fields are set from get_handle(), reading
  // information from the context which is data that is the same
  // across multiple ranks/colors and should be used ONLY as read-only data

  field_id_t fid;
  size_t index_space;

  // These depend on color but are only used in specifying
  // the region requirements
  Legion::LogicalRegion color_region;
  Legion::LogicalRegion exclusive_lr;
  Legion::LogicalRegion shared_lr;
  Legion::LogicalRegion ghost_lr;
  std::vector<Legion::LogicalRegion> ghost_owners_lregions;

  // Tuple-walk copies data_handle then discards updates at the end.
  // Some pointers are necessary for updates to live between walks.
  Legion::PhaseBarrier* pbarrier_as_owner_ptr;
  std::vector<Legion::PhaseBarrier*> ghost_owners_pbarriers_ptrs;
  const Legion::STL::map<LegionRuntime::Arrays::coord_t,
    LegionRuntime::Arrays::coord_t>* global_to_local_color_map_ptr;
  bool* ghost_is_readable;

  // +++ The following fields are set on the execution side of the handle
  // inside the actual Legion task once we have the physical regions

  Legion::Context context;
  Legion::Runtime* runtime;
  Legion::PhysicalRegion exclusive_pr;
  Legion::PhysicalRegion shared_pr;
  Legion::PhysicalRegion ghost_pr;
  size_t exclusive_priv;
  size_t shared_priv;
  size_t ghost_priv;
}; // class legion_data_handle_policy_t

} // namespace flecsi

#endif // flecsi_data_legion_data_handle_policy_h

/*~-------------------------------------------------------------------------~-*
 * Formatting options for vim.
 * vim: set tabstop=2 shiftwidth=2 expandtab :
 *~-------------------------------------------------------------------------~-*/