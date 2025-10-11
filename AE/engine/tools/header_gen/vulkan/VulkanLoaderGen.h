// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "VulkanParser.h"

namespace AE::Parsers
{

	//
	// Vulkan Loader Generator
	//

	class VulkanLoaderGen : public VulkanParser
	{
	// types
	private:
		struct FeatureSet
		{
			Array< FeatureInfo >	instance;
			Array< FeatureInfo >	device;
			HashSet< StringView >	enabledExt;		// for loader
			Version2				minVer;
			usize					maxNameLen	= 0;
		};


	// methods
	public:
		bool  GenVulkanLoaders (const Path &outputFolder, Version2 minVer) const;
		bool  GenVulkanFeatures (const Path &outputFolder, Version2 minVer) const;
		
		bool  GenEnumToString (const Path &outputFolder) const;

	private:
		ND_ FeatureSet  _GetFeatures (Version2 minVer) const;

		ND_ String  _GetFeaturesBoolStruct (const FeatureSet &feats) const;
		ND_ String  _GetFeaturesPropsStruct (const FeatureSet &feats) const;
		ND_ String  _GetExtensionsListFunc (const FeatureSet &feats) const;
		ND_ String  _GetFeaturesAndPropertiesFunc (const FeatureSet &feats) const;
		ND_ String  _GetLogFeaturesFunc (const FeatureSet &feats) const;
		ND_ String  _GetCheckFeaturesFunc (const FeatureSet &feats) const;
	};


} // AE::Parsers
