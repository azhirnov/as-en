// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Should be used for sequential tasks with different execution time, like a rendering on the GPU.

    | c c c   |
    | b c c c | -- [time] cccc
    | a c c b | -- [time] bbbbb
    | a a b b | /
    | a a a a | -- [time] aaaaa
*/

#pragma once

#ifdef AE_ENABLE_IMGUI
# include "profiler/ImGui/Common.h"

namespace AE::Profiler
{

    //
    // ImGui Column History Diagram
    //

    class ImColumnHistoryDiagram
    {
    // types
    private:
        enum class UniqueNameIdx : uint {};     // index in '_uniqueNameArr'

        using UNameInfo_t   = Tuple< UniqueNameIdx, RGBA8u, String >;
        using UniqueNames_t = HashMap< StringView, UniqueNameIdx >;

        struct Item
        {
            UniqueNameIdx   name;
            RGBA8u          color;      // same as in '_uniqueNameArr'
            double          begin;      // nanoseconds
            double          end;
        };

        struct FrameData
        {
            Array<Item>     items;
            double          min     = 0.0;      // nanoseconds
            double          max     = 0.0;

            ND_ double  Height ()   const   { return max - min; }
        };

        static constexpr int    _HistorySize    = 100;  // in frames

        using FrameHistory_t    = StaticArray< FrameData, _HistorySize >;
        using MaxTimeHistory_t  = RingBuffer< double >;


    // variables
    private:
        mutable SharedMutex     _guard;

        double                  _maxHeight          = 0.0;
        int                     _currentFrameIdx    = 0;

        UniqueNames_t           _uniqueNames;
        Array<UNameInfo_t>      _uniqueNameArr;
        FrameHistory_t          _frames;


    // methods
    public:
        void  Draw (INOUT RectF &region) const;

        void  Begin ();
        void  Add (const String &name, RGBA8u color, double begin, double end);
        void  End (double min, double max);
        void  End ();
    };


} // AE::Profiler

#endif // AE_ENABLE_IMGUI
