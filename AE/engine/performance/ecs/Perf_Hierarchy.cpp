// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Perf_Common.h"

namespace
{
#ifdef AE_DEBUG
	static constexpr usize	c_HierarchySize = 1'000;
#else
	static constexpr usize	c_HierarchySize = 10'000'000;
#endif
	static constexpr uint	c_ToChunkIdx	= 10;
	static constexpr usize	c_ChunkSize		= 1u << c_ToChunkIdx;
	static constexpr uint	c_MaxIterations	= 8;
	static constexpr uint	c_MaxDepth		= 8;
	static constexpr uint	c_Cycles		= 1;


	struct ChunkS
	{
		uint				count		= 0;
		uint				capacity	= 0;
		uint *				parentIds	= null;
		packed_float3 *		positions	= null;
		packed_float4 *		scaleBias	= null;

		void  Alloc (const usize newCount)
		{
			count		= 0;
			capacity	= uint(newCount);

			usize	align	= AE_CACHE_LINE;
			Bytes	size	= CalcSize( capacity );
			void*	ptr		= UntypedAllocator::Allocate( size );
			Bytes	offset	= 0_b;

			parentIds	= Cast<uint>( ptr + offset );
			offset		= AlignUp( offset + SizeOf<uint> * capacity, align );
			positions	= Cast<packed_float3>( ptr + offset );
			offset		= AlignUp( offset + SizeOf<packed_float3> * capacity, align );
			scaleBias	= Cast<packed_float4>( ptr + offset );
			offset		= offset + SizeOf<packed_float4> * capacity;
			
			ZeroMem( parentIds, SizeOf<uint> * capacity );

			CHECK( offset == size );
		}

		void  Dealloc ()
		{
			UntypedAllocator::Deallocate( parentIds );
		}

		bool  IsEmpty () const	{ return count == 0; }
		bool  IsFull () const	{ return count >= capacity; }

		static Bytes  CalcSize (usize c)
		{
			usize	align	= AE_CACHE_LINE;
			Bytes	size	= 0_b;
			size += SizeOf<uint> * c;
			size  = AlignUp( size, align );
			size += SizeOf<packed_float3> * c;
			size  = AlignUp( size, align );
			size += SizeOf<packed_float4> * c;
			return size;
		}
	};

	
	struct ChunkV
	{
		uint			count		= 0;
		uint			capacity	= 0;
		uint *			parentIds	= null;
		float3 *		positions	= null;
		float4 *		scaleBias	= null;

		void  Alloc (const usize newCount)
		{
			count		= 0;
			capacity	= uint(newCount);

			usize	align	= AE_CACHE_LINE;
			Bytes	size	= CalcSize( capacity );
			void*	ptr		= UntypedAllocator::Allocate( size );
			Bytes	offset	= 0_b;

			parentIds	= Cast<uint>( ptr + offset );
			offset		= AlignUp( offset + SizeOf<uint> * capacity, align );
			positions	= Cast<float3>( ptr + offset );
			offset		= AlignUp( offset + SizeOf<float3> * capacity, align );
			scaleBias	= Cast<float4>( ptr + offset );
			offset		= offset + SizeOf<float4> * capacity;
			
			ZeroMem( parentIds, SizeOf<uint> * capacity );

			CHECK( offset == size );
		}

		void  Dealloc ()
		{
			UntypedAllocator::Deallocate( parentIds );
		}

		bool  IsEmpty () const	{ return count == 0; }
		bool  IsFull () const	{ return count >= capacity; }

		static Bytes  CalcSize (usize c)
		{
			usize	align	= AE_CACHE_LINE;
			Bytes	size	= 0_b;
			size += SizeOf<uint> * c;
			size  = AlignUp( size, align );
			size += SizeOf<float3> * c;
			size  = AlignUp( size, align );
			size += SizeOf<float4> * c;
			return size;
		}
	};

	
	struct ChunkAVX
	{
		uint		count		= 0;
		uint		capacity	= 0;

		uint *		parentIds	= null;

		float *		positionX	= null;
		float *		positionY	= null;
		float *		positionZ	= null;

		float *		scaleBiasX	= null;
		float *		scaleBiasY	= null;
		float *		scaleBiasZ	= null;
		float *		scaleBiasW	= null;

		void  Alloc (const usize newCount)
		{
			count		= 0;
			capacity	= uint(newCount);
			
			usize	align	= Max( AE_CACHE_LINE, sizeof(SimdFloat8) );
			Bytes	size	= CalcSize( capacity );
			void*	ptr		= UntypedAllocator::Allocate( SizeAndAlign{ size, align });
			Bytes	offset	= 0_b;

			parentIds	= Cast<uint>( ptr + offset );

			offset		= AlignUp( offset + SizeOf<uint> * capacity, align );
			positionX	= Cast<float>( ptr + offset );
			CheckPointerCast<SimdFloat8>( positionX );

			offset		= AlignUp( offset + SizeOf<float> * capacity, align );
			positionY	= Cast<float>( ptr + offset );
			CheckPointerCast<SimdFloat8>( positionY );

			offset		= AlignUp( offset + SizeOf<float> * capacity, align );
			positionZ	= Cast<float>( ptr + offset );
			CheckPointerCast<SimdFloat8>( positionZ );

			offset		= AlignUp( offset + SizeOf<float> * capacity, align );
			scaleBiasX	= Cast<float>( ptr + offset );
			CheckPointerCast<SimdFloat8>( scaleBiasX );

			offset		= AlignUp( offset + SizeOf<float> * capacity, align );
			scaleBiasY	= Cast<float>( ptr + offset );
			CheckPointerCast<SimdFloat8>( scaleBiasY );

			offset		= AlignUp( offset + SizeOf<float> * capacity, align );
			scaleBiasZ	= Cast<float>( ptr + offset );
			CheckPointerCast<SimdFloat8>( scaleBiasZ );

			offset		= AlignUp( offset + SizeOf<float> * capacity, align );
			scaleBiasW	= Cast<float>( ptr + offset );
			CheckPointerCast<SimdFloat8>( scaleBiasW );

			offset		= AlignUp( offset + SizeOf<float> * capacity, align );

			ZeroMem( parentIds, SizeOf<uint> * capacity );

			CHECK( offset == size );
		}

