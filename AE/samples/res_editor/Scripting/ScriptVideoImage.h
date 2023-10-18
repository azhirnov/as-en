// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Scripting/ScriptDynamicVars.h"
#include "res_editor/Resources/VideoImage.h"

namespace AE::ResEditor
{

    //
    // Video Image
    //

    class ScriptVideoImage final : public EnableScriptRC
    {
    // variables
    private:
        EResourceUsage          _resUsage       = Default;
        EPixelFormat            _format         = Default;
        uint                    _imageType      = 0;        // PipelineCompiler::EImageType
        VFS::FileName           _videoFile;
        String                  _dbgName;

        ScriptDynamicDimPtr     _outDynSize;    // image dimension will change this value

        RC<VideoImage>          _resource;


    // methods
    public:
        ScriptVideoImage () {}
        ScriptVideoImage (EPixelFormat format, const String &filename)  __Th___;
        ~ScriptVideoImage ();

        void  Name (const String &name)                                 __Th___;
        void  AddUsage (EResourceUsage usage)                           __Th___;

        ND_ ScriptDynamicDim*   Dimension ()                            __Th___;
        ND_ uint                ImageType ()                            C_NE___ { return _imageType; }


        static void  Bind (const ScriptEnginePtr &se)                   __Th___;

        ND_ RC<VideoImage>  ToResource ()                               __Th___;
    };


} // AE::ResEditor
