namespace InputActions
{
	using Hash_t          = AE::Base::HashVal32;
	using InputModeName_t = AE::App::InputModeName;

	static constexpr struct _Main_UI
	{
		constexpr operator InputModeName_t () const { return InputModeName_t{Hash_t{0xcedcb9fbu}}; }  // 'Main.UI'

		static constexpr uint  actionCount = 6;
		enum Bindings : uint {
			SwitchInputMode  = 0x788070a3u,  // InputActionName{"SwitchInputMode"}
			UI_MouseLBDown  = 0x27e160bu,  // InputActionName{"UI.MouseLBDown"}
			UI_MousePos  = 0x2b2426e3u,  // InputActionName{"UI.MousePos"}
			UI_MouseWheel  = 0x8d5595f3u,  // InputActionName{"UI.MouseWheel"}
			UI_ReloadScript  = 0xbc2fa9b4u,  // InputActionName{"UI.ReloadScript"}
			UI_ShowHide  = 0xbebd792fu,  // InputActionName{"UI.ShowHide"}
		};
	} Main_UI = {};

	static constexpr struct _Screen_UI
	{
		constexpr operator InputModeName_t () const { return InputModeName_t{Hash_t{0xa048d26cu}}; }  // 'Screen.UI'

		static constexpr uint  actionCount = 10;
		enum Bindings : uint {
			SwitchInputMode  = 0x788070a3u,  // InputActionName{"SwitchInputMode"}
			ui_MousePos  = 0xe2e5aae4u,  // InputActionName{"ui.MousePos"}
			ui_MouseWheel  = 0x261a05feu,  // InputActionName{"ui.MouseWheel"}
			ui_ScaleRotate2D  = 0xb452e041u,  // InputActionName{"ui.ScaleRotate2D"}
			ui_TouchClick  = 0xfdc74d98u,  // InputActionName{"ui.TouchClick"}
			ui_TouchDoubleClick  = 0xd2cb3a6cu,  // InputActionName{"ui.TouchDoubleClick"}
			ui_TouchDown  = 0x9fbf57d5u,  // InputActionName{"ui.TouchDown"}
			ui_TouchLongPressMove  = 0xb6ff65c2u,  // InputActionName{"ui.TouchLongPressMove"}
			ui_TouchMove  = 0x6c7ef096u,  // InputActionName{"ui.TouchMove"}
			ui_TouchPress  = 0x3af5f305u,  // InputActionName{"ui.TouchPress"}
		};
	} Screen_UI = {};

}
