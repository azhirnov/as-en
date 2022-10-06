// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL

# include "base/Utils/Version.h"
# include "base/CompileTime/StringToID.h"
# include "graphics/Public/DeviceProperties.h"
# include "graphics/Public/ResourceEnums.h"
# include "graphics/Public/FeatureSet.h"
# include "graphics/Public/DescriptorSet.h"
# include "graphics/Public/GraphicsCreateInfo.h"
# include "graphics/Metal/MQueue.h"
# include "graphics/Metal/MFeatureSet.h"

namespace AE::Graphics
{

	//
	// Metal Device
	//

	class MDevice : public Noncopyable
	{
	// types
	public:
		using MetalVersion	= TVersion2< "Metal"_StringToID >;

		// contains all available resource usage & options and memory types
		struct ResourceFlags
		{
			EBufferUsage	bufferUsage		= Default;
			EBufferOpt		bufferOptions	= Default;

			EImageUsage		imageUsage		= Default;
			EImageOpt		imageOptions	= Default;

			EnumBitSet<EDescriptorType>	descrTypes;

			FixedSet<EMemoryType, 8>	memTypes;
		};

		using Features		= MFeatureSet::Features;
		using Properties	= MFeatureSet::Properties;


	protected:
		using Queues_t			= FixedArray< MQueue, MConfig::MaxQueues >;
		using QueueTypes_t		= StaticArray< MQueuePtr, uint(EQueueType::_Count) >;


	// variables
	protected:
		MetalDeviceRC			_device;
		MetalVersion			_version;

		EQueueMask				_queueMask		= Default;
		QueueTypes_t			_queueTypes		= {};
		Queues_t				_queues;

		DeviceProperties		_devProps;		// platform independent
		MFeatureSet				_mtlFS;
		ResourceFlags			_resFlags;

		DRC_ONLY(
			RWDataRaceCheck		_drCheck;
		)


	// methods
	public:
		MDevice ();
		~MDevice ();
		
		ND_ DeviceProperties const&	GetDeviceProperties ()		const	{ DRC_SHAREDLOCK( _drCheck );  return _devProps; }
		ND_ Features const&			GetFeatures ()				const	{ DRC_SHAREDLOCK( _drCheck );  return _mtlFS.features; }
		ND_ Properties const&		GetProperties ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _mtlFS.properties; }
		ND_ ResourceFlags const&	GetResourceFlags ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _resFlags; }

		ND_ MetalVersion			GetVersion ()				const	{ DRC_SHAREDLOCK( _drCheck );  return _version; }
		ND_ MetalDevice				GetMtlDevice ()				const	{ DRC_SHAREDLOCK( _drCheck );  return _device; }
		ND_ ArrayView<MQueue>		GetQueues ()				const	{ DRC_SHAREDLOCK( _drCheck );  return _queues; }
		ND_ MQueuePtr				GetQueue (EQueueType type)	const	{ DRC_SHAREDLOCK( _drCheck );  return uint(type) < _queueTypes.size() ? _queueTypes[uint(type)] : null; }
		ND_ EQueueMask				GetAvailableQueues ()		const	{ DRC_SHAREDLOCK( _drCheck );  return _queueMask; }
		
		ND_ bool					IsInitialized ()			const	{ DRC_SHAREDLOCK( _drCheck );  return bool(_device); }

		bool  CheckConstantLimits () const;
		bool  CheckExtensions () const;

		void  InitFeatureSet (OUT FeatureSet &outFeatureSet)	const	{ DRC_SHAREDLOCK( _drCheck );  _mtlFS.InitFeatureSet( outFeatureSet ); }

	};
	


	//
	// Metal Device Initializer
	//

	class MDeviceInitializer final : public MDevice
	{
	// variables
	private:
		const bool		_enableInfoLog	= false;


	// methods
	public:
		explicit MDeviceInitializer (bool enableInfoLog = false);
		~MDeviceInitializer ();
		
		ND_ bool  CreateDefaultQueue ();
		ND_ bool  CreateDefaultQueues (EQueueMask required, EQueueMask optional = Default);

		ND_ bool  CreateLogicalDevice ();
			bool  DestroyLogicalDevice ();
		
		ND_ bool  Init (const GraphicsCreateInfo &ci);

	private:
		void  _SetResourceFlags (OUT ResourceFlags &) const;
		void  _InitDeviceProperties (OUT DeviceProperties &props) const;
		void  _InitFeaturesAndProperties (OUT Features &, OUT Properties &) const;

		void  _LogLogicalDevice () const;

		ND_ MGPUFamilies  _GetGPUFamilies () const;
	};


}	// AE::Graphics

#endif	// AE_ENABLE_METAL
