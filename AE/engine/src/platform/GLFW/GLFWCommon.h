// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_GLFW

typedef struct GLFWwindow   GLFWwindow;
typedef struct GLFWmonitor  GLFWmonitor;

namespace AE::App
{
    class ApplicationGLFW;
    class InputActionsGLFW;
    class WindowGLFW;
}

#endif // AE_ENABLE_GLFW


// check definitions
#ifdef AE_CPP_DETECT_MISMATCH

#  ifdef AE_ENABLE_GLFW
#   pragma detect_mismatch( "AE_ENABLE_GLFW", "1" )
#  else
#   pragma detect_mismatch( "AE_ENABLE_GLFW", "0" )
#  endif

#endif // AE_CPP_DETECT_MISMATCH
