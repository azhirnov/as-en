// Copyright (c)  Zhirnov Andrey. For more information see 'LICENSE'

#include "base/ObjC/NS.h"
#include "base/Algorithms/StringUtils.h"

#undef null
#include <CoreFoundation/CFBase.h>
#include <Foundation/NSString.h>
#include <Foundation/NSError.h>
#include <Foundation/NSArray.h>
#include <Foundation/NSData.h>
#include <Foundation/NSURL.h>
#define null	nullptr

#if __has_feature(objc_arc)
#	error "ARC is not supported"
#endif

#include "base/ObjC/NS.mm.h"

namespace AE::NS
{
	StaticAssert( sizeof(NS::UInteger) == sizeof(NSUInteger) );


	Object::Object (const Object& other) __NE___ :
		_ptr{ other._ptr }
	{
		if ( _ptr )
			CFRetain( _ptr );
	}

	Object::Object (Object &&other) __NE___ :
		_ptr{ other._ptr }
	{
		other._ptr = null;
	}

	Object::~Object() __NE___
	{
		if ( _ptr )
			CFRelease( _ptr );
	}

	Object&  Object::operator = (const Object& rhs) __NE___
	{
		if ( rhs._ptr == _ptr )
			return *this;

		if ( rhs._ptr != null )
			CFRetain( rhs._ptr );

		if ( _ptr != null )
			CFRelease( _ptr );

		_ptr = rhs._ptr;
		return *this;
	}

	Object&  Object::operator = (Object &&rhs) __NE___
	{
		if ( rhs._ptr == _ptr )
			return *this;

		if ( _ptr != null )
			CFRelease( _ptr );

		_ptr	 = rhs._ptr;
		rhs._ptr = null;

		return *this;
	}

	UInteger  Object::RetainCount () __NE___
	{
		ASSERT( _ptr != null );
		return CFGetRetainCount( _ptr );
	}

	void  Object::Attach (const void* ptr) __NE___
	{
		if ( _ptr )
			CFRelease( _ptr );

		_ptr = ptr;
	}

	void  Object::Retain (const void* ptr) __NE___
	{
		if ( _ptr )
			CFRelease( _ptr );

		_ptr = ptr;

		if ( _ptr )
			CFRetain( _ptr );
	}
//-----------------------------------------------------------------------------



	void  ObjectRef::_AddRef () __NE___
	{
		if ( _ptr )
			CFRetain( _ptr );
	}

	void  ObjectRef::_ReleaseRef () __NE___
	{
		if ( _ptr )
		{
			CFRelease( _ptr );
			_ptr = null;
		}
	}

	void  ObjectRef::_Retain (const void* ptr) __NE___
	{
		ASSERT( _ptr == null );

		_ptr = ptr;

		if ( _ptr )
			CFRetain( _ptr );
	}

	UInteger  ObjectRef::_RetainCount () __NE___
	{
		ASSERT( _ptr != null );
		return CFGetRetainCount( _ptr );
	}
//-----------------------------------------------------------------------------



namespace _hidden_
{
	UInteger  ArrayBase::size () C_NE___
	{
		return UInteger([(__bridge NSArray *)Ptr() count ]);
	}

	void*  ArrayBase::GetItem (UInteger index) C_NE___
	{
		ASSERT( index < size() );
		return (__bridge void*) [(__bridge NSArray *)Ptr() objectAtIndexedSubscript : index ];
	}


	UInteger  MutableArrayBase::size () C_NE___
	{
		return UInteger([(__bridge NSMutableArray *)Ptr() count ]);
	}

	void*  MutableArrayBase::GetItem (UInteger index) C_NE___
	{
		ASSERT( index < size() );
		return (__bridge void*) [(__bridge NSMutableArray *)Ptr() objectAtIndexedSubscript : index ];
	}

} // _hidden_
//-----------------------------------------------------------------------------



	String::String (Base::StringView view) __NE___
	{
		ASSERT( Base::IsAnsiString( view.data(), view.size() ));
		@autoreleasepool
		{
			Retain( (__bridge void *)[[NSString alloc] initWithBytes : view.data()
															  length : view.size() * sizeof(view[0])
															encoding : NSASCIIStringEncoding ]);
		}
	}

	String::String (Base::U8StringView view) __NE___
	{
		@autoreleasepool
		{
			Retain( (__bridge void *)[[NSString alloc] initWithBytes : view.data()
															  length : view.size() * sizeof(view[0])
															encoding : NSUTF8StringEncoding ]);
		}
	}

