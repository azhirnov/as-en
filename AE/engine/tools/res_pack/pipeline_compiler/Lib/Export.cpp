#include "../PipelineCompiler.h"

#ifdef AE_PLATFORM_UNIX_BASED
	extern "C" void* AE_DLL_EXPORT GetCompilePipelines ()
	{
		return reinterpret_cast<void*>(&AE::PipelineCompiler::CompilePipelines);
	}
#endif
