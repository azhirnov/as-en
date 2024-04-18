namespace InputActions
{
	using Hash_t          = AE::Base::HashVal32;
	using InputModeName_t = AE::App::InputModeName;

	static constexpr struct _Camera3D
	{
		constexpr operator InputModeName_t () const { return InputModeName_t{Hash_t{0x7b09036u}}; }  // 'Camera3D'

		static constexpr uint  actionCount = 1;
		enum Bindings : uint {
			Camera_Rotate  = 0xcc3b70a5u,  // InputActionName{"Camera.Rotate"}
		};
		static constexpr struct _Desktop {
			static constexpr uint  actionCount = 1;
			enum Bindings : uint {
				Camera_Move  = 0x87c9a2a2u,  // InputActionName{"Camera.Move"}
			};
		} Desktop;
		static constexpr struct _Mobile {
			static constexpr uint  actionCount = 1;
			enum Bindings : uint {
				Camera_Sensor3f  = 0xdcd099adu,  // InputActionName{"Camera.Sensor3f"}
			};
		} Mobile;
	} Camera3D;

	static constexpr struct _Canvas2D
	{
		constexpr operator InputModeName_t () const { return InputModeName_t{Hash_t{0xe271b1e4u}}; }  // 'Canvas2D'

		static constexpr uint  actionCount = 2;
		enum Bindings : uint {
			Cursor  = 0xc2d36fb8u,  // InputActionName{"Cursor"}
			Enter  = 0x78b1ef6au,  // InputActionName{"Enter"}
		};
	} Canvas2D;

	static constexpr struct _HwCamera
	{
		constexpr operator InputModeName_t () const { return InputModeName_t{Hash_t{0x3be7a204u}}; }  // 'HwCamera'

		static constexpr uint  actionCount = 2;
		enum Bindings : uint {
			Touch_Click  = 0x5fb61d88u,  // InputActionName{"Touch.Click"}
			Touch_Move  = 0x7a5f92b5u,  // InputActionName{"Touch.Move"}
		};
	} HwCamera;

	static constexpr struct _SampleSelector
	{
		constexpr operator InputModeName_t () const { return InputModeName_t{Hash_t{0xc682d438u}}; }  // 'SampleSelector'

	} SampleSelector;

	static constexpr struct _imGUI
	{
		constexpr operator InputModeName_t () const { return InputModeName_t{Hash_t{0x2655069fu}}; }  // 'imGUI'

		static constexpr uint  actionCount = 2;
		enum Bindings : uint {
			Touch_Click  = 0x5fb61d88u,  // InputActionName{"Touch.Click"}
			Touch_Move  = 0x7a5f92b5u,  // InputActionName{"Touch.Move"}
		};
		static constexpr struct _Desktop {
			static constexpr uint  actionCount = 3;
			enum Bindings : uint {
				MouseLBDown  = 0xd0f2e875u,  // InputActionName{"MouseLBDown"}
				MousePos  = 0x4f44c5bu,  // InputActionName{"MousePos"}
				MouseWheel  = 0xc2e16d5u,  // InputActionName{"MouseWheel"}
			};
		} Desktop;
	} imGUI;

}
