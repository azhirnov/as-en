// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/Common.h"
#include "graphics/Public/VertexEnums.h"

namespace AE::Graphics
{

	//
	// Index description
	//
	template <typename T>
	struct IndexDesc {};

	template <>
	struct IndexDesc <ushort>
	{
		using					type	= ushort;
		static constexpr EIndex	value	= EIndex::UShort;
	};

	template <>
	struct IndexDesc <uint>
	{
		using					type	= uint;
		static constexpr EIndex	value	= EIndex::UInt;
	};



	//
	// Vertex description
	//
	template <typename T>
	struct VertexDesc {};


	// Float types
	template <>
	struct VertexDesc <half>
	{
		using							type	= half;
		static constexpr EVertexType	value	= EVertexType::Half;
	};

	template <>
	struct VertexDesc <float>
	{
		using							type	= float;
		static constexpr EVertexType	value	= EVertexType::Float;
	};

	template <>
	struct VertexDesc <double>
	{
		using							type	= double;
		static constexpr EVertexType	value	= EVertexType::Double;
	};


	// Integer types
	template <>
	struct VertexDesc <sbyte>
	{
		using							type	= sbyte;
		static constexpr EVertexType	value	= EVertexType::Byte;
	};

	template <>
	struct VertexDesc <ubyte>
	{
		using							type	= ubyte;
		static constexpr EVertexType	value	= EVertexType::UByte;
	};


	template <>
	struct VertexDesc <sshort>
	{
		using							type	= sshort;
		static constexpr EVertexType	value	= EVertexType::Short;
	};

	template <>
	struct VertexDesc <ushort>
	{
		using							type	= ushort;
		static constexpr EVertexType	value	= EVertexType::UShort;
	};


	template <>
	struct VertexDesc <int>
	{
		using							type	= int3;
		static constexpr EVertexType	value	= EVertexType::Int;
	};

	template <>
	struct VertexDesc <uint>
	{
		using							type	= uint;
		static constexpr EVertexType	value	= EVertexType::UInt;
	};


	template <>
	struct VertexDesc <slong>
	{
		using							type	= slong;
		static constexpr EVertexType	value	= EVertexType::Long;
	};

	template <>
	struct VertexDesc <ulong>
	{
		using							type	= ulong;
		static constexpr EVertexType	value	= EVertexType::ULong;
	};


	// Vector types
	template <typename T, uint I, glm::qualifier Q>
	struct VertexDesc < TVec<T,I,Q> >
	{
		using							type	= TVec<T,I,Q>;
		static constexpr EVertexType	value	= EVertexType(
													(VertexDesc< T >::value & EVertexType::_TypeMask) |
													EVertexType((I-1) << uint(EVertexType::_VecOffset)) );
	};


	template <>
	struct VertexDesc <RGBA32f>
	{
		using							type	= RGBA32f;
		static constexpr EVertexType	value	= EVertexType::Float4;
	};

	template <>
	struct VertexDesc <RGBA32i>
	{
		using							type	= RGBA32i;
		static constexpr EVertexType	value	= EVertexType::Int4;
	};

	template <>
	struct VertexDesc <RGBA32u>
	{
		using							type	= RGBA32u;
		static constexpr EVertexType	value	= EVertexType::UInt4;
	};

	template <>
	struct VertexDesc <RGBA8u>
	{
		using							type	= RGBA8u;
		static constexpr EVertexType	value	= EVertexType::UByte4_Norm;
	};


	template <typename T>
	struct VertexDesc< const T &> : VertexDesc<T>
	{};

	template <typename T>
	struct VertexDesc< T &> : VertexDesc<T>
	{};


} // AE::Graphics
