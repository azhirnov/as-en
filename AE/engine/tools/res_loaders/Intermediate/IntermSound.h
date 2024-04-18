// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_loaders/Public/Common.h"

namespace AE::ResLoader
{

	//
	// Intermediate Sound
	//

	class IntermSound final : public EnableRC<IntermSound>
	{
	// variables
	private:
		Path			_srcPath;

		void*			_data			= null;
		RC<SharedMem>	_storage;		// can be null

		AudioDataDesc	_desc;

		bool			_immutable		= false;


	// methods
	public:
		IntermSound ()															__NE___ {}
		IntermSound (IntermSound &&)											__NE___;
		explicit IntermSound (Path path)										__NE___ : _srcPath{RVRef(path)} {}

		IntermSound&  operator = (IntermSound &&)								__NE___;

			void  MakeImmutable ()												__NE___	{ _immutable = true; }

		ND_ bool  SetData (const AudioDataDesc &desc, RC<SharedMem> storage)	__NE___;
		ND_ bool  SetData (const AudioDataDesc &desc, void* data)				__NE___;

		ND_ bool  Allocate (const AudioDataDesc &desc, RC<IAllocator> allocator)__NE___;
		ND_ bool  Allocate (const AudioDataDesc &desc)							__NE___;

		ND_ Path const&		GetPath ()											C_NE___	{ return _srcPath; }
		ND_ bool			IsImmutable ()										C_NE___	{ return _immutable; }
		ND_ bool			IsMutable ()										C_NE___	{ return not _immutable; }

		ND_ auto const&		Description ()										C_NE___	{ return _desc; }
		ND_ ArrayView<char>	GetData ()											C_NE___	{ return ArrayView<char>{ Cast<char>(_data), usize{_desc.size} }; }
	};


} // AE::ResLoader
