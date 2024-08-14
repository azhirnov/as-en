// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_CDT

# ifdef AE_COMPILER_MSVC
#	pragma warning (push, 0)
#	pragma warning (disable: 4701)
#	pragma warning (disable: 4305)
# endif

# include "CDT.h"

# ifdef AE_COMPILER_MSVC
#	pragma warning (pop)
# endif

# include "CDT_Triangulator.h"

namespace AE::GeometryTools
{
namespace
{
	template <typename T>	ND_ T const&  GetXv2 (const Vec<T,2> &v)			__NE___	{ return v.x; }
	template <typename T>	ND_ T const&  GetYv2 (const Vec<T,2> &v)			__NE___	{ return v.y; }

							ND_ CDT::VertInd  EdgeGetI1 (const CDT::Edge& e)	__NE___	{ return e.v1(); }
							ND_ CDT::VertInd  EdgeGetI2 (const CDT::Edge& e)	__NE___	{ return e.v2(); }


/*
=================================================
	AppendIndexedLineList
=================================================
*/
	static void  AppendIndexedLineList (CDT::Triangulation<float>				&cdt,
										MutableArrayView<CDT::Edge>				tempEdges,
										CDT_Triangulator::InArrayOfVertices_t	verticesArray,
										CDT_Triangulator::InArrayOfIndices_t	indicesArray,
										const bool								conformingDelaunayTriangulation) __Th___
	{
		CHECK_THROW( verticesArray.size() == indicesArray.size() );

		uint	edge_cnt	= 0;
		uint	base_vert	= 0;

		for (usize i = 0; i < indicesArray.size(); ++i)
		{
			auto&	vertices	= verticesArray[i];
			auto&	indices		= indicesArray[i];
			usize	idx_pos		= 0;

			CHECK_THROW( indices.size() >= 3 );
			CHECK_THROW( IsMultipleOf( indices.size(), 2 ));
			ASSERT_MSG( AdditionIsSafe( base_vert, vertices.size() ), "overflow!" );

			for (; idx_pos < indices.size();)
			{
				const usize	count = Min( (indices.size() - idx_pos)/2, tempEdges.size() );

				for (edge_cnt = 0; edge_cnt < count; ++edge_cnt, idx_pos += 2)
				{
					ASSERT( indices[idx_pos+0] < vertices.size() );
					ASSERT( indices[idx_pos+1] < vertices.size() );

					tempEdges[edge_cnt] = CDT::Edge{ indices[idx_pos+0] + base_vert,
													 indices[idx_pos+1] + base_vert };
				}
				ASSERT( idx_pos <= indices.size() );

				if ( edge_cnt > 0 )
				{
					if ( conformingDelaunayTriangulation ) {
						cdt.conformToEdges( tempEdges.data(), tempEdges.data() + edge_cnt, EdgeGetI1, EdgeGetI2 );
					} else {
						cdt.insertEdges( tempEdges.data(), tempEdges.data() + edge_cnt, EdgeGetI1, EdgeGetI2 );
					}
				}
			}

			base_vert += uint(vertices.size());
		}
	}

/*
=================================================
	AppendIndexedLineStrip
=================================================
*/
	static void  AppendIndexedLineStrip (CDT::Triangulation<float>				&cdt,
										 MutableArrayView<CDT::Edge>			tempEdges,
										 CDT_Triangulator::InArrayOfVertices_t	verticesArray,
										 CDT_Triangulator::InArrayOfIndices_t	indicesArray,
										 const bool								conformingDelaunayTriangulation) __Th___
	{
		CHECK_THROW( verticesArray.size() == indicesArray.size() );

		uint	edge_cnt	= 0;
		uint	base_vert	= 0;

		for (usize i = 0; i < indicesArray.size(); ++i)
		{
			auto&	vertices	= verticesArray[i];
			auto&	indices		= indicesArray[i];
			usize	idx_pos		= 0;

			CHECK_THROW( indices.size() >= 3 );
			ASSERT_MSG( AdditionIsSafe( base_vert, vertices.size() ), "overflow!" );

			for (; idx_pos < indices.size();)
			{
				const usize	count = Min( indices.size()-1 - idx_pos, tempEdges.size()-1 );

				for (edge_cnt = 0; edge_cnt < count; ++edge_cnt, ++idx_pos)
				{
					ASSERT( indices[idx_pos+0] < vertices.size() );
					ASSERT( indices[idx_pos+1] < vertices.size() );

					tempEdges[edge_cnt] = CDT::Edge{ indices[idx_pos+0] + base_vert,
													 indices[idx_pos+1] + base_vert };
				}
				ASSERT( idx_pos < indices.size() );

				if ( idx_pos+1 == indices.size() )
				{
					tempEdges[edge_cnt] = CDT::Edge{ uint(indices[idx_pos] + base_vert),
													 uint(indices[0] + base_vert) };
					++idx_pos;
					++edge_cnt;
				}

				if ( edge_cnt > 0 )
				{
					if ( conformingDelaunayTriangulation ) {
						cdt.conformToEdges( tempEdges.data(), tempEdges.data() + edge_cnt, EdgeGetI1, EdgeGetI2 );
					} else {
						cdt.insertEdges( tempEdges.data(), tempEdges.data() + edge_cnt, EdgeGetI1, EdgeGetI2 );
					}
				}
			}

			base_vert += uint(vertices.size());
		}
	}

/*
=================================================
	AppendLineStrip
=================================================
*/
	static void  AppendLineStrip (CDT::Triangulation<float>				&cdt,
								  MutableArrayView<CDT::Edge>			tempEdges,
								  CDT_Triangulator::InArrayOfVertices_t	verticesArray,
								  const bool							conformingDelaunayTriangulation) __Th___
	{
		uint	edge_cnt	= 0;
		uint	base_vert	= 0;

		for (usize i = 0; i < verticesArray.size(); ++i)
		{
			auto&	vertices	= verticesArray[i];
			usize	vert_idx	= 0;

			CHECK_THROW( vertices.size() >= 3 );
			ASSERT_MSG( AdditionIsSafe( base_vert, vertices.size() ), "overflow!" );

			for (; vert_idx < vertices.size();)
			{
				const usize	count = Min( vertices.size()-1 - vert_idx, tempEdges.size()-1 );

				for (edge_cnt = 0; edge_cnt < count; ++edge_cnt, ++vert_idx)
				{
					tempEdges[edge_cnt] = CDT::Edge{ uint(vert_idx+0 + base_vert),
													 uint(vert_idx+1 + base_vert) };
				}
				ASSERT( vert_idx < vertices.size() );

				if ( vert_idx+1 == vertices.size() )
				{
					tempEdges[edge_cnt] = CDT::Edge{ uint(vert_idx + base_vert),
													 uint(0 + base_vert) };
					++vert_idx;
					++edge_cnt;
				}

				if ( edge_cnt > 0 )
				{
					if ( conformingDelaunayTriangulation ) {
						cdt.conformToEdges( tempEdges.data(), tempEdges.data() + edge_cnt, EdgeGetI1, EdgeGetI2 );
					} else {
						cdt.insertEdges( tempEdges.data(), tempEdges.data() + edge_cnt, EdgeGetI1, EdgeGetI2 );
					}
				}
			}

			base_vert += uint(vertices.size());
		}
	}

} // namespace
//-----------------------------------------------------------------------------


/*
=================================================
	_Triangulate
=================================================
*/
	bool  CDT_Triangulator::_Triangulate (InArrayOfVertices_t		verticesArray,
										  InArrayOfIndices_t		indicesArray,
										  EFlags					flags,
										  OUT OutVertices_t			&outVertices,
										  OUT OutIndices_t			&outIndices,
										  OUT OutBoundaryEdges_t*	optBoundary) __Th___
	{
		CHECK_ERR( IsSingleBitSet( flags & EFlags::_TriangulationFlags ));
		CHECK_ERR( IsSingleBitSet( flags & EFlags::_IndicesMode ));

		outVertices.clear();
		outIndices.clear();

		constexpr float	c_minDistToConstraintEdge	= 1.0e-5f;
		constexpr uint	c_TempEdgeCount				= 512;

		CDT::Triangulation<float>	cdt{ CDT::VertexInsertionOrder::Auto,
										 AllBits( flags, EFlags::ResolveIntersections ) ?
											CDT::IntersectingConstraintEdges::TryResolve :
											CDT::IntersectingConstraintEdges::DontCheck,
										 c_minDistToConstraintEdge };

		// add vertices
		for (auto& vertices : verticesArray)
		{
			CHECK_ERR( not vertices.empty() );
			cdt.insertVertices( vertices.begin(), vertices.end(), GetXv2<float>, GetYv2<float> );
		}

		// add indices
		{
			using EdgeArr_t = UninitializedStaticArray< CDT::Edge, c_TempEdgeCount >;

			EdgeArr_t	edge_storage;
			auto		edge_arr	= MutableArrayView<CDT::Edge>{ edge_storage.Ptr<CDT::Edge>(), c_TempEdgeCount };

			switch ( flags & EFlags::_IndicesMode )
			{
				case EFlags::IndexedLineStrip :
					AppendIndexedLineStrip( cdt, edge_arr, verticesArray, indicesArray, AllBits( flags, EFlags::ConformingDelaunayTriangulation ));
					break;

				case EFlags::IndexedLineList :
					AppendIndexedLineList( cdt, edge_arr, verticesArray, indicesArray, AllBits( flags, EFlags::ConformingDelaunayTriangulation ));
					break;

				case EFlags::LineStrip :
					AppendLineStrip( cdt, edge_arr, verticesArray, AllBits( flags, EFlags::ConformingDelaunayTriangulation ));
					break;

				default :	return false;
			}
		}

		switch ( flags & EFlags::_TriangulationFlags )
		{
			case EFlags::ConformingDelaunayTriangulation :		cdt.eraseOuterTrianglesAndHoles();	break;
			case EFlags::ConstrainedDelaunayTriangulation :		cdt.eraseOuterTrianglesAndHoles();	break;
			case EFlags::DelaunayTriangulation :				cdt.eraseSuperTriangle();			break;
			default :											return false;
		}

		// copy vertices
		outVertices.resize( cdt.vertices.size() );  // throw

		for (const auto [v,i] : WithIndex( cdt.vertices )) {
			outVertices[i] = float2{ v.x, v.y };
		}

		// copy indices
		outIndices.resize( cdt.triangles.size() * 3 );  // throw

		for (const auto [tri, i] : WithIndex( cdt.triangles ))
		{
			const usize	j = i*3;
			outIndices[j+0] = tri.vertices[0];
			outIndices[j+1] = tri.vertices[1];
			outIndices[j+2] = tri.vertices[2];
		}

		// copy boundary
		if ( optBoundary != null )
		{
		#if 0
			optBoundary->resize( cdt.fixedEdges.size()*2 );  // throw

			usize	i = 0;
			for (auto& edge : cdt.fixedEdges)
			{
				(*optBoundary)[i+0] = edge.v1();
				(*optBoundary)[i+1] = edge.v2();
				i += 2;
			}
		#else

			optBoundary->clear();
			optBoundary->reserve( cdt.fixedEdges.size()*2 );  // throw

			for (const auto [tri, i] : WithIndex( cdt.triangles ))
			{
				if ( tri.neighbors[0] == CDT::noNeighbor )
				{
					optBoundary->push_back( tri.vertices[0] );
					optBoundary->push_back( tri.vertices[1] );
				}

				if ( tri.neighbors[1] == CDT::noNeighbor )
				{
					optBoundary->push_back( tri.vertices[1] );
					optBoundary->push_back( tri.vertices[2] );
				}

				if ( tri.neighbors[2] == CDT::noNeighbor )
				{
					optBoundary->push_back( tri.vertices[2] );
					optBoundary->push_back( tri.vertices[0] );
				}
			}
		#endif
		}

		return true;
	}

/*
=================================================
	Triangulate
=================================================
*/
	bool  CDT_Triangulator::Triangulate (InArrayOfVertices_t		verticesArray,
										 InArrayOfIndices_t			indicesArray,
										 EFlags						flags,
										 OUT OutVertices_t			&outVertices,
										 OUT OutIndices_t			&outIndices,
										 OUT OutBoundaryEdges_t*	optBoundary) __NE___
	{
		TRY{
			return _Triangulate( verticesArray, indicesArray, flags,
								 OUT outVertices, OUT outIndices, OUT optBoundary );
		}
		CATCH_ALL(
			return false;
		)
	}

	bool  CDT_Triangulator::Triangulate (InVertices_t				vertices,
										 InIndices_t				indices,
										 EFlags						flags,
										 OUT OutVertices_t			&outVertices,
										 OUT OutIndices_t			&outIndices,
										 OUT OutBoundaryEdges_t*	optBoundary) __NE___
	{
		TRY{
			return _Triangulate( InArrayOfVertices_t{vertices}, InArrayOfIndices_t{indices}, flags,
								 OUT outVertices, OUT outIndices, OUT optBoundary );
		}
		CATCH_ALL(
			return false;
		)
	}

	bool  CDT_Triangulator::Triangulate (InVertices_t				contour,
										 EFlags						flags,
										 OUT OutVertices_t			&outVertices,
										 OUT OutIndices_t			&outIndices,
										 OUT OutBoundaryEdges_t*	optBoundary) __NE___
	{
		TRY{
			CHECK_ERR( AllBits( flags, EFlags::LineStrip ));

			return _Triangulate( InArrayOfVertices_t{contour}, InArrayOfIndices_t{}, flags,
								 OUT outVertices, OUT outIndices, OUT optBoundary );
		}
		CATCH_ALL(
			return false;
		)
	}


} // AE::GeometryTools

#endif // AE_ENABLE_CDT
