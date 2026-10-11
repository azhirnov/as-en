#include "DPP-shared.as"

#ifdef SCRIPT

	void ASmain ()
	{
		CreatePipeline( "DPP-Opaque", /*prePass*/true, /*alphaTest*/false, /*cmpEq*/false );
	}

#endif
