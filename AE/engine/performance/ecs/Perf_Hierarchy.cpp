// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Perf_Common.h"

namespace
{
#ifdef AE_DEBUG
	static constexpr usize	c_HierarchySize = 1'000;
#else
	static constexpr usize	c_HierarchySize = 250;
#endif
	static constexpr uint	c_ToChunkIdx	= 10;
	static constexpr usize	c_ChunkSize		= 1u << c_ToChunkIdx;
	static constexpr uint	c_MaxIterations	= 32;
	static constexpr uint	c_MaxDepth		= 8;
	static constexpr uint	c_Cycles		= 10;


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
							[s = ChunksSize( chunks ), totalCount] (secondsd dt)
							{
								double	bandwidth	= double(usize(s) * c_Cycles) / dt.count();
								double	flops		= double(totalCount * (3+3) * c_Cycles) / dt.count();

								return	ToStringSfx( bandwidth ) << "B/s | " <<
										ToStringSfx( flops ) << "FLOPS";
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
							[s = ChunksSize( chunks ), totalCount] (secondsd dt)
							{
								double	bandwidth	= double(usize(s) * c_Cycles) / dt.count();
								double	flops		= double(totalCount * (3+3) * c_Cycles) / dt.count();

								return	ToStringSfx( bandwidth ) << "B/s | " <<
										ToStringSfx( flops ) << "FLOPS";
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


		profiler.BeginTest( "SSE2, best locality",
							[s = ChunksSize( chunks ), totalCount] (secondsd dt)
							{
								double	bandwidth	= double(usize(s) * c_Cycles) / dt.count();
								double	flops		= double(totalCount * (3+3) * c_Cycles) / dt.count();

								return	ToStringSfx( bandwidth ) << "B/s | " <<
										ToStringSfx( flops ) << "FLOPS";
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


		profiler.BeginTest( "SSE2, layered",
							[s = ChunksSize( chunks ), totalCount] (secondsd dt)
							{
								double	bandwidth	= double(usize(s) * c_Cycles) / dt.count();
								double	flops		= double(totalCount * (3+3) * c_Cycles) / dt.count();

								return	ToStringSfx( bandwidth ) << "B/s | " <<
										ToStringSfx( flops ) << "FLOPS";
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
							[s = ChunksSize( chunks ), totalCount] (secondsd dt)
							{
								double	bandwidth	= double(usize(s) * c_Cycles) / dt.count();
								double	flops		= double(totalCount * (3+3) * c_Cycles) / dt.count();

								return	ToStringSfx( bandwidth ) << "B/s | " <<
										ToStringSfx( flops ) << "FLOPS";
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

				ch.parentIds[i]		= parent_ids[k];
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
							[s = ChunksSize( chunks ), totalCount] (secondsd dt)
							{
								double	bandwidth	= double(usize(s) * c_Cycles) / dt.count();
								double	flops		= double(totalCount * (3+3) * c_Cycles) / dt.count();

								return	ToStringSfx( bandwidth ) << "B/s | " <<
										ToStringSfx( flops ) << "FLOPS";
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
			
			for (uint i = 0; i < 8; ++i)
			{
				ch.parentIds[i]		= UMax;
				ch.positionX[i]		= 0.f;
				ch.positionY[i]		= 0.f;
				ch.positionZ[i]		= 0.f;
				ch.scaleBiasX[i]	= 0.f;
				ch.scaleBiasY[i]	= 0.f;
				ch.scaleBiasZ[i]	= 0.f;
				ch.scaleBiasW[i]	= 0.f;
			}
			ch.count += 8;
		}

		// layout:
		//	[0000][1111][2222] [0000][1111] ...

		HashMap< uint, uint >	same_depth;
		
		for (uint depth : hierarchyDepths)
		{
			same_depth.emplace( depth, 0u ).first->second ++;
		}

		for (auto [levels, count] : same_depth)
		{
			global_idx += count * levels;
			count = AlignUp( count, 8 );

			for (uint k = 0; k < count; k += 8)
			{
				if ( chunks.back().count + 8*levels > chunks.back().capacity )
					chunks.emplace_back();
				
				ChunkAVX&	ch = chunks.back();

				if ( ch.IsEmpty() )
					ch.Alloc( c_ChunkSize );

				uint	parent_id	= 0;
				uint	i			= ch.count;

				ASSERT( i%8 == 0 );
				ch.count += 8*levels;

				for (uint depth = 0; depth < levels; ++depth)
				{
					for (; i < ch.count; ++i)
					{
						uint	id = parent_id + i%8;

						ch.parentIds[i]		= id;
						ch.positionX[i]		= 0.f;
						ch.positionY[i]		= 0.f;
						ch.positionZ[i]		= 0.f;
						ch.scaleBiasX[i]	= 0.f;
						ch.scaleBiasY[i]	= 0.f;
						ch.scaleBiasZ[i]	= 0.f;
						ch.scaleBiasW[i]	= 0.f;
					
						if ( i%8 == 7 )
						{
							uint	cur_idx	= uint( (chunks.size()-1) * c_ChunkSize + (i & ~7) );
							//ASSERT( cur_idx == global_idx+8 );
							parent_id = cur_idx;
						}
					}
				}
			}
		}
		CHECK( global_idx == totalCount );
		
		AE_LOGI( "AVX v2 Size: "s << ToString( ChunksSize( chunks )));


		profiler.BeginTest( "AVX v2, layered",
							[s = ChunksSize( chunks ), totalCount] (secondsd dt)
							{
								double	bandwidth	= double(usize(s) * c_Cycles) / dt.count();
								double	flops		= double(totalCount * (3+3) * c_Cycles) / dt.count();

								return	ToStringSfx( bandwidth ) << "B/s | " <<
										ToStringSfx( flops ) << "FLOPS";
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

					for (uint k = (c == 0 ? 8 : 0); k < ch.count; k += 8)
					{
						uint		parent_idx		= ch.parentIds[k];
						uint		chunk_id		= parent_idx >> c_ToChunkIdx;
						uint		idx_in_chunk	= parent_idx & (c_ChunkSize-1);
						SimdFloat8	parent_pos_x	= *Cast<SimdFloat8>(&chunks[ chunk_id ].positionX[ idx_in_chunk ]);
						SimdFloat8	parent_pos_y	= *Cast<SimdFloat8>(&chunks[ chunk_id ].positionY[ idx_in_chunk ]);
						SimdFloat8	parent_pos_z	= *Cast<SimdFloat8>(&chunks[ chunk_id ].positionZ[ idx_in_chunk ]);
						SimdFloat8	scale			= *Cast<SimdFloat8>(ch.scaleBiasW + k);
						SimdFloat8	bias_x			= *Cast<SimdFloat8>(ch.scaleBiasX + k);
						SimdFloat8	bias_y			= *Cast<SimdFloat8>(ch.scaleBiasY + k);
						SimdFloat8	bias_z			= *Cast<SimdFloat8>(ch.scaleBiasZ + k);
						const uint	cur_idx			= uint(c * c_ChunkSize + k);

						ASSERT( parent_idx < cur_idx );
						Unused( cur_idx );

						#ifdef AE_DEBUG
						for (uint j = 0; j < 8; ++j)
						{
							ASSERT_Eq( parent_idx+j, ch.parentIds[k+j] );
						}
						#endif

						*Cast<SimdFloat8>(ch.positionX + k) = bias_x + (parent_pos_x * scale);
						*Cast<SimdFloat8>(ch.positionY + k) = bias_y + (parent_pos_y * scale);
						*Cast<SimdFloat8>(ch.positionZ + k) = bias_z + (parent_pos_z * scale);
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
			
			for (uint i = 0; i < 8; ++i)
			{
				ch.parentIds[i]		= UMax;
				ch.positionX[i]		= 0.f;
				ch.positionY[i]		= 0.f;
				ch.positionZ[i]		= 0.f;
				ch.scaleBiasX[i]	= 0.f;
				ch.scaleBiasY[i]	= 0.f;
				ch.scaleBiasZ[i]	= 0.f;
				ch.scaleBiasW[i]	= 0.f;
			}
			ch.count += 8;
		}

		// layout:
		//	[0000][1111][2222] [0000][1111] ...
		//  read root once

		HashMap< uint, uint >	same_depth;
		
		for (uint depth : hierarchyDepths)
		{
			same_depth.emplace( depth, 0u ).first->second ++;
		}

		for (auto [levels, count] : same_depth)
		{
			global_idx += count * levels;
			count = AlignUp( count, 8 );

			for (uint k = 0; k < count; k += 8)
			{
				if ( chunks.back().count + 8*levels > chunks.back().capacity )
					chunks.emplace_back();
				
				ChunkAVX&	ch = chunks.back();

				if ( ch.IsEmpty() )
					ch.Alloc( c_ChunkSize );

				uint	parent_id	= 0;
				uint	i			= ch.count;

				ASSERT( i%8 == 0 );
				ch.count += 8*levels;

				for (uint depth = 0; depth < levels; ++depth)
				{
					for (; i < ch.count; ++i)
					{
						uint	id = parent_id + i%8;

						ch.parentIds[i]		= id;
						ch.positionX[i]		= 0.f;
						ch.positionY[i]		= 0.f;
						ch.positionZ[i]		= 0.f;
						ch.scaleBiasX[i]	= 0.f;
						ch.scaleBiasY[i]	= 0.f;
						ch.scaleBiasZ[i]	= 0.f;
						ch.scaleBiasW[i]	= 0.f;
					
						if ( i%8 == 7 )
						{
							uint	cur_idx	= uint( (chunks.size()-1) * c_ChunkSize + (i & ~7) );
							//ASSERT( cur_idx == global_idx+8 );
							parent_id = cur_idx;
						}
					}
				}
			}
		}
		CHECK( global_idx == totalCount );
		
		AE_LOGI( "AVX v3 Size: "s << ToString( ChunksSize( chunks )));


		profiler.BeginTest( "AVX v3, layered",
							[s = ChunksSize( chunks ), totalCount] (secondsd dt)
							{
								double	bandwidth	= double(usize(s) * c_Cycles) / dt.count();
								double	flops		= double(totalCount * (3+3) * c_Cycles) / dt.count();

								return	ToStringSfx( bandwidth ) << "B/s | " <<
										ToStringSfx( flops ) << "FLOPS";
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

					for (uint k = (c == 0 ? 8 : 0); k < ch.count; k += 8)
					{
						uint		parent_idx		= ch.parentIds[k];
						uint		chunk_id		= parent_idx >> c_ToChunkIdx;
						uint		idx_in_chunk	= parent_idx & (c_ChunkSize-1);
						SimdFloat8	parent_pos_x	= *Cast<SimdFloat8>(&chunks[ chunk_id ].positionX[ idx_in_chunk ]);
						SimdFloat8	parent_pos_y	= *Cast<SimdFloat8>(&chunks[ chunk_id ].positionY[ idx_in_chunk ]);
						SimdFloat8	parent_pos_z	= *Cast<SimdFloat8>(&chunks[ chunk_id ].positionZ[ idx_in_chunk ]);
						SimdFloat8	scale			= *Cast<SimdFloat8>(ch.scaleBiasW + k);
						SimdFloat8	bias_x			= *Cast<SimdFloat8>(ch.scaleBiasX + k);
						SimdFloat8	bias_y			= *Cast<SimdFloat8>(ch.scaleBiasY + k);
						SimdFloat8	bias_z			= *Cast<SimdFloat8>(ch.scaleBiasZ + k);
						const uint	cur_idx			= uint(c * c_ChunkSize + k);

						ASSERT( parent_idx < cur_idx );
						Unused( cur_idx );

						#ifdef AE_DEBUG
						for (uint j = 0; j < 8; ++j)
						{
							ASSERT_Eq( parent_idx+j, ch.parentIds[k+j] );
						}
						#endif

						*Cast<SimdFloat8>(ch.positionX + k) = bias_x + (parent_pos_x * scale);
						*Cast<SimdFloat8>(ch.positionY + k) = bias_y + (parent_pos_y * scale);
						*Cast<SimdFloat8>(ch.positionZ + k) = bias_z + (parent_pos_z * scale);
					}
				}
			}
			profiler.EndIteration();

			ResetCache( chunks );
		}
		profiler.EndTest();
	}
	//-----------------------------------------------
}


extern void Perf_Hierarchy ()
{
	IntervalProfiler	profiler{ "ECS Hierarchy", IntervalProfiler::EFlags::SortByPerf | IntervalProfiler::EFlags::ExcludeDelta };

	Random				rnd;
	ulong				total_count = 0;
	Array<uint>			hierarchy_depths;
	hierarchy_depths.resize( c_HierarchySize );

	for (usize i = 0; i < c_HierarchySize; ++i)
	{
		hierarchy_depths[i] = rnd.Uniform( 0u, c_MaxDepth-1 );
	//	hierarchy_depths[i] = 4; //c_MaxDepth-2;

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
	Hierarchy_Test7( profiler, hierarchy_depths, uint(total_count) );	// x3 faster
//	Hierarchy_Test8( profiler, hierarchy_depths, uint(total_count) );
}
