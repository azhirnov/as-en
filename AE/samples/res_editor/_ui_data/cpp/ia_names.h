namespace InputActions
{
    using InputModeName_t   = AE::App::InputModeName;
    using InputActionName_t = AE::App::InputActionName;

    static constexpr struct _Controller_Camera3D
    {
        constexpr operator InputModeName_t () const { return InputModeName_t{"Controller.Camera3D"}; }

        static constexpr uint  actionCount = 10;
        static constexpr uint  Camera_Move  = uint{InputActionName_t{"Camera.Move"}};
        static constexpr uint  Camera_Reset  = uint{InputActionName_t{"Camera.Reset"}};
        static constexpr uint  Camera_Rotate  = uint{InputActionName_t{"Camera.Rotate"}};
        static constexpr uint  Camera_Zoom  = uint{InputActionName_t{"Camera.Zoom"}};
        static constexpr uint  CustomKey1  = uint{InputActionName_t{"CustomKey1"}};
        static constexpr uint  PauseRendering  = uint{InputActionName_t{"PauseRendering"}};
        static constexpr uint  ShowHelp  = uint{InputActionName_t{"ShowHelp"}};
        static constexpr uint  SwitchInputMode  = uint{InputActionName_t{"SwitchInputMode"}};
        static constexpr uint  UI_Screenshot  = uint{InputActionName_t{"UI.Screenshot"}};
        static constexpr uint  UI_ShowHide  = uint{InputActionName_t{"UI.ShowHide"}};
    } Controller_Camera3D;

    static constexpr struct _Controller_FPSCamera
    {
        constexpr operator InputModeName_t () const { return InputModeName_t{"Controller.FPSCamera"}; }

        static constexpr uint  actionCount = 10;
        static constexpr uint  Camera_Move  = uint{InputActionName_t{"Camera.Move"}};
        static constexpr uint  Camera_Reset  = uint{InputActionName_t{"Camera.Reset"}};
        static constexpr uint  Camera_Rotate  = uint{InputActionName_t{"Camera.Rotate"}};
        static constexpr uint  Camera_Zoom  = uint{InputActionName_t{"Camera.Zoom"}};
        static constexpr uint  CustomKey1  = uint{InputActionName_t{"CustomKey1"}};
        static constexpr uint  PauseRendering  = uint{InputActionName_t{"PauseRendering"}};
        static constexpr uint  ShowHelp  = uint{InputActionName_t{"ShowHelp"}};
        static constexpr uint  SwitchInputMode  = uint{InputActionName_t{"SwitchInputMode"}};
        static constexpr uint  UI_Screenshot  = uint{InputActionName_t{"UI.Screenshot"}};
        static constexpr uint  UI_ShowHide  = uint{InputActionName_t{"UI.ShowHide"}};
    } Controller_FPSCamera;

    static constexpr struct _Controller_FPVCamera
    {
        constexpr operator InputModeName_t () const { return InputModeName_t{"Controller.FPVCamera"}; }

        static constexpr uint  actionCount = 10;
        static constexpr uint  Camera_Move  = uint{InputActionName_t{"Camera.Move"}};
        static constexpr uint  Camera_Reset  = uint{InputActionName_t{"Camera.Reset"}};
        static constexpr uint  Camera_Rotate  = uint{InputActionName_t{"Camera.Rotate"}};
        static constexpr uint  Camera_Zoom  = uint{InputActionName_t{"Camera.Zoom"}};
        static constexpr uint  CustomKey1  = uint{InputActionName_t{"CustomKey1"}};
        static constexpr uint  PauseRendering  = uint{InputActionName_t{"PauseRendering"}};
        static constexpr uint  ShowHelp  = uint{InputActionName_t{"ShowHelp"}};
        static constexpr uint  SwitchInputMode  = uint{InputActionName_t{"SwitchInputMode"}};
        static constexpr uint  UI_Screenshot  = uint{InputActionName_t{"UI.Screenshot"}};
        static constexpr uint  UI_ShowHide  = uint{InputActionName_t{"UI.ShowHide"}};
    } Controller_FPVCamera;

    static constexpr struct _Controller_FlightCamera
    {
        constexpr operator InputModeName_t () const { return InputModeName_t{"Controller.FlightCamera"}; }

        static constexpr uint  actionCount = 11;
        static constexpr uint  CustomKey1  = uint{InputActionName_t{"CustomKey1"}};
        static constexpr uint  FlightCamera_Reset  = uint{InputActionName_t{"FlightCamera.Reset"}};
        static constexpr uint  FlightCamera_ResetRoll  = uint{InputActionName_t{"FlightCamera.ResetRoll"}};
        static constexpr uint  FlightCamera_Rotate  = uint{InputActionName_t{"FlightCamera.Rotate"}};
        static constexpr uint  FlightCamera_Thrust  = uint{InputActionName_t{"FlightCamera.Thrust"}};
        static constexpr uint  FlightCamera_Zoom  = uint{InputActionName_t{"FlightCamera.Zoom"}};
        static constexpr uint  PauseRendering  = uint{InputActionName_t{"PauseRendering"}};
        static constexpr uint  ShowHelp  = uint{InputActionName_t{"ShowHelp"}};
        static constexpr uint  SwitchInputMode  = uint{InputActionName_t{"SwitchInputMode"}};
        static constexpr uint  UI_Screenshot  = uint{InputActionName_t{"UI.Screenshot"}};
        static constexpr uint  UI_ShowHide  = uint{InputActionName_t{"UI.ShowHide"}};
    } Controller_FlightCamera;

    static constexpr struct _Controller_Orbital
    {
        constexpr operator InputModeName_t () const { return InputModeName_t{"Controller.Orbital"}; }

        static constexpr uint  actionCount = 10;
        static constexpr uint  Camera_Move  = uint{InputActionName_t{"Camera.Move"}};
        static constexpr uint  Camera_Reset  = uint{InputActionName_t{"Camera.Reset"}};
        static constexpr uint  Camera_Rotate  = uint{InputActionName_t{"Camera.Rotate"}};
        static constexpr uint  Camera_Zoom  = uint{InputActionName_t{"Camera.Zoom"}};
        static constexpr uint  CustomKey1  = uint{InputActionName_t{"CustomKey1"}};
        static constexpr uint  PauseRendering  = uint{InputActionName_t{"PauseRendering"}};
        static constexpr uint  ShowHelp  = uint{InputActionName_t{"ShowHelp"}};
        static constexpr uint  SwitchInputMode  = uint{InputActionName_t{"SwitchInputMode"}};
        static constexpr uint  UI_Screenshot  = uint{InputActionName_t{"UI.Screenshot"}};
        static constexpr uint  UI_ShowHide  = uint{InputActionName_t{"UI.ShowHide"}};
    } Controller_Orbital;

    static constexpr struct _Controller_ScaleBias
    {
        constexpr operator InputModeName_t () const { return InputModeName_t{"Controller.ScaleBias"}; }

        static constexpr uint  actionCount = 9;
        static constexpr uint  Camera_Bias  = uint{InputActionName_t{"Camera.Bias"}};
        static constexpr uint  Camera_Reset  = uint{InputActionName_t{"Camera.Reset"}};
        static constexpr uint  Camera_Scale  = uint{InputActionName_t{"Camera.Scale"}};
        static constexpr uint  CustomKey1  = uint{InputActionName_t{"CustomKey1"}};
        static constexpr uint  PauseRendering  = uint{InputActionName_t{"PauseRendering"}};
        static constexpr uint  ShowHelp  = uint{InputActionName_t{"ShowHelp"}};
        static constexpr uint  SwitchInputMode  = uint{InputActionName_t{"SwitchInputMode"}};
        static constexpr uint  UI_Screenshot  = uint{InputActionName_t{"UI.Screenshot"}};
        static constexpr uint  UI_ShowHide  = uint{InputActionName_t{"UI.ShowHide"}};
    } Controller_ScaleBias;

    static constexpr struct _Controller_TopDown
    {
        constexpr operator InputModeName_t () const { return InputModeName_t{"Controller.TopDown"}; }

        static constexpr uint  actionCount = 11;
        static constexpr uint  Camera_Move  = uint{InputActionName_t{"Camera.Move"}};
        static constexpr uint  Camera_Reset  = uint{InputActionName_t{"Camera.Reset"}};
        static constexpr uint  Camera_Rotate  = uint{InputActionName_t{"Camera.Rotate"}};
        static constexpr uint  CustomKey1  = uint{InputActionName_t{"CustomKey1"}};
        static constexpr uint  PauseRendering  = uint{InputActionName_t{"PauseRendering"}};
        static constexpr uint  ShowHelp  = uint{InputActionName_t{"ShowHelp"}};
        static constexpr uint  SwitchInputMode  = uint{InputActionName_t{"SwitchInputMode"}};
        static constexpr uint  UI_MousePos  = uint{InputActionName_t{"UI.MousePos"}};
        static constexpr uint  UI_MouseRBDown  = uint{InputActionName_t{"UI.MouseRBDown"}};
        static constexpr uint  UI_Screenshot  = uint{InputActionName_t{"UI.Screenshot"}};
        static constexpr uint  UI_ShowHide  = uint{InputActionName_t{"UI.ShowHide"}};
    } Controller_TopDown;

    static constexpr struct _Main_UI
    {
        constexpr operator InputModeName_t () const { return InputModeName_t{"Main.UI"}; }

        static constexpr uint  actionCount = 13;
        static constexpr uint  PauseRendering  = uint{InputActionName_t{"PauseRendering"}};
        static constexpr uint  ShowHelp  = uint{InputActionName_t{"ShowHelp"}};
        static constexpr uint  UI_FrameCapture  = uint{InputActionName_t{"UI.FrameCapture"}};
        static constexpr uint  UI_MouseLBDown  = uint{InputActionName_t{"UI.MouseLBDown"}};
        static constexpr uint  UI_MousePos  = uint{InputActionName_t{"UI.MousePos"}};
        static constexpr uint  UI_MouseRBDown  = uint{InputActionName_t{"UI.MouseRBDown"}};
        static constexpr uint  UI_MouseWheel  = uint{InputActionName_t{"UI.MouseWheel"}};
        static constexpr uint  UI_ReloadScript  = uint{InputActionName_t{"UI.ReloadScript"}};
        static constexpr uint  UI_Screenshot  = uint{InputActionName_t{"UI.Screenshot"}};
        static constexpr uint  UI_ShaderDebugger  = uint{InputActionName_t{"UI.ShaderDebugger"}};
        static constexpr uint  UI_ShowHide  = uint{InputActionName_t{"UI.ShowHide"}};
        static constexpr uint  UI_StartStopRecording  = uint{InputActionName_t{"UI.StartStopRecording"}};
        static constexpr uint  UI_SwitchInputMode  = uint{InputActionName_t{"UI.SwitchInputMode"}};
    } Main_UI;

    static constexpr struct _Shared
    {
        constexpr operator InputModeName_t () const { return InputModeName_t{"Shared"}; }

        static constexpr uint  actionCount = 4;
        static constexpr uint  PauseRendering  = uint{InputActionName_t{"PauseRendering"}};
        static constexpr uint  ShowHelp  = uint{InputActionName_t{"ShowHelp"}};
        static constexpr uint  UI_Screenshot  = uint{InputActionName_t{"UI.Screenshot"}};
        static constexpr uint  UI_ShowHide  = uint{InputActionName_t{"UI.ShowHide"}};
    } Shared;

    static constexpr struct _SwitchInputMode
    {
        constexpr operator InputModeName_t () const { return InputModeName_t{"SwitchInputMode"}; }

        static constexpr uint  actionCount = 6;
        static constexpr uint  CustomKey1  = uint{InputActionName_t{"CustomKey1"}};
        static constexpr uint  PauseRendering  = uint{InputActionName_t{"PauseRendering"}};
        static constexpr uint  ShowHelp  = uint{InputActionName_t{"ShowHelp"}};
        static constexpr uint  SwitchInputMode  = uint{InputActionName_t{"SwitchInputMode"}};
        static constexpr uint  UI_Screenshot  = uint{InputActionName_t{"UI.Screenshot"}};
        static constexpr uint  UI_ShowHide  = uint{InputActionName_t{"UI.ShowHide"}};
    } SwitchInputMode;

}
