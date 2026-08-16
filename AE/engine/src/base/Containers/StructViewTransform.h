// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "base/Containers/StructView.h"

namespace AE::Base
{

	//
	// Structure View Transform Converter
	//
	template <typename SrcType, typename DstType>
	struct StructViewTransform_CastConverter
	{
		StaticAssert( std::is_convertible_v< SrcType, DstType >);

		ND_ DstType  operator () (SrcType src)			C_NE___	{ return DstType{src}; }
	};

	template <typename SrcType, typename DstType>
	struct StructViewTransform_BitCastConverter
	{
		ND_ DstType  operator () (const SrcType &src)	C_NE___	{ return BitCast<DstType>( src ); }
	};



	//
	// Structure View Transform
	//
	template <typename SrcType,
			  typename DstType,
			  typename Converter = StructViewTransform_CastConverter< SrcType, DstType >>
	struct StructViewTransform
	{
	// types
	public:
		using Self			= StructViewTransform< SrcType, DstType, Converter >;
		using View_t		= StructView< SrcType >;
		using SrcValue_t	= SrcType;
		using DstValue_t	= DstType;
		using Converter_t	= Converter;

		struct const_iterator
		{
			friend struct StructViewTransform;

		private:
			Self const&		_ref;
			usize			_index = 0;

			const_iterator (const Self &ref, usize idx)					__NE___ : _ref{ref}, _index{idx} {}

		public:
			const_iterator&		operator ++ ()							__NE___	{ ++_index;  return *this; }
			ND_ exact_t			operator * ()							C_NE___	{ return _ref[_index]; }
			ND_ bool			operator == (const const_iterator &rhs)	C_NE___	{ return &_ref == &rhs._ref and _index == rhs._index; }
			ND_ bool			operator != (const const_iterator &rhs)	C_NE___	{ return not (*this == rhs); }
		};


		struct large_iterator
		{
			friend struct StructViewTransform;

		private:
			Self const		_ref;
			usize			_index = 0;

			large_iterator (const Self &ref, usize idx)					__NE___ : _ref{ref}, _index{idx} {}

		public:
			large_iterator&		operator ++ ()							__NE___	{ ++_index;  return *this; }
			ND_ exact_t			operator * ()							C_NE___	{ return _ref[_index]; }
			ND_ bool			operator != (const large_iterator &rhs)	C_NE___	{ return not (*this == rhs); }
			ND_ bool			operator == (const large_iterator &rhs)	C_NE___	{ return (_ref._view  == rhs._ref._view) and (_index == rhs._index); }
		};


	// variables
	private:
		View_t		_view;


	// methods
	public:
		StructViewTransform ()						__NE___ = default;
		StructViewTransform (const Self &)			__NE___ = default;
		StructViewTransform (Self &&)				__NE___ = default;

		explicit StructViewTransform (View_t view)	__NE___ : _view{RVRef(view)} {}

		Self&  operator = (const Self &)			__NE___ = default;
		Self&  operator = (Self &&)					__NE___ = default;

		Self&  operator = (View_t rhs)				__NE___	{ _view = RVRef(rhs);  return *this; }

		ND_ usize			size ()					C_NE___	{ return _view.size(); }
		ND_ bool			empty ()				C_NE___	{ return _view.empty(); }
		ND_ exact_t			operator [] (usize i)	C_NE___	{ return Converter_t{}( _view[i] ); }

		ND_ exact_t			front ()				C_NE___	{ return operator[] (0); }
		ND_ exact_t			back ()					C_NE___	{ return operator[] (size()-1); }

		ND_ const_iterator	begin ()				CrNE___	{ return const_iterator{ *this, 0 }; }
		ND_ const_iterator	end ()					CrNE___	{ return const_iterator{ *this, size() }; }

		ND_ large_iterator	begin ()				rvNE___	{ return large_iterator{ *this, 0 }; }
		ND_ large_iterator	end ()					rvNE___	{ return large_iterator{ *this, size() }; }

		ND_ const_iterator	begin ()				r_NE___	{ return const_iterator{ *this, 0 }; }
		ND_ const_iterator	end ()					r_NE___	{ return const_iterator{ *this, size() }; }

		ND_ Bytes			SrcDataSize ()			C_NE___	{ return SizeOf<SrcType> * size(); }
		ND_ Bytes			DstDataSize ()			C_NE___	{ return SizeOf<DstType> * size(); }


		ND_ Self  section (usize first, usize count)C_NE___	{ return Self{ _view.section( first, count )}; }

		ND_ explicit operator Array<DstType> ()		C_NE___
		{
			Array<DstType>	result;
			result.resize( size() );

			for (usize i = 0; i < result.size(); ++i) {
				result[i] = (*this)[i];
			}
			return result;
		}
	};

} // AE::Base
