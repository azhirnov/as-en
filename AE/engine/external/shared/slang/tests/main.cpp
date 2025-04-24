
#ifndef AE_ENABLE_SLANG
# error AE_ENABLE_SLANG required
#endif

#include "slang.h"
#include "slang-com-ptr.h"

using namespace slang;
using Slang::ComPtr;

int main ()
{
	ComPtr<IGlobalSession> globalSession;
	createGlobalSession( globalSession.writeRef() );
	return 0;
}
