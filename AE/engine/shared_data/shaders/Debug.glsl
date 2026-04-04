// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Helper functions and macros for debugging shaders.
*/

#ifdef __cplusplus
# pragma once
#endif

// Dummy function, used in [ShaderTrace](https://github.com/azhirnov/as-en/blob/dev/AE/engine/tools/res_pack/shader_trace/Readme.md)
//
// empty functions will be replaced during shader compilation
void dbg_EnableTraceRecording (bool b) {}
void dbg_PauseTraceRecording (bool b) {}
void dbg_EnableProfiling (bool b) {}
void dbg_Assert (uint b) {}

#define ASSERT( _expr_ )	\
	if ( !(_expr_) ){		\
		dbg_Assert(0);		\
	}

// check and return void on error
#define CHECK_RETV( _expr_ )	\
	if ( !(_expr_) ){			\
		dbg_Assert(0);			\
		return;					\
	}

// check and return '_ret_' on error
#define CHECK_RET( _expr_, _ret_ )	\
	if ( !(_expr_) ){				\
		dbg_Assert(0);				\
		return _ret_;				\
	}


#ifdef AE_expect_assume
#	define ASSUME( _expr_ )		gl.Assume( _expr_ )
#else
#	define ASSUME				ASSERT
#endif
