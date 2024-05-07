#include "../PipelineCompiler.h"

#ifdef AE_PLATFORM_UNIX_BASED
	extern "C" AE_DLL_EXPORT void* GetCompilePipelines ()
	{
		return reinterpret_cast<void*>(&AE::PipelineCompiler::CompilePipelines);
	}
#endif
