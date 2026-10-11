#include "DPP-shared.as"

#ifdef SCRIPT

	void ASmain ()
	{
		CreatePipeline( "DPP-Alpha", /*prePass*/true, /*alphaTest*/true, /*cmpEq*/false );
	}

#endif
