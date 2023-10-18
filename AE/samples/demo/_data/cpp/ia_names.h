namespace InputActions
{
    using Hash_t          = AE::Base::HashVal32;
    using InputModeName_t = AE::App::InputModeName;

    static constexpr struct _Camera3D
    {
        constexpr operator InputModeName_t () const { return InputModeName_t{Hash_t{0x7b09036u}}; }  // 'Camera3D'

        static constexpr uint  actionCount = 1;
        static constexpr uint  Camera_Rotate  = 0xcc3b70a5u;  // InputActionName{"Camera.Rotate"}
        static constexpr struct _Desktop {
            static constexpr uint  actionCount = 1;
            static constexpr uint  Camera_Move  = 0x87c9a2a2u;  // InputActionName{"Camera.Move"}
        } Desktop;
        static constexpr struct _VR {
            static constexpr uint  actionCount = 1;
            static constexpr uint  Camera_Move  = 0x87c9a2a2u;  // InputActionName{"Camera.Move"}
        } VR;
    } Camera3D;

    static constexpr struct _Canvas2D
    {
        constexpr operator InputModeName_t () const { return InputModeName_t{Hash_t{0xe271b1e4u}}; }  // 'Canvas2D'

        static constexpr uint  actionCount = 2;
        static constexpr uint  Cursor  = 0xc2d36fb8u;  // InputActionName{"Cursor"}
        static constexpr uint  Enter  = 0x78b1ef6au;  // InputActionName{"Enter"}
    } Canvas2D;

    static constexpr struct _SampleSelector
    {
        constexpr operator InputModeName_t () const { return InputModeName_t{Hash_t{0xc682d438u}}; }  // 'SampleSelector'

    } SampleSelector;

    static constexpr struct _imGUI
    {
        constexpr operator InputModeName_t () const { return InputModeName_t{Hash_t{0x2655069fu}}; }  // 'imGUI'

        static constexpr uint  actionCount = 6;
        static constexpr uint  Test_DoubleClick  = 0x5b940a39u;  // InputActionName{"Test.DoubleClick"}
        static constexpr uint  Test_Down  = 0xfff75e16u;  // InputActionName{"Test.Down"}
        static constexpr uint  Test_LongPress  = 0x3549fbe0u;  // InputActionName{"Test.LongPress"}
        static constexpr uint  Test_Move  = 0xc36f955u;  // InputActionName{"Test.Move"}
        static constexpr uint  Test_ScaleRotate2D  = 0xccc2a002u;  // InputActionName{"Test.ScaleRotate2D"}
        static constexpr uint  Touch_Click  = 0x5fb61d88u;  // InputActionName{"Touch.Click"}
        static constexpr struct _Desktop {
            static constexpr uint  actionCount = 5;
            static constexpr uint  MouseLBDown  = 0xd0f2e875u;  // InputActionName{"MouseLBDown"}
            static constexpr uint  MousePos  = 0x4f44c5bu;  // InputActionName{"MousePos"}
            static constexpr uint  MouseWheel  = 0xc2e16d5u;  // InputActionName{"MouseWheel"}
            static constexpr uint  Test_Click  = 0xffca969bu;  // InputActionName{"Test.Click"}
            static constexpr uint  Test_Hold  = 0xfcc2c32au;  // InputActionName{"Test.Hold"}
        } Desktop;
        static constexpr struct _VR {
            static constexpr uint  actionCount = 5;
            static constexpr uint  MouseLBDown  = 0xd0f2e875u;  // InputActionName{"MouseLBDown"}
            static constexpr uint  MousePos  = 0x4f44c5bu;  // InputActionName{"MousePos"}
            static constexpr uint  MouseWheel  = 0xc2e16d5u;  // InputActionName{"MouseWheel"}
            static constexpr uint  Test_Click  = 0xffca969bu;  // InputActionName{"Test.Click"}
            static constexpr uint  Test_Hold  = 0xfcc2c32au;  // InputActionName{"Test.Hold"}
        } VR;
    } imGUI;

}
