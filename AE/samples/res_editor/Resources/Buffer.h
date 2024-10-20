// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Resources/IResource.h"
#include "res_editor/Resources/DataTransferQueue.h"

namespace AE::ResEditor
{

	//
	// Buffer
	//

	class Buffer final : public IResource
	{
	// types
	public:
		enum class EBufferFlags : uint
		{
			Unknown			= 0,
			WithHistory		= 1 << 0,	// unique buffer for frame cycle with content history
		};

		struct LoadOp
		{
			RC<AsyncRDataSource>	file;
			Array<ubyte>			data;
			bool					clear	= false;
		};

		struct StoreOp
		{
			RC<AsyncWDataSource>	file;

			StoreOp () {}
		};

		using IDs_t	= StaticArray< Strong<BufferID>, GraphicsConfig::MaxFrames >;

	private:
		struct LoadOp2 : LoadOp
		{
			AsyncDSRequest			request;
			BufferStream			stream;

			LoadOp2 () = default;
			LoadOp2 (LoadOp &&op) : LoadOp{RVRef(op)} {}

			ND_ bool  IsDefined ()	C_NE___;
		};

		struct StoreOp2 : StoreOp
		{
			// mutable
			BufferStream			stream;
			bool					complete	= false;

			StoreOp2 () {}
			StoreOp2 (const StoreOp &other) : StoreOp{other} {}

			ND_ bool  IsDefined ()		C_NE___	{ return bool{file}; }
			ND_ bool  IsCompleted ()	C_NE___	{ return complete; }
		};

		using _IDs_t		= StaticArray< StrongAtom<BufferID>, GraphicsConfig::MaxFrames >;
		using _Address_t	= StaticArray< ulong, GraphicsConfig::MaxFrames >;


	// variables
	private:
		_IDs_t						_ids;
		//StrongAtom<BufferViewID>	_view;
		_Address_t					_address;

		const ShaderStructName		_typeName;
		const Bytes					_staticSize;
		const Bytes					_elemSize;

		Synchronized< RWSpinLock,
			BufferDesc >			_bufDesc;
		RC<DynamicUInt>				_inDynCount;
		RC<DynamicUInt>				_outDynCount;

		LoadOp2						_loadOp;
		StoreOp2					_storeOp;

		const EBufferFlags			_flags;
		const String				_dbgName;

		Array<RC<Buffer>>			_refBuffers;


	// methods
	private:
		Buffer (Renderer&	renderer,
			    StringView	dbgName);

	public:
		Buffer (IDs_t				ids,
				const BufferDesc &	desc,
				Bytes				staticSize,
				Bytes				elemSize,
				LoadOp				loadOp,
				ShaderStructName	typeName,
				Renderer &			renderer,
				RC<DynamicUInt>		inDynCount,
				RC<DynamicUInt>		outDynCount,
				StringView			dbgName,
				EBufferFlags		flags,
				Array<RC<Buffer>>	refBuffers)						__Th___;

		~Buffer ()													__NE_OV;

		ND_ BufferID				GetBufferId (uint fid)			const	{ return _ids[ fid ].Get(); }
		ND_ BufferID				GetBufferId (FrameUID fid)		const	{ return _ids[ fid.Index() ].Get(); }

		ND_ BufferDesc				GetBufferDesc ()				const	{ return _bufDesc.Read(); }
		ND_ ShaderStructName		GetContentType ()				const	{ return _typeName; }

		ND_ ulong					GetDeviceAddress (uint fid)		const	{ return _address[ fid ]; }
		ND_ ulong					GetDeviceAddress (FrameUID fid)	const	{ return _address[ fid.Index() ]; }

		ND_ Bytes					ElementSize ()					const	{ return _elemSize; }
		ND_ ulong					ArraySize ()					const	{ return ulong((_bufDesc->size - _staticSize) / _elemSize); }

		ND_ StringView				Name ()							const	{ return _dbgName; }
		ND_ bool					HasHistory ()					const	{ return AllBits( _flags, EBufferFlags::WithHistory ); }

		ND_ ArrayView<RC<Buffer>>	GetRefBuffers ()				const	{ return _refBuffers; }

	// IResource //
		bool			Resize (TransferCtx_t &ctx)					__Th_OV;
		bool			RequireResize ()							C_Th_OV;
		EUploadStatus	Upload (TransferCtx_t &)					__Th_OV;
		EUploadStatus	Readback (TransferCtx_t &)					__Th_OV;
		void			Cancel ()									__NE_OV;


		ND_ static RC<Buffer>  CreateAndStore (const Buffer  &src,
											   const StoreOp &storeOp,
											   StringView    dbgName) __Th___;
	};

	AE_BIT_OPERATORS( Buffer::EBufferFlags );


} // AE::ResEditor
