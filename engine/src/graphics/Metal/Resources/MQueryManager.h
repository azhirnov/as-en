// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL

# include "graphics/Public/QueryManager.h"
# include "graphics/Metal/MDevice.h"

namespace AE::Graphics
{
	
	//
	// Metal Query Manager
	//

	class MQueryManager final : public IQueryManager
	{
	// types
	public:
		struct Query
		{
		};
		
		struct PipelineStatistic
		{
		};


	// variables
	private:


	// methods
	public:
		MQueryManager ()																													__NE___;
		~MQueryManager ()																													__NE___;

		ND_ bool  Initialize (const MDevice &dev, uint maxFrames)																			__NE___;
			void  Deinitialize (const MDevice &dev)																							__NE___;

			void  NextFrame (const MDevice &dev, FrameUID frameId)																			__NE___;

		//ND_ Query  AllocQuery (EQueueType queue, EQueryType type, uint count = 1)															C_NE___;

		/*
			VkResult  GetTimestamp (const MDevice &dev, const Query &q, OUT ulong* result, Bytes size)										C_NE___;	// raw
			VkResult  GetTimestamp (const MDevice &dev, const Query &q, OUT double* result, Bytes size)										C_NE___;	// nanoseconds
			VkResult  GetTimestamp (const MDevice &dev, const Query &q, OUT nanosecondsd* result, Bytes size)								C_NE___;
			VkResult  GetTimestampCalibrated (const MDevice &dev, const Query &q, OUT ulong* result, OUT ulong* maxDeviation, Bytes size)	C_NE___;
			VkResult  GetPipelineStatistic (const MDevice &dev, const Query &q, OUT PipelineStatistic* result, Bytes size)					C_NE___;
		*/
	};


} // AE::Graphics

#endif // AE_ENABLE_METAL
