// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_loaders/Intermediate/IntermVertexAttribs.h"

namespace AE::ResLoader
{
	using AE::Graphics::EPrimitive;
	using AE::Graphics::EIndex;


	//
	// Intermediate Mesh
	//

	class IntermMesh final : public EnableRC<IntermMesh>
	{
	// types
	private:
		using VertName_t	= VertexAttributeName::Name_t;

	public:
		struct Meshlet
		{
		};


	// variables
	private:
		Array<ubyte>				_vertices;
		RC<IntermVertexAttribs>		_attribs;
		Bytes						_vertexStride;
		EPrimitive					_topology		= Default;

		Array<ubyte>				_indices;
		EIndex						_indexType		= Default;

		Optional<AABB>				_boundingBox;


	// methods
	public:
		IntermMesh ()														__NE___	{}

		IntermMesh (Array<ubyte> vertices, RC<IntermVertexAttribs> attribs,
					Bytes vertStride, EPrimitive topology,
					Array<ubyte> indices, EIndex indexType)					__NE___;

		template <typename V, typename I>
		IntermMesh (ArrayView<V> vertices, RC<IntermVertexAttribs> attribs,
					EPrimitive topology, ArrayView<I> indices)				__NE___;

			void  CalcAABB ()												__NE___;
		ND_ bool  IsValid ()												C_NE___;

		ND_ ArrayView<ubyte>			Vertices ()							C_NE___	{ return _vertices; }
		ND_ ArrayView<ubyte>			Indices ()							C_NE___	{ return _indices; }
		ND_ IntermVertexAttribs const*	Attribs ()							C_NE___	{ return _attribs.get(); }

		ND_ size_t						VertexCount ()						C_NE___	{ return size_t(ArraySizeOf(_vertices) / _vertexStride); }
		ND_ Bytes						VertexStride ()						C_NE___	{ return _vertexStride; }
		ND_ EPrimitive					Topology ()							C_NE___	{ return _topology; }
		ND_ EIndex						IndexType ()						C_NE___	{ return _indexType; }
		ND_ Bytes						IndexStride ()						C_NE___;
		ND_ size_t						IndexCount ()						C_NE___	{ return size_t(ArraySizeOf(_indices) / IndexStride()); }

		ND_ Optional<AABB> const&		GetAABB ()							C_NE___	{ return _boundingBox; }

		template <typename T>
		ND_ StructView<T>				GetData (const VertName_t &id)		C_NE___;

		template <typename T>
		ND_ StructView<T>				GetDataOpt (const VertName_t &id)	C_NE___;

		template <typename T>
		ND_ StructView<T>				GetIndexData ()						C_NE___;
	};



/*
=================================================
	constructor
=================================================
*/
	template <typename V, typename I>
	IntermMesh::IntermMesh (ArrayView<V> vertices, RC<IntermVertexAttribs> attribs,
							EPrimitive topology, ArrayView<I> indices) __NE___ :
		_vertexStride{ SizeOf<V> }, _attribs{ RVRef(attribs) },
		_topology{ topology }, _indexType{ sizeof(I) == sizeof(uint) ? EIndex::UInt : EIndex::UShort }
	{
		StaticAssert(( IsSameTypes< I, uint > or IsSameTypes< I, ushort >));

		auto*	verts	= vertices.data();
		auto*	indcs	= indices.data();

		_vertices.assign( verts, verts + ArraySizeOf(vertices) );
		_indices.assign( indcs, indcs + ArraySizeOf(indices) );
	}

/*
=================================================
	GetData
=================================================
*/
	template <typename T>
	StructView<T>  IntermMesh::GetData (const VertName_t &id) C_NE___
	{
		ASSERT( _attribs );
		ASSERT( _vertexStride != 0 );
		return _attribs->GetData<T>( id, _vertices.data(), VertexCount(), _vertexStride );
	}

	template <typename T>
	StructView<T>  IntermMesh::GetDataOpt (const VertName_t &id) C_NE___
	{
		ASSERT( _attribs );
		ASSERT( _vertexStride != 0 );
		return _attribs->GetDataOpt<T>( id, _vertices.data(), VertexCount(), _vertexStride );
	}

/*
=================================================
	GetIndexData
=================================================
*/
	template <typename T>
	StructView<T>  IntermMesh::GetIndexData () C_NE___
	{
		if constexpr( IsSameTypes< T, uint >)
		{
			CHECK_ERR( _indexType == EIndex::UInt );
			return StructView<T>{ _indices.data(), _indices.size()/4, 4_b };
		}
		else
		if constexpr( IsSameTypes< T, ushort >)
		{
			CHECK_ERR( _indexType == EIndex::UShort );
			return StructView<T>{ _indices.data(), _indices.size()/2, 2_b };
		}
	}


} // AE::ResLoader
