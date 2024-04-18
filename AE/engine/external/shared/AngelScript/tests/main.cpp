
#ifndef AE_ENABLE_ANGELSCRIPT
# error AE_ENABLE_ANGELSCRIPT required
#endif

#include "angelscript.h"
#include "scriptarray.h"
#include "scriptstdstring.h"

using namespace AngelScript;

#ifdef ANDROID
# include "TestHelper.h"

extern int AE_AngelScript_Test ()
#else
int main ()
#endif
{
	asIScriptEngine* eng;

	eng = asCreateScriptEngine( ANGELSCRIPT_VERSION );
	if ( !eng )
		return 1;

	RegisterScriptArray( eng, true );
	RegisterStdString( eng );

	eng->ShutDownAndRelease();
	return 0;
}
