namespace InputActions
{
    using Hash_t          = AE::Base::HashVal32;
    using InputModeName_t = AE::App::InputModeName;

    static constexpr struct _Controller_Camera3D
    {
        constexpr operator InputModeName_t () const { return InputModeName_t{Hash_t{0x2a68f4afu}}; }  // 'Controller.Camera3D'

        static constexpr uint  actionCount = 13;
        static constexpr uint  Camera_Move  = 0x87c9a2a2u;  // InputActionName{"Camera.Move"}
        static constexpr uint  Camera_Reset  = 0x36f37312u;  // InputActionName{"Camera.Reset"}
        static constexpr uint  Camera_Rotate  = 0xcc3b70a5u;  // InputActionName{"Camera.Rotate"}
        static constexpr uint  Camera_Zoom  = 0xdfdcecaeu;  // InputActionName{"Camera.Zoom"}
        static constexpr uint  CustomKey1  = 0x23e12a05u;  // InputActionName{"CustomKey1"}
        static constexpr uint  FullscreenOnOff  = 0xf5db7d10u;  // InputActionName{"FullscreenOnOff"}
        static constexpr uint  PauseRendering  = 0xba3bd32fu;  // InputActionName{"PauseRendering"}
        static constexpr uint  ShowHelp  = 0x3d738410u;  // InputActionName{"ShowHelp"}
        static constexpr uint  SwitchInputMode  = 0x788070a3u;  // InputActionName{"SwitchInputMode"}
        static constexpr uint  UI_ReloadScript  = 0xbc2fa9b4u;  // InputActionName{"UI.ReloadScript"}
        static constexpr uint  UI_ResExport  = 0xce5140e2u;  // InputActionName{"UI.ResExport"}
        static constexpr uint  UI_Screenshot  = 0x96bf9eb7u;  // InputActionName{"UI.Screenshot"}
        static constexpr uint  UI_ShowHide  = 0xbebd792fu;  // InputActionName{"UI.ShowHide"}
    } Controller_Camera3D;

    static constexpr struct _Controller_FPSCamera
    {
        constexpr operator InputModeName_t () const { return InputModeName_t{Hash_t{0x816d7dbu}}; }  // 'Controller.FPSCamera'

        static constexpr uint  actionCount = 13;
        static constexpr uint  Camera_Move  = 0x87c9a2a2u;  // InputActionName{"Camera.Move"}
        static constexpr uint  Camera_Reset  = 0x36f37312u;  // InputActionName{"Camera.Reset"}
        static constexpr uint  Camera_Rotate  = 0xcc3b70a5u;  // InputActionName{"Camera.Rotate"}
        static constexpr uint  Camera_Zoom  = 0xdfdcecaeu;  // InputActionName{"Camera.Zoom"}
        static constexpr uint  CustomKey1  = 0x23e12a05u;  // InputActionName{"CustomKey1"}
        static constexpr uint  FullscreenOnOff  = 0xf5db7d10u;  // InputActionName{"FullscreenOnOff"}
        static constexpr uint  PauseRendering  = 0xba3bd32fu;  // InputActionName{"PauseRendering"}
        static constexpr uint  ShowHelp  = 0x3d738410u;  // InputActionName{"ShowHelp"}
        static constexpr uint  SwitchInputMode  = 0x788070a3u;  // InputActionName{"SwitchInputMode"}
        static constexpr uint  UI_ReloadScript  = 0xbc2fa9b4u;  // InputActionName{"UI.ReloadScript"}
        static constexpr uint  UI_ResExport  = 0xce5140e2u;  // InputActionName{"UI.ResExport"}
        static constexpr uint  UI_Screenshot  = 0x96bf9eb7u;  // InputActionName{"UI.Screenshot"}
        static constexpr uint  UI_ShowHide  = 0xbebd792fu;  // InputActionName{"UI.ShowHide"}
    } Controller_FPSCamera;

