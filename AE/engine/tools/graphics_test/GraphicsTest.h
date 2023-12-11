// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics/Public/ImageMemView.h"
#include "res_loaders/Intermediate/IntermImage.h"

namespace AE::GraphicsTest
{
    using namespace AE::Graphics;


    //
    // Image Comparator
    //

    class ImageComparator
    {
    // variables
    private:
        Mutex                   _guard;
        RC<WStream>             _dstFile;
        Path                    _fname;
        bool                    _loaded     = false;
        bool                    _newRef     = false;
        ResLoader::IntermImage  _image;


    // methods
    public:
        ImageComparator () {}
        ~ImageComparator ();

            bool  LoadReference (RC<RStream> imgFile, Path imgName);
            void  Reset (RC<WStream> imgFile, Path imgName);

        ND_ bool  Compare (const ImageMemView &view);
    };


} // AE::GraphicsTest
