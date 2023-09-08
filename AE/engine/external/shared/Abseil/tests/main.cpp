
#ifndef AE_ENABLE_ABSEIL
# error AE_ENABLE_ABSEIL required
#endif

#include "absl/container/flat_hash_set.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/node_hash_set.h"
#include "absl/container/node_hash_map.h"


#ifdef ANDROID
# include "TestHelper.h"

extern int AE_Abseil_Test ()
#else
int main ()
#endif
{
    absl::flat_hash_set< int >          absl_flat_set;
    absl::flat_hash_map< int, float >   absl_flat_map;
    absl::node_hash_set< int >          absl_node_set;
    absl::node_hash_map< int, float >   absl_node_map;

    return 0;
}