    static constexpr struct _Controller_FPVCamera
    {
        constexpr operator InputModeName_t () const { return InputModeName_t{Hash_t{0x5a2ef87cu}}; }  // 'Controller.FPVCamera'

        static constexpr uint  actionCount = 13;
        static constexpr uint  Camera_Move  = 0x87c9a2a2u;  // InputActionName{"Camera.Move"}
        static constexpr uint  Camera_Reset  = 0x36f37312u;  // InputActionName{"Camera.Reset"}
        static constexpr uint  Camera_Rotate  = 0xcc3b70a5u;  // InputActionName{"Camera.Rotate"}
        static constexpr uint  Camera_Zoom  = 0xdfdcecaeu;  // InputActionName{"Camera.Zoom"}
        static constexpr uint  CustomKey1  = 0x23e12a05u;  // InputActionName{"CustomKey1"}
        static constexpr uint  FullscreenOnOff  = 0xf5db7d10u;  // InputActionName{"FullscreenOnOff"}
        static constexpr uint  PauseRendering  = 0xba3bd32fu;  // InputActionName{"PauseRendering"}
        static constexpr uint  ShowHelp  = 0x3d738410u;  // InputActionName{"ShowHelp"}
        static constexpr uint  SwitchInputMode  = 0x788070a3u;  // InputActionName{"SwitchInputMode"}
        static constexpr uint  UI_ReloadScript  = 0xbc2fa9b4u;  // InputActionName{"UI.ReloadScript"}
        static constexpr uint  UI_ResExport  = 0xce5140e2u;  // InputActionName{"UI.ResExport"}
        static constexpr uint  UI_Screenshot  = 0x96bf9eb7u;  // InputActionName{"UI.Screenshot"}
        static constexpr uint  UI_ShowHide  = 0xbebd792fu;  // InputActionName{"UI.ShowHide"}
    } Controller_FPVCamera;

    static constexpr struct _Controller_FlightCamera
    {
        constexpr operator InputModeName_t () const { return InputModeName_t{Hash_t{0xfe46c841u}}; }  // 'Controller.FlightCamera'

        static constexpr uint  actionCount = 14;
        static constexpr uint  CustomKey1  = 0x23e12a05u;  // InputActionName{"CustomKey1"}
        static constexpr uint  FlightCamera_Reset  = 0x80fdc727u;  // InputActionName{"FlightCamera.Reset"}
        static constexpr uint  FlightCamera_ResetRoll  = 0x6b289662u;  // InputActionName{"FlightCamera.ResetRoll"}
        static constexpr uint  FlightCamera_Rotate  = 0x9a3eba32u;  // InputActionName{"FlightCamera.Rotate"}
        static constexpr uint  FlightCamera_Thrust  = 0xe5f2677au;  // InputActionName{"FlightCamera.Thrust"}
        static constexpr uint  FlightCamera_Zoom  = 0xb745e491u;  // InputActionName{"FlightCamera.Zoom"}
        static constexpr uint  FullscreenOnOff  = 0xf5db7d10u;  // InputActionName{"FullscreenOnOff"}
        static constexpr uint  PauseRendering  = 0xba3bd32fu;  // InputActionName{"PauseRendering"}
        static constexpr uint  ShowHelp  = 0x3d738410u;  // InputActionName{"ShowHelp"}
        static constexpr uint  SwitchInputMode  = 0x788070a3u;  // InputActionName{"SwitchInputMode"}
        static constexpr uint  UI_ReloadScript  = 0xbc2fa9b4u;  // InputActionName{"UI.ReloadScript"}
        static constexpr uint  UI_ResExport  = 0xce5140e2u;  // InputActionName{"UI.ResExport"}
        static constexpr uint  UI_Screenshot  = 0x96bf9eb7u;  // InputActionName{"UI.Screenshot"}
        static constexpr uint  UI_ShowHide  = 0xbebd792fu;  // InputActionName{"UI.ShowHide"}
    } Controller_FlightCamera;

    static constexpr struct _Controller_Orbital
    {
        constexpr operator InputModeName_t () const { return InputModeName_t{Hash_t{0x7d884966u}}; }  // 'Controller.Orbital'

        static constexpr uint  actionCount = 13;
        static constexpr uint  Camera_Move  = 0x87c9a2a2u;  // InputActionName{"Camera.Move"}
        static constexpr uint  Camera_Reset  = 0x36f37312u;  // InputActionName{"Camera.Reset"}
        static constexpr uint  Camera_Rotate  = 0xcc3b70a5u;  // InputActionName{"Camera.Rotate"}
        static constexpr uint  Camera_Zoom  = 0xdfdcecaeu;  // InputActionName{"Camera.Zoom"}
        static constexpr uint  CustomKey1  = 0x23e12a05u;  // InputActionName{"CustomKey1"}
        static constexpr uint  FullscreenOnOff  = 0xf5db7d10u;  // InputActionName{"FullscreenOnOff"}
        static constexpr uint  PauseRendering  = 0xba3bd32fu;  // InputActionName{"PauseRendering"}
        static constexpr uint  ShowHelp  = 0x3d738410u;  // InputActionName{"ShowHelp"}
        static constexpr uint  SwitchInputMode  = 0x788070a3u;  // InputActionName{"SwitchInputMode"}
        static constexpr uint  UI_ReloadScript  = 0xbc2fa9b4u;  // InputActionName{"UI.ReloadScript"}
        static constexpr uint  UI_ResExport  = 0xce5140e2u;  // InputActionName{"UI.ResExport"}
        static constexpr uint  UI_Screenshot  = 0x96bf9eb7u;  // InputActionName{"UI.Screenshot"}
        static constexpr uint  UI_ShowHide  = 0xbebd792fu;  // InputActionName{"UI.ShowHide"}
    } Controller_Orbital;

