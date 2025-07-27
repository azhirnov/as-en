#include "res_pack/input_actions/InputActionsBinding.h"

#ifdef AE_PLATFORM_UNIX_BASED
	extern "C" bool AE_DLL_IMPORT ConvertInputActions (const InputActionsInfo* info);

	extern "C" AE_DLL_EXPORT void* GetConvertInputActions ()
	{
		return reinterpret_cast<void*>(&AE::InputActions::ConvertInputActions);
	}
#endif
