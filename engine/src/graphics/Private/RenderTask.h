// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#if defined(AE_ENABLE_VULKAN)
#   define CMDBATCH         VCommandBatch

#elif defined(AE_ENABLE_METAL)
#   define CMDBATCH         MCommandBatch

#elif defined(AE_ENABLE_REMOTE_GRAPHICS)
#   define CMDBATCH         RCommandBatch

#else
#   error not implemented
#endif

namespace AE::Graphics
{

    //
    // Render Task interface
    //
    class RenderTask : public Threading::IAsyncTask
    {
        friend class CMDBATCH;

    // variables
    private:
        RC<CMDBATCH>        _batch;
        uint                _exeIndex   = UMax;     // execution order index
        bool                _submit     = false;

        DBG_GRAPHICS_ONLY(
            String          _dbgName;
             RGBA8u         _dbgColor;
        )


    // methods
    protected:
        RenderTask (RC<CMDBATCH> batch, DebugLabel dbg) __Th___;

        enum class _DelayedInit {};
        explicit RenderTask (_DelayedInit)              __NE___ :
            IAsyncTask{ ETaskQueue::Renderer }
        {}

        ND_ bool  _Init (RC<CMDBATCH> batch, DebugLabel dbg) __NE___;

    public:
        ~RenderTask ()                              __NE___ { _CancelTaskInBatch(); }

        ND_ RC<CMDBATCH>    GetBatchRC ()           C_NE___ { return _batch; }
        ND_ Ptr<CMDBATCH>   GetBatchPtr ()          C_NE___ { return _batch.get(); }
        ND_ FrameUID        GetFrameId ()           C_NE___ { return _batch->GetFrameId(); }
        ND_ uint            GetExecutionIndex ()    C_NE___ { return _exeIndex; }
        ND_ bool            IsValid ()              C_NE___ { return _exeIndex != UMax; }
        ND_ EQueueType      GetQueueType ()         C_NE___ { return _batch->GetQueueType(); }
        ND_ EQueueMask      GetQueueMask ()         C_NE___ { return EQueueMask(0) | GetQueueType(); }
        ND_ bool            IsFirstInBatch ()       C_NE___ { return _exeIndex == 0; }
        ND_ bool            IsLastInBatch ()        C_NE___;

        template <typename CmdBufType>
        void  Execute (CmdBufType &cmdbuf)          __Th___;


    // IAsyncTask
    public:
            void            OnCancel ()             __NE_OV { _CancelTaskInBatch();  IAsyncTask::OnCancel(); }

      #if AE_DBG_GRAPHICS
        ND_ String          DbgFullName ()          C_NE___;
        ND_ StringView      DbgName ()              C_NE_OF { return _dbgName; }
        ND_ RGBA8u          DbgColor ()             C_NE___ { return _dbgColor; }
      #else
        ND_ String          DbgFullName ()          C_NE___ { return Default; }
        ND_ StringView      DbgName ()              C_NE_OF { return Default; }
        ND_ RGBA8u          DbgColor ()             C_NE___ { return DebugLabel::ColorTable::GraphicsQueue; }
      #endif


    protected:
        void  OnFailure ()                          __NE___ { _CancelTaskInBatch();  IAsyncTask::OnFailure(); }


    private:
        ND_ CMDBATCH::CmdBufPool&  _GetPool ()      __NE___ { return _batch->_cmdPool; }

            void  _CancelTaskInBatch ()             __NE___;

      #if AE_DBG_GRAPHICS
            void  _DbgCheckFrameId ()               C_NE___;

        ND_ static RGBA8u  _ValidateDbgColor (EQueueType queue, RGBA8u color);
      #endif
    };



    //
    // Command Batch on Submit dependency
    //
    struct CmdBatchOnSubmit
    {
        RC<CMDBATCH>    ptr;

        CmdBatchOnSubmit ()                             __NE___ {}
        explicit CmdBatchOnSubmit (CMDBATCH &batch)     __NE___ : ptr{batch.GetRC()}    { ASSERT( ptr == null or ptr->_submitMode != ESubmitMode::Deferred ); }
        explicit CmdBatchOnSubmit (CMDBATCH* batch)     __NE___ : ptr{batch}            { ASSERT( ptr == null or ptr->_submitMode != ESubmitMode::Deferred ); }
        explicit CmdBatchOnSubmit (RC<CMDBATCH> batch)  __NE___ : ptr{RVRef(batch)}     { ASSERT( ptr == null or ptr->_submitMode != ESubmitMode::Deferred ); }

