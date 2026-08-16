// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "base/Common.h"

#if defined(__cpp_lib_stacktrace) and not defined(AE_COMPILER_GCC)
#	include <stacktrace>
#	define AE_HAS_CALLSTACK

#elif defined(AE_PLATFORM_ANDROID)
#	include <unwind.h>
#	include <dlfcn.h>
#	include <cstdint>
#	include <cstdio>
#endif

#include "base/Debug/StackTrace.h"
#include "base/FileSystem/FileSystem.h"
#include "base/Algorithms/ToString.h"

namespace AE::Base
{

#ifdef AE_CFG_RELEASE
	String  StackTrace::ToString (StringView, StringView) __Th___
	{
		return {};
	}

#elif defined(AE_HAS_CALLSTACK)
/*
=================================================
	GetCallStack (std::stacktrace)
=================================================
*/
	String  StackTrace::ToString (StringView skipFile, StringView prefix) __Th___
	{
		String		str;
		auto		stack	= std::stacktrace::current();
		auto		it		= stack.begin();
		usize		i		= 0;
		const usize	count	= stack.size();

		// skip current
		{
			++it;
			++i;
		}

		// skip logger functions
		if ( not skipFile.empty() )
		{
			for (; i < count; ++i, ++it) {
				if_unlikely( HasSubString( it->source_file(), skipFile )) {
					++i;  ++it;
					break;
				}
			}
			for (; i < count; ++i, ++it) {
				if_unlikely( not HasSubString( it->source_file(), skipFile ))
					break;
			}
		}

		for (; i < count; ++i, ++it)
		{
			if ( it->source_file().empty() )
				break;

			str << prefix << FileSystem::ToShortPath( it->source_file() ) << '(' << Base::ToString( it->source_line() ) << "): " << it->description() << '\n';
		}

		str.pop_back();
		return str;
	}

#elif defined(AE_PLATFORM_ANDROID)
/*
=================================================
	GetCallStack
=================================================
*/
namespace
{
	struct UnwindState
	{
		uintptr_t* current;
		uintptr_t* end;
	};

	static _Unwind_Reason_Code  UnwindCallback (_Unwind_Context* context, void* argument)
	{
		auto* state = static_cast<UnwindState*>(argument);

		uintptr_t pc = _Unwind_GetIP(context);
		if ( pc and state->current != state->end )
			*state->current++ = pc;

		return state->current == state->end
			? _URC_END_OF_STACK
			: _URC_NO_REASON;
	}

} // namespace

	String  StackTrace::ToString (StringView skipFile, StringView prefix) __Th___
	{
		String		str;
		uintptr_t	frames [64]	{};
		UnwindState	state		{frames, frames + 64};

		_Unwind_Backtrace( UnwindCallback, OUT &state );

		int index = 0;
		for (uintptr_t* frame = frames; frame != state.current; ++frame, ++index)
		{
			Dl_info info{};

			if ( ::dladdr( reinterpret_cast<const void*>(*frame), OUT &info ) and info.dli_fname != null )
			{
				uintptr_t	offset	= *frame - reinterpret_cast<uintptr_t>( info.dli_fbase );
				String		off_str	= "0x"s << Base::ToString( offset );

				str << prefix << info.dli_fname << ": " << (info.dli_sname ? info.dli_sname : off_str.c_str()) << '\n';
			}
			else
			{
				str << prefix << "addr: " << Base::ToString( BitCast<usize>(*frame) ) << '\n';
			}
		}
		return str;
	}

#else

	String  StackTrace::ToString (StringView, StringView) __Th___
	{
		return {};
	}
#endif

} // AE::Base