		void  Dealloc ()
		{
			UntypedAllocator::Deallocate( parentIds );
		}

		bool  IsEmpty () const	{ return count == 0; }
		bool  IsFull () const	{ return count >= capacity; }
		
		static Bytes  CalcSize (usize c)
		{
			usize	align	= Max( AE_CACHE_LINE, sizeof(SimdFloat8) );
			Bytes	size	= 0_b;
			size = AlignUp( size + SizeOf<uint> * c, align );
			
			size = AlignUp( size + SizeOf<float> * c, align );
			size = AlignUp( size + SizeOf<float> * c, align );
			size = AlignUp( size + SizeOf<float> * c, align );
			
			size = AlignUp( size + SizeOf<float> * c, align );
			size = AlignUp( size + SizeOf<float> * c, align );
			size = AlignUp( size + SizeOf<float> * c, align );
			size = AlignUp( size + SizeOf<float> * c, align );

			return size;
		}
	};


	template <typename T>
	  requires( IsSame< T, ChunkS > or IsSame< T, ChunkV >)
	static void  ResetCache (Array<T> &chunks)
	{
		for (auto& ch : chunks)
		{
			for (uint i = 0; i < c_ChunkSize; ++i)
			{
				ch.positions[i]		= float3{float(i)};
				ch.scaleBias[i]		= float4{float( usize(&ch) & ((1u << 24) - 1) )};
			}
		}
	}
	
	static void  ResetCache (Array<ChunkAVX> &chunks)
	{
		for (auto& ch : chunks)
		{
			for (uint i = 0; i < c_ChunkSize; ++i)
			{
				ch.positionX[i]		= float(i);
				ch.positionY[i]		= float(i);
				ch.positionZ[i]		= float(i);

				ch.scaleBiasX[i]	= float( usize(&ch) & ((1u << 24) - 1) );
				ch.scaleBiasY[i]	= float( usize(&ch) & ((1u << 24) - 1) );
				ch.scaleBiasZ[i]	= float( usize(&ch) & ((1u << 24) - 1) );
				ch.scaleBiasW[i]	= float( usize(&ch) & ((1u << 24) - 1) );
			}
		}
	}

	template <typename T>
	static Bytes  ChunksSize (const Array<T> &chunks)
	{
		Bytes	size;
		for (auto& ch : chunks)
		{
			size += ch.CalcSize( ch.count );
		}
		return size;
	}
	//-----------------------------------------------


