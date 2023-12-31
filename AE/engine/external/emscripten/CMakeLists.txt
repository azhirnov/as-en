# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

cmake_minimum_required( VERSION 3.10 FATAL_ERROR )

#----------------------------------------------------------

set( AE_ENABLE_ABSEIL           OFF CACHE INTERNAL "" FORCE )
set( AE_ENABLE_ANGELSCRIPT      OFF CACHE INTERNAL "" FORCE )

set( AE_ENABLE_BASS             OFF CACHE INTERNAL "" FORCE )
set( AE_ENABLE_BROTLI           OFF CACHE INTERNAL "" FORCE )

set( AE_ENABLE_GLFW             OFF CACHE INTERNAL "" FORCE )
set( AE_ENABLE_GLSLANG          OFF CACHE INTERNAL "" FORCE )
set( AE_ENABLE_GLM              ON  CACHE INTERNAL "" FORCE )

set( AE_ENABLE_FREETYPE         OFF CACHE INTERNAL "" FORCE )

set( AE_ENABLE_IMGUI            OFF CACHE INTERNAL "" FORCE )

set( AE_ENABLE_KTX              OFF CACHE INTERNAL "" FORCE )

set( AE_ENABLE_MSDFGEN          OFF CACHE INTERNAL "" FORCE )
set( AE_ENABLE_METAL            OFF CACHE INTERNAL "" FORCE )

set( AE_ENABLE_OPENGL           OFF CACHE INTERNAL "" FORCE )

set( AE_ENABLE_REMOTE_GRAPHICS  OFF CACHE INTERNAL "" FORCE )

set( AE_ENABLE_SPIRV_CROSS      OFF CACHE INTERNAL "" FORCE )
set( AE_ENABLE_STB              OFF CACHE INTERNAL "" FORCE )

set( AE_ENABLE_TINYGLTF         OFF CACHE INTERNAL "" FORCE )

set( AE_ENABLE_VULKAN           OFF CACHE INTERNAL "" FORCE )
set( AE_ENABLE_VMA              OFF CACHE INTERNAL "" FORCE )

set( AE_ENABLE_UTF8PROC         OFF CACHE INTERNAL "" FORCE )

set( AE_ENABLE_WEBGL2           ON  CACHE BOOL "use WebGL2" )

#----------------------------------------------------------

set( CMAKE_INSTALL_PREFIX   "${CMAKE_BINARY_DIR}/install"                   CACHE INTERNAL "installation directory" FORCE )
set( AE_DST_PATH            "${AE_EXTERNAL_REP_PATH}/external/emscripten"   CACHE INTERNAL ""                       FORCE )
set( AE_DST_SHARED_PATH     "${AE_EXTERNAL_REP_PATH}/external/emscripten"   CACHE INTERNAL ""                       FORCE )
