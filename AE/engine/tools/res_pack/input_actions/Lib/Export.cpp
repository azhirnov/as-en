#include "../InputActionsBinding.h"

#ifdef AE_PLATFORM_UNIX_BASED
	extern "C" AE_DLL_EXPORT void* GetConvertInputActions ()
	{
		return reinterpret_cast<void*>(&AE::InputActions::ConvertInputActions);
	}
#endif
