
#ifndef AE_ENABLE_GLSLANG
# error AE_ENABLE_GLSLANG required
#endif

// glslang includes
#include "glslang/build_info.h"
#include "glslang/Public/ShaderLang.h"
#include "glslang/MachineIndependent/localintermediate.h"
#include "glslang/Include/intermediate.h"
#include "glslang/SPIRV/doc.h"
#include "glslang/SPIRV/disassemble.h"
#include "glslang/SPIRV/GlslangToSpv.h"
#include "glslang/SPIRV/GLSL.std.450.h"

// SPIRV-Tools includes
#ifdef ENABLE_OPT
#   include "spirv-tools/optimizer.hpp"
#   include "spirv-tools/libspirv.h"
#endif

#if GLSLANG_VERSION_MAJOR != 12 || GLSLANG_VERSION_MINOR != 1 || GLSLANG_VERSION_PATCH != 0
# error invalid glslang version
#endif

int main ()
{
    glslang::InitializeProcess();

    glslang::FinalizeProcess();
    return 0;
}