	static void  Hierarchy_Test1 (IntervalProfiler &profiler, ArrayView<uint> hierarchyDepths, const uint totalCount)
	{
		Array<ChunkS>	chunks;
		chunks.emplace_back();

		uint	global_idx = 0;

		// root
		{
			ChunkS&	ch = chunks.back();

			if ( ch.IsEmpty() )
				ch.Alloc( c_ChunkSize );

			ch.parentIds[0] = UMax;
			ch.positions[0] = packed_float3{};
			ch.scaleBias[0] = packed_float4{};

			ch.count++;
			global_idx++;
		}

		for (uint depth : hierarchyDepths)
		{
			uint	parent_id = 0;

			for (uint d = 0; d < depth; ++d)
			{
				if ( chunks.back().IsFull() )
					chunks.emplace_back();

				ChunkS&	ch = chunks.back();

				if ( ch.IsEmpty() )
					ch.Alloc( c_ChunkSize );

				const uint	i = ch.count++;

				ch.parentIds[i] = parent_id;
				ch.positions[i] = packed_float3{};
				ch.scaleBias[i] = packed_float4{};

				parent_id = global_idx;
				++global_idx;
			}
		}
		CHECK( global_idx == totalCount+1 );

		AE_LOGI( "Size: "s << ToString( ChunksSize( chunks )));


		profiler.BeginTest( "scalar, best locality",
							[s = ChunksSize( chunks )](secondsd dt)
							{
								double	bandwidth = double(usize(s) * c_Cycles) / dt.count();
								return ToStringSfx( bandwidth ) << "B/s";
							});

		for (uint i = 0; i < c_MaxIterations; ++i)
		{
			profiler.BeginIteration();

			for (uint cy = 0; cy < c_Cycles; ++cy)
			{
				// for each entity except root
				for (usize c = 0; c < chunks.size(); ++c)
				{
					ChunkS&	ch = chunks[c];

					for (uint k = (c == 0 ? 1 : 0); k < c_ChunkSize; ++k)
					{
						uint	parent_idx	= ch.parentIds[k];
						uint	cur_idx		= uint(c * c_ChunkSize + k);
						ASSERT( parent_idx < cur_idx );
						Unused( cur_idx );

						uint	chunk_id		= parent_idx >> c_ToChunkIdx;
						uint	idx_in_chunk	= parent_idx & (c_ChunkSize-1);

						auto	parent_pos		= chunks[ chunk_id ].positions[ idx_in_chunk ];

						ch.positions[k] = packed_float3{ch.scaleBias[k]} + (parent_pos * ch.scaleBias[k].w);
					}
				}
			}
			profiler.EndIteration();

			ResetCache( chunks );
		}
		profiler.EndTest();
	}
	//-----------------------------------------------

	
	static void  Hierarchy_Test2 (IntervalProfiler &profiler, ArrayView<uint> hierarchyDepths, const uint totalCount)
	{
		Array<ChunkS>	chunks;
		chunks.emplace_back();
		
		uint	global_idx = 0;

		// root
		{
			ChunkS&	ch = chunks.back();

			if ( ch.IsEmpty() )
				ch.Alloc( c_ChunkSize );

			ch.parentIds[0] = UMax;
			ch.positions[0] = packed_float3{};
			ch.scaleBias[0] = packed_float4{};

			ch.count++;
			global_idx++;
		}

		Array<uint>		parent_ids;
		parent_ids.resize( hierarchyDepths.size(), 0u );

		for (uint depth = 0; depth < c_MaxDepth; ++depth)
		{
			for (usize k = 0; k < hierarchyDepths.size(); ++k)
			{
				const uint	levels = hierarchyDepths[k];
				if ( depth >= levels )
					continue;
				
				if ( chunks.back().IsFull() )
					chunks.emplace_back();

				ChunkS&	ch = chunks.back();

				if ( ch.IsEmpty() )
					ch.Alloc( c_ChunkSize );

				const uint	i = ch.count++;
				uint	parent_idx = parent_ids[k];

				ch.parentIds[i] = parent_idx;
				ch.positions[i] = packed_float3{};
				ch.scaleBias[i] = packed_float4{};
				
				uint	cur_idx	= uint((chunks.size()-1) * c_ChunkSize + i);
				ASSERT( cur_idx == global_idx );

				parent_ids[k] = cur_idx;
				++global_idx;
			}
		}
		CHECK( global_idx == totalCount+1 );


		profiler.BeginTest( "scalar, layered",
							[s = ChunksSize( chunks )](secondsd dt)
							{
								double	bandwidth = double(usize(s) * c_Cycles) / dt.count();
								return ToStringSfx( bandwidth ) << "B/s";
							});
		
		for (uint i = 0; i < c_MaxIterations; ++i)
		{
			profiler.BeginIteration();
			
			for (uint cy = 0; cy < c_Cycles; ++cy)
			{
				// for each entity except root
				for (usize c = 0; c < chunks.size(); ++c)
				{
					ChunkS&	ch = chunks[c];

					for (uint k = (c == 0 ? 1 : 0); k < c_ChunkSize; ++k)
					{
						uint	parent_idx	= ch.parentIds[k];
						uint	cur_idx		= uint(c * c_ChunkSize + k);
						ASSERT( parent_idx < cur_idx );
						Unused( cur_idx );

						uint	chunk_id		= parent_idx >> c_ToChunkIdx;
						uint	idx_in_chunk	= parent_idx & (c_ChunkSize-1);

						auto	parent_pos		= chunks[ chunk_id ].positions[ idx_in_chunk ];

						ch.positions[k] = packed_float3{ch.scaleBias[k]} + (parent_pos * ch.scaleBias[k].w);
					}
				}
			}
			profiler.EndIteration();
			
			ResetCache( chunks );
		}
		profiler.EndTest();
	}
	//-----------------------------------------------


