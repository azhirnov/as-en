// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Input:
	 * REQUIRE_MIT			- MIT license				// https://en.wikipedia.org/wiki/MIT_License
	 * REQUIRE_BSD_3		- BSD 3 license				// https://en.wikipedia.org/wiki/BSD_licenses
	 * REQUIRE_APACHE_2		- Apache 2 license			// https://en.wikipedia.org/wiki/Apache_License
	 * REQUIRE_LGPLv2		- GNU LGPL 2 license		// https://en.wikipedia.org/wiki/GNU_Lesser_General_Public_License
	 * REQUIRE_LGPLv3		- GNU LGPL 3 license
	 * REQUIRE_GPLv2		- GNU GPL 2 license			// https://en.wikipedia.org/wiki/GNU_General_Public_License
	 * REQUIRE_GPLv3		- GNU GPL 3 license
	 * REQUIRE_COMMERCIAL	- commercial license, closed source, compatible with copyleft license types

	Output:
		AE_LICENSE	"<name>"

	Defined by third party libraries:
	 * AE_LICENSE_APACHE_2
	 * AE_LICENSE_BSD		- BSD-like
	 * AE_LICENSE_BSD2
	 * AE_LICENSE_BSD3
	 * AE_LICENSE_GPLv2
	 * AE_LICENSE_GPLv3
	 * AE_LICENSE_AGPLv3	- GNU Affero General Public License	// TODO	// https://en.wikipedia.org/wiki/Affero_General_Public_License
	 * AE_LICENSE_LGPLv2
	 * AE_LICENSE_LGPLv3
	 * AE_LICENSE_LGPLv2_SHAREDLIB
	 * AE_LICENSE_LGPLv3_SHAREDLIB
	 * AE_LICENSE_MIT
	 * AE_LICENSE_MPL_2		- Mozilla Public License	// https://en.wikipedia.org/wiki/Mozilla_Public_License
	 * AE_LICENSE_ZLIB									// https://en.wikipedia.org/wiki/Zlib_License
	 * AE_LICENSE_UNLICENSE
	 * AE_LICENSE_CC_BY_NC_SA_3	- CC BY-NC-SA 3.0		// https://creativecommons.org/licenses/by-nc-sa/3.0/
	 * AE_LICENSE_FREE_NON_COMMERCIAL

	reference:
	https://en.wikipedia.org/wiki/Comparison_of_free_and_open-source_software_licenses
*/


// commercial license
#ifdef REQUIRE_COMMERCIAL
//	AE_LICENSE_APACHE_2			- ok
//	AE_LICENSE_BSD/BSD2/BSD3	- ok
//	AE_LICENSE_LGPLv2_SHAREDLIB	- ok
//	AE_LICENSE_LGPLv3_SHAREDLIB	- ok
//	AE_LICENSE_MIT				- ok
//	AE_LICENSE_MPL_2			- ok
//	AE_LICENSE_ZLIB				- ok
//	AE_LICENSE_UNLICENSE		- ok

#	if defined(AE_LICENSE_FREE_NON_COMMERCIAL) or defined(AE_LICENSE_CC_BY_NC_SA_3)
#	  error only for non-commercial use!
#	endif
#	if defined(AE_LICENSE_LGPLv2) or defined(AE_LICENSE_LGPLv3)
#	  error LGPL with static linking requires to open sources
#	endif
#	if defined(AE_LICENSE_GPLv2) or defined(AE_LICENSE_GPLv3) or defined(AE_LICENSE_AGPLv3)
#	  error GPL requires to open sources
#	endif

#	define AE_LICENSE		"Commercial"


// MIT & BSD
#elif defined(REQUIRE_MIT) or defined(REQUIRE_BSD_3)
//	AE_LICENSE_BSD/BSD2/BSD3	- ok
//	AE_LICENSE_LGPLv2_SHAREDLIB	- ok
//	AE_LICENSE_LGPLv3_SHAREDLIB	- ok
//	AE_LICENSE_MIT				- ok
//	AE_LICENSE_MPL_2			- ?
//	AE_LICENSE_ZLIB				- ok
//	AE_LICENSE_UNLICENSE		- ok

#	if defined(AE_LICENSE_APACHE_2)
#	  error Apache 2.0 is not compatible with MIT/BSD
#	endif
#	if defined(AE_LICENSE_GPLv2) or defined(AE_LICENSE_GPLv3) or defined(AE_LICENSE_LGPLv2) or defined(AE_LICENSE_LGPLv3)
#	  error GPL/LGPL is not compatible with MIT/BSD
#	endif

#	if defined(REQUIRE_MIT)
#	  define AE_LICENSE		"MIT"
#	elif defined(REQUIRE_BSD_3)
#	  define AE_LICENSE		"BSD 3"
#	endif


