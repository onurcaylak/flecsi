/*~--------------------------------------------------------------------------~*
 * Copyright (c) 2017 Los Alamos National Security, LLC
 * All rights reserved.
 *~--------------------------------------------------------------------------~*/

#ifndef flecsi_data_accessor_h
#define flecsi_data_accessor_h
 
///
/// \file accessor.h
/// \authors nickm
/// \date Initial file creation: Feb 24, 2017
///

namespace flecsi {

class accessor_base{

};

template<typename T>
class accessor__ : public accessor_base{
  using type = T;
};

} // namespace flecsi

#endif // flecsi_data_accessor_h

/*~-------------------------------------------------------------------------~-*
 * Formatting options for vim.
 * vim: set tabstop=2 shiftwidth=2 expandtab :
 *~-------------------------------------------------------------------------~-*/