	static void  Hierarchy_Test3 (IntervalProfiler &profiler, ArrayView<uint> hierarchyDepths, const uint totalCount)
	{
		Array<ChunkV>	chunks;
		chunks.emplace_back();

		uint	global_idx = 0;

		// root
		{
			ChunkV&	ch = chunks.back();

			if ( ch.IsEmpty() )
				ch.Alloc( c_ChunkSize );

			ch.parentIds[0] = UMax;
			ch.positions[0] = float3{};
			ch.scaleBias[0] = float4{};

			ch.count++;
			global_idx++;
		}

		for (uint depth : hierarchyDepths)
		{
			uint	parent_id = 0;

			for (uint d = 0; d < depth; ++d)
			{
				if ( chunks.back().IsFull() )
					chunks.emplace_back();

				ChunkV&	ch = chunks.back();

				if ( ch.IsEmpty() )
					ch.Alloc( c_ChunkSize );

				const uint	i = ch.count++;

				ch.parentIds[i] = parent_id;
				ch.positions[i] = float3{};
				ch.scaleBias[i] = float4{};

				parent_id = global_idx;
				++global_idx;
			}
		}
		CHECK( global_idx == totalCount+1 );

		AE_LOGI( "Size: "s << ToString( ChunksSize( chunks )));


		profiler.BeginTest( "vec3, best locality",
							[s = ChunksSize( chunks )](secondsd dt)
							{
								double	bandwidth = double(usize(s) * c_Cycles) / dt.count();
								return ToStringSfx( bandwidth ) << "B/s";
							});

		for (uint i = 0; i < c_MaxIterations; ++i)
		{
			profiler.BeginIteration();

			for (uint cy = 0; cy < c_Cycles; ++cy)
			{
				// for each entity except root
				for (usize c = 0; c < chunks.size(); ++c)
				{
					ChunkV&	ch = chunks[c];

					for (uint k = (c == 0 ? 1 : 0); k < c_ChunkSize; ++k)
					{
						uint	parent_idx	= ch.parentIds[k];
						uint	cur_idx		= uint(c * c_ChunkSize + k);
						ASSERT( parent_idx < cur_idx );
						Unused( cur_idx );

						uint	chunk_id		= parent_idx >> c_ToChunkIdx;
						uint	idx_in_chunk	= parent_idx & (c_ChunkSize-1);

						float3	parent_pos		= chunks[ chunk_id ].positions[ idx_in_chunk ];

						ch.positions[k] = float3{ch.scaleBias[k]} + (parent_pos * ch.scaleBias[k].w);
					}
				}
			}
			profiler.EndIteration();

			ResetCache( chunks );
		}
		profiler.EndTest();
	}
	//-----------------------------------------------

	
	static void  Hierarchy_Test4 (IntervalProfiler &profiler, ArrayView<uint> hierarchyDepths, const uint totalCount)
	{
		Array<ChunkV>	chunks;
		chunks.emplace_back();
		
		uint	global_idx = 0;

		// root
		{
			ChunkV&	ch = chunks.back();

			if ( ch.IsEmpty() )
				ch.Alloc( c_ChunkSize );

			ch.parentIds[0] = UMax;
			ch.positions[0] = float3{};
			ch.scaleBias[0] = float4{};

			ch.count++;
			global_idx++;
		}

		Array<uint>		parent_ids;
		parent_ids.resize( hierarchyDepths.size(), 0u );

		for (uint depth = 0; depth < c_MaxDepth; ++depth)
		{
			for (usize k = 0; k < hierarchyDepths.size(); ++k)
			{
				const uint	levels = hierarchyDepths[k];
				if ( depth >= levels )
					continue;
				
				if ( chunks.back().IsFull() )
					chunks.emplace_back();

				ChunkV&	ch = chunks.back();

				if ( ch.IsEmpty() )
					ch.Alloc( c_ChunkSize );

				const uint	i = ch.count++;
				uint	parent_idx = parent_ids[k];

				ch.parentIds[i] = parent_idx;
				ch.positions[i] = float3{};
				ch.scaleBias[i] = float4{};
				
				uint	cur_idx	= uint((chunks.size()-1) * c_ChunkSize + i);
				ASSERT( cur_idx == global_idx );

				parent_ids[k] = cur_idx;
				++global_idx;
			}
		}
		CHECK( global_idx == totalCount+1 );


		profiler.BeginTest( "vec3, layered",
							[s = ChunksSize( chunks )](secondsd dt)
							{
								double	bandwidth = double(usize(s) * c_Cycles) / dt.count();
								return ToStringSfx( bandwidth ) << "B/s";
							});
		
		for (uint i = 0; i < c_MaxIterations; ++i)
		{
			profiler.BeginIteration();
			
			for (uint cy = 0; cy < c_Cycles; ++cy)
			{
				// for each entity except root
				for (usize c = 0; c < chunks.size(); ++c)
				{
					ChunkV&	ch = chunks[c];

					for (uint k = (c == 0 ? 1 : 0); k < c_ChunkSize; ++k)
					{
						uint	parent_idx	= ch.parentIds[k];
						uint	cur_idx		= uint(c * c_ChunkSize + k);
						ASSERT( parent_idx < cur_idx );
						Unused( cur_idx );

						uint	chunk_id		= parent_idx >> c_ToChunkIdx;
						uint	idx_in_chunk	= parent_idx & (c_ChunkSize-1);

						float3	parent_pos		= chunks[ chunk_id ].positions[ idx_in_chunk ];

						ch.positions[k] = float3{ch.scaleBias[k]} + (parent_pos * ch.scaleBias[k].w);
					}
				}
			}
			profiler.EndIteration();
			
			ResetCache( chunks );
		}
		profiler.EndTest();
	}
	//-----------------------------------------------

	
	static void  Hierarchy_Test5 (IntervalProfiler &profiler, ArrayView<uint> hierarchyDepths, const uint totalCount)
	{
		Array<ChunkAVX>	chunks;
		chunks.emplace_back();

		uint	global_idx = 0;

		// root
		{
			ChunkAVX&	ch = chunks.back();

			if ( ch.IsEmpty() )
				ch.Alloc( c_ChunkSize );

			ch.parentIds[0]		= UMax;
			ch.positionX[0]		= 0.f;
			ch.positionY[0]		= 0.f;
			ch.positionZ[0]		= 0.f;
			ch.scaleBiasX[0]	= 0.f;
			ch.scaleBiasY[0]	= 0.f;
			ch.scaleBiasZ[0]	= 0.f;
			ch.scaleBiasW[0]	= 0.f;

			ch.count += 8;
			global_idx += 8;
		}

		for (uint depth : hierarchyDepths)
		{
			uint	parent_id = 0;

			for (uint d = 0; d < depth; ++d)
			{
				if ( chunks.back().IsFull() )
					chunks.emplace_back();

				ChunkAVX&	ch = chunks.back();

				if ( ch.IsEmpty() )
					ch.Alloc( c_ChunkSize );

				const uint	i = ch.count++;

				ch.parentIds[i]		= parent_id;
				ch.positionX[i]		= 0.f;
				ch.positionY[i]		= 0.f;
				ch.positionZ[i]		= 0.f;
				ch.scaleBiasX[i]	= 0.f;
				ch.scaleBiasY[i]	= 0.f;
				ch.scaleBiasZ[i]	= 0.f;
				ch.scaleBiasW[i]	= 0.f;

				parent_id = global_idx;
				++global_idx;
			}
		}
		CHECK( global_idx == totalCount+8 );
		
		AE_LOGI( "AVX Size: "s << ToString( ChunksSize( chunks )));


		profiler.BeginTest( "AVX, best locality",
							[s = ChunksSize( chunks )](secondsd dt)
							{
								double	bandwidth = double(usize(s) * c_Cycles) / dt.count();
								return ToStringSfx( bandwidth ) << "B/s";
							});

		for (uint i = 0; i < c_MaxIterations; ++i)
		{
			profiler.BeginIteration();
			
			for (uint cy = 0; cy < c_Cycles; ++cy)
			{
				// for each entity except root
				for (usize c = 0; c < chunks.size(); ++c)
				{
					ChunkAVX&	ch = chunks[c];

					for (uint k = (c == 0 ? 8 : 0); k < c_ChunkSize; k += 8)
					{
						alignas(SimdFloat8) float	parent_pos_x [8];
						alignas(SimdFloat8) float	parent_pos_y [8];
						alignas(SimdFloat8) float	parent_pos_z [8];
						const uint					cur_idx		= uint(c * c_ChunkSize + k);

						for (uint j = 0; j < 8; ++j)
						{
							uint	parent_idx	= ch.parentIds[k + j];
						
							ASSERT( parent_idx < cur_idx );
							Unused( cur_idx );

							uint	chunk_id		= parent_idx >> c_ToChunkIdx;
							uint	idx_in_chunk	= parent_idx & (c_ChunkSize-1);

							parent_pos_x[j] = chunks[ chunk_id ].positionX[ idx_in_chunk ];
							parent_pos_y[j] = chunks[ chunk_id ].positionY[ idx_in_chunk ];
							parent_pos_z[j] = chunks[ chunk_id ].positionZ[ idx_in_chunk ];
						}

						*Cast<SimdFloat8>(ch.positionX + k) = *Cast<SimdFloat8>(ch.scaleBiasX + k) + (*Cast<SimdFloat8>(parent_pos_x) * *Cast<SimdFloat8>(ch.scaleBiasW + k));
						*Cast<SimdFloat8>(ch.positionY + k) = *Cast<SimdFloat8>(ch.scaleBiasY + k) + (*Cast<SimdFloat8>(parent_pos_y) * *Cast<SimdFloat8>(ch.scaleBiasW + k));
						*Cast<SimdFloat8>(ch.positionZ + k) = *Cast<SimdFloat8>(ch.scaleBiasZ + k) + (*Cast<SimdFloat8>(parent_pos_z) * *Cast<SimdFloat8>(ch.scaleBiasW + k));
					}
				}
			}
			profiler.EndIteration();

			ResetCache( chunks );
		}
		profiler.EndTest();
	}
	//-----------------------------------------------

	
	static void  Hierarchy_Test6 (IntervalProfiler &profiler, ArrayView<uint> hierarchyDepths, const uint totalCount)
	{
		Array<ChunkAVX>	chunks;
		chunks.emplace_back();

		uint	global_idx = 0;

		// root
		{
			ChunkAVX&	ch = chunks.back();

			if ( ch.IsEmpty() )
				ch.Alloc( c_ChunkSize );

			ch.parentIds[0]		= UMax;
			ch.positionX[0]		= 0.f;
			ch.positionY[0]		= 0.f;
			ch.positionZ[0]		= 0.f;
			ch.scaleBiasX[0]	= 0.f;
			ch.scaleBiasY[0]	= 0.f;
			ch.scaleBiasZ[0]	= 0.f;
			ch.scaleBiasW[0]	= 0.f;
			
			ch.count += 8;
		}
		
		Array<uint>		parent_ids;
		parent_ids.resize( hierarchyDepths.size(), 0u );

		for (uint depth = 0; depth < c_MaxDepth; ++depth)
		{
			for (usize k = 0; k < hierarchyDepths.size(); ++k)
			{
				const uint	levels = hierarchyDepths[k];
				if ( depth >= levels )
					continue;
				
				if ( chunks.back().IsFull() )
					chunks.emplace_back();

				ChunkAVX&	ch = chunks.back();

				if ( ch.IsEmpty() )
					ch.Alloc( c_ChunkSize );

				const uint	i = ch.count++;

				ch.parentIds[i] = parent_ids[k];
				ch.positionX[i]		= 0.f;
				ch.positionY[i]		= 0.f;
				ch.positionZ[i]		= 0.f;
				ch.scaleBiasX[i]	= 0.f;
				ch.scaleBiasY[i]	= 0.f;
				ch.scaleBiasZ[i]	= 0.f;
				ch.scaleBiasW[i]	= 0.f;
				
				uint	cur_idx	= uint((chunks.size()-1) * c_ChunkSize + i);
				ASSERT( cur_idx == global_idx+8 );

				parent_ids[k] = cur_idx;
				++global_idx;
			}
		}
		CHECK( global_idx == totalCount );


		profiler.BeginTest( "AVX, layered",
							[s = ChunksSize( chunks )](secondsd dt)
							{
								double	bandwidth = double(usize(s) * c_Cycles) / dt.count();
								return ToStringSfx( bandwidth ) << "B/s";
							});

		for (uint i = 0; i < c_MaxIterations; ++i)
		{
			profiler.BeginIteration();
			
			for (uint cy = 0; cy < c_Cycles; ++cy)
			{
				// for each entity except root
				for (usize c = 0; c < chunks.size(); ++c)
				{
					ChunkAVX&	ch = chunks[c];

					for (uint k = (c == 0 ? 8 : 0); k < c_ChunkSize; k += 8)
					{
						alignas(SimdFloat8) float	parent_pos_x [8];
						alignas(SimdFloat8) float	parent_pos_y [8];
						alignas(SimdFloat8) float	parent_pos_z [8];
						const uint					cur_idx		= uint(c * c_ChunkSize + k);

						for (uint j = 0; j < 8; ++j)
						{
							uint	parent_idx	= ch.parentIds[k + j];
						
							ASSERT( parent_idx < cur_idx );
							Unused( cur_idx );

							uint	chunk_id		= parent_idx >> c_ToChunkIdx;
							uint	idx_in_chunk	= parent_idx & (c_ChunkSize-1);

							parent_pos_x[j] = chunks[ chunk_id ].positionX[ idx_in_chunk ];
							parent_pos_y[j] = chunks[ chunk_id ].positionY[ idx_in_chunk ];
							parent_pos_z[j] = chunks[ chunk_id ].positionZ[ idx_in_chunk ];
						}

						*Cast<SimdFloat8>(ch.positionX + k) = *Cast<SimdFloat8>(ch.scaleBiasX + k) + (*Cast<SimdFloat8>(parent_pos_x) * *Cast<SimdFloat8>(ch.scaleBiasW + k));
						*Cast<SimdFloat8>(ch.positionY + k) = *Cast<SimdFloat8>(ch.scaleBiasY + k) + (*Cast<SimdFloat8>(parent_pos_y) * *Cast<SimdFloat8>(ch.scaleBiasW + k));
						*Cast<SimdFloat8>(ch.positionZ + k) = *Cast<SimdFloat8>(ch.scaleBiasZ + k) + (*Cast<SimdFloat8>(parent_pos_z) * *Cast<SimdFloat8>(ch.scaleBiasW + k));
					}
				}
			}
			profiler.EndIteration();

			ResetCache( chunks );
		}
		profiler.EndTest();
	}
	//-----------------------------------------------


