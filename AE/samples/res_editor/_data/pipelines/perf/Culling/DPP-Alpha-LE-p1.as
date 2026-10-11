#include "DPP-shared.as"

#ifdef SCRIPT

	void ASmain ()
	{
		CreatePipeline( "Alpha-LEqual", /*prePass*/false, /*alphaTest*/true, /*cmpEq*/false );
	}

#endif
