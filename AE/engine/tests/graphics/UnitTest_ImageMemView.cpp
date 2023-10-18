// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics/Public/ImageMemView.h"
#include "UnitTest_Common.h"

namespace
{
    static void  ImageMemView_Test1 ()
    {
        const uint3     dim     {62, 57, 1};
        const Bytes     bpp     = 4_b;
        const Bytes     pitch_a = dim.x * bpp;
        const Bytes     pitch_b = AlignUp( dim.x, 8 ) * bpp;

        Array<ubyte>    data_a;     data_a.resize( usize( pitch_a * dim.y ));
        Array<ubyte>    data_b;     data_b.resize( usize( pitch_b * dim.y ));

        for (uint y = 0; y < dim.y; ++y)
        {
            ubyte*  row_a   = &data_a[usize( pitch_a * y )];
            ubyte*  row_b   = &data_b[usize( pitch_b * y )];

            for (uint x = 0; x < dim.x; ++x)
            {
                *(row_a++) = *(row_b++) = ubyte( (5 + x + y * dim.y) * 2 );
                *(row_a++) = *(row_b++) = ubyte( (2 + x + y * dim.y) * 7 );
                *(row_a++) = *(row_b++) = ubyte( (3 + x + y * dim.y) * 3 );
                *(row_a++) = *(row_b++) = ubyte( (8 + x + y * dim.y) * 4 );
            }
        }

        BufferMemView   buf_a;      buf_a.PushBack( data_a.data(), ArraySizeOf(data_a) );
        BufferMemView   buf_b;      buf_b.PushBack( data_b.data(), ArraySizeOf(data_b) );

        ImageMemView    image_a{ buf_a, uint3{}, dim, pitch_a, 0_b, EPixelFormat::RGBA8_UNorm, EImageAspect::Color };
        ImageMemView    image_b{ buf_b, uint3{}, dim, pitch_b, 0_b, EPixelFormat::RGBA8_UNorm, EImageAspect::Color };

        TEST( image_a.RowPitch() == pitch_a );
        TEST( image_b.RowPitch() == pitch_b );
        TEST( image_a.BitsPerBlock() == 4*8 );

        Bytes   d = image_a.Compare( image_b );
        TEST( d == 0_b );
    }


    static void  ImageMemView_Test2 ()
    {
        const uint3     dim_a   { 61, 57, 1};
        const uint3     dim_b   {113, 97, 1};
        const Bytes     bpp     = 4_b;
        const Bytes     pitch_a = AlignUp( dim_a.x, 4 ) * bpp;
        const Bytes     pitch_b = AlignUp( dim_b.x, 8 ) * bpp;

        Array<ubyte>    data_a;     data_a.resize( usize( pitch_a * dim_a.y ));
        Array<ubyte>    data_b;     data_b.resize( usize( pitch_b * dim_b.y ));

        for (uint y = 0; y < dim_a.y; ++y)
        {
            ubyte*  row = &data_a[usize( pitch_a * y )];
            for (uint x = 0; x < dim_a.x; ++x)
            {
                for (uint c = 0; c < 4; ++c)
                    *(row++) = ubyte( c + x*4 + 4*y*dim_a.x );
            }
        }

        for (uint y = 0; y < dim_b.y; ++y)
        {
            ubyte*  row = &data_b[usize( pitch_b * y )];
            for (uint x = 0; x < dim_b.x; ++x)
            {
                for (uint c = 0; c < 4; ++c)
                    *(row++) = ubyte( c + x*4 + 4*y*dim_b.x );
            }
        }

        const uint3     off_a   {15, 13, 0};
        const uint3     off_b   {42, 37, 0};
        const uint3     dim     {32, 32, 1};

        for (uint y = 0; y < dim.y; ++y)
        {
            Bytes   offa    = pitch_a * (y + off_a.y) + bpp * off_a.x;
            Bytes   offb    = pitch_b * (y + off_b.y) + bpp * off_b.x;

            ubyte*  row_a   = &data_a[usize( offa )];
            ubyte*  row_b   = &data_b[usize( offb )];

            for (uint x = 0; x < dim.x; ++x)
            {
                *(row_a++) = *(row_b++) = ubyte( (5 + x + y * dim.x) * 2 );
                *(row_a++) = *(row_b++) = ubyte( (2 + x + y * dim.x) * 7 );
                *(row_a++) = *(row_b++) = ubyte( (3 + x + y * dim.x) * 3 );
                *(row_a++) = *(row_b++) = ubyte( (8 + x + y * dim.x) * 4 );
            }
        }

        BufferMemView   buf_a;      buf_a.PushBack( data_a.data(), ArraySizeOf(data_a) );
        BufferMemView   buf_b;      buf_b.PushBack( data_b.data(), ArraySizeOf(data_b) );

        ImageMemView    image_a{ buf_a, uint3{}, dim_a, pitch_a, 0_b, EPixelFormat::RGBA8_UNorm, EImageAspect::Color };
        ImageMemView    image_b{ buf_b, uint3{}, dim_b, pitch_b, 0_b, EPixelFormat::RGBA8_UNorm, EImageAspect::Color };

        TEST( image_a.RowPitch() == pitch_a );
        TEST( image_b.RowPitch() == pitch_b );
        TEST( image_a.BitsPerBlock() == 4*8 );

        Bytes   d = image_a.Compare( off_a, off_b, image_b, dim );
        TEST( d == 0_b );
    }


