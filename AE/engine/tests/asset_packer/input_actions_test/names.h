namespace InputActions
{
    using Hash_t          = AE::Base::HashVal32;
    using InputModeName_t = AE::App::InputModeName;

    static constexpr struct _UI
    {
        constexpr operator InputModeName_t () const { return InputModeName_t{Hash_t{0x891542dau}}; }  // 'UI'

        static constexpr struct _Desktop {
            static constexpr uint  actionCount = 5;
            enum Bindings : uint {
                Back  = 0xcdfc6e09u,  // InputActionName{"Back"}
                Cursor  = 0xc2d36fb8u,  // InputActionName{"Cursor"}
                Enter  = 0x78b1ef6au,  // InputActionName{"Enter"}
                Options  = 0x1f88c31bu,  // InputActionName{"Options"}
                Push  = 0xff08b95au,  // InputActionName{"Push"}
            };
        } Desktop;
    } UI;

}
