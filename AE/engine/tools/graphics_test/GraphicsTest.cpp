// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/DataSource/FileStream.h"
#include "GraphicsTest.h"
#include "res_loaders/DDS/DDSImageLoader.h"
#include "res_loaders/DDS/DDSImageSaver.h"

namespace AE::GraphicsTest
{
    using namespace AE::ResLoader;


/*
=================================================
    destructor
=================================================
*/
    ImageComparator::~ImageComparator ()
    {
        EXLOCK( _guard );
        if ( _newRef )
        {
            FileWStream file{ _fname };
            if ( file.IsOpen() )
            {
                DDSImageSaver   saver;
                saver.SaveImage( file, _image );
            }
        }
    }

/*
=================================================
    LoadReference
=================================================
*/
    bool  ImageComparator::LoadReference (Path imgName)
    {
        EXLOCK( _guard );

        _fname  = RVRef(imgName);
        _loaded = false;
        _newRef = false;

        if ( FileSystem::IsFile( _fname ))
        {
            FileRStream     file{ _fname };
            if ( file.IsOpen() )
            {
                DDSImageLoader  loader;
                _loaded = loader.LoadImage( OUT _image, file, False{"don't flipY"}, null, Default );

                CHECK_ERR( _image.MipLevels() == 1 );
                CHECK_ERR( _image.ArrayLayers() == 1 );
            }
        }
        return _loaded;
    }

/*
=================================================
    Compare
=================================================
*/
    bool  ImageComparator::Compare (const ImageMemView &view)
    {
        EXLOCK( _guard );

        if ( _loaded )
        {
            CHECK_ERR_MSG( view == _image.ToView( 0_mipmap, 0_layer ),
                "Image is not equal to reference '"s << ToString(_fname.filename()) << "'" );
        }
        else
        {
            _newRef = true;
            _loaded = true;
            CHECK_ERR( _image.Copy( view ));
        }
        return true;
    }

} // AE::GraphicsTest
