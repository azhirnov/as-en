// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/FeatureSet.h"
#include "graphics/Public/DeviceProperties.h"

namespace AE::Graphics
{
	struct FeatureSetExt : FeatureSet
	{
		struct ExtFields
		{
		#define AE_FEATURE_SET_FIELDS2( _visitor_ ) \
			/* ResourceAlignment */\
			_visitor_( Bytes,	minUniformBufferOffsetAlignment				)\
			_visitor_( Bytes,	minStorageBufferOffsetAlignment				)\
			_visitor_( Bytes,	minTexelBufferOffsetAlignment				)\
			_visitor_( Bytes,	maxUniformBufferRange						)\
			_visitor_( uint,	maxBoundDescriptorSets						)\
			_visitor_( Bytes,	minMemoryMapAlignment						)\
			_visitor_( Bytes,	nonCoherentAtomSize							)\
			_visitor_( Bytes,	optimalBufferCopyOffsetAlignment			)\
			_visitor_( Bytes,	optimalBufferCopyRowPitchAlignment			)\
			/* RayTracingProperties */\
			_visitor_( Bytes,	minAccelerationStructureScratchOffsetAlignment )\
			_visitor_( ulong,	maxGeometryCount							)\
			_visitor_( ulong,	maxInstanceCount							)\
			_visitor_( ulong,	maxPrimitiveCount							)\
			_visitor_( uint,	maxRayRecursionDepth						)\

			#define AE_FEATURE_SET_VISIT( _type_, _name_ )		_type_	_name_;
			AE_FEATURE_SET_FIELDS2( AE_FEATURE_SET_VISIT )
			#undef AE_FEATURE_SET_VISIT
		};

		ExtFields	ext;

		FeatureSetExt () = default;
		FeatureSetExt (const FeatureSetExt &) = default;

		FeatureSetExt&  operator = (const FeatureSetExt &)		= default;
		FeatureSetExt&  operator = (const FeatureSet &rhs)		{ FeatureSet::operator = (rhs);  return *this; }
	};


	bool  FeatureSetFromJSON (const Path &jsonFile, OUT FeatureSetExt &fs, OUT String &name);
	bool  FeatureSetToScript (const Path &outFile, StringView fsName, const FeatureSet &fs, StringView comment = Default);

} // AE::Graphics
