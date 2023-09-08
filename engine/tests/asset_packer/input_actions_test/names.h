namespace InputActions
{
	using InputModeName_t   = AE::App::InputModeName;
	using InputActionName_t = AE::App::InputActionName;

	static constexpr struct _UI
	{
		constexpr operator InputModeName_t () const { return InputModeName_t{"UI"}; }

		static constexpr struct _Desktop {
			static constexpr uint  actionCount = 5;
			static constexpr uint  Back  = uint{InputActionName_t{"Back"}};
			static constexpr uint  Cursor  = uint{InputActionName_t{"Cursor"}};
			static constexpr uint  Enter  = uint{InputActionName_t{"Enter"}};
			static constexpr uint  Options  = uint{InputActionName_t{"Options"}};
			static constexpr uint  Push  = uint{InputActionName_t{"Push"}};
		} Desktop;
		static constexpr struct _VR {
			static constexpr uint  actionCount = 5;
			static constexpr uint  Back  = uint{InputActionName_t{"Back"}};
			static constexpr uint  Cursor  = uint{InputActionName_t{"Cursor"}};
			static constexpr uint  Enter  = uint{InputActionName_t{"Enter"}};
			static constexpr uint  Options  = uint{InputActionName_t{"Options"}};
			static constexpr uint  Push  = uint{InputActionName_t{"Push"}};
		} VR;
	} UI;

}
