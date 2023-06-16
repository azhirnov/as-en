// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	File - any data source with fixed size.

	FileGroup - list of file which can be preloaded or preload file headers.

	Storage - collection of files and file groups.

TODO:
	- block allocator for RStream
*/

#pragma once

#include "vfs/Common.h"

namespace AE { VFS::VirtualFileSystem&  GetVFS () __NE___; }

namespace AE::VFS
{

	//
	// Virtual File System Storage interface
	//
	class IVirtualFileStorage : public EnableRC<IVirtualFileStorage>
	{
		friend class VirtualFileSystem;

	// types
	protected:
		struct GlobalFileRef
		{
			IVirtualFileStorage const*	storage;
			void const*					ref;		// map iterator, only for static storage
		};

		using GlobalFileMap_t = FlatHashMap< FileName::Optimized_t, GlobalFileRef >;


	// interface
	public:
		ND_ virtual bool	Open (OUT RC<RStream> &stream, const FileName &name)							C_NE___ = 0;
		ND_ virtual bool	Open (OUT RC<RDataSource> &ds, const FileName &name)							C_NE___ = 0;
		ND_ virtual bool	Open (OUT RC<AsyncRDataSource> &ds, const FileName &name)						C_NE___ = 0;

		ND_ virtual bool	Exists (const FileName &name)													C_NE___ = 0;
		ND_ virtual bool	Exists (const FileGroupName &name)												C_NE___ = 0;

	protected:
			virtual void  _Append (INOUT GlobalFileMap_t &)													C_Th___ = 0;

		ND_ virtual bool  _OpenByIter (OUT RC<RStream> &stream, const FileName &name, const void* ref)		C_NE___ = 0;
		ND_ virtual bool  _OpenByIter (OUT RC<RDataSource> &ds, const FileName &name, const void* ref)		C_NE___ = 0;
		ND_ virtual bool  _OpenByIter (OUT RC<AsyncRDataSource> &ds, const FileName &name, const void* ref)	C_NE___ = 0;
	};



	//
	// Virtual File System
	//
	class VirtualFileSystem final
	{
		friend struct InPlace<VirtualFileSystem>;

	// types
	private:
		using GlobalFileMap_t = IVirtualFileStorage::GlobalFileMap_t;


	// variables
	private:
		GlobalFileMap_t						_globalMap;
		Array< RC<IVirtualFileStorage> >	_storageArr;
		
		DRC_ONLY( RWDataRaceCheck			_drCheck;)


	// methods
	public:
		static void  CreateInstance ();
		static void  DestroyInstance ();
		

		ND_ bool  AddStorage (RC<IVirtualFileStorage> storage)				__NE___;
		
		ND_ bool  Open (OUT RC<RStream> &stream, const FileName &name)		C_NE___;
		ND_ bool  Open (OUT RC<RDataSource> &ds, const FileName &name)		C_NE___;
		ND_ bool  Open (OUT RC<AsyncRDataSource> &ds, const FileName &name)	C_NE___;

		ND_ auto  OpenAsStream (const FileName &name)						C_NE___ -> RC<RStream>;
		ND_ auto  OpenAsSource (const FileName &name)						C_NE___ -> RC<RDataSource>;
		ND_ auto  OpenAsAsyncDS (const FileName &name)						C_NE___ -> RC<AsyncRDataSource>;

		ND_ bool  Exists (const FileName &name)								C_NE___;
		ND_ bool  Exists (const FileGroupName &name)						C_NE___;

	private:
		VirtualFileSystem ()												__NE___	{}
		~VirtualFileSystem ()												__NE___	{}

		template <typename ResultType>
		ND_ bool  _Open (OUT ResultType &, const FileName &name)			C_NE___;

		friend VirtualFileSystem&		AE::GetVFS ()						__NE___;
		ND_ static VirtualFileSystem&	_Instance ()						__NE___;
	};



	//
	// Virtual File Storage Factory
	//
	class VirtualFileStorageFactory : public Noninstanceable
	{
	public:
		ND_ static RC<IVirtualFileStorage>  CreateStaticArchive (RC<RDataSource> archive)							__NE___;
		ND_ static RC<IVirtualFileStorage>  CreateStaticArchive (const Path &filename)								__NE___;
		
		ND_ static RC<IVirtualFileStorage>  CreateStaticFolder (const Path &folder, StringView prefix = Default)	__NE___;
		ND_ static RC<IVirtualFileStorage>  CreateDynamicFolder (const Path &folder, StringView prefix = Default)	__NE___;
	};

	
} // AE::VFS


namespace AE
{
/*
=================================================
	GetVFS
=================================================
*/
	ND_ forceinline VFS::VirtualFileSystem&  GetVFS () __NE___
	{
		return VFS::VirtualFileSystem::_Instance();
	}

} // AE
