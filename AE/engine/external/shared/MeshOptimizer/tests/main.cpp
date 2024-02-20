
#ifndef AE_ENABLE_MESH_OPTIMIZER
# error AE_ENABLE_MESH_OPTIMIZER required
#endif

#include "meshoptimizer.h"

#ifdef ANDROID
# include "TestHelper.h"

extern int AE_MeshOptimizer_Test ()
#else
int main ()
#endif
{
    (void)&meshopt_buildMeshlets<unsigned>;

    return 0;
}
