// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Input:
	 * REQUIRE_MIT				- MIT license				// https://en.wikipedia.org/wiki/MIT_License
	 * REQUIRE_BSD_3			- BSD 3 license				// https://en.wikipedia.org/wiki/BSD_licenses
	 * REQUIRE_APACHE_2			- Apache 2.0 license		// https://en.wikipedia.org/wiki/Apache_License
	 * REQUIRE_LGPLv2_1			- GNU LGPL 2.1 license		// https://en.wikipedia.org/wiki/GNU_Lesser_General_Public_License
	 * REQUIRE_LGPLv3			- GNU LGPL 3.0 license
	 * REQUIRE_GPLv2			- GNU GPL 2.0 license		// https://en.wikipedia.org/wiki/GNU_General_Public_License
	 * REQUIRE_GPLv3			- GNU GPL 3.0 license
	 * REQUIRE_CC_BY_NC_SA_3	- CC BY-NC-SA 3.0 license (Any derivative work must be licensed under the same license)	// https://creativecommons.org/licenses/by-nc-sa/3.0/
	 * REQUIRE_COMMERCIAL		- commercial/proprietary license, closed source, compatible with copyleft license types

	Output:
		AE_LICENSE	"<name>"

	Defined by third party libraries:
	 * AE_LICENSE_APACHE_2
	 * AE_LICENSE_BSD				- BSD-like
	 * AE_LICENSE_BSD2
	 * AE_LICENSE_BSD3
	 * AE_LICENSE_GPLv2
	 * AE_LICENSE_GPLv3
	 * AE_LICENSE_AGPLv3			- GNU Affero General Public License	// TODO	// https://en.wikipedia.org/wiki/Affero_General_Public_License
	 * AE_LICENSE_LGPLv2_0			- static linking
	 * AE_LICENSE_LGPLv2_1			- static linking
	 * AE_LICENSE_LGPLv3			- static linking
	 * AE_LICENSE_LGPLv2_SHAREDLIB	- dynamic linking
	 * AE_LICENSE_LGPLv3_SHAREDLIB	- dynamic linking
	 * AE_LICENSE_MIT
	 * AE_LICENSE_MPL_2				- Mozilla Public License	// https://en.wikipedia.org/wiki/Mozilla_Public_License
	 * AE_LICENSE_ZLIB											// https://en.wikipedia.org/wiki/Zlib_License
	 * AE_LICENSE_UNLICENSE
	 * AE_LICENSE_CC_BY_NC_SA_3		- CC BY-NC-SA 3.0			// https://creativecommons.org/licenses/by-nc-sa/3.0/
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
#	if defined(AE_LICENSE_LGPLv2_0) or defined(AE_LICENSE_LGPLv2_1) or defined(AE_LICENSE_LGPLv3)
#	  error LGPL with static linking requires to open sources
#	endif
#	if defined(AE_LICENSE_GPLv2) or defined(AE_LICENSE_GPLv3) or defined(AE_LICENSE_AGPLv3)
#	  error GPL requires to open sources
#	endif

#	define AE_LICENSE		"Commercial"


// MIT & BSD
#elif defined(REQUIRE_MIT) or defined(REQUIRE_BSD_3)
//	AE_LICENSE_BSD/BSD2/BSD3		- ok
//	AE_LICENSE_LGPLv2_SHAREDLIB		- ok
//	AE_LICENSE_LGPLv3_SHAREDLIB		- ok
//	AE_LICENSE_MIT					- ok
//	AE_LICENSE_ZLIB					- ok
//	AE_LICENSE_UNLICENSE			- ok

#	if defined(AE_LICENSE_APACHE_2)
	  // MIT/BSD + Apache -> Apache
#	  error Apache 2.0 is not compatible with MIT/BSD
#	endif
#	if defined(AE_LICENSE_MPL_2)
	  // MIT/BSD + MPL2 -> MPL2
#	  error MPL 2.0 is not compatible with MIT/BSD
#	endif
#	if defined(AE_LICENSE_GPLv2) or defined(AE_LICENSE_GPLv3) or defined(AE_LICENSE_AGPLv3) or \
	   defined(AE_LICENSE_LGPLv2_0) or defined(AE_LICENSE_LGPLv2_1) or defined(AE_LICENSE_LGPLv3)
