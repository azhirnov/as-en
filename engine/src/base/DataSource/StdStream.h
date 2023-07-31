// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/BitMath.h"
#include "base/DataSource/Stream.h"

namespace AE::Base
{

    //
    // Streambuf Wrapper
    //

    template <typename CharT>
    class StreambufWrap final : public std::basic_streambuf<CharT>
    {
    // types
    public:
        using Base_t    = std::basic_streambuf<CharT>;
        using char_type = typename Base_t::char_type;
        using int_type  = typename Base_t::int_type;
        using pos_type  = typename Base_t::pos_type;
        using off_type  = typename Base_t::off_type;

    private:
        static constexpr Bytes  charSoze = SizeOf<CharT>;


    // variables
    private:
        RC<RStream>     _src;


    // methods
    public:
        explicit StreambufWrap (const RC<RStream> &src) : _src{src} {}


    protected:
        // obtains the number of characters available for input in the associated input sequence, if known
        std::streamsize  showmanyc () _____OV
        {
            if ( _src and _src->IsOpen() )
                return std::streamsize(_src->RemainingSize() / charSoze);
            else
                return 0;
        }

        // reads characters from the associated input sequence to the get area
        int_type  underflow () _____OV
        {
            if ( _src and _src->IsOpen() )
                return int_type(_src->RemainingSize() / charSoze);
            else
                return 0;
        }

        // reads characters from the associated input sequence to the get area and advances the next pointer
        int_type  uflow () _____OV
        {
            return Base_t::uflow();
        }

        // reads multiple characters from the input sequence
        std::streamsize  xsgetn (char_type* s, std::streamsize count) _____OV
        {
            if_likely( _src and _src->IsOpen() )
                return std::streamsize(_src->ReadSeq( s, charSoze * ulong(count) ) / charSoze);
            else
                return 0;
        }

        pos_type  seekoff (off_type off, std::ios_base::seekdir dir, std::ios_base::openmode which) _____OV
        {
            Unused( which );

            ASSERT( AllBits( which, std::ios_base::in ));   // only 'in' is supported
            const auto  err = pos_type(off_type{-1});

            if_likely( _src and _src->IsOpen() )
            {
                Bytes   new_pos;
                switch ( dir )
                {
                    case std::ios_base::beg :   new_pos = Bytes{CheckCast<ulong>(off)};     break;
                    case std::ios_base::end :   new_pos = _src->Size() - off;               break;
                    case std::ios_base::cur :   new_pos = _src->Position() + off;           break;
                    default :                   new_pos = Bytes::Max();                     break;
                }

                if ( _src->SeekSet( new_pos ))
                    return pos_type(usize( new_pos ));
            }
            return err;
        }

        pos_type  seekpos (pos_type pos, std::ios_base::openmode which) _____OV
        {
            return seekoff( pos - pos_type(off_type{0}), std::ios_base::beg, which );
        }
    };

} // AE::Base
