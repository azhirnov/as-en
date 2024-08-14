// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_IMGUI
# include "profiler/ImGui/ImLineGraph.h"

namespace AE::Profiler::_hidden_
{
	// tag: NamedID UID
	static constexpr uint	NamedIDs_Start		= 4 << 24;

} // AE::Profiler::_hidden_

namespace AE::Profiler
{
	using SecName	= NamedID< 32, Profiler::_hidden_::NamedIDs_Start + 1,  AE_OPTIMIZE_IDS >;
	using GraphName	= NamedID< 32, Profiler::_hidden_::NamedIDs_Start + 2,  AE_OPTIMIZE_IDS >;


	//
	// ImGui Line Graph Table
	//

	class ImLineGraphTable final : public Noncopyable
	{
	// types
	private:
		struct GraphMap
		{
			String											caption;
			Array< Unique< ImLineGraph >>					ordered;
			FlatHashMap< GraphName::Optimized_t, uint >		map;

			ND_ bool  Empty ()	const;
		};

		struct SectionMap
		{
			Array< GraphMap >								ordered;
			FlatHashMap< SecName::Optimized_t, uint >		map;
		};

		using GraphSearchMap_t	= FlatHashMap< GraphName::Optimized_t, uint2 >;


	// variables
	private:
		SectionMap			_sections;
		GraphSearchMap_t	_allGraphs;


	// methods
	public:
		ImLineGraphTable ()															{}
		ImLineGraphTable (ImLineGraphTable &&)										{}

		void  Draw (float width, float rowHeight, float2 padding, bool isHover)		const;

		ND_ ImLineGraph&		Add (SecName::Ref section, GraphName::Ref graph);
		ND_ Ptr<ImLineGraph>	Get (SecName::Ref section, GraphName::Ref graph);
		ND_ Ptr<ImLineGraph>	Get (GraphName::Ref graph);

		bool  SetCaption (SecName::Ref section, String cap);
	};


} // AE::Profiler

#endif // AE_ENABLE_IMGUI
