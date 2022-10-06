// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Vec.h"
#include "base/Math/Rectangle.h"
#include "base/Math/Color.h"
#include "base/Math/PhysicalQuantityVec.h"

#include "base/Containers/FixedString.h"
#include "base/Containers/FixedMap.h"
#include "base/Containers/FixedSet.h"
#include "base/Containers/FixedTupleArray.h"
#include "base/Containers/Union.h"
#include "base/Containers/StructView.h"
#include "base/Containers/AnyTypeRef.h"

#include "base/Stream/Stream.h"
#include "base/Stream/FastStream.h"
#include "base/Utils/NamedID.h"
#include "base/Utils/NamedID_HashCollisionCheck.h"
#include "base/Utils/Noncopyable.h"

#include "base/Algorithms/ArrayUtils.h"
#include "base/Memory/IAllocator.h"

#include "threading/Common.h"

// for NamedID
#define AE_SERIALIZE_HASH_ONLY	1
#define AE_DEBUG_SERIALIZER		0

namespace AE::Serializing
{
	using namespace AE::Base;

	using SharedMutex = Threading::SharedMutex;
	
	using SerializedID = NamedID< 32, 0x400, AE_OPTIMIZE_IDS, UMax >;
	
	struct Serializer;
	struct Deserializer;
	class ObjectFactory;

	// IsTrivialySerializable<> defined in 'base/CompileTime/TypeTraits.h'

}	// AE::Serializing