// Apache 2.0
#elif defined(REQUIRE_APACHE_2)
//	AE_LICENSE_APACHE_2			- ok
//	AE_LICENSE_BSD/BSD2/BSD3	- ok
//	AE_LICENSE_LGPLv2_SHAREDLIB	- ok
//	AE_LICENSE_LGPLv3_SHAREDLIB	- ok
//	AE_LICENSE_MIT				- ok
//	AE_LICENSE_MPL_2			- ?
//	AE_LICENSE_ZLIB				- ok
//	AE_LICENSE_UNLICENSE		- ok

#	if defined(AE_LICENSE_GPLv2) or defined(AE_LICENSE_GPLv3) or defined(AE_LICENSE_LGPLv2) or defined(AE_LICENSE_LGPLv3)
#	  error GPL/LGPL 2/3 is not compatible with Apache 2.0
#	endif

#	define AE_LICENSE	"APACHE 2.0"


// LGPLv2
#elif defined(REQUIRE_LGPLv2)
//	AE_LICENSE_BSD/BSD2/BSD3	- ok
//	AE_LICENSE_LGPLv2_SHAREDLIB	- ok
//	AE_LICENSE_LGPLv3_SHAREDLIB	- ok
//	AE_LICENSE_MIT				- ok
//	AE_LICENSE_ZLIB				- ok
//	AE_LICENSE_MPL_2			- ok
//	AE_LICENSE_UNLICENSE		- ok

#	if defined(AE_LICENSE_APACHE_2) or defined(AE_LICENSE_MPL_2)
#	  error Apache 2.0 / MPL is not compatible with LGPLv2
#	endif
#	if defined(AE_LICENSE_GPLv3) or defined(AE_LICENSE_LGPLv3)
#	  error GPLv3 / LGPLv3 is not compatible with LGPLv2
#	endif

#	define AE_LICENSE	"GNU LGPL 2.1"


#elif defined(REQUIRE_LGPLv3)
//	AE_LICENSE_APACHE_2			- ok
//	AE_LICENSE_BSD/BSD2/BSD3	- ok
//	AE_LICENSE_LGPLv2_SHAREDLIB	- ok
//	AE_LICENSE_LGPLv3_SHAREDLIB	- ok
//	AE_LICENSE_MIT				- ok
//	AE_LICENSE_ZLIB				- ok
//	AE_LICENSE_MPL_2			- ok
//	AE_LICENSE_UNLICENSE		- ok

#	if defined(AE_LICENSE_GPLv2)
#	  error GPLv2 is not compatible with LGPLv3
#	endif

#	define AE_LICENSE	"GNU LGPL 3.0"


// GPLv2
#elif defined(REQUIRE_GPLv2)
//	AE_LICENSE_BSD/BSD2/BSD3	- ok
//	AE_LICENSE_GPLv2			- ok
//	AE_LICENSE_LGPLv2			- ok
//	AE_LICENSE_LGPLv2_SHAREDLIB	- ok
//	AE_LICENSE_LGPLv3_SHAREDLIB	- ok
//	AE_LICENSE_MIT				- ok
//	AE_LICENSE_ZLIB				- ok
//	AE_LICENSE_MPL_2			- ok
//	AE_LICENSE_UNLICENSE		- ok

#	if defined(AE_LICENSE_APACHE_2)
#	  error Apache 2.0 is not compatible with GPLv2, try GPLv3
#	endif
#	if defined(AE_LICENSE_GPLv3)
#	  error GPLv3 is not compatible with GPLv2
#	endif

#	define AE_LICENSE	"GNU GPL 2.0"


#elif defined(REQUIRE_GPLv3)
//	AE_LICENSE_APACHE_2			- ok
//	AE_LICENSE_BSD/BSD2/BSD3	- ok
//	AE_LICENSE_GPLv2			- ok
//	AE_LICENSE_GPLv3			- ok
//	AE_LICENSE_LGPLv2			- ok
//	AE_LICENSE_LGPLv3			- ok
//	AE_LICENSE_LGPLv2_SHAREDLIB	- ok
//	AE_LICENSE_LGPLv3_SHAREDLIB	- ok
//	AE_LICENSE_MIT				- ok
//	AE_LICENSE_ZLIB				- ok
//	AE_LICENSE_MPL_2			- ok
//	AE_LICENSE_UNLICENSE		- ok

#	if defined(AE_LICENSE_GPLv2)
#	  error GPLv3 is not compatible with GPLv2
#	endif

#	define AE_LICENSE	"GNU GPL 3.0"


#else
#	error unknown license
#endif

#ifndef AE_LICENSE
#	error output license is not defined
#endif
