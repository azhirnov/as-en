// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Thread-safe: no
*/

#pragma once

#include "hulang/Common.h"

#include "base/Containers/FixedArray.h"
#include "base/Memory/LinearAllocator.h"

#include "threading/Primitives/SpinLock.h"
#include "threading/Primitives/DataRaceCheck.h"

namespace AE::HuLang
{

    //
    // Console
    //

    class Console
    {
    // types
    public:
        struct ShortSuggestionItem
        {
            uint    depth   : 4;
            uint    index   : 28;

            ShortSuggestionItem () : depth{0}, index{0} {}
            ShortSuggestionItem (uint depth, uint index) : depth{depth}, index{index} {}
        };
        using ShortSuggestion_t = Array< ShortSuggestionItem >;


        struct SuggestionItem
        {
            StringView                      name;
            ConsoleCmdName::Optimized_t     command;
            uint                            depth       : 4;
            uint                            index       : 28;
            float                           probability;        // 0..1

            SuggestionItem () : depth{0}, index{0}, probability{0.f} {}
        };
        using Suggestion_t  = Array< SuggestionItem >;


        using CommandName_t = FixedArray< StringView, 8 >;

        enum class EIndex : uint
        {
            NSBit       = 0u << 30,             // in '_nsArr'
            ActionBit   = 1u << 30,             // in '_actions'
            Synonym     = 1u << 31,
            _Mask       = ActionBit | Synonym,
        };

    private:
        using SynonymsArr_t = FixedArray< EIndex, 16 >;
        using NameMap_t     = HashMap< StringView, SynonymsArr_t >;

        struct NameSpace
        {
            const usize     index;
            NameMap_t       map;    // registred names with synonyms

            explicit NameSpace (usize idx) : index{idx} {}
        };
        using NameSpaceMap_t    = HashMap< StringView, Ptr<NameSpace> >;

        struct CommandInfo
        {
            ConsoleCmdName::Optimized_t     command;
            uint                            depth   = 0;
            StringView                      name;           // stored in '_allocator'

            CommandInfo (ConsoleCmdName::Optimized_t    command,
                         usize                          depth,
                         StringView                     name) :
                command{command}, depth{uint(depth)}, name{name}
            {}
        };
        using CommandArr_t      = Array< CommandInfo >;
        using NameSpaceArr_t    = Array< Unique<NameSpace> >;

        using Allocator_t       = LinearAllocator< UntypedAllocator, 8, false >;


    // variables
    private:
        NameSpaceMap_t      _nsMap;

        NameSpaceArr_t      _nsArr;
        CommandArr_t        _commands;

        Allocator_t         _allocator;

        DRC_ONLY(
            Threading::RWDataRaceCheck  _drCheck;
        )


    // methods
    public:
        Console ();

            bool  Register (StringView name, const ConsoleCmdName &command);
        ND_ bool  IsRegistred (StringView name) const;
            void  Clear ();

        ND_ bool  GetSuggestion (StringView userInput, OUT ShortSuggestion_t &result) const;
        ND_ bool  GetSuggestion (StringView userInput, OUT Suggestion_t &result) const;

        ND_ static bool  ProcessUserInput (StringView userInput, OUT CommandName_t &result);

    private:
        ND_ bool  _GetSuggestion (const CommandName_t &userInput, const CommandName_t &userInputLC, OUT ShortSuggestion_t &result) const;
        ND_ bool  _FindSynonyms (const NameMap_t &map, StringView name, usize depth, OUT ShortSuggestion_t &result) const;

            static bool  _InsertSynonym (SynonymsArr_t &arr, EIndex idx);
            static bool  _AddSynonyms (Allocator_t &alloc, NameMap_t &map, StringView name, StringView nameLC, EIndex idx);
        ND_ static bool  _CopyString (Allocator_t &alloc, INOUT StringView &name);
        ND_ static bool  _CopyStringLowerCase (Allocator_t &alloc, INOUT StringView &name);
        ND_ static bool  _ProcessUserInput (StringView userInput, StringView userInputLC, OUT CommandName_t &result, OUT CommandName_t &resultLC);

        ND_ static bool  _IsLowerCase (StringView str);

        ND_ static StringView  _CutName (StringView fullName, usize depth);
    };


} // AE::HuLang
