// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics/Public/BufferDesc.h"

namespace AE::Graphics
{

/*
=================================================
	Validate
=================================================
*/
	void BufferDesc::Validate () __NE___
	{
		// validate memory type
		memType &= ~EMemoryType::Transient;

		if ( memType == Default )
			memType = EMemoryType::DeviceLocal;

		if ( NoBits( memType, EMemoryType::DeviceLocal ))
		{
			options &= ~EBufferOpt::SparseResidencyAliased;
			usage   &= ~(EBufferUsage::StorageTexel | EBufferUsage::Storage | EBufferUsage::ShaderAddress |
						 EBufferUsage::ShaderBindingTable | EBufferUsage::ASBuild_ReadOnly | EBufferUsage::ASBuild_Scratch);
		}

		if ( usage == Default )
			usage = EBufferUsage::Transfer;

		if ( NoBits( usage, EBufferUsage::Storage | EBufferUsage::StorageTexel ))
			options &= ~(EBufferOpt::VertexPplnStore | EBufferOpt::FragmentPplnStore);

		if ( NoBits( usage, EBufferUsage::StorageTexel ))
			options &= ~EBufferOpt::StorageTexelAtomic;

		if ( AnyBits( usage, EBufferUsage::ShaderBindingTable | EBufferUsage::ASBuild_ReadOnly | EBufferUsage::ASBuild_Scratch ))
			usage |= EBufferUsage::ShaderAddress;

		ASSERT( usage != Default );
	}

/*
=================================================
	operator ==
=================================================
*/
	bool  BufferDesc::operator == (const BufferDesc &rhs) C_NE___
	{
		return	(size		== rhs.size)	and
				(usage		== rhs.usage)	and
				(options	== rhs.options)	and
				(queues		== rhs.queues)	and
				(memType	== rhs.memType);
	}
//-----------------------------------------------------------------------------


/*
=================================================
	Validate
=================================================
*/
	void BufferViewDesc::Validate (const BufferDesc &desc) __NE___
	{
		ASSERT( offset < desc.size );
		ASSERT( format != Default );
		ASSERT( AnyBits( desc.usage, EBufferUsage::UniformTexel | EBufferUsage::StorageTexel ));

		offset	= Min( offset, desc.size-1 );
		size	= Min( size, desc.size - offset );
	}

/*
=================================================
	operator ==
=================================================
*/
	bool  BufferViewDesc::operator == (const BufferViewDesc &rhs) C_NE___
	{
		return	(format	== rhs.format)	and
				(offset	== rhs.offset)	and
				(size	== rhs.size);
	}

} // AE::Graphics
