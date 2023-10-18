namespace InputActions
{
    using Hash_t          = AE::Base::HashVal32;
    using InputModeName_t = AE::App::InputModeName;

    static constexpr struct _UI
    {
        constexpr operator InputModeName_t () const { return InputModeName_t{Hash_t{0x891542dau}}; }  // 'UI'

        static constexpr struct _Desktop {
            static constexpr uint  actionCount = 5;
            static constexpr uint  Back  = 0xcdfc6e09u;  // InputActionName{"Back"}
            static constexpr uint  Cursor  = 0xc2d36fb8u;  // InputActionName{"Cursor"}
            static constexpr uint  Enter  = 0x78b1ef6au;  // InputActionName{"Enter"}
            static constexpr uint  Options  = 0x1f88c31bu;  // InputActionName{"Options"}
            static constexpr uint  Push  = 0xff08b95au;  // InputActionName{"Push"}
        } Desktop;
        static constexpr struct _VR {
            static constexpr uint  actionCount = 5;
            static constexpr uint  Back  = 0xcdfc6e09u;  // InputActionName{"Back"}
            static constexpr uint  Cursor  = 0xc2d36fb8u;  // InputActionName{"Cursor"}
            static constexpr uint  Enter  = 0x78b1ef6au;  // InputActionName{"Enter"}
            static constexpr uint  Options  = 0x1f88c31bu;  // InputActionName{"Options"}
            static constexpr uint  Push  = 0xff08b95au;  // InputActionName{"Push"}
        } VR;
    } UI;

}
