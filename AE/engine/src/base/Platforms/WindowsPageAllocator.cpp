// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_PLATFORM_WINDOWS
# include "base/Platforms/WindowsHeader.cpp.h"
# include "base/Platforms/WindowsPageAllocator.h"
# include "base/Platforms/WindowsUtils.h"
# include "base/Algorithms/StringUtils.h"
# include "base/Containers/UntypedStorage.h"

namespace AE::Base
{
/*
=================================================
	https://stackoverflow.com/questions/42354504/enable-large-pages-in-windows-programmatically
=================================================
*/
namespace
{
	static void  InitLsaString (OUT PLSA_UNICODE_STRING lsaString, LPWSTR str)
	{
		if ( str == null )
		{
			lsaString->Buffer		= null;
			lsaString->Length		= 0;
			lsaString->MaximumLength= 0;
			return;
		}

		DWORD	str_len			= DWORD(wcslen( str ));
		lsaString->Buffer		= str;
		lsaString->Length		= USHORT(str_len) * sizeof(WCHAR);
		lsaString->MaximumLength= USHORT(str_len + 1) * sizeof(WCHAR);
	}

	static NTSTATUS  OpenPolicy (LPWSTR serverName, DWORD desiredAccess, OUT PLSA_HANDLE policyHandle)
	{
		LSA_OBJECT_ATTRIBUTES	object_attribs;
		LSA_UNICODE_STRING		server_str;
		PLSA_UNICODE_STRING		server		= null;

		// Always initialize the object attributes to all zeroes.
		::ZeroMemory( OUT &object_attribs, sizeof(object_attribs) );

		if ( serverName != null )
		{
			// Make a LSA_UNICODE_STRING out of the LPWSTR passed in
			InitLsaString( OUT &server_str, serverName );
			server = &server_str;
		}

		// Attempt to open the policy.
		return ::LsaOpenPolicy(
			server,
			&object_attribs,
			desiredAccess,
			policyHandle
		);
	}

	static NTSTATUS  SetPrivilegeOnAccount (LSA_HANDLE policyHandle, PSID accountSid, LPWSTR privilegeName, BOOL enable)
	{
		LSA_UNICODE_STRING privilege_str;

		// Create a LSA_UNICODE_STRING for the privilege name.
		InitLsaString( OUT &privilege_str, privilegeName );

		// grant or revoke the privilege, accordingly
		if ( enable )
		{
			return ::LsaAddAccountRights(
				policyHandle,		// open policy handle
				accountSid,			// target SID
				&privilege_str,		// privileges
				1					// privilege count
			);
		}
		else
		{
			return ::LsaRemoveAccountRights(
				policyHandle,		// open policy handle
				accountSid,			// target SID
				FALSE,				// do not disable all rights
				&privilege_str,		// privileges
				1					// privilege count
			);
		}
	}

