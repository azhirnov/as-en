namespace InputActions
{
	using InputModeName_t   = AE::App::InputModeName;
	using InputActionName_t = AE::App::InputActionName;

	static constexpr struct _Bloom
	{
		constexpr operator InputModeName_t () const { return InputModeName_t{"Bloom"}; }

		static constexpr uint  actionCount = 2;
		static constexpr uint  Touch  = uint{InputActionName_t{"Touch"}};
		static constexpr uint  Touch_Click  = uint{InputActionName_t{"Touch.Click"}};
		static constexpr struct _Desktop {
			static constexpr uint  actionCount = 2;
			static constexpr uint  MouseLBDown  = uint{InputActionName_t{"MouseLBDown"}};
			static constexpr uint  MousePos  = uint{InputActionName_t{"MousePos"}};
		} Desktop;
		static constexpr struct _VR {
			static constexpr uint  actionCount = 2;
			static constexpr uint  MouseLBDown  = uint{InputActionName_t{"MouseLBDown"}};
			static constexpr uint  MousePos  = uint{InputActionName_t{"MousePos"}};
		} VR;
	} Bloom;

	static constexpr struct _Canvas2D
	{
		constexpr operator InputModeName_t () const { return InputModeName_t{"Canvas2D"}; }

		static constexpr uint  actionCount = 2;
		static constexpr uint  Cursor  = uint{InputActionName_t{"Cursor"}};
		static constexpr uint  Enter  = uint{InputActionName_t{"Enter"}};
	} Canvas2D;

	static constexpr struct _SampleSelector
	{
		constexpr operator InputModeName_t () const { return InputModeName_t{"SampleSelector"}; }

	} SampleSelector;

	static constexpr struct _Simple3D
	{
		constexpr operator InputModeName_t () const { return InputModeName_t{"Simple3D"}; }

		static constexpr uint  actionCount = 1;
		static constexpr uint  Camera_Rotate  = uint{InputActionName_t{"Camera.Rotate"}};
		static constexpr struct _Desktop {
			static constexpr uint  actionCount = 1;
			static constexpr uint  Camera_Move  = uint{InputActionName_t{"Camera.Move"}};
		} Desktop;
		static constexpr struct _VR {
			static constexpr uint  actionCount = 1;
			static constexpr uint  Camera_Move  = uint{InputActionName_t{"Camera.Move"}};
		} VR;
	} Simple3D;

	static constexpr struct _imGUI
	{
		constexpr operator InputModeName_t () const { return InputModeName_t{"imGUI"}; }

		static constexpr uint  actionCount = 6;
		static constexpr uint  Test_DoubleClick  = uint{InputActionName_t{"Test.DoubleClick"}};
		static constexpr uint  Test_Down  = uint{InputActionName_t{"Test.Down"}};
		static constexpr uint  Test_LongPress  = uint{InputActionName_t{"Test.LongPress"}};
		static constexpr uint  Test_Move  = uint{InputActionName_t{"Test.Move"}};
		static constexpr uint  Test_ScaleRotate2D  = uint{InputActionName_t{"Test.ScaleRotate2D"}};
		static constexpr uint  Touch_Click  = uint{InputActionName_t{"Touch.Click"}};
		static constexpr struct _Desktop {
			static constexpr uint  actionCount = 5;
			static constexpr uint  MouseLBDown  = uint{InputActionName_t{"MouseLBDown"}};
			static constexpr uint  MousePos  = uint{InputActionName_t{"MousePos"}};
			static constexpr uint  MouseWheel  = uint{InputActionName_t{"MouseWheel"}};
			static constexpr uint  Test_Click  = uint{InputActionName_t{"Test.Click"}};
			static constexpr uint  Test_Hold  = uint{InputActionName_t{"Test.Hold"}};
		} Desktop;
		static constexpr struct _VR {
			static constexpr uint  actionCount = 5;
			static constexpr uint  MouseLBDown  = uint{InputActionName_t{"MouseLBDown"}};
			static constexpr uint  MousePos  = uint{InputActionName_t{"MousePos"}};
			static constexpr uint  MouseWheel  = uint{InputActionName_t{"MouseWheel"}};
			static constexpr uint  Test_Click  = uint{InputActionName_t{"Test.Click"}};
			static constexpr uint  Test_Hold  = uint{InputActionName_t{"Test.Hold"}};
		} VR;
	} imGUI;

}
