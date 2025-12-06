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
			// TODO
		};

		enum class ENormalEncoding
		{
			Unknown		= 0,	// without encoding
		};


	// variables
	private:
		Array<ubyte>				_vertices;
		RC<IntermVertexAttribs>		_attribs;
		Bytes						_vertexStride;
		EPrimitive					_topology		= Default;

		Array<ubyte>				_indices;
		EIndex						_indexType		= Default;

		ENormalEncoding				_normalEncoding	= Default;

		Optional<AABB>				_boundingBox;
		Optional<Sphere>			_boundingSphere;


	// methods
	public:
		IntermMesh ()														__NE___	{}

		void  Set (Array<ubyte> vertices, RC<IntermVertexAttribs> attribs,
				   Bytes vertStride, EPrimitive topology,
				   Array<ubyte> indices, EIndex indexType,
				   ENormalEncoding normalEncoding = Default)				__NE___;

		template <typename V, typename I>
		void  Set (ArrayView<V> vertices, RC<IntermVertexAttribs> attribs,
				   EPrimitive topology, ArrayView<I> indices,
				   ENormalEncoding normalEncoding = Default)				__NE___;

			void  CalcAABB ()												__NE___;
			void  CalcSphere ()												__NE___;
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
		ND_ ENormalEncoding				NormalEncoding ()					C_NE___	{ return _normalEncoding; }

		ND_ Optional<AABB> const&		GetAABB ()							C_NE___	{ return _boundingBox; }
		ND_ Optional<Sphere> const&		GetSphere ()						C_NE___	{ return _boundingSphere; }

		template <typename T>
		ND_ StructView<T>				GetData (const VertName_t &id)		C_NE___;

		template <typename T>
		ND_ StructView<T>				GetDataOpt (const VertName_t &id)	C_NE___;

		template <typename T>
		ND_ ArrayView<T>				GetIndexData ()						C_NE___;

		template <typename T>
		ND_ MutableArrayView<T>			GetIndexData ()						__NE___;


		// Transform //

		// convert all normalized/scaled vertices to float-point type
		ND_ bool  ConvertToFloatPointFormat (OUT IntermMesh &)				C_NE___;

		// change triangle index order: CW <-> CCW
		ND_ bool  InvertFrontFace ()										__NE___;
		ND_ bool  InvertNormals ()											__NE___;
	};



/*
=================================================
	Set
=================================================
*/
	template <typename V, typename I>
	void  IntermMesh::Set (ArrayView<V> vertices, RC<IntermVertexAttribs> attribs, EPrimitive topology, ArrayView<I> indices,
						   ENormalEncoding normalEncoding) __NE___
	{
		StaticAssert(( IsSame< I, uint > or IsSame< I, ushort >));

		_attribs		= RVRef(attribs);
		_vertexStride	= SizeOf<V>;
		_topology		= topology;
		_indexType		= (sizeof(I) == sizeof(uint) ? EIndex::UInt : EIndex::UShort);
		_normalEncoding	= normalEncoding;

		auto*	verts	= vertices.data();
		auto*	indcs	= indices.data();

		_vertices.assign( verts, verts + ArraySizeOf(vertices) );
		_indices.assign( indcs, indcs + ArraySizeOf(indices) );

		_boundingBox.reset();
		_boundingSphere.reset();
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
	ArrayView<T>  IntermMesh::GetIndexData () C_NE___
	{
		if constexpr( IsSame< T, uint >)
		{
			CHECK_ERR( _indexType == EIndex::UInt );
			return ArrayView<T>{ Cast<T>(_indices.data()), _indices.size()/sizeof(T) };
		}
		else
		if constexpr( IsSame< T, ushort >)
		{
			CHECK_ERR( _indexType == EIndex::UShort );
			return ArrayView<T>{ Cast<T>(_indices.data()), _indices.size()/sizeof(T) };
		}
	}

/*
=================================================
	GetIndexData
=================================================
*/
	template <typename T>
	MutableArrayView<T>  IntermMesh::GetIndexData () __NE___
	{
		if constexpr( IsSame< T, uint >)
		{
			CHECK_ERR( _indexType == EIndex::UInt );
			return MutableArrayView<T>{ Cast<T>(_indices.data()), _indices.size()/sizeof(T) };
		}
		else
		if constexpr( IsSame< T, ushort >)
		{
			CHECK_ERR( _indexType == EIndex::UShort );
			return MutableArrayView<T>{ Cast<T>(_indices.data()), _indices.size()/sizeof(T) };
		}
	}

} // AE::ResLoader
