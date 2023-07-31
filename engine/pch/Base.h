// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Common.h"

// CompileTime
#include "base/CompileTime/Math.h"
#include "base/CompileTime/StringToID.h"
#include "base/CompileTime/TypeList.h"
#include "base/CompileTime/FunctionInfo.h"

// Algorithms
#include "base/Algorithms/ArrayUtils.h"
#include "base/Algorithms/Cast.h"
#include "base/Algorithms/StringUtils.h"
#include "base/Algorithms/StringParser.h"

// Containers
#include "base/Containers/AnyTypeRef.h"
#include "base/Containers/ArrayView.h"
#include "base/Containers/FixedArray.h"
#include "base/Containers/FixedMap.h"
#include "base/Containers/FixedSet.h"
#include "base/Containers/FixedString.h"
#include "base/Containers/FixedTupleArray.h"
#include "base/Containers/InPlace.h"
#include "base/Containers/NtStringView.h"
#include "base/Containers/Ptr.h"
#include "base/Containers/RingBuffer.h"
#include "base/Containers/StructView.h"
#include "base/Containers/TupleArrayView.h"
#include "base/Containers/Union.h"
#include "base/Containers/UntypedStorage.h"

// DataSource
#include "base/DataSource/Stream.h"
#include "base/DataSource/DataSource.h"
#include "base/DataSource/StdStream.h"
#include "base/DataSource/MemStream.h"
#include "base/DataSource/MemDataSource.h"
#include "base/DataSource/FileStream.h"
#include "base/DataSource/WindowsFile.h"
#include "base/DataSource/BrotliStream.h"
#include "base/DataSource/BufferedStream.h"
#include "base/DataSource/DataSourceAsStream.h"
#include "base/DataSource/DataSourceRange.h"
#include "base/DataSource/FastStream.h"

// Crypto
#include "base/Crypto/SimpleCrypto.h"

// Math
#include "base/Math/Bytes.h"
#include "base/Math/Radians.h"
#include "base/Math/VecSwizzle.h"
#include "base/Math/Fractional.h"
#include "base/Math/Float8.h"
#include "base/Math/Float16.h"
#include "base/Math/Vec.h"
#include "base/Math/Color.h"
#include "base/Math/Quat.h"
#include "base/Math/Matrix.h"
#include "base/Math/Transformation.h"
#include "base/Math/Spherical.h"
#include "base/Math/Sphere.h"
#include "base/Math/Rectangle.h"
#include "base/Math/Range.h"
#include "base/Math/Random.h"
#include "base/Math/PseudoRandom.h"
#include "base/Math/POTValue.h"
#include "base/Math/PhysicalQuantity.h"
#include "base/Math/PhysicalQuantityVec.h"
#include "base/Math/PhysicalQuantityMatrix.h"
#include "base/Math/SIMD_Neon.h"
#include "base/Math/SIMD_SSE.h"
#include "base/Math/Packing.h"
#include "base/Math/Frustum.h"
#include "base/Math/AABB.h"
#include "base/Math/Camera.h"
#include "base/Math/FPVCamera.h"
#include "base/Math/FlightCamera.h"
#include "base/Math/OrbitalCamera.h"
#include "base/Math/PackedInt.h"
#include "base/Math/MatrixStorage.h"

// Memory
#include "base/Memory/AllocatorFwdDecl.h"
#include "base/Memory/IAllocator.h"
#include "base/Memory/UntypedAllocator.h"
#include "base/Memory/SharedMem.h"
#include "base/Memory/StackAllocator.h"
#include "base/Memory/LinearAllocator.h"
#include "base/Memory/AllocatorRef.h"
#include "base/Memory/MemUtils.h"
#include "base/Memory/MemWriter.h"

// Platforms
#include "base/Platforms/CPUInfo.h"
#include "base/Platforms/Platform.h"

// Utils
#include "base/Utils/Date.h"
#include "base/Utils/EnumBitSet.h"
#include "base/Utils/FileSystem.h"
#include "base/Utils/HandleTmpl.h"
#include "base/Utils/Helpers.h"
#include "base/Utils/NamedID.h"
#include "base/Utils/NamedID_HashCollisionCheck.h"
#include "base/Utils/NamedID_HashCollisionCheckTS.h"
#include "base/Utils/RefCounter.h"
#include "base/Utils/RestrictPtr.h"
#include "base/Utils/SourceLoc.h"
#include "base/Utils/Threading.h"
#include "base/Utils/TimeProfiler.h"
#include "base/Utils/TypeId.h"
#include "base/Utils/Version.h"