    static constexpr struct _Controller_ScaleBias
    {
        constexpr operator InputModeName_t () const { return InputModeName_t{Hash_t{0xba9857f5u}}; }  // 'Controller.ScaleBias'

        static constexpr uint  actionCount = 12;
        static constexpr uint  Camera_Bias  = 0x2a70ff81u;  // InputActionName{"Camera.Bias"}
        static constexpr uint  Camera_Reset  = 0x36f37312u;  // InputActionName{"Camera.Reset"}
        static constexpr uint  Camera_Scale  = 0x8a28e9dbu;  // InputActionName{"Camera.Scale"}
        static constexpr uint  CustomKey1  = 0x23e12a05u;  // InputActionName{"CustomKey1"}
        static constexpr uint  FullscreenOnOff  = 0xf5db7d10u;  // InputActionName{"FullscreenOnOff"}
        static constexpr uint  PauseRendering  = 0xba3bd32fu;  // InputActionName{"PauseRendering"}
        static constexpr uint  ShowHelp  = 0x3d738410u;  // InputActionName{"ShowHelp"}
        static constexpr uint  SwitchInputMode  = 0x788070a3u;  // InputActionName{"SwitchInputMode"}
        static constexpr uint  UI_ReloadScript  = 0xbc2fa9b4u;  // InputActionName{"UI.ReloadScript"}
        static constexpr uint  UI_ResExport  = 0xce5140e2u;  // InputActionName{"UI.ResExport"}
        static constexpr uint  UI_Screenshot  = 0x96bf9eb7u;  // InputActionName{"UI.Screenshot"}
        static constexpr uint  UI_ShowHide  = 0xbebd792fu;  // InputActionName{"UI.ShowHide"}
    } Controller_ScaleBias;

    static constexpr struct _Controller_TopDown
    {
        constexpr operator InputModeName_t () const { return InputModeName_t{Hash_t{0xd84bb7cu}}; }  // 'Controller.TopDown'

        static constexpr uint  actionCount = 14;
        static constexpr uint  Camera_Move  = 0x87c9a2a2u;  // InputActionName{"Camera.Move"}
        static constexpr uint  Camera_Reset  = 0x36f37312u;  // InputActionName{"Camera.Reset"}
        static constexpr uint  Camera_Rotate  = 0xcc3b70a5u;  // InputActionName{"Camera.Rotate"}
        static constexpr uint  CustomKey1  = 0x23e12a05u;  // InputActionName{"CustomKey1"}
        static constexpr uint  FullscreenOnOff  = 0xf5db7d10u;  // InputActionName{"FullscreenOnOff"}
        static constexpr uint  PauseRendering  = 0xba3bd32fu;  // InputActionName{"PauseRendering"}
        static constexpr uint  ShowHelp  = 0x3d738410u;  // InputActionName{"ShowHelp"}
        static constexpr uint  SwitchInputMode  = 0x788070a3u;  // InputActionName{"SwitchInputMode"}
        static constexpr uint  UI_MousePos  = 0x2b2426e3u;  // InputActionName{"UI.MousePos"}
        static constexpr uint  UI_MouseRBDown  = 0x3ba275e0u;  // InputActionName{"UI.MouseRBDown"}
        static constexpr uint  UI_ReloadScript  = 0xbc2fa9b4u;  // InputActionName{"UI.ReloadScript"}
        static constexpr uint  UI_ResExport  = 0xce5140e2u;  // InputActionName{"UI.ResExport"}
        static constexpr uint  UI_Screenshot  = 0x96bf9eb7u;  // InputActionName{"UI.Screenshot"}
        static constexpr uint  UI_ShowHide  = 0xbebd792fu;  // InputActionName{"UI.ShowHide"}
    } Controller_TopDown;

