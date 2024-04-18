// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_loaders/Public/Common.h"

namespace AE::ResLoader
{
	using Graphics::EVertexType;
	using Graphics::EVertexInputRate;
	using Graphics::VertexDesc;


	struct VertexAttributeName
	{
		using Name_t = FixedString<32>;

		static constexpr Name_t		Position		{"Position"};
		static constexpr Name_t		Position_1		{"Position_1"};

		static constexpr Name_t		Normal			{"Normal"};
		static constexpr Name_t		BiTangent		{"BiTangent"};
		static constexpr Name_t		Tangent			{"Tangent"};

		static constexpr Name_t		ObjectID		{"ObjectID"};
		static constexpr Name_t		MaterialID		{"MaterialID"};

		static constexpr Name_t		BoneWeights		{"BoneWeights"};

		static constexpr Name_t		LightmapUV		{"LightmapUV"};
		static constexpr Name_t		TextureUVs[] =	{"TextureUV", "TextureUV_1", "TextureUV_2", "TextureUV_3"};

		static constexpr Name_t		Color			{"Color"};
	};



	//
	// Intermediate Vertex Attributes
	//

	class IntermVertexAttribs final : public EnableRC<IntermVertexAttribs>
	{
	// types
	public:
		using Name_t = VertexAttributeName::Name_t;

		enum class BufferIndex : uint { Unknown = ~0u };


		struct VertexInput
		{
			EVertexType			type			= Default;	// src type, if src type is normalized short3, then dst type is float3.
			uint				index			= UMax;
			Bytes				offset;
			BufferIndex			bufferBinding	= Default;

			ND_ HashVal	CalcHash ()							C_NE___;
			ND_ bool	operator == (const VertexInput &)	C_NE___;
		};


		struct BufferBinding
		{
			BufferIndex			index			= Default;
			Bytes				stride;
			EVertexInputRate	rate			= Default;

			ND_ HashVal	CalcHash ()							C_NE___;
			ND_ bool	operator == (const BufferBinding &) C_NE___;
		};

		using Vertices_t	= FixedMap< Name_t,	VertexInput,	Graphics::GraphicsConfig::MaxVertexAttribs >;
		using Bindings_t	= FixedMap< Name_t,	BufferBinding,	Graphics::GraphicsConfig::MaxVertexBuffers >;

		class Setter;


	// variables
	private:
		Vertices_t		_vertices;
		Bindings_t		_bindings;


	// methods
	public:
		IntermVertexAttribs ()														__NE___ {}

		template <typename T>
		ND_ StructView<T>	GetData (const Name_t &id, const void* vertexData,
									 usize vertexCount, Bytes stride)				C_NE___;

		template <typename T>
		ND_ StructView<T>	GetDataOpt (const Name_t &id, const void* vertexData,
										usize vertexCount, Bytes stride)			C_NE___;

		ND_ HashVal			CalcHash ()												C_NE___;

		ND_ bool			operator == (const IntermVertexAttribs &rhs)			C_NE___;

		ND_ bool			HasVertex (StringView name, EVertexType type)			C_NE___;

		ND_ Vertices_t				Vertices ()										C_NE___	{ return _vertices; }
		ND_ Bindings_t const&		BufferBindings ()								C_NE___	{ return _bindings; }

		ND_ VertexInput const*		FindVertex (StringView name)					C_NE___;
		ND_ BufferBinding const*	FindBuffer (StringView name)					C_NE___;

	private:
		ND_ bool  _GetData (const Name_t &id, Bytes stride, EVertexType type,
							bool optional, OUT Bytes &offset)						C_NE___;
	};



/*
=================================================
	GetData
=================================================
*/
	template <typename T>
	StructView<T>  IntermVertexAttribs::GetData (const Name_t &id, const void* vertexData, usize vertexCount, Bytes stride) C_NE___
	{
		Bytes	offset;
		if ( _GetData( id, stride, VertexDesc<T>::value, false, OUT offset ))
			return StructView<T>{ vertexData + offset, vertexCount, stride };
		else
			return Default;
	}

	template <typename T>
	StructView<T>  IntermVertexAttribs::GetDataOpt (const Name_t &id, const void* vertexData, usize vertexCount, Bytes stride) C_NE___
	{
		Bytes	offset;
		if ( _GetData( id, stride, VertexDesc<T>::value, true, OUT offset ))
			return StructView<T>{ vertexData + offset, vertexCount, stride };
		else
			return Default;
	}


} // AE::ResLoader
