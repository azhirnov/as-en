// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "threading/TaskSystem/ThreadManager.h"
#include "UnitTest_Common.h"

namespace
{
    class MsgProducer final :
        public AsyncCSMessageProducer< LfLinearAllocator< usize{32_Mb}, AE_CACHE_LINE, 16 >>
    {
    public:
        EnumBitSet<EChannel>  GetChannels ()    C_NE_OV { return {EChannel::Reliable}; }
    };


    static void  GenRandomString (OUT char* dst, uint len, Random &rnd)
    {
        for (uint i = 0; i < len; ++i) {
            dst[i] = char(0xFF); //char(rnd.Uniform( uint('a'), uint('z') ));
        }
    }


    static void  AsyncCSMessageProducer_Test1 ()
    {
        MsgProducer     msg_prod;

        const auto      ThreadFn = [&msg_prod] ()
        {{
            Random  rnd;
            for (;;)
            {
                uint    len = rnd.Uniform( 3u, 200u );

                auto    msg = msg_prod.CreateMsg< CSMsg_Log >( Bytes{len} );
                if ( msg )
                {
                    GenRandomString( msg.Extra<char>(), len, rnd );
                    msg->msg = StringView{ msg.Extra<char>(), len };

                    if ( not msg_prod.AddMessage( msg ))
                        break;
                }else
                    break;
            }
        }};

        StaticArray< StdThread, 4 >     threads;

        for (auto& t : threads)
        {
            t = StdThread{ ThreadFn };
        }

        for (auto& t : threads)
        {
            t.join();
        }
    }
}


extern void UnitTest_AsyncCSMessageProducer ()
{
    AsyncCSMessageProducer_Test1();

    TEST_PASSED();
}
