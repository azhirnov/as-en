// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Sort functions
*/

#if 0
	// as macros
	void  BubbleSort (uint count, bool (*cmp)(int i, int j), void (*swap)(int i, int j));
	void  InsertionSort (uint count, bool (*cmpWithNewValue)(int i), void (*set)(int dst, int src), void (*setNewValue)(int i));
#endif
//-----------------------------------------------------------------------------


/*
=================================================
	BubbleSort
=================================================
*/
#define BubbleSort( _count_, _cmp_, _swap_ )									\
	{																			\
		const int	_sf_count	= int(_count_);									\
		for (int _sf_i = _sf_count-2; _sf_i >= 0; --_sf_i)						\
		{																		\
			for (int _sf_j = 0; _sf_j <= _sf_i; ++_sf_j)						\
			{																	\
				if ( _cmp_( _sf_j, _sf_j+1 ))									\
				{																\
					_swap_( _sf_j, _sf_j+1 );									\
				}																\
			}																	\
		}																		\
	}

/*
=================================================
	InsertionSort
=================================================
*/
#define InsertionSort( _count_, _cmp_, _set_, _set2_ )							\
	{																			\
		const int	_sf_count	= int(_count_);									\
		for (int _sf_i = 0; _sf_i < _sf_count; ++_sf_i)							\
		{																		\
			if ( _cmp_( _sf_i ))												\
			{																	\
				for (int _sf_j = _sf_count-1; _sf_j > _sf_i; --_sf_j)			\
				{																\
					_set_( _sf_j, _sf_j-1 );									\
				}																\
				_set2_( _sf_i );												\
				break;															\
			}																	\
		}																		\
	}
