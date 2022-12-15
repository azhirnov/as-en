
void main ()
{
	{
		FeatureSet@  fset = FeatureSet( "part.AsyncCompute" );

		fset.supportedQueues(EQueueMask( EQueueMask::Graphics | EQueueMask::AsyncCompute ));
	}
	{
		FeatureSet@  fset = FeatureSet( "part.AllQueues" );

		fset.supportedQueues(EQueueMask( EQueueMask::Graphics | EQueueMask::AsyncCompute | EQueueMask::AsyncTransfer ));
	}
}
