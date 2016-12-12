#include <cinchtest.h>
#include <iostream>
#include <sstream>
#include <cereal/archives/binary.hpp>

#include "flecsi/execution/legion/dpd.h"

#include "legion.h"
#include "legion_config.h"

#define np(X)                                                            \
 std::cout << __FILE__ << ":" << __LINE__ << ": " << __PRETTY_FUNCTION__ \
           << ": " << #X << " = " << (X) << std::endl

using namespace std;

using namespace Legion;
using namespace LegionRuntime::Accessor;

using namespace flecsi;
using namespace execution;

enum TaskIDs{
  TOP_LEVEL_TID
};

using entity_id = size_t;
using partition_id = size_t;

using partition_vec = vector<entity_id>;
using connectivity_vec = legion_dpd::connectivity_vec;

const size_t NUM_CELLS = 16;
const size_t NUM_PARTITIONS = 4;

void top_level_task(const Task* task,
                    const std::vector<PhysicalRegion>& regions,
                    Context ctx, Runtime* runtime){

  legion_helper h(runtime, ctx);

  partition_vec cells(NUM_CELLS);

  size_t partition_size = NUM_CELLS / NUM_PARTITIONS;

  for(size_t c = 0; c < NUM_CELLS; ++c){
    size_t p = c / partition_size;

    cells[c] = p;
  }

  legion_dpd::partitioned_unstructured cells_part;

  {
    cells_part.size = NUM_CELLS;

    IndexSpace is = h.create_index_space(cells_part.size);

    IndexAllocator ia = runtime->create_index_allocator(ctx, is);
    
    Coloring coloring;

    for(size_t c = 0; c < cells.size(); ++c){
      size_t p = cells[c]; 
      ++cells_part.count_map[p];
    }

    for(auto& itr : cells_part.count_map){
      size_t p = itr.first;
      int count = itr.second;
      ptr_t start = ia.alloc(count);

      for(int i = 0; i < count; ++i){
        coloring[p].points.insert(start + i);
      }
    }

    FieldSpace fs = h.create_field_space();

    FieldAllocator fa = h.create_field_allocator(fs);

    fa.allocate_field(sizeof(entity_id), legion_dpd::ENTITY_FID);
    
    fa.allocate_field(sizeof(legion_dpd::ptr_count),
                      legion_dpd::DATA_PTR_COUNT_FID);

    cells_part.lr = h.create_logical_region(is, fs);

    RegionRequirement rr(cells_part.lr, WRITE_DISCARD, 
      EXCLUSIVE, cells_part.lr);
    
    rr.add_field(legion_dpd::ENTITY_FID);
    InlineLauncher il(rr);

    PhysicalRegion pr = runtime->map_region(ctx, il);

    pr.wait_until_valid();

    auto ac = 
      pr.get_field_accessor(legion_dpd::ENTITY_FID).typeify<entity_id>();

    IndexIterator itr(runtime, ctx, is);
    ptr_t ptr = itr.next();

    for(size_t c = 0; c < cells.size(); ++c){
      ac.write(ptr, c);
      ++ptr;
    }

    cells_part.ip = runtime->create_index_partition(ctx, is, coloring, true);

    runtime->unmap_region(ctx, pr);
  }

  legion_dpd dpd(ctx, runtime);
  dpd.create_data<double>(cells_part, 1024, 16);
}

TEST(legion, test1) {
  Runtime::set_top_level_task_id(TOP_LEVEL_TID);
  
  Runtime::register_legion_task<top_level_task>(TOP_LEVEL_TID,
    Processor::LOC_PROC, true, false);

  Runtime::register_legion_task<legion_dpd::init_data_task>(
    legion_dpd::INIT_DATA_TID,
    Processor::LOC_PROC, false, true);

  int argc = 1;
  char** argv = (char**)malloc(sizeof(char*));
  argv[0] = strdup("-test");

  Runtime::start(argc, argv);
}
