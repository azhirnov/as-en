// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Screen		- surface for UI
	Widget		- window in screen, stored in stack
	UISystem	- additional data
*/

#pragma once

#include "graphics/Resources/FormattedText.h"
#include "graphics_rhi/Public/IDs.h"
#include "graphics_rhi/Public/RenderStateEnums.h"
#include "graphics_rhi/Public/DescriptorSet.h"

namespace AE::UI::_hidden_
{
	static constexpr uint	NamedIDs_Start	= 2 << 24;

} // AE::UI::_hidden_


namespace AE::Graphics
{
	class IRenderTechPipelines;

	class LoadableImage;
	class StaticImageAtlas;
	class RasterFont;

	class ResourceCache;
	class ResourceUploadManager;

	using RenderTechPipelinesPtr = RC< IRenderTechPipelines >;

} // AE::Graphics


namespace AE::UI
{
	using namespace AE::Base;

	ImportBitOperators;
	using namespace AE::ImportCoroutines;

	#if AE_ENABLE_DATA_RACE_CHECK
	using AE::Threading::RWDataRaceCheck;
	#endif

	using AE::Graphics::GraphicsPipelineID;
	using AE::Graphics::DescriptorSetID;
	using AE::Graphics::BufferID;
	using AE::Graphics::ImageInAtlasName;
	using AE::Graphics::RasterFont;
	using AE::Graphics::LoadableImage;
	using AE::Graphics::StaticImageAtlas;
	using AE::Graphics::EPrimitive;
	using AE::Graphics::IRenderTechPipelines;
	using AE::Graphics::RenderTechPipelinesPtr;
	using AE::Graphics::DescSetBinding;

	class ILayout;
	class IDrawable;
	class IController;
	class Widget;
	class Screen;

	using ScreenPtr = RC<Screen>;
	using WidgetPtr	= RC<Widget>;

	using ActionName	= NamedID< 32, UI::_hidden_::NamedIDs_Start + 2, false >;

	using StyleName		= NamedID< 32, UI::_hidden_::NamedIDs_Start + 4, false >;

	//using ResourceName	= NamedID< 64, UI::_hidden_::NamedIDs_Start + 5, true >;

} // AE::UI