#	  error GPL/LGPL is not compatible with MIT/BSD
#	endif
#	if defined(AE_LICENSE_CC_BY_NC_SA_3)
#	  error CC BY-NC-SA 3.0 is not compatible with other licenses
#	endif

#	ifdef AE_LICENSE_FREE_NON_COMMERCIAL
#		if defined(REQUIRE_MIT)
#		  define AE_LICENSE		"MIT, non-commercial"
#		elif defined(REQUIRE_BSD_3)
#		  define AE_LICENSE		"BSD 3, non-commercial"
#		endif
#	else
#		if defined(REQUIRE_MIT)
#		  define AE_LICENSE		"MIT"
#		elif defined(REQUIRE_BSD_3)
#		  define AE_LICENSE		"BSD 3"
#		endif
#	endif


// Apache 2.0
#elif defined(REQUIRE_APACHE_2)
//	AE_LICENSE_APACHE_2				- ok
//	AE_LICENSE_BSD/BSD2/BSD3		- ok
//	AE_LICENSE_LGPLv2_SHAREDLIB		- ok
//	AE_LICENSE_LGPLv3_SHAREDLIB		- ok
//	AE_LICENSE_MIT					- ok
//	AE_LICENSE_ZLIB					- ok
//	AE_LICENSE_UNLICENSE			- ok

#	if defined(AE_LICENSE_GPLv2) or defined(AE_LICENSE_GPLv3) or defined(AE_LICENSE_AGPLv3) or \
	   defined(AE_LICENSE_LGPLv2_0) or defined(AE_LICENSE_LGPLv2_1) or defined(AE_LICENSE_LGPLv3)
	  // GPLv2 - result under GPLv2
	  // AGPLv3 - result under AGPLv3
	  // GPLv3, LGPLv2/3 - result is under dual license
#	  error GPL/LGPL 2/3 is not compatible with Apache 2.0
#	endif
#	if defined(AE_LICENSE_MPL_2)
	  // result is under dual license
#	  error MPL 2 is not compatible with Apache 2.0
#	endif
#	if defined(AE_LICENSE_CC_BY_NC_SA_3)
#	  error CC BY-NC-SA 3.0 is not compatible with other licenses
#	endif

#	ifdef AE_LICENSE_FREE_NON_COMMERCIAL
#	  define AE_LICENSE		"APACHE 2.0, non-commercial"
#	else
#	  define AE_LICENSE		"APACHE 2.0"
#	endif


// LGPLv2
#elif defined(REQUIRE_LGPLv2_1)
//	AE_LICENSE_BSD/BSD2/BSD3		- ok
//	AE_LICENSE_LGPLv2_SHAREDLIB		- ok
//	AE_LICENSE_LGPLv3_SHAREDLIB		- ok
//	AE_LICENSE_MIT					- ok
//	AE_LICENSE_ZLIB					- ok
//	AE_LICENSE_MPL_2				- ok
//	AE_LICENSE_UNLICENSE			- ok

#	if defined(AE_LICENSE_APACHE_2) or defined(AE_LICENSE_MPL_2)
#	  error Apache 2.0 / MPL 2 is not compatible with LGPLv2
#	endif
#	if defined(AE_LICENSE_GPLv3) or defined(AE_LICENSE_LGPLv3)
#	  error GPLv3 / LGPLv3 is not compatible with LGPLv2
#	endif
#	if defined(AE_LICENSE_AGPLv3)
#	  error AGPLv3 is not compatible with LGPLv2
#	endif
#	if defined(AE_LICENSE_CC_BY_NC_SA_3)
#	  error CC BY-NC-SA 3.0 is not compatible with other licenses
#	endif

#	ifdef AE_LICENSE_FREE_NON_COMMERCIAL
#	  define AE_LICENSE		"GNU LGPL 2.1, non-commercial"
#	else
#	  define AE_LICENSE		"GNU LGPL 2.1"
#endif


#elif defined(REQUIRE_LGPLv3)
//	AE_LICENSE_APACHE_2				- ok
//	AE_LICENSE_BSD/BSD2/BSD3		- ok
//	AE_LICENSE_LGPLv2_SHAREDLIB		- ok
//	AE_LICENSE_LGPLv3_SHAREDLIB		- ok
//	AE_LICENSE_MIT					- ok
//	AE_LICENSE_ZLIB					- ok
//	AE_LICENSE_MPL_2				- ok
//	AE_LICENSE_UNLICENSE			- ok

