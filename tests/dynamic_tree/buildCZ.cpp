#include <iostream>
#include <vector>

#include <omp.h>

#include "bhtree_dynamic.h"
typedef sphlatch::BHTree treeType;

#include "bhtree_part_insertmover.h"
typedef sphlatch::BHTreePartsInsertMover inserterType;

#include "bhtree_cz_builder.h"

#include "bhtree_worker_grav.h"
typedef sphlatch::BHTreeWorkerGrav gravworkerType;

#include "bhtree_treedump.h"
typedef sphlatch::BHTreeDump dumpType;

typedef sphlatch::particleNode partType;
typedef sphlatch::genericNode nodeType;
typedef sphlatch::quadrupoleCellNode cellType;
typedef sphlatch::quadrupoleCellNode* cellPtr;
typedef sphlatch::costzoneCellNode czllType;

#include "particle_manager.h"
typedef sphlatch::ParticleManager part_type;

int main()
{
  part_type& PartManager(part_type::instance());

  sleep(1);
  treeType& Tree(treeType::instance());
  sleep(1);

  /*dumpType dumper(&Tree);
  dumper.dotDump("test.dot");

  inserterType inserter(&Tree);
  
  gravworkerType worker(&Tree);
#pragma omp parallel for firstprivate(worker)
  for (int i = 0; i < 8; i++)
  {
    //const size_t tid = omp_get_thread_num();
    //std::cout << tid << ":" << i << ":" << &worker << "\n";
  }
  sleep(1);*/
  return 0;
}
