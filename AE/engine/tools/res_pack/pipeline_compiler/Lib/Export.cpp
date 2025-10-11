#include "res_pack/pipeline_compiler/PipelineCompiler.h"

#ifdef AE_PLATFORM_UNIX_BASED
namespace AE::PipelineCompiler
{
	extern "C" bool AE_DLL_EXPORT CompilePipelines (const PipelinesInfo* info);
}
	extern "C" AE_DLL_EXPORT void* GetCompilePipelines ()
	{
		return reinterpret_cast<void*>(&AE::PipelineCompiler::CompilePipelines);
	}
#endif