	static void  Hierarchy_Test7 (IntervalProfiler &profiler, ArrayView<uint> hierarchyDepths, const uint totalCount)
	{
		Array<ChunkAVX>	chunks;
		chunks.emplace_back();

		uint	global_idx = 0;

		// root
		{
			ChunkAVX&	ch = chunks.back();

			if ( ch.IsEmpty() )
				ch.Alloc( c_ChunkSize );

			ch.parentIds[0]		= UMax;
			ch.positionX[0]		= 0.f;
			ch.positionY[0]		= 0.f;
			ch.positionZ[0]		= 0.f;
			ch.scaleBiasX[0]	= 0.f;
			ch.scaleBiasY[0]	= 0.f;
			ch.scaleBiasZ[0]	= 0.f;
			ch.scaleBiasW[0]	= 0.f;
			
			ch.count += 16;
		}
		
		Array<uint>		parent_ids;
		parent_ids.resize( hierarchyDepths.size(), 0u );

		for (uint depth = 0; depth < c_MaxDepth; ++depth)
		{
			for (usize k = 0; k < hierarchyDepths.size(); ++k)
			{
				const uint	levels = hierarchyDepths[k];
				if ( depth >= levels )
					continue;
				
				if ( chunks.back().IsFull() )
					chunks.emplace_back();

				ChunkAVX&	ch = chunks.back();

				if ( ch.IsEmpty() )
					ch.Alloc( c_ChunkSize );

				const uint	i = ch.count++;

				ch.parentIds[i] = parent_ids[k];
				ch.positionX[i]		= 0.f;
				ch.positionY[i]		= 0.f;
				ch.positionZ[i]		= 0.f;
				ch.scaleBiasX[i]	= 0.f;
				ch.scaleBiasY[i]	= 0.f;
				ch.scaleBiasZ[i]	= 0.f;
				ch.scaleBiasW[i]	= 0.f;
				
				uint	cur_idx	= uint((chunks.size()-1) * c_ChunkSize + i);
				ASSERT( cur_idx == global_idx+16 );

				parent_ids[k] = cur_idx;
				++global_idx;
			}
		}
		CHECK( global_idx == totalCount );


		profiler.BeginTest( "AVX-2, layered",
							[s = ChunksSize( chunks )](secondsd dt)
							{
								double	bandwidth = double(usize(s) * c_Cycles) / dt.count();
								return ToStringSfx( bandwidth ) << "B/s";
							});

		for (uint i = 0; i < c_MaxIterations; ++i)
		{
			profiler.BeginIteration();
			
			for (uint cy = 0; cy < c_Cycles; ++cy)
			{
				// for each entity except root
				for (usize c = 0; c < chunks.size(); ++c)
				{
					ChunkAVX&	ch = chunks[c];

					for (uint k = (c == 0 ? 16 : 0); k < c_ChunkSize; k += 16)
					{
						alignas(SimdFloat8) float	parent_pos_x [16];
						alignas(SimdFloat8) float	parent_pos_y [16];
						alignas(SimdFloat8) float	parent_pos_z [16];
						const uint					cur_idx		= uint(c * c_ChunkSize + k);

						for (uint j = 0; j < 16; ++j)
						{
							uint	parent_idx	= ch.parentIds[k + j];
						
							ASSERT( parent_idx < cur_idx );
							Unused( cur_idx );

							uint	chunk_id		= parent_idx >> c_ToChunkIdx;
							uint	idx_in_chunk	= parent_idx & (c_ChunkSize-1);

							parent_pos_x[j] = chunks[ chunk_id ].positionX[ idx_in_chunk ];
							parent_pos_y[j] = chunks[ chunk_id ].positionY[ idx_in_chunk ];
							parent_pos_z[j] = chunks[ chunk_id ].positionZ[ idx_in_chunk ];
						}

						SimdFloat8*	pos_x	= Cast<SimdFloat8>(ch.positionX + k);
						SimdFloat8*	pos_y	= Cast<SimdFloat8>(ch.positionY + k);
						SimdFloat8*	pos_z	= Cast<SimdFloat8>(ch.positionZ + k); 

						SimdFloat8	bias_x    {ch.scaleBiasX + k};
						SimdFloat8	bias_y    {ch.scaleBiasY + k};
						SimdFloat8	bias_z    {ch.scaleBiasZ + k};
						SimdFloat8	scale     {ch.scaleBiasW + k};

						SimdFloat8	parent_x  {parent_pos_x};
						SimdFloat8	parent_y  {parent_pos_y};
						SimdFloat8	parent_z  {parent_pos_z};
						
						SimdFloat8	bias_x2   {ch.scaleBiasX + k + 8};
						SimdFloat8	bias_y2   {ch.scaleBiasY + k + 8};
						SimdFloat8	bias_z2   {ch.scaleBiasZ + k + 8};
						SimdFloat8	scale2    {ch.scaleBiasW + k + 8};

						SimdFloat8	parent_x2 {parent_pos_x + 8};
						SimdFloat8	parent_y2 {parent_pos_y + 8};
						SimdFloat8	parent_z2 {parent_pos_z + 8};

						pos_x[0] = bias_x + (parent_x * scale);
						pos_y[0] = bias_y + (parent_y * scale);
						pos_z[0] = bias_z + (parent_z * scale);
						
						pos_x[1] = bias_x2 + (parent_x2 * scale2);
						pos_y[1] = bias_y2 + (parent_y2 * scale2);
						pos_z[1] = bias_z2 + (parent_z2 * scale2);
					}
				}
			}
			profiler.EndIteration();

			ResetCache( chunks );
		}
		profiler.EndTest();
	}
	//-----------------------------------------------


