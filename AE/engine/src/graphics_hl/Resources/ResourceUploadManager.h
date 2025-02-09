// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Thread-safe:  yes
*/

#pragma once

#include "graphics_hl/GraphicsHL.pch.h"
#include "AssetPackerImpl.h"

namespace AE::Graphics
{
	enum class EUploadFlags : ubyte
	{
		Unknown					= 0,

		// Resource may be used (for rendering) every frame during uploading, ResourceUploadManager must issue barrier to support it.
		// User must transit resource to 'finalState' before commands from 'Upload()' will be executed on GPU.
		// If flag is not enabled, single barrier will be issued when uploading complete or failed.
		UsedWhileUploading		= 1 << 0,
	};
	AE_BIT_OPERATORS( EUploadFlags );



	//
	// Resource Upload Manager
	//

	class ResourceUploadManager final : public Threading::ITaskDependencyManager
	{
	// types
	private:
		using TransferContext_t		= DirectCtx::Transfer;
		using AsyncRDataSource		= Threading::AsyncRDataSource;
		using ImagePackerHeader		= AssetPacker::ImagePacker::Header;

		//
		// Upload Task
		//
		class UploadTask final : public EnableRC<UploadTask>
		{
			friend class ResourceUploadManager;

		// types
		public:
			enum class EType : ubyte
			{
				Unknown,
				Buffer,
				Image,
				//RTGeometry
				//RTScene
			};

			enum class EUploadRes : ubyte
			{
				Downloading,	// from Disk
				Uploading,		// to VRAM
				Complete,
				NoStagingMemory,
				Failed,
			};

			enum class EStatus : ubyte
			{
				Initial,
				InProgress,
				Complete,
				Failed,
			};

		private:
			using TaskDependency	= Threading::IAsyncTask::TaskDependency;
			using Dependencies_t	= FixedTupleArray< 4, AsyncTask, TaskDependency >;		// TODO: use IAsyncTask::OutputChunk
			using ReadResult_t		= Threading::AsyncDSRequest::Value_t::Result;

			struct BufferInfo
			{
				BufferID			id;
				Bytes				size;

			// mutable
				AtomicBytes<ulong>	offset;

				BufferInfo (BufferID id, Bytes size) __NE___ : id{id}, size{size} {}
			};

			struct ImageInfo : ImagePackerHeader
			{
				ImageID			id;

			// mutable
				ImageDim2_t		offsetYZ;
				ImageDim_t		currentDim;
				ImageLayer		currentLayer;
				Atomic<ushort>	currentMip;
				Bytes32u		rowPitch;
				Bytes			slicePitch;

				ImageInfo (const ImagePackerHeader &hdr, ImageID id) __NE___ : ImagePackerHeader{hdr}, id{id} {}
			};

			union BufferOrImageInfo
			{
				char			none;
				BufferInfo		buf;
				ImageInfo		img;

				BufferOrImageInfo () __NE___ : none{} {}
			};


		// variables
		private:
			BufferOrImageInfo			_info;
			EType						_type		= Default;
			Atomic<EStatus>				_status		{EStatus::Initial};
			EUploadFlags				_flags		= Default;

			EResourceState				_curState	= Default;
			EResourceState				_finalState	= Default;

			Bytes32u					_memOffset;				// in '_readReq->GetResult().data'

			Threading::AsyncDSRequest	_readReq;
			RC<AsyncRDataSource>		_file;
			Bytes						_baseOffset;			// in '_file'

			SpinLock					_depsGuard;
			Dependencies_t				_deps;


		// methods
		public:
			UploadTask ()								__NE___ {}

			ND_ bool		IsCompleted ()				C_NE___	{ return _status.load() == EStatus::Complete; }
			ND_ bool		IsFailed ()					C_NE___	{ return _status.load() == EStatus::Failed; }
			ND_ bool		IsFinished ()				C_NE___	{ return _status.load() >= EStatus::Complete; }

			ND_ MipmapLevel	CompletelyUploadedMipmap ()	C_NE___;	// for image
			ND_ Bytes		CompletelyUploadedSize ()	C_NE___;	// for buffer

		private:
			template <typename TransferContext>
			ND_ auto  _Upload (TransferContext &, EStagingHeapType)									__NE___ -> EUploadRes;

			template <typename TransferContext>
			ND_ auto  _UploadImage (const ReadResult_t &,TransferContext &, EStagingHeapType)		__NE___ -> EUploadRes;

			template <typename TransferContext>
			ND_ auto  _UploadBuffer (const ReadResult_t &,TransferContext &, EStagingHeapType)		__NE___ -> EUploadRes;

			ND_ bool  _AddOnCompleteDependency (AsyncTask task, INOUT uint &index, Bool isStrong)	__NE___;
				void  _SetDependencyCompleteStatus (Bool complete)									__NE___;

				void  _ReadNextImageBlock ()			__NE___;
				void  _ReadNextBufferBlock ()			__NE___;
		};


		//
		// Upload Render Task
		//
		class UploadRenderTask final : public RenderTask
		{
		private:
			RC<ResourceUploadManager>	_self;
			const EStagingHeapType		_heapType;
			const bool					_isFirst;

		public:
			UploadRenderTask (ResourceUploadManager* p, EStagingHeapType heapType, bool isFirst, CommandBatchPtr batch, DebugLabel) __NE___;

			void  Run () __Th_OV;
		};


		struct FirstTransitionData
		{
			ImageID				imageId;
			EResourceState		initialState;
			EResourceState		finalState;
		};