        #ifdef AE_HAS_COROUTINE
        ND_ auto  operator co_await () C_NE___
        {
            // use global operator with 'CoroutineRunnerAwaiter'
            return AE::Threading::operator co_await( Tuple{ *this });
        }
        #endif
    };



    //
    // Render Task Function
    //

    class RenderTaskFn final : public RenderTask
    {
    // types
    public:
        using Func_t    = Function< void (RenderTaskFn &) >;


    // variables
    private:
        Func_t  _fn;


    // methods
    public:
        template <typename Fn>
        RenderTaskFn (Fn && fn, RC<CMDBATCH> batch, DebugLabel dbg) __Th___ :
            RenderTask{ RVRef(batch), dbg },
            _fn{ FwdArg<Fn>(fn) }
        {}

        template <typename CmdBufType>
        void  Execute (CmdBufType &cmdbuf) __Th___
        {
            return RenderTask::Execute( cmdbuf );
        }

    private:
        void  Run () __Th_OV
        {
            return _fn( *this );
        }
    };
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    inline RenderTask::RenderTask (RC<CMDBATCH> batch, DebugLabel dbg) __Th___ :
        IAsyncTask{ ETaskQueue::Renderer },
        _batch{ RVRef(batch) },
        _exeIndex{ _GetPool().Acquire() }
        DBG_GRAPHICS_ONLY(,
            _dbgName{ dbg.label },
            _dbgColor{ _ValidateDbgColor( GetQueueType(), dbg.color )})
    {
        CHECK_THROW( IsValid() );   // command buffer pool overflow
        Unused( dbg );
    }

/*
=================================================
    Execute
=================================================
*/
    template <typename CmdBufType>
    void  RenderTask::Execute (CmdBufType &cmdbuf) __Th___
    {
        ASSERT( IAsyncTask::DbgIsRunning() );       // must be inside 'Run()'
        CHECK_ERRV( IsValid() );

        #if defined(AE_ENABLE_VULKAN)
            _GetPool().Add( INOUT _exeIndex, cmdbuf.EndCommandBuffer() );               // throw

        #elif defined(AE_ENABLE_METAL)
            if constexpr( CmdBufType::IsIndirectContext )
                _GetPool().Add( INOUT _exeIndex, cmdbuf.EndCommandBuffer() );           // throw
            else
                _GetPool().Add( INOUT _exeIndex, cmdbuf.EndCommandBuffer().Release() ); // throw

        #elif defined(AE_ENABLE_REMOTE_GRAPHICS)
            _GetPool().Add( INOUT _exeIndex, cmdbuf.EndCommandBuffer() );               // throw

        #else
        #   error not implemented
        #endif

        ASSERT( not IsValid() );
        ASSERT( not GetBatchPtr()->IsSubmitted() );

        if ( _submit )
        {
            _submit = false;
            CHECK_THROW( GetBatchPtr()->_Submit() );    // throw
        }

        DBG_GRAPHICS_ONLY( _DbgCheckFrameId();)
    }

/*
=================================================
    _CancelTaskInBatch
=================================================
*/
    inline void  RenderTask::_CancelTaskInBatch () __NE___
    {
        if_likely( IsValid() )
        {
            _GetPool().Complete( INOUT _exeIndex );
            ASSERT( not IsValid() );
        }

        if ( _submit )
        {
            _submit = false;
            CHECK( GetBatchPtr()->_Submit() );
        }
    }

/*
=================================================
    IsLastInBatch
=================================================
*/
    inline bool  RenderTask::IsLastInBatch () C_NE___
    {
        const uint  count = _batch->PendingCmdBufs();
        ASSERT( _exeIndex < count );
        return _exeIndex+1 == count;
    }

/*
=================================================
    IsLastInBatch
=================================================
*/
    inline bool  RenderTask::_Init (RC<CMDBATCH> batch, DebugLabel dbg) __NE___
    {
        _batch      = RVRef(batch);
        _exeIndex   = _GetPool().Acquire();

        DBG_GRAPHICS_ONLY(
            _dbgName    = dbg.label;
            _dbgColor   = _ValidateDbgColor( GetQueueType(), dbg.color );
        )
        return IsValid();
    }

/*
=================================================
    _ValidateDbgColor
=================================================
*/
#if AE_DBG_GRAPHICS
    inline RGBA8u  RenderTask::_ValidateDbgColor (EQueueType queue, RGBA8u color)
    {
        if ( color == DebugLabel::ColorTable::Undefined )
        {
            BEGIN_ENUM_CHECKS();
            switch ( queue ) {
                case EQueueType::Graphics :         return DebugLabel::ColorTable::GraphicsQueue;
                case EQueueType::AsyncCompute :     return DebugLabel::ColorTable::AsyncComputeQueue;
                case EQueueType::AsyncTransfer :    return DebugLabel::ColorTable::AsyncTransfersQueue;
                case EQueueType::VideoEncode :      
                case EQueueType::VideoDecode :
                case EQueueType::Unknown :
                case EQueueType::_Count :           break;
            }
            END_ENUM_CHECKS();
        }
        return color;
    }
#endif

} // AE::Graphics
//-----------------------------------------------------------------------------



