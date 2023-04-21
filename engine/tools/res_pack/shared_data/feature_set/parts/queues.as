#include <pipeline_compiler>

void ASmain ()
{
	{
		RC<FeatureSet>  fset = FeatureSet( "part.AsyncCompute" );

		fset.supportedQueues(EQueueMask( EQueueMask::Graphics | EQueueMask::AsyncCompute ));
	}
	{
		RC<FeatureSet>  fset = FeatureSet( "part.AllQueues" );

		fset.supportedQueues(EQueueMask( EQueueMask::Graphics | EQueueMask::AsyncCompute | EQueueMask::AsyncTransfer ));
	}
}