    static constexpr struct _Main_UI
    {
        constexpr operator InputModeName_t () const { return InputModeName_t{Hash_t{0xcedcb9fbu}}; }  // 'Main.UI'

        static constexpr uint  actionCount = 15;
        static constexpr uint  FullscreenOnOff  = 0xf5db7d10u;  // InputActionName{"FullscreenOnOff"}
        static constexpr uint  PauseRendering  = 0xba3bd32fu;  // InputActionName{"PauseRendering"}
        static constexpr uint  ShowHelp  = 0x3d738410u;  // InputActionName{"ShowHelp"}
        static constexpr uint  UI_FrameCapture  = 0x678708edu;  // InputActionName{"UI.FrameCapture"}
        static constexpr uint  UI_MouseLBDown  = 0x27e160bu;  // InputActionName{"UI.MouseLBDown"}
        static constexpr uint  UI_MousePos  = 0x2b2426e3u;  // InputActionName{"UI.MousePos"}
        static constexpr uint  UI_MouseRBDown  = 0x3ba275e0u;  // InputActionName{"UI.MouseRBDown"}
        static constexpr uint  UI_MouseWheel  = 0x8d5595f3u;  // InputActionName{"UI.MouseWheel"}
        static constexpr uint  UI_ReloadScript  = 0xbc2fa9b4u;  // InputActionName{"UI.ReloadScript"}
        static constexpr uint  UI_ResExport  = 0xce5140e2u;  // InputActionName{"UI.ResExport"}
        static constexpr uint  UI_Screenshot  = 0x96bf9eb7u;  // InputActionName{"UI.Screenshot"}
        static constexpr uint  UI_ShaderDebugger  = 0x19a1caeeu;  // InputActionName{"UI.ShaderDebugger"}
        static constexpr uint  UI_ShowHide  = 0xbebd792fu;  // InputActionName{"UI.ShowHide"}
        static constexpr uint  UI_StartStopRecording  = 0xd679d80cu;  // InputActionName{"UI.StartStopRecording"}
        static constexpr uint  UI_SwitchInputMode  = 0xb3d19d69u;  // InputActionName{"UI.SwitchInputMode"}
    } Main_UI;

    static constexpr struct _Shared
    {
        constexpr operator InputModeName_t () const { return InputModeName_t{Hash_t{0x1420f18du}}; }  // 'Shared'

        static constexpr uint  actionCount = 7;
        static constexpr uint  FullscreenOnOff  = 0xf5db7d10u;  // InputActionName{"FullscreenOnOff"}
        static constexpr uint  PauseRendering  = 0xba3bd32fu;  // InputActionName{"PauseRendering"}
        static constexpr uint  ShowHelp  = 0x3d738410u;  // InputActionName{"ShowHelp"}
        static constexpr uint  UI_ReloadScript  = 0xbc2fa9b4u;  // InputActionName{"UI.ReloadScript"}
        static constexpr uint  UI_ResExport  = 0xce5140e2u;  // InputActionName{"UI.ResExport"}
        static constexpr uint  UI_Screenshot  = 0x96bf9eb7u;  // InputActionName{"UI.Screenshot"}
        static constexpr uint  UI_ShowHide  = 0xbebd792fu;  // InputActionName{"UI.ShowHide"}
    } Shared;

    static constexpr struct _SwitchInputMode
    {
        constexpr operator InputModeName_t () const { return InputModeName_t{Hash_t{0x788070a3u}}; }  // 'SwitchInputMode'

        static constexpr uint  actionCount = 9;
        static constexpr uint  CustomKey1  = 0x23e12a05u;  // InputActionName{"CustomKey1"}
        static constexpr uint  FullscreenOnOff  = 0xf5db7d10u;  // InputActionName{"FullscreenOnOff"}
        static constexpr uint  PauseRendering  = 0xba3bd32fu;  // InputActionName{"PauseRendering"}
        static constexpr uint  ShowHelp  = 0x3d738410u;  // InputActionName{"ShowHelp"}
        static constexpr uint  SwitchInputMode  = 0x788070a3u;  // InputActionName{"SwitchInputMode"}
        static constexpr uint  UI_ReloadScript  = 0xbc2fa9b4u;  // InputActionName{"UI.ReloadScript"}
        static constexpr uint  UI_ResExport  = 0xce5140e2u;  // InputActionName{"UI.ResExport"}
        static constexpr uint  UI_Screenshot  = 0x96bf9eb7u;  // InputActionName{"UI.Screenshot"}
        static constexpr uint  UI_ShowHide  = 0xbebd792fu;  // InputActionName{"UI.ShowHide"}
    } SwitchInputMode;

}