# ifdef AE_HAS_COROUTINE
namespace AE::Threading::_hidden_
{

    //
    // Async Render Task Coroutine
    //
    class RenderTaskCoro final
    {
    // types
    public:
        class promise_type;
        using Handle_t  = std::coroutine_handle< promise_type >;

        //
        // promise_type
        //
        class promise_type final : public AE::Graphics::RenderTask
        {
            friend class RenderTaskCoro;

        // methods
        public:
            promise_type ()                                     __NE___ : RenderTask{ _DelayedInit{0} } {}

            ND_ RenderTaskCoro      get_return_object ()        __NE___ { return RenderTaskCoro{ *this }; }

            ND_ std::suspend_always initial_suspend ()          C_NE___ { return {}; }          // delayed start
            ND_ std::suspend_always final_suspend ()            C_NE___ { return {}; }          // must not be 'suspend_never'  // TODO: don't suspend

                void                return_void ()              C_NE___ {}

                void                unhandled_exception ()      C_Th___ { throw; }              // rethrow exceptions

        public:
                void  Cancel ()                                 __NE___ { Unused( RenderTask::_SetCancellationState() ); }
                void  Fail ()                                   __NE___ { RenderTask::OnFailure(); }
            ND_ bool  IsCanceled ()                             __NE___ { return RenderTask::IsCanceled(); }

            template <typename ...Deps>
            void  Continue (const Tuple<Deps...> &deps)         __NE___ { return RenderTask::Continue( deps ); }

            template <typename CmdBufType>
            void  Execute (CmdBufType &cmdbuf)                  __Th___ { return RenderTask::Execute( cmdbuf ); }

            ND_ bool  _Init (RC<AE::Graphics::CMDBATCH> batch,
                             AE::Graphics::DebugLabel   dbg)    __NE___
            {
                return RenderTask::_Init( RVRef(batch), dbg );
            }

        private:
            void  Run ()                                        __Th_OV
            {
                auto    coro_handle = Handle_t::from_promise( *this );
                coro_handle.resume();   // throw

                if_unlikely( bool{coro_handle} and not coro_handle.done() )
                    ASSERT( AnyEqual( Status(), EStatus::Cancellation, EStatus::Continue, EStatus::Failed ));
            }

            void  _ReleaseObject ()                             __NE_OV
            {
                MemoryBarrier( EMemoryOrder::Acquire );
                ASSERT( IsFinished() );

                auto    coro_handle = Handle_t::from_promise( *this );

                // internally calls 'promise_type' dtor
                coro_handle.destroy();
            }
        };


    // variables
    private:
        RC<promise_type>    _coro;


    // methods
    public:
        RenderTaskCoro ()                                       __NE___ {}
        explicit RenderTaskCoro (promise_type &p)               __NE___ : _coro{ p.GetRC<promise_type>() } {}
        explicit RenderTaskCoro (Handle_t handle)               __NE___ : _coro{ handle.promise().GetRC<promise_type>() } {}
        ~RenderTaskCoro ()                                      __NE___ {}

        RenderTaskCoro (RenderTaskCoro &&)                      __NE___ = default;
        RenderTaskCoro (const RenderTaskCoro &)                 __NE___ = default;

        RenderTaskCoro&  operator = (RenderTaskCoro &&)         __NE___ = default;
        RenderTaskCoro&  operator = (const RenderTaskCoro &)    __NE___ = default;

        operator AsyncTask ()                                   C_NE___ { return _coro; }
        explicit operator RC<Graphics::RenderTask> ()           C_NE___ { return _coro; }
        explicit operator bool ()                               C_NE___ { return bool{_coro}; }

        ND_ Graphics::RenderTask&   AsRenderTask ()             __NE___ { return *_coro; }
        ND_ promise_type&           Promise ()                  __NE___ { return *_coro; }
    };



    //
    // Get Render Task Handle
    //
    struct RenderTask_Get
    {
        constexpr RenderTask_Get ()     __NE___ {}