	static void  Hierarchy_Test8 (IntervalProfiler &profiler, ArrayView<uint> hierarchyDepths, const uint totalCount)
	{
		Array<ChunkAVX>	chunks;
		chunks.emplace_back();

		uint	global_idx = 0;

		// root
		{
			ChunkAVX&	ch = chunks.back();

			if ( ch.IsEmpty() )
				ch.Alloc( c_ChunkSize );

			ch.parentIds[0]		= UMax;
			ch.positionX[0]		= 0.f;
			ch.positionY[0]		= 0.f;
			ch.positionZ[0]		= 0.f;
			ch.scaleBiasX[0]	= 0.f;
			ch.scaleBiasY[0]	= 0.f;
			ch.scaleBiasZ[0]	= 0.f;
			ch.scaleBiasW[0]	= 0.f;
			
			ch.count += 32;
		}
		
		Array<uint>		parent_ids;
		parent_ids.resize( hierarchyDepths.size(), 0u );

		for (uint depth = 0; depth < c_MaxDepth; ++depth)
		{
			for (usize k = 0; k < hierarchyDepths.size(); ++k)
			{
				const uint	levels = hierarchyDepths[k];
				if ( depth >= levels )
					continue;
				
				if ( chunks.back().IsFull() )
					chunks.emplace_back();

				ChunkAVX&	ch = chunks.back();

				if ( ch.IsEmpty() )
					ch.Alloc( c_ChunkSize );

				const uint	i = ch.count++;

				ch.parentIds[i] = parent_ids[k];
				ch.positionX[i]		= 0.f;
				ch.positionY[i]		= 0.f;
				ch.positionZ[i]		= 0.f;
				ch.scaleBiasX[i]	= 0.f;
				ch.scaleBiasY[i]	= 0.f;
				ch.scaleBiasZ[i]	= 0.f;
				ch.scaleBiasW[i]	= 0.f;
				
				uint	cur_idx	= uint((chunks.size()-1) * c_ChunkSize + i);
				ASSERT( cur_idx == global_idx+32 );

				parent_ids[k] = cur_idx;
				++global_idx;
			}
		}
		CHECK( global_idx == totalCount );


		profiler.BeginTest( "AVX-3, layered",
							[s = ChunksSize( chunks )](secondsd dt)
							{
								double	bandwidth = double(usize(s) * c_Cycles) / dt.count();
								return ToStringSfx( bandwidth ) << "B/s";
							});

		for (uint i = 0; i < c_MaxIterations; ++i)
		{
			profiler.BeginIteration();
			
			for (uint cy = 0; cy < c_Cycles; ++cy)
			{
				// for each entity except root
				for (usize c = 0; c < chunks.size(); ++c)
				{
					ChunkAVX&	ch = chunks[c];

					for (uint k = (c == 0 ? 32 : 0); k < c_ChunkSize; k += 32)
					{
						alignas(SimdFloat8) float	parent_pos_x [32];
						alignas(SimdFloat8) float	parent_pos_y [32];
						alignas(SimdFloat8) float	parent_pos_z [32];
						const uint					cur_idx		= uint(c * c_ChunkSize + k);

						for (uint j = 0; j < 32; ++j)
						{
							uint	parent_idx	= ch.parentIds[k + j];
						
							ASSERT( parent_idx < cur_idx );
							Unused( cur_idx );

							uint	chunk_id		= parent_idx >> c_ToChunkIdx;
							uint	idx_in_chunk	= parent_idx & (c_ChunkSize-1);

							parent_pos_x[j] = chunks[ chunk_id ].positionX[ idx_in_chunk ];
							parent_pos_y[j] = chunks[ chunk_id ].positionY[ idx_in_chunk ];
							parent_pos_z[j] = chunks[ chunk_id ].positionZ[ idx_in_chunk ];
						}

						SimdFloat8*	pos_x	= Cast<SimdFloat8>(ch.positionX + k);
						SimdFloat8*	pos_y	= Cast<SimdFloat8>(ch.positionY + k);
						SimdFloat8*	pos_z	= Cast<SimdFloat8>(ch.positionZ + k); 

						SimdFloat8	bias_x    {ch.scaleBiasX + k};
						SimdFloat8	bias_y    {ch.scaleBiasY + k};
						SimdFloat8	bias_z    {ch.scaleBiasZ + k};
						SimdFloat8	scale     {ch.scaleBiasW + k};

						SimdFloat8	parent_x  {parent_pos_x};
						SimdFloat8	parent_y  {parent_pos_y};
						SimdFloat8	parent_z  {parent_pos_z};
						
						SimdFloat8	bias_x2   {ch.scaleBiasX + k + 8};
						SimdFloat8	bias_y2   {ch.scaleBiasY + k + 8};
						SimdFloat8	bias_z2   {ch.scaleBiasZ + k + 8};
						SimdFloat8	scale2    {ch.scaleBiasW + k + 8};

						SimdFloat8	parent_x2 {parent_pos_x + 8};
						SimdFloat8	parent_y2 {parent_pos_y + 8};
						SimdFloat8	parent_z2 {parent_pos_z + 8};
						
						SimdFloat8	bias_x3   {ch.scaleBiasX + k + 16};
						SimdFloat8	bias_y3   {ch.scaleBiasY + k + 16};
						SimdFloat8	bias_z3   {ch.scaleBiasZ + k + 16};
						SimdFloat8	scale3    {ch.scaleBiasW + k + 16};

						SimdFloat8	parent_x3 {parent_pos_x + 16};
						SimdFloat8	parent_y3 {parent_pos_y + 16};
						SimdFloat8	parent_z3 {parent_pos_z + 16};
						
						SimdFloat8	bias_x4   {ch.scaleBiasX + k + 24};
						SimdFloat8	bias_y4   {ch.scaleBiasY + k + 24};
						SimdFloat8	bias_z4   {ch.scaleBiasZ + k + 24};
						SimdFloat8	scale4    {ch.scaleBiasW + k + 24};

						SimdFloat8	parent_x4 {parent_pos_x + 24};
						SimdFloat8	parent_y4 {parent_pos_y + 24};
						SimdFloat8	parent_z4 {parent_pos_z + 24};

						pos_x[0] = bias_x + (parent_x * scale);
						pos_y[0] = bias_y + (parent_y * scale);
						pos_z[0] = bias_z + (parent_z * scale);
						
						pos_x[1] = bias_x2 + (parent_x2 * scale2);
						pos_y[1] = bias_y2 + (parent_y2 * scale2);
						pos_z[1] = bias_z2 + (parent_z2 * scale2);
						
						pos_x[2] = bias_x3 + (parent_x3 * scale3);
						pos_y[2] = bias_y3 + (parent_y3 * scale3);
						pos_z[2] = bias_z3 + (parent_z3 * scale3);

						pos_x[3] = bias_x4 + (parent_x4 * scale4);
						pos_y[3] = bias_y4 + (parent_y4 * scale4);
						pos_z[3] = bias_z4 + (parent_z4 * scale4);
					}
				}
			}
			profiler.EndIteration();

			ResetCache( chunks );
		}
		profiler.EndTest();
	}
}