	String::String (Base::U16StringView view) __NE___
	{
		@autoreleasepool
		{
			Retain( (__bridge void *)[[NSString alloc] initWithBytes : view.data()
															  length : view.size() * sizeof(view[0])
															encoding : NSUTF16StringEncoding ]);
		}
	}

	String::String (Base::U32StringView view) __NE___
	{
		@autoreleasepool
		{
			Retain( (__bridge void *)[[NSString alloc] initWithBytes : view.data()
															  length : view.size() * sizeof(view[0])
															encoding : NSUTF32StringEncoding ]);
		}
	}

	const CharAnsi*  String::ToAnsi () C_NE___
	{
		return [(__bridge NSString *)Ptr() cStringUsingEncoding : NSASCIIStringEncoding ];
	}

	const CharUtf8*  String::ToUtf8 () C_NE___
	{
		return Base::Cast<CharUtf8>([(__bridge NSString *)Ptr() cStringUsingEncoding : NSUTF8StringEncoding ]);
	}

	const CharUtf16*  String::ToUtf16 () C_NE___
	{
		return Base::Cast<CharUtf16>([(__bridge NSString *)Ptr() cStringUsingEncoding : NSUTF16StringEncoding ]);
	}

	const CharUtf32*  String::ToUtf32 () C_NE___
	{
		return Base::Cast<CharUtf32>([(__bridge NSString *)Ptr() cStringUsingEncoding : NSUTF32StringEncoding ]);
	}

	UInteger String::LengthOfUtf16 () C_NE___
	{
		return [(__bridge NSString *)Ptr() length ];
	}

	Base::String  String::ToAnsiString () C_NE___
	{
		Base::String	result;
		const usize		size_in_bytes = [(__bridge NSString *)Ptr() lengthOfBytesUsingEncoding : NSASCIIStringEncoding ];
		NOTHROW_ERR( result.resize( size_in_bytes / sizeof(result[0]) ));

		[(__bridge NSString *)Ptr() getCString : &result[0]
									 maxLength : result.size() * sizeof(result[0])
									  encoding : NSASCIIStringEncoding ];
		return result;
	}

	Base::U8String  String::ToUtf8String () C_NE___
	{
		Base::U8String	result;
		const usize		size_in_bytes = [(__bridge NSString *)Ptr() lengthOfBytesUsingEncoding : NSUTF8StringEncoding ];
		NOTHROW_ERR( result.resize( size_in_bytes / sizeof(result[0]) ));

		[(__bridge NSString *)Ptr() getCString : Base::Cast<char>(&result[0])
									 maxLength : result.size() * sizeof(result[0])
									  encoding : NSUTF8StringEncoding ];
		return result;
	}

	Base::U16String  String::ToUtf16String () C_NE___
	{
		Base::U16String	result;
		const usize		size_in_bytes = [(__bridge NSString *)Ptr() lengthOfBytesUsingEncoding : NSUTF16StringEncoding ];
		NOTHROW_ERR( result.resize( size_in_bytes / sizeof(result[0]) ));

		[(__bridge NSString *)Ptr() getCString : Base::Cast<char>(&result[0])
									 maxLength : result.size() * sizeof(result[0])
									  encoding : NSUTF16StringEncoding ];
		return result;
	}

	Base::U32String  String::ToUtf32String () C_NE___
	{
		Base::U32String	result;
		const usize		size_in_bytes = [(__bridge NSString *)Ptr() lengthOfBytesUsingEncoding : NSUTF32StringEncoding ];
		NOTHROW_ERR( result.resize( size_in_bytes / sizeof(result[0]) ));

		[(__bridge NSString *)Ptr() getCString : Base::Cast<char>(&result[0])
									 maxLength : result.size() * sizeof(result[0])
									  encoding : NSUTF32StringEncoding ];
		return result;
	}
//-----------------------------------------------------------------------------



	URL::URL (const char* path) __NE___
	{
		@autoreleasepool
		{
			NSString* 	str = [NSString stringWithUTF8String : path];
			NSURL*		url = [NSURL URLWithString : str];
			Retain( (__bridge void *)url );
		}
	}

	URL::URL (const Base::Path &path) __NE___
	{
		@autoreleasepool
		{
			Retain( (__bridge void *)[NSURL fileURLWithPath : [NSString stringWithUTF8String : path.c_str()]
												isDirectory : Base::FileSystem::IsDirectory( path ) ]);
		}
	}


} // AE::NS