        ND_ auto  operator co_await ()  C_NE___
        {
            using Promise_t     = AE::Threading::_hidden_::RenderTaskCoro::promise_type;
            using RenderTask    = AE::Graphics::RenderTask;

            struct Awaiter
            {
            private:
                RC<RenderTask>  _rtask;

            public:
                ND_ bool            await_ready ()  C_NE___ { return false; }   // call 'await_suspend()' to get coroutine handle
                ND_ RC<RenderTask>  await_resume () __NE___ { return RVRef(_rtask); }

                ND_ bool  await_suspend (std::coroutine_handle< Promise_t > curCoro) __NE___
                {
                    _rtask = curCoro.promise().GetRC<RenderTask>();
                    return false;   // resume coroutine
                }
            };
            return Awaiter{};
        }
    };


    //
    // Get Render Task Handle reference
    //
    struct RenderTask_GetRef
    {
        constexpr RenderTask_GetRef ()  __NE___ {}

        ND_ auto  operator co_await ()  C_NE___
        {
            using Promise_t     = AE::Threading::_hidden_::RenderTaskCoro::promise_type;
            using RenderTask    = AE::Graphics::RenderTask;

            struct Awaiter
            {
            private:
                RenderTask *    _rtask  = null;

            public:
                ND_ bool            await_ready ()  C_NE___ { return false; }   // call 'await_suspend()' to get coroutine handle
                ND_ RenderTask &    await_resume () __NE___ { ASSERT( _rtask != null );  return *_rtask; }

                ND_ bool  await_suspend (std::coroutine_handle< Promise_t > curCoro) __NE___
                {
                    _rtask = &curCoro.promise();
                    return false;   // resume coroutine
                }
            };
            return Awaiter{};
        }
    };

} // AE::Threading::_hidden_


namespace AE::Graphics
{
    using RenderTaskCoro = Threading::_hidden_::RenderTaskCoro;

    static constexpr Threading::_hidden_::RenderTask_Get        RenderTask_Get      {};
    static constexpr Threading::_hidden_::RenderTask_GetRef     RenderTask_GetRef   {};



    //
    // Render Task Add Input Dependency
    //
    struct RenderTask_AddInputDependency
    {
        CMDBATCH const*     ptr = null;

        RenderTask_AddInputDependency (const RenderTask_AddInputDependency &)   __NE___ = default;
        explicit RenderTask_AddInputDependency (CMDBATCH &batch)                __NE___ : ptr{&batch} {}
        explicit RenderTask_AddInputDependency (CMDBATCH* batch)                __NE___ : ptr{batch} {}
        explicit RenderTask_AddInputDependency (const RC<CMDBATCH> &batch)      __NE___ : ptr{batch.get()} {}

        ND_ auto  operator co_await ()                                          C_NE___
        {
            using Promise_t = AE::Threading::_hidden_::RenderTaskCoro::promise_type;

            struct Awaiter
            {
            private:
                CMDBATCH const*     _ptr    = null;

            public:
                explicit Awaiter (CMDBATCH const* ptr) __NE___ : _ptr{ptr} {}

                ND_ bool  await_ready ()    C_NE___ { return _ptr == null; }
                    void  await_resume ()   C_NE___ {}

                ND_ bool  await_suspend (std::coroutine_handle< Promise_t > curCoro) C_NE___
                {
                    auto&   rtask = curCoro.promise();

                    ASSERT( _ptr != null );     // because of 'await_ready()'
                    CHECK( rtask.GetBatchPtr()->AddInputDependency( *_ptr ));

                    return false;   // resume coroutine
                }
            };
            return Awaiter{ ptr };
        }
    };


    //
    // Render Task Execute
    //
    template <typename CmdBufType>
    struct RenderTask_Execute
    {
        CmdBufType &    _cmdbuf;

        explicit RenderTask_Execute (CmdBufType &cmdbuf)__NE___ : _cmdbuf{cmdbuf} {}

        ND_ auto  operator co_await ()                  __NE___
        {
            using Promise_t = AE::Threading::_hidden_::RenderTaskCoro::promise_type;

            struct Awaiter
            {
            private:
                CmdBufType &    _cmdbuf;

            public:
                explicit Awaiter (CmdBufType &cmdbuf)   __NE___ : _cmdbuf{cmdbuf} {}

                ND_ bool    await_ready ()              C_NE___ { return false; }   // call 'await_suspend()' to get coroutine handle
                    void    await_resume ()             C_NE___ {}

                ND_ bool    await_suspend (std::coroutine_handle< Promise_t > curCoro) __Th___
                {
                    auto&   rtask = curCoro.promise();
                    rtask.Execute( _cmdbuf );   // throw
                    return false;               // resume coroutine
                }
            };
            return Awaiter{ _cmdbuf };
        }
    };

} // AE::Graphics
# endif // AE_HAS_COROUTINE