#	if defined(AE_LICENSE_GPLv2)
#	  error GPLv2 is not compatible with LGPLv3
#	endif
#	if defined(AE_LICENSE_AGPLv3)
#	  error AGPLv3 is not compatible with LGPLv3
#	endif
#	if defined(AE_LICENSE_CC_BY_NC_SA_3)
#	  error CC BY-NC-SA 3.0 is not compatible with other licenses
#	endif

#	ifdef AE_LICENSE_FREE_NON_COMMERCIAL
#	  define AE_LICENSE		"GNU LGPL 3.0, non-commercial"
#	else
#	  define AE_LICENSE		"GNU LGPL 3.0"
#	endif

// GPLv2
#elif defined(REQUIRE_GPLv2)
//	AE_LICENSE_BSD/BSD2/BSD3		- ok
//	AE_LICENSE_GPLv2				- ok
//	AE_LICENSE_LGPLv2_0				- ok
//	AE_LICENSE_LGPLv2_1				- ok
//	AE_LICENSE_LGPLv2_SHAREDLIB		- ok
//	AE_LICENSE_MIT					- ok
//	AE_LICENSE_ZLIB					- ok
//	AE_LICENSE_MPL_2				- ok
//	AE_LICENSE_UNLICENSE			- ok

#	if defined(AE_LICENSE_APACHE_2)
#	  error Apache 2.0 is not compatible with GPLv2, try GPLv3
#	endif
#	if defined(AE_LICENSE_GPLv3) or defined(AE_LICENSE_LGPLv3) or defined(AE_LICENSE_LGPLv3_SHAREDLIB)
#	  error GPLv3 is not compatible with GPLv2/LGPLv3
#	endif
#	if defined(AE_LICENSE_AGPLv3)
#	  error AGPLv3 is not compatible with GPLv2, result will be in AGPLv3
#	endif
#	if defined(AE_LICENSE_CC_BY_NC_SA_3)
#	  error CC BY-NC-SA 3.0 is not compatible with other licenses
#	endif

#	ifdef AE_LICENSE_FREE_NON_COMMERCIAL
#	  define AE_LICENSE		"GNU GPL 2.0, non-commercial"
#	else
#	  define AE_LICENSE		"GNU GPL 2.0"
#	endif


#elif defined(REQUIRE_GPLv3)
//	AE_LICENSE_APACHE_2				- ok
//	AE_LICENSE_BSD/BSD2/BSD3		- ok
//	AE_LICENSE_GPLv3				- ok
//	AE_LICENSE_LGPLv2_1				- ok
//	AE_LICENSE_LGPLv3				- ok
//	AE_LICENSE_LGPLv2_SHAREDLIB		- ok
//	AE_LICENSE_LGPLv3_SHAREDLIB		- ok
//	AE_LICENSE_MIT					- ok
//	AE_LICENSE_ZLIB					- ok
//	AE_LICENSE_MPL_2				- ok
//	AE_LICENSE_UNLICENSE			- ok

#	if defined(AE_LICENSE_LGPLv2_0)
	  // LGPL v2.0: combined work must allow the user to swap out the LGPL library.
	  // GPLv3: No additional restrictions may be imposed on the rights granted by the license.
#	  error GPLv3 is not compatible with LGPLv2, but LGPLv2.1 is compatible
#	endif
#	if defined(AE_LICENSE_GPLv2)
	  // TODO: GPLv2 + FSF GPL (2007) is compatible
#	  error GPLv3 is not compatible with GPLv2
#	endif
#	if defined(AE_LICENSE_AGPLv3)
#	  error AGPLv3 is not compatible with GPLv2, result will be in AGPLv3
#	endif
#	if defined(AE_LICENSE_CC_BY_NC_SA_3)
#	  error CC BY-NC-SA 3.0 is not compatible with other licenses
#	endif

#	ifdef AE_LICENSE_FREE_NON_COMMERCIAL
#	  define AE_LICENSE		"GNU GPL 3.0, non-commercial"
#	else
#	  define AE_LICENSE		"GNU GPL 3.0"
#	endif


#elif defiend(REQUIRE_CC_BY_NC_SA_3)
//	AE_LICENSE_CC_BY_NC_SA_3		- ok
//	TODO

#else
#	error unknown license
#endif

#ifndef AE_LICENSE
#	error output license is not defined
#endif
