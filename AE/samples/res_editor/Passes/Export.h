// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Passes/IPass.h"

namespace AE::ResEditor
{

    //
    // Image Export pass
    //

    class ExportImage final : public IPass
    {
    // variables
    private:
        RC<Image>       _src;
        RC<Image>       _temp;
        const Path      _filePath;
        Path            _currPath;
        uint            _frameId    = UMax;


    // methods
    public:
        ExportImage (RC<Image> src, const Path &path) : _src{RVRef(src)}, _filePath{path} {}

    // IPass //
        EPassType   GetType ()                                          C_NE_OV { return EPassType::Export; }
        bool        Update (TransferCtx_t &, const UpdatePassData &)    __Th_OV;
        void        GetResourcesToResize (INOUT Array<RC<IResource>> &) __NE_OV {}
    };



    //
    // Buffer Export pass
    //

    class ExportBuffer final : public IPass
    {
    // types
    public:
        using ParserFn_t = Function< void (const BufferMemView &, WStream &) >;

    // variables
    private:
        RC<Buffer>      _src;
        const Path      _filePath;
        uint            _frameId    = UMax;
        Atomic<bool>    _complete   {false};
        ParserFn_t      _parser;


    // methods
    public:
        ExportBuffer (RC<Buffer> src, const Path &path, ParserFn_t parser) :
            _src{RVRef(src)}, _filePath{path}, _parser{RVRef(parser)} {}

    // IPass //
        EPassType   GetType ()                                          C_NE_OV { return EPassType::Export; }
        bool        Update (TransferCtx_t &, const UpdatePassData &)    __Th_OV;
        void        GetResourcesToResize (INOUT Array<RC<IResource>> &) __NE_OV {}
    };


} // AE::ResEditor
