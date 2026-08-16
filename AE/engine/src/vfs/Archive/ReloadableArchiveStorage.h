// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Should not be used in Release.
*/

#pragma once

#include "vfs/Archive/ArchiveStaticStorage.h"

namespace AE::VFS
{

	//
	// Reloadable Archive Storage
	//

	class ReloadableArchiveStorage final : public IVirtualFileStorage
	{
	// variables
	private:
		mutable SharedMutex			_guard;
		RC<ArchiveStaticStorage>	_storage;


	// methods
	public:
		ReloadableArchiveStorage ()													__NE___	{}
		~ReloadableArchiveStorage ()												__NE_OV {}

		ND_ bool  Reload (RC<RDataSource>)											__NE___;
		ND_ bool  Reload (RC<AsyncRDataSource>, RC<RDataSource>)					__NE___;
		ND_ bool  Reload (const Path &filename)										__NE___;

			void  Reset ()															__NE___;


	  // IVirtualFileStorage //
		bool  Open (OUT RC<RStream> &stream, FileName::Ref name)					C_NE_OV;
		bool  Open (OUT RC<RDataSource> &ds, FileName::Ref name)					C_NE_OV;
		bool  Open (OUT RC<AsyncRStream> &stream, FileName::Ref name)				C_NE_OV;
		bool  Open (OUT RC<AsyncRDataSource> &ds, FileName::Ref name)				C_NE_OV;

		using IVirtualFileStorage::Open;

		bool  Exists (FileName::Ref name)											C_NE_OV;
		bool  Exists (FileGroupName::Ref name)										C_NE_OV;


	private:
		void  _Append (INOUT GlobalFileMap_t &)										C_Th_OV	{}

		bool  _OpenByIter (OUT RC<RStream>&, FileName::Ref, const void*)			C_NE_OV	{ DBG_WARNING("not supported");  return false; }
		bool  _OpenByIter (OUT RC<RDataSource>&, FileName::Ref, const void*)		C_NE_OV	{ DBG_WARNING("not supported");  return false; }
		bool  _OpenByIter (OUT RC<AsyncRStream>&, FileName::Ref, const void*)		C_NE_OV	{ DBG_WARNING("not supported");  return false; }
		bool  _OpenByIter (OUT RC<AsyncRDataSource>&, FileName::Ref, const void*)	C_NE_OV	{ DBG_WARNING("not supported");  return false; }

		bool  _GetPath (FileName::Ref, OUT Path &)									C_NE_OV	{ return false; }

		using IVirtualFileStorage::_OpenByIter;
	};


} // AE::VFS
