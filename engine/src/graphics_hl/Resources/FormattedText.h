// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	[b][/b] - bold
	[i][/i] - italic
	[u][/u] - underline
	[s][/s] - strikethrough

	[style ...][/style]
		color=#00000000	- RRGGBBAA
		size=10			- size in pixels
*/

#pragma once

#include "base/Memory/LinearAllocator.h"
#include "serializing/ISerializable.h"
#include "graphics/Public/Common.h"

namespace AE::Graphics
{

	//
	// Formatted Text
	//

	class FormattedText final : public Serializing::ISerializable
	{
	// types
	public:
		struct Chunk
		{
			Chunk const*	next		= null;
			RGBA8u			color;
			uint			length		: 16;
			uint			height		: 9;	// pixels
			uint			bold		: 1;	// [b]
			uint			italic		: 1;	// [i]
			uint			underline	: 1;	// [u]
			uint			strikeout	: 1;	// [s]
			CharUtf8		string[1];			// null terminated utf8 string

			Chunk () = default;

			ND_ float			Height ()	const	{ return float(height); }
			ND_ U8StringView	Text ()		const	{ return U8StringView{ string, length }; }
		};

		using Self			= FormattedText;
		using Allocator_t	= LinearAllocator<>;	// TODO: optimize


	// variables
	private:
		Chunk*			_first		= null;
		uint			_maxChars	= 0;
		Allocator_t		_alloc;


	// methods
	public:
		FormattedText ()												{ _alloc.SetBlockSize( 512_b ); }
		explicit FormattedText (StringView str) : FormattedText()		{ Append( str ); }
		explicit FormattedText (U8StringView str) : FormattedText()		{ Append( str ); }
		FormattedText (const FormattedText &other) : FormattedText()	{ Append( other ); }
		FormattedText (FormattedText &&other) : _first{other._first}, _alloc{RVRef(other._alloc)} { other._first = null; }

		Self&  Append (const FormattedText &);
		Self&  Append (U8StringView str);
		Self&  Append (StringView str);

		void   Clear ();

		FormattedText&  operator = (const FormattedText &rhs)	{ _alloc.Discard();  _first = null;  Append( rhs );  return *this; }
		FormattedText&  operator = (FormattedText &&rhs)		{ _alloc = RVRef(rhs._alloc);  _first = rhs._first;  rhs._first = null;  return *this; }

		ND_ bool			Empty ()		const				{ return _first == null; }
		ND_ Chunk const*	GetFirst ()		const				{ return _first; }
		ND_ uint			GetMaxChars ()	const				{ return _maxChars; }

		ND_ U8String		ToString ()		const;
		

		// ISerializable //
		bool  Serialize (Serializing::Serializer &) const override;
		bool  Deserialize (Serializing::Deserializer &) override;
	};
	


	//
	// Precalculated Formatted Text
	//

	class PrecalculatedFormattedText final : public Serializing::ISerializable
	{
		friend class RasterFont;

	// types
	private:
		using Self	= PrecalculatedFormattedText;


	// variables
	private:
		Array<float>	_lineHeight;		// in pixels	// TODO: small vector
		float2			_size;				// in pixels
		FormattedText	_text;
		float			_spacing	= 1.0f;	// in normalized coords
		bool			_wordWrap	= false;

	// methods
	public:
		PrecalculatedFormattedText () {}

		Self&  SetText (FormattedText value)	{ _text = RVRef(value);  return *this; }
		Self&  SetSpacing (float value)			{ _spacing  = value;  return *this; }
		Self&  SetWordWrap (bool value)			{ _wordWrap = value;  return *this; }

		ND_ FormattedText const&	Text ()			const	{ return _text; }
		ND_ float					Spacing ()		const	{ return _spacing; }
		ND_ float2 const&			RegionSize ()	const	{ return _size; }
		ND_ bool					IsWordWrap ()	const	{ return _wordWrap; }
		ND_ ArrayView<float>		LineHeights ()	const	{ return _lineHeight; }
			
		// ISerializable //
		bool  Serialize (Serializing::Serializer &) const override;
		bool  Deserialize (Serializing::Deserializer &) override;
	};


} // AE::Graphics
