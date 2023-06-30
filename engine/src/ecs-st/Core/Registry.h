// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "ecs-st/Core/ArchetypeStorage.h"
#include "ecs-st/Core/EntityPool.h"
#include "ecs-st/Core/MessageBuilder.h"
#include "ecs-st/Core/ComponentAccessTypes.h"

namespace AE::ECS
{

    //
    // System Event helpers
    //

    template <typename T>
    struct AfterEvent {};

    template <typename T>
    struct BeforeEvent {};



    //
    // Registry
    //

    class Registry final : public EnableRC< Registry >
    {
    // types
    public:
        DEBUG_ONLY(
            using IComponentDbgView = ArchetypeStorage::IComponentDbgView;
            using CompDbgView_t     = ArchetypeStorage::CompDbgView_t;
        )

        struct ComponentInfo
        {
            using Ctor_t = void (*) (void *);

            Ctor_t      ctor        = null;
            Bytes16u    align;
            Bytes16u    size;
            bool        created     = false;

            DEBUG_ONLY(
                using DbgView_t = Unique<IComponentDbgView> (*) (void *, usize);
                DbgView_t       dbgView     = null;
            )

            ND_ bool    IsTag ()    C_NE___ { return size == 0; }
            ND_ bool    HasData ()  C_NE___ { return size > 0; }
        };


    private:
        using ComponentMap_t        = Unique< StaticArray< ComponentInfo, ECS_Config::MaxComponents >>;

        using ArchetypeStoragePtr   = Unique< ArchetypeStorage >;
        using ArchetypeMap_t        = HashMap< Archetype, ArchetypeStoragePtr >;    // require pointer stability
        using Index_t               = ArchetypeStorage::Index_t;
        using ArchetypePair_t       = Pair< const Archetype, ArchetypeStoragePtr >;

        class SingleCompWrap final : public Noncopyable
        {
        public:
            void*   data            = null;
            void(*  deleter)(void*) = null;

            SingleCompWrap () {}

            ~SingleCompWrap ()
            {
                if ( data and deleter )
                    deleter( data );
            }
        };

        using SingleCompMap_t   = HashMap< TypeId, SingleCompWrap >;
        using SCAllocator_t     = UntypedAllocator;

        using EventListener_t   = Function< void (Registry &) >;
        using EventListeners_t  = HashMultiMap< TypeId, EventListener_t >;
        using EventQueue_t      = Array< Function< void () >>;

        struct Query
        {
            ArchetypeQueryDesc          desc;
            Array<ArchetypePair_t *>    archetypes;
            mutable bool                locked  = true;
        };
        using Queries_t         = Array< Query >;


    // variables
    private:
        // entity + components
        EntityPool          _entities;
        ArchetypeMap_t      _archetypes;        // don't erase elements!
        MessageBuilder      _messages;
        ComponentMap_t      _componentInfo;

        // single components
        SingleCompMap_t     _singleComponents;
        //SCAllocator_t     _scAllocator;       // TODO

        EventListeners_t    _eventListeners;

        EventQueue_t        _eventQueue;
        EventQueue_t        _pendingEvents;

        Queries_t           _queries;

        DRC_ONLY(
            DataRaceCheck   _drCheck;
        )


    // methods
    public:
        Registry ()                                                                     __Th___;
        ~Registry ()                                                                    __NE___;


        // entity
            template <typename ...Components>
            EntityID    CreateEntity ()                                                 __NE___;
            template <typename ...Components>
            EntityID    CreateEntity (Components&& ...comps)                            __NE___;
        ND_ EntityID    CreateEntity ()                                                 __NE___;
            bool        DestroyEntity (EntityID entId)                                  __NE___;
            void        DestroyAllEntities ()                                           __NE___;

        ND_ Ptr<Archetype const>  GetArchetype (EntityID entId)                         __NE___;

        // component
            template <typename T>
            void  RegisterComponent ()                                                  __NE___;

            template <typename ...Types>
            void  RegisterComponents ()                                                 __NE___;

        ND_ Ptr<ComponentInfo const>  GetComponentInfo (ComponentID compId)             C_NE___;

            template <typename T>
            EnableIf< not IsEmpty<T>, T& >  AssignComponent (EntityID entId)            __NE___;

            template <typename T>
            EnableIf< IsEmpty<T>, void >  AssignComponent (EntityID entId)              __NE___;

