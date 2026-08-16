// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#ifdef AE_ENABLE_CDT
# include "geometry_tools/Common.h"

namespace AE::GeometryTools
{

	//
	// CDT Triangulator
	//

	class CDT_Triangulator final : public Noninstanceable
	{
	// types
	public:
		enum class EFlags : uint
		{
			// set only one triangulation mode
			DelaunayTriangulation				= 1 << 0,	// convex hull
			ConstrainedDelaunayTriangulation	= 1 << 1,
			ConformingDelaunayTriangulation		= 1 << 2,
			_TriangulationFlags					= DelaunayTriangulation | ConstrainedDelaunayTriangulation | ConformingDelaunayTriangulation,

			// additional triangulation flags
			ResolveIntersections				= 1 << 3,	// will add new points and split edges

			// line mode
			IndexedLineStrip					= 1 << 4,	// vertices + indices
			IndexedLineList						= 1 << 5,	// vertices + indices
			LineStrip							= 1 << 6,	// only vertices
			//LineList							= 1 << 7,	// duplicate vertices are not supported
			_IndicesMode						= IndexedLineStrip | IndexedLineList | LineStrip,

			Unknown								= 0,
			_BITOPS_							= 0
		};

		using Index_t				= uint;
		using InIndices_t			= ArrayView< Index_t >;
		using InArrayOfIndices_t	= ArrayView< InIndices_t >;

		using InVertices_t			= ArrayView< float2 >;
		using InArrayOfVertices_t	= ArrayView< InVertices_t >;

		using OutVertices_t			= Array< float2 >;
		using OutIndices_t			= Array< Index_t >;		// triangle list
		using OutBoundaryEdges_t	= Array< Index_t >;		// line list (edges)


	// functions
	public:
		static bool  Triangulate (InArrayOfVertices_t		vertices,
								  InArrayOfIndices_t		indices,
								  EFlags					flags,
								  OUT OutVertices_t			&outVertices,
								  OUT OutIndices_t			&outIndices,
								  OUT OutBoundaryEdges_t*	optBoundary = null)	__NE___;

		static bool  Triangulate (InVertices_t				vertices,
								  InIndices_t				indices,
								  EFlags					flags,
								  OUT OutVertices_t			&outVertices,
								  OUT OutIndices_t			&outIndices,
								  OUT OutBoundaryEdges_t*	optBoundary = null)	__NE___;

		static bool  Triangulate (InVertices_t				contour,
								  EFlags					flags,
								  OUT OutVertices_t			&outVertices,
								  OUT OutIndices_t			&outIndices,
								  OUT OutBoundaryEdges_t*	optBoundary = null)	__NE___;

	private:
		ND_ static bool  _Triangulate (InArrayOfVertices_t		vertices,
									   InArrayOfIndices_t		indices,
									   EFlags					flags,
									   OUT OutVertices_t		&outVertices,
									   OUT OutIndices_t			&outIndices,
									   OUT OutBoundaryEdges_t*	optBoundary)	__Th___;
	};


} // AE::GeometryTools

#endif // AE_ENABLE_CDT
