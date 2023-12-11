// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics/Private/SoftwareCmdBufBase.h"

namespace AE::Graphics::_hidden_
{

/*
=================================================
    _Allocate
=================================================
*/
    void*  SoftwareCmdBufBase::_Allocate (Bytes size) __Th___
    {
        if_unlikely( (_dataPtr == null) | (size > _remainSize) )
        {
            Allocator_t::Allocator_t    alloc;
            Allocator_t                 alloc_ref{ alloc };

            Header* old = _curBlock;

            {
                Header* tmp = Cast<Header>( alloc_ref.Allocate( BlockSize ));
                CHECK_THROW( tmp != null, std::bad_alloc{} );

                _curBlock = tmp;
            }

            _curBlock->next = null;
            _curBlock->size = BlockSize;

            if ( old != null )
            {
                old->next   = _curBlock;
                old->size   -= _remainSize;
            }

            if ( _root == null )
                _root = _curBlock;

            _dataPtr    = _curBlock + 1;
            _remainSize = BlockSize - SizeOf<Header>;
        }

        void*   res = _dataPtr;
        _dataPtr    += size;
        _remainSize -= size;

        ASSERT( CheckPointerAlignment( res, usize(BaseAlign) ));
        return res;
    }

/*
=================================================
    Deallocate
=================================================
*/
    void  SoftwareCmdBufBase::Deallocate (void* root) __NE___
    {
        // TODO: not needed because used frame allocator

        Allocator_t::Allocator_t    alloc;
        Allocator_t                 alloc_ref{ alloc };

        for (auto* hdr = Cast<Header>( root ); hdr != null;)
        {
            void*   ptr = hdr;
            hdr = hdr->next;

            alloc_ref.Deallocate( ptr, BlockSize );
        }
    }

/*
=================================================
    _Validate
=================================================
*/
    bool  SoftwareCmdBufBase::_Validate (const void* root, usize cmdCount) C_NE___
    {
        if ( root == null )
            return false;

        Header const*   hdr     = Cast<Header>( root );
        BaseCmd const*  base    = Cast<BaseCmd>( hdr + 1 );
        Bytes           size    = hdr->size;

        for (;;)
        {
            Bytes   offset = Bytes{base} - Bytes{hdr};

            // move to next memory block
            if_unlikely( offset >= size )
            {
                if ( hdr->next == null )
                    return (offset == size);

                hdr     = hdr->next;
                base    = Cast<BaseCmd>( hdr + 1 );
                size    = hdr->size;
            }

            ASSERT( base->_magicNumber == BaseCmd::MAGIC );

            if_unlikely( base->_commandID >= cmdCount )
                return false;

            base = base + Bytes{base->_size};
        }
    }

} // AE::Graphics::_hidden_