extern void Per_Hierarchy ()
{
	IntervalProfiler	profiler{ "ECS Hierarchy", IntervalProfiler::EFlags::SortByPerf | IntervalProfiler::EFlags::ExcludeDelta };

	Random				rnd;
	ulong				total_count = 0;
	Array<uint>			hierarchy_depths;
	hierarchy_depths.resize( c_HierarchySize );

	for (usize i = 0; i < c_HierarchySize; ++i)
	{
		hierarchy_depths[i] = rnd.Uniform( 0u, c_MaxDepth-1 );
	//	hierarchy_depths[i] = c_MaxDepth-2;

		total_count += hierarchy_depths[i];
	}

	AE_LOGI( "Entity count: "s << ToString(total_count) );
	CHECK( total_count < ulong{~0u} );

	Hierarchy_Test1( profiler, hierarchy_depths, uint(total_count) );
	Hierarchy_Test2( profiler, hierarchy_depths, uint(total_count) );
	Hierarchy_Test3( profiler, hierarchy_depths, uint(total_count) );
	Hierarchy_Test4( profiler, hierarchy_depths, uint(total_count) );
//	Hierarchy_Test5( profiler, hierarchy_depths, uint(total_count) );
	Hierarchy_Test6( profiler, hierarchy_depths, uint(total_count) );
	Hierarchy_Test7( profiler, hierarchy_depths, uint(total_count) );
	Hierarchy_Test8( profiler, hierarchy_depths, uint(total_count) );
}