    static void  ImageMemView_Test3 ()
    {
        const uint3     dim     {62, 57, 1};
        const Bytes     bpp     = 4_b;
        const Bytes     pitch   = dim.x * bpp;

        Array<ubyte>    data_a;     data_a.resize( usize( pitch * dim.y ));
        Array<ubyte>    data_b;     data_b.resize( usize( pitch * dim.y ));

        for (uint y = 0; y < dim.y; ++y)
        {
            ubyte*  row = &data_b[usize( pitch * y )];
            for (uint x = 0; x < dim.x; ++x)
            {
                *(row++) = ubyte( (5 + x + y * dim.y) * 2 );
                *(row++) = ubyte( (2 + x + y * dim.y) * 7 );
                *(row++) = ubyte( (3 + x + y * dim.y) * 3 );
                *(row++) = ubyte( (8 + x + y * dim.y) * 4 );
            }
        }
        TEST( data_a != data_b );

        BufferMemView   buf_a;      buf_a.PushBack( data_a.data(), ArraySizeOf(data_a) );
        BufferMemView   buf_b;      buf_b.PushBack( data_b.data(), ArraySizeOf(data_b) );

        ImageMemView    image_a{ buf_a, uint3{}, dim, pitch, 0_b, EPixelFormat::RGBA8_UNorm, EImageAspect::Color };
        ImageMemView    image_b{ buf_b, uint3{}, dim, pitch, 0_b, EPixelFormat::RGBA8_UNorm, EImageAspect::Color };

        TEST( image_a.RowPitch() == pitch );
        TEST( image_b.RowPitch() == pitch );
        TEST( image_a.BitsPerBlock() == 4*8 );

        Bytes   d;
        TEST( image_a.CopyFrom( image_b, OUT d ));
        TEST( d == ArraySizeOf(data_a) );
        TEST( data_a == data_b );
    }


    static void  ImageMemView_Test4 ()
    {
        const uint3     dim     {32, 32, 1};
        const Bytes     bpp     = 4_b;
        const Bytes     pitch   = dim.x * bpp;

        Array<ubyte>    data;   data.resize( usize( pitch * dim.y ));
        RWImageMemView  image   { BufferMemView{data}, uint3{}, dim, pitch, 0_b, EPixelFormat::RGBA8_UNorm, EImageAspect::Color };

        RGBA32f     a0{ 0.2f, 0.6f, 0.9f, 1.0f };
        image.Store( uint3{0,0,0}, a0 );

        RGBA32f     b0;
        image.Load( uint3{0,0,0}, OUT b0 );
        TEST( All( Equals( a0, b0, 0.002f )));

        RGBA32u     c0;
        image.Load( uint3{0,0,0}, OUT c0 );
        TEST( All( Equals( RGBA32u{ 0x33, 0x99, 0xE5, 0xFF }, c0 )));


        RGBA32f     a1{ -0.5f, 0.8f, 0.3f, 0.0f };
        image.Store( uint3{1,1,0}, a1 );

        RGBA32f     b1;
        image.Load( uint3{1,1,0}, OUT b1 );
        TEST( All( Equals( Max( RGBA32f{}, a1 ), b1, 0.002f )));

        RGBA32u     c1;
        image.Load( uint3{1,1,0}, OUT c1 );
        TEST( All( Equals( RGBA32u{ 0x00, 0xCC, 0x4C, 0x00 }, c1 )));
    }