	static bool  TryEnableLargePageSupport ()
	{
		HANDLE	token = null;

		if ( not ::OpenProcessToken( ::GetCurrentProcess(), TOKEN_QUERY, OUT &token ))
		{
			WIN_CHECK_DEV( "OpenProcessToken failed: " );
			return false;
		}

		DWORD	buf_size = 0;

		// Probe the buffer size required for PTOKEN_USER structure
		if ( not ::GetTokenInformation( token, TokenUser, null, 0, OUT &buf_size ) and
			(::GetLastError() != ERROR_INSUFFICIENT_BUFFER))
		{
			WIN_CHECK_DEV( "GetTokenInformation failed: " );

			// Cleanup
			::CloseHandle( token );
			token = null;

			return false;
		}

		DynUntypedStorage	token_user_storage{ Bytes{buf_size}, AlignOf<TOKEN_USER> };
		PTOKEN_USER			token_user = token_user_storage.Ptr<TOKEN_USER>();

		// Retrieve the token information in a TOKEN_USER structure
		if ( not ::GetTokenInformation(
					token,
					TokenUser,
					OUT token_user,
					buf_size,
					OUT &buf_size ))
		{
			WIN_CHECK_DEV( "GetTokenInformation failed: " );

			// Cleanup
			::CloseHandle( token );
			token = null;

			return false;
		}

		// Print SID string
		//LPWSTR strsid;
		//ConvertSidToStringSid( token_user->User.Sid, OUT &strsid );
		//AE_LOGI( "User SID: "s << strsid );

		// Cleanup
		::CloseHandle( token );
		token = null;

		LSA_HANDLE  policy_hnd	= null;
		NTSTATUS	status		= OpenPolicy( null, POLICY_CREATE_ACCOUNT | POLICY_LOOKUP_NAMES, OUT &policy_hnd );

		if ( status != STATUS_SUCCESS )
		{
			WIN_CHECK_DEV2( ::LsaNtStatusToWinError(status), "OpenPolicy failed: " );
		}

		// Add new privilege to the account
		WString		lock_mem_name {SE_LOCK_MEMORY_NAME};

		status = SetPrivilegeOnAccount( policy_hnd, token_user->User.Sid, &lock_mem_name[0], TRUE );
		if ( status != STATUS_SUCCESS )
		{
			WIN_CHECK_DEV2( ::LsaNtStatusToWinError(status), "SetPrivilegeOnAccount failed: " );
		}

		// Enable this priveledge for the current process
		token = null;
		TOKEN_PRIVILEGES tp;

		if ( not ::OpenProcessToken( ::GetCurrentProcess(), TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, OUT &token ))
		{
			WIN_CHECK_DEV( "OpenProcessToken #2 failed: " );
			return false;
		}

		tp.PrivilegeCount			= 1;
		tp.Privileges[0].Attributes	= SE_PRIVILEGE_ENABLED;

		if ( not ::LookupPrivilegeValue( null, SE_LOCK_MEMORY_NAME, OUT &tp.Privileges[0].Luid ))
		{
			WIN_CHECK_DEV( "LookupPrivilegeValue failed: " );
			return false;
		}

		BOOL	result  = ::AdjustTokenPrivileges( token, FALSE, &tp, 0, PTOKEN_PRIVILEGES(null), 0 );
		DWORD   error   = ::GetLastError();

		if ( not result or (error != ERROR_SUCCESS) )
		{
			WIN_CHECK_DEV( "AdjustTokenPrivileges failed: " );
			return false;
		}

		// Cleanup
		::CloseHandle( token );
		token = null;

		return true;
	}

} // namespace


/*
=================================================
	EnableLargePageSupport
----
	run as admin
=================================================
*/
	bool  WindowsPageAllocator::EnableLargePageSupport () __NE___
	{
		return TryEnableLargePageSupport();
	}

/*
=================================================
	LargePageMinSize
=================================================
*/
	Bytes  WindowsPageAllocator::LargePageMinSize () __NE___
	{
		return Bytes{ ::GetLargePageMinimum() };
	}

/*
=================================================
	Alloc
=================================================
*/
	void*  WindowsPageAllocator::Alloc (Bytes size) __NE___
	{
		return ::VirtualAlloc( null, usize(size), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE );
	}

/*
=================================================
	AllocLarge
=================================================
*/
	void*  WindowsPageAllocator::AllocLarge (Bytes size) __NE___
	{
		return ::VirtualAlloc( null, usize(size), MEM_RESERVE | MEM_COMMIT | MEM_LARGE_PAGES, PAGE_READWRITE );
	}

/*
=================================================
	Dealloc
=================================================
*/
	bool  WindowsPageAllocator::Dealloc (void* ptr) __NE___
	{
		return ::VirtualFree( ptr, 0, MEM_RELEASE ) != FALSE;
	}

/*
=================================================
	Protect
=================================================
*/
	bool  WindowsPageAllocator::Protect (void* ptr, Bytes size, EMemProtection newFlag) __NE___
	{
		EMemProtection	old_flag;
		return Protect( ptr, size, newFlag, OUT old_flag );
	}

	bool  WindowsPageAllocator::Protect (void* ptr, Bytes size, EMemProtection newFlag, OUT EMemProtection &oldFlag) __NE___
	{
		DWORD	new_protect	= 0;
		DWORD	old_protect	= 0;

		switch_enum( newFlag )
		{
			case EMemProtection::NoAccess :		new_protect = PAGE_NOACCESS;	break;
			case EMemProtection::Readonly :		new_protect = PAGE_READONLY;	break;
			case EMemProtection::ReadWrite :	new_protect = PAGE_READWRITE;	break;
			default :							DBG_WARNING( "unknown protection flag" );
		}
		switch_end

		bool	res = ::VirtualProtect( ptr, usize(size), new_protect, OUT &old_protect ) != FALSE;

		switch ( old_protect )
		{
			case PAGE_NOACCESS :	oldFlag = EMemProtection::NoAccess;		break;
			case PAGE_READONLY :	oldFlag = EMemProtection::Readonly;		break;
			case PAGE_READWRITE :	oldFlag = EMemProtection::ReadWrite;	break;
			default :				DBG_WARNING( "unknown protection flag" );
		}

		return res;
	}

/*
=================================================
	Lock
=================================================
*/
	bool  WindowsPageAllocator::Lock (void* ptr, Bytes size) __NE___
	{
		return ::VirtualLock( ptr, usize(size) );
	}

/*
=================================================
	Unlock
=================================================
*/
	bool  WindowsPageAllocator::Unlock (void* ptr, Bytes size) __NE___
	{
		return ::VirtualUnlock( ptr, usize(size) );
	}

} // AE::Base

#endif // AE_PLATFORM_WINDOWS