		using UploadQueue_t		= RingBuffer< RC<UploadTask> >;
		using TransQueue_t		= Array< FirstTransitionData >;

		static constexpr uint	MaxNoStagingMemory	= 4;
		static constexpr uint	MaxTasksPerFrame	= 64;
		static constexpr uint	MaxUploadRTaskCount	= 8;
		static constexpr Bytes	MaxBlockSize		{1_MiB};	// row size for 32k RGBA32F is 1 MiB - this is minimal size for block.

	public:
		using UploadResult			= RC<UploadTask>;	// TODO
		using AtomicUploadResult	= AtomicRC<UploadTask>;
		using WeakUploadResult		= Threading::_hidden_::_TaskDependency< UploadResult, False{"weak"} >;

		// For better performance user should issue barrier as later as possible.
		// 'CopyDst' is same state as used to upload resource data, so no additional barrier where issued by 'ResourceUploadManager'
		// and user can issue barrier exactly before using resource.
		static constexpr auto	FinalState			= EResourceState::CopyDst;

		// For better performance used Transfer stage for first transition.
		static constexpr auto	InitialState		= EResourceState::CopyDst | EResourceState::Invalidate;


	// variables
	private:
		//alignas(AE_CACHE_LINE)
		//  AtomicBytes<ulong>	_usedRAM;
		//Bytes					_maxRAM;

		SpinLock				_uploadQueueGuard;
		UploadQueue_t			_uploadQueue;

		SpinLock				_transQueueGuard;
		TransQueue_t			_transQueue;		// TODO: lock-free double buffered array


	// methods
	public:
		ResourceUploadManager ()													__NE___;
		~ResourceUploadManager ()													__NE___;

			void  Deinitialize ()													__NE___;

		// Returns empty task which must be used to enqueue resource uploading.
		// Also this task can be used as dependency for 'AsyncTask'.
		//
		ND_ RC<UploadTask>  CreateTask ()											__NE___;

		ND_ bool  EnqueueImage (RC<UploadTask>			emptyTask,
								ImageID					imageId,
								ImagePackerHeader const	&desc,
								RC<AsyncRDataSource>	file,
								Bytes					posInFile,
								EUploadFlags			flags		 = Default,
								EResourceState			initialState = InitialState,
								EResourceState			finalState	 = FinalState)	__NE___;

		ND_ bool  EnqueueImage (RC<UploadTask>			emptyTask,
								ImageID					imageId,
								RC<AsyncRDataSource>	file,
								Bytes					posInFile,
								EUploadFlags			flags		 = Default,
								EResourceState			initialState = InitialState,
								EResourceState			finalState	 = FinalState)	__NE___;

		ND_ bool  EnqueueBuffer (RC<UploadTask>			emptyTask,
								 BufferID				bufferId,
								 Bytes					dataSize,
								 RC<AsyncRDataSource>	file,
								 Bytes					posInFile,
								 EUploadFlags			flags		 = Default,
								 EResourceState			initialState = InitialState,
								 EResourceState			finalState	 = FinalState)	__NE___;

		// * When image created its layout is 'Invalidate'.
		// * For mutable resource user may add 'Invalidate' flag to srcState in first barrier, so this method is not needed.
		// * When used streaming to update image, flag 'Invalidate' may drop previous content.
		//   So you need to use 'FirstTransition()' to move resource from 'Invalidate' to some expected state,
		//   which will be used later.
		// * For RenderGraph 'finalState' must be default state which is passed to 'ResStateTracker::SetDefaultState()' or
		//   use 'ResStateTracker::GetDefaultState()' to get auto-deduced default state.
		//
		bool  FirstTransition (ImageID					imageId,
							   EResourceState			initialState,
							   EResourceState			finalState)					__NE___;

		// Record upload commands.
		// May be slow on CPU side if uploaded a lot of data.
		// Use 'UploadAsync' for better performance.
		//
		void  Upload (TransferContext_t	&ctx,
					  EStagingHeapType	heapType = EStagingHeapType::Dynamic)		__NE___;

		// Schedule upload tasks.
		// * Single thread has limited PCI/memory bandwidth, multiple tasks
		//   will be distributed to multiple threads and increase performance.
		// * Image first transition is always happens in first task.
		//   Only execution order on GPU is matters, so you not need to depends on this tasks
		//   on CPU side to gen correct state for image.
		// * Command batch submission must happens when all recording tasks are complete,
		//   so returned 'AsyncTask' should be used only for this purpose.
		// * Warning: heap type 'Static' and 'Any' may use whole static memory which
		//   is needed to update uniform buffers. Static memory reserved for each command queue type,
		//   so it is safe to use static memory in 'AsyncTransfer' queue which is never used to update uniforms.
		//
		ND_ AsyncTask  UploadAsync (CommandBatch			&batch,
									uint					taskCount	= 0, // auto
									EStagingHeapType		heapType	= EStagingHeapType::Dynamic,
									ArrayView<AsyncTask>	deps		= Default)	__NE___;


	private:
		// ITaskDependencyManager //
		bool  Resolve (AnyTypeCRef dep, AsyncTask task, INOUT uint &bitIndex)		__NE_OV;

		ND_ static EResourceState  _DetectImageFinalState (ImageID)					__NE___;
		ND_ static EResourceState  _DetectBufferFinalState (BufferID)				__NE___;

		template <typename TransferContext>
		void  _RecordFirstTransitions (TransferContext &ctx)						__NE___;

		template <typename TransferContext>
		void  _Upload (TransferContext &ctx, EStagingHeapType heapType)				__NE___;
	};


} // AE::Graphics
