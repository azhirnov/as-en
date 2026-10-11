#include "DPP-shared.as"

#ifdef SCRIPT

	void ASmain ()
	{
		CreatePipeline( "Alpha-Equal", /*prePass*/false, /*alphaTest*/true, /*cmpEq*/true );
	}

#endif
