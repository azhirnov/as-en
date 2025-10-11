// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "OpenXRParser.h"

namespace AE::Parsers
{

	//
	// OpenXR Loader Generator
	//

	class OpenXRLoaderGen : public OpenXRParser
	{
	// types
	private:
		struct FeatureSet
		{
			Array< FeatureInfo >	instance;
			HashSet< StringView >	enabledExt;		// for loader
			Version2				minVer;
			usize					maxNameLen	= 0;
		};


	// methods
	public:
		OpenXRLoaderGen () {}

		bool  GenXrLoaders (const Path &outputFolder, Version2 minVer) const;
		bool  GenXrFeatures (const Path &outputFolder, Version2 minVer) const;

		bool  GenEnumToString (const Path &outputFolder) const;

	private:
		ND_ FeatureSet  _GetFeatures (Version2 minVer) const;

		ND_ String  _GetFeaturesBoolStruct (const FeatureSet &feats) const;
		ND_ String  _GetLogFeaturesFunc (const FeatureSet &feats) const;
		ND_ String  _GetCheckFeaturesFunc (const FeatureSet &feats) const;
		ND_ String  _GetExtensionsListFunc (const FeatureSet &feats) const;

		ND_ bool  _IsPlatformSpecificExt (StringView ext) const;
	};

} // AE::Parsers