            template <typename T>
            bool  RemoveComponent (EntityID entId)                                      __NE___;
            bool  RemoveComponent (EntityID entId, ComponentID compId)                  __NE___;

            template <typename T>
        ND_ Ptr<T>  GetComponent (EntityID entId)                                       __NE___;

            template <typename T>
        ND_ Ptr<AddConst<T>>  GetComponent (EntityID entId)                             C_NE___;

            template <typename ...Types>
        ND_ Tuple<Ptr<Types>...>  GetComponents (EntityID entId)                        __NE___;

            template <typename ...Types>
        ND_ Tuple<Ptr<AddConst<Types>>...>  GetComponents (EntityID entId)              C_NE___;

            template <typename ...Types>
            void  RemoveComponents (QueryID query)                                      __NE___;
            void  RemoveComponents (QueryID query, const ArchetypeDesc &removeComps)    __NE___;

        DEBUG_ONLY(
         ND_ CompDbgView_t  EntityDbgView (EntityID entId)                              C_NE___;
        )


        // single component
            template <typename T>
            T&      AssignSingleComponent ()                                            __NE___;

            template <typename T>
            bool    RemoveSingleComponent ()                                            __NE___;

            template <typename T>
        ND_ Ptr<T>  GetSingleComponent ()                                               __NE___;

            void    DestroyAllSingleComponents ()                                       __NE___;


        // system
            template <typename ...Args>
            ND_ QueryID  CreateQuery ()                                                 __NE___;
            ND_ QueryID  CreateQuery (const ArchetypeQueryDesc &desc)                   __NE___;

            template <typename Fn>
            void  Execute (QueryID query, Fn &&fn)                                      __NE___;

            template <typename Fn>
            void  Enque (QueryID query, Fn &&fn)                                        __NE___;

            template <typename Obj, typename Class, typename ...Args>
            void  Enque (QueryID query, Obj obj, void (Class::*)(Args&&...))            __NE___;

            template <typename Ev>
            void  EnqueEvent ()                                                         __NE___;

            void  Process ()                                                            __NE___;

            template <typename Ev, typename Fn>
            void  AddEventListener (Fn &&fn)                                            __NE___;

            template <typename Comp, typename Tag, typename Fn>
            bool  AddMessageListener (Fn &&fn)                                          __NE___;

            template <typename Tag>
            bool  AddMessage (EntityID entId, ComponentID compId)                       __NE___;

            template <typename Tag, typename Comp>
            bool  AddMessage (EntityID entId, const Comp& comp)                         __NE___;


    private:
            template <typename Ev>
            void  _RunEvent ()                                                          __NE___;

            bool  _RemoveEntity (EntityID entId)                                                                            __NE___;
            void  _AddEntity (const Archetype &arch, EntityID entId, OUT ArchetypeStorage* &storage, OUT Index_t &index)    __NE___;
            void  _AddEntity (const Archetype &arch, EntityID entId)                                                        __NE___;
            void  _MoveEntity (const Archetype &arch, EntityID entId, ArchetypeStorage* srcStorage, Index_t srcIndex,
                               OUT ArchetypeStorage* &dstStorage, OUT Index_t &dstIndex)                                    __NE___;

            void  _OnNewArchetype (ArchetypePair_t *)                                                       __NE___;

            static void  _IncreaseStorageSize (ArchetypeStorage *, usize addCount)                          __NE___;
            static void  _DecreaseStorageSize (ArchetypeStorage *)                                          __NE___;

            template <typename ArgsList, usize I>
        ND_ static bool  _IsArchetypeSupported (const Archetype &arch)                                      __NE___;

            template <typename ...Args>
        ND_ static Tuple<usize, Args...>  _GetChunk (ArchetypeStorage* storage, const TypeList<Args...> *)  __NE___;

            template <typename Fn, typename Chunk, typename ...Types>
            void  _WithSingleComponents (Fn &&fn, ArrayView<Chunk> chunks, const Tuple<Types...> *)         __NE___;

            template <typename T>
        ND_ decltype(auto)  _GetSingleComponent ()                                                          __NE___;


            template <typename Fn>
            void  _Execute_v1 (QueryID query, Fn &&fn)                                                      __NE___;

            template <typename Fn, typename ...Args>
            void  _Execute_v2 (QueryID query, Fn &&fn, const TypeList<Args...>*)                            __NE___;
    };


} // AE::ECS

#include "ecs-st/Core/Registry.inl.h"
