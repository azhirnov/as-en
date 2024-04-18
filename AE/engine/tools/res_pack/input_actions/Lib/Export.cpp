#include "../InputActionsBinding.h"

#ifdef AE_PLATFORM_UNIX_BASED
	extern "C" void* AE_DLL_EXPORT GetConvertInputActions ()
	{
		return reinterpret_cast<void*>(&AE::InputActions::ConvertInputActions);
	}
#endif