    static void  ImageMemView_Test5 ()
    {
        const uint3     dim     {32, 32, 1};
        const Bytes     bpp     = 16_b;
        const Bytes     pitch   = dim.x * bpp;

        Array<ubyte>    data;   data.resize( usize( pitch * dim.y ));
        RWImageMemView  image   { BufferMemView{data}, uint3{}, dim, pitch, 0_b, EPixelFormat::RGBA32F, EImageAspect::Color };

        RGBA32f     a0{ 0.2f, 0.6f, 0.9f, 1.0f };
        image.Store( uint3{0,0,0}, a0 );

        RGBA32f     b0;
        image.Load( uint3{0,0,0}, OUT b0 );
        TEST( All( Equals( a0, b0 )));

        RGBA32u     c0;
        image.Load( uint3{0,0,0}, OUT c0 );
        TEST( All( Equals( RGBA32u{ 0x3e4ccccd, 0x3f19999a, 0x3f666666, 0x3f800000 }, c0 )));


        RGBA32f     a1{ -0.5f, 0.8f, 0.3f, 1000.0f };
        image.Store( uint3{1,1,0}, a1 );

        RGBA32f     b1;
        image.Load( uint3{1,1,0}, OUT b1 );
        TEST( All( Equals( a1, b1 )));

        RGBA32u     c1;
        image.Load( uint3{1,1,0}, OUT c1 );
        TEST( All( Equals( RGBA32u{ 0xbf000000, 0x3f4ccccd, 0x3e99999a, 0x447a0000 }, c1 )));
    }


    static void  ImageMemView_Test6 ()
    {
        const uint3     dim     {32, 32, 1};
        const Bytes     bpp     = 8_b;
        const Bytes     pitch   = dim.x * bpp;

        Array<ubyte>    data;   data.resize( usize( pitch * dim.y ));
        RWImageMemView  image   { BufferMemView{data}, uint3{}, dim, pitch, 0_b, EPixelFormat::RGBA16F, EImageAspect::Color };

        RGBA32f     a0{ 0.2f, 0.6f, 0.9f, 1.0f };
        image.Store( uint3{0,0,0}, a0 );

        RGBA32f     b0;
        image.Load( uint3{0,0,0}, OUT b0 );
        TEST( All( Equals( a0, b0, 0.001f )));

        RGBA32u     c0;
        image.Load( uint3{0,0,0}, OUT c0 );
        TEST( All( Equals( RGBA32u{ 0x3266, 0x38CD, 0x3B33, 0x3C00 }, c0 )));


        RGBA32f     a1{ -0.5f, 0.8f, 0.3f, 100.0f };
        image.Store( uint3{1,1,0}, a1 );

        RGBA32f     b1;
        image.Load( uint3{1,1,0}, OUT b1 );
        TEST( All( Equals( a1, b1, 0.001f )));

        RGBA32u     c1;
        image.Load( uint3{1,1,0}, OUT c1 );
        TEST( All( Equals( RGBA32u{ 0xB800, 0x3A66, 0x34CD, 0x5640 }, c1 )));
    }


    static void  ImageMemView_Test7 ()
    {
        const uint3     dim     {32, 32, 1};
        const Bytes     bpp     = 4_b;
        const Bytes     pitch   = dim.x * bpp;

        Array<ubyte>    data;   data.resize( usize( pitch * dim.y ));
        RWImageMemView  image   { BufferMemView{data}, uint3{}, dim, pitch, 0_b, EPixelFormat::RGBA8_SNorm, EImageAspect::Color };

        RGBA32f     a0{ 0.2f, 0.6f, 0.9f, 1.0f };
        image.Store( uint3{0,0,0}, a0 );

        RGBA32f     b0;
        image.Load( uint3{0,0,0}, OUT b0 );
        TEST( All( Equals( a0, b0, 0.01f )));

        RGBA32u     c0;
        image.Load( uint3{0,0,0}, OUT c0 );
        TEST( All( Equals( RGBA32u{ 0x19, 0x4C, 0x73, 0x7F }, c0 )));


        RGBA32f     a1{ -0.2f, -0.6f, -0.9f, -1.0f };
        image.Store( uint3{2,2,0}, a1 );

        RGBA32f     b1;
        image.Load( uint3{2,2,0}, OUT b1 );
        TEST( All( Equals( a1, b1, 0.01f )));

        RGBA32u     c1;
        image.Load( uint3{2,2,0}, OUT c1 );
        TEST( All( Equals( RGBA32u{ 0xE6, 0xB3, 0x8C, 0x80 }, c1 )));
    }
}


extern void UnitTest_ImageMemView ()
{
    ImageMemView_Test1();
    ImageMemView_Test2();
    ImageMemView_Test3();
    ImageMemView_Test4();
    ImageMemView_Test5();
    ImageMemView_Test6();
    ImageMemView_Test7();

    TEST_PASSED();
}
