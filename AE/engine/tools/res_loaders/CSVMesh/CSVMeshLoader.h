// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Thread-safe:	yes
*/

#pragma once

#include "res_loaders/Public/MeshLoader.h"

namespace AE::ResLoader
{
	using Graphics::EVertexType;
	using Graphics::EPrimitive;


	//
	// CSV Mesh Loader
	//

	class CSVMeshLoader final : public IMeshLoader
	{
	// types
	public:
		struct VertexMapping
		{
			EVertexType		type	= Default;
			ushort			index	= UMax;
			ushort			comp	= UMax;

			ND_ bool  IsIndex ()		C_NE___	{ return AnyEqual( type, EVertexType::UByte, EVertexType::UShort, EVertexType::UInt ) and index == UMax and comp == 0; }
			ND_ bool  IsNotDefined ()	C_NE___	{ return type == Default or comp == UMax; }
		};

		// put it in 'Config::spec'
		struct CSVConfig
		{
			static constexpr uint				MAGIC			= "CSV-mesh"_Hash;

			uint								magic			= MAGIC;
			EPrimitive							topology		= EPrimitive::TriangleList;
			HashMap< String, VertexMapping >	columnToVertex;
		};

	// methods
	public:
		bool  LoadMesh (OUT IntermMesh		&mesh,
						RStream				&stream,
						const Config		&cfg,
						EModelFormat		format	= Default)	__NE_OV;

		bool  LoadMesh (OUT IntermMesh		&mesh,
						const Path			&meshPath,
						const Config		&cfg)				__NE_OV;
	};


} // AE::ResLoader
