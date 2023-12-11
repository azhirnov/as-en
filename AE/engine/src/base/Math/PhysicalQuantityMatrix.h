// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Matrix.h"
#include "base/Math/PhysicalQuantityVec.h"

namespace AE::Math
{

    template <typename       Quantity,
              uint           Columns,
              uint           Rows,
              glm::qualifier Q
             >
    struct PhysicalQuantityMatrix;

    template <typename       Quantity,
              uint           Columns,
              uint           Rows
             >
    using PhysicalQuantitySIMDMatrix = PhysicalQuantityMatrix< Quantity, Columns, Rows, GLMSimdQualifier >;

    template <typename       Quantity,
              uint           Columns,
              uint           Rows
             >
    using PhysicalQuantityPackedMatrix = PhysicalQuantityMatrix< Quantity, Columns, Rows, GLMPackedQualifier >;

    template <uint           Columns,
              uint           Rows,
              typename       ValueType,
              typename       Dimension,
              typename       ValueScale,
              glm::qualifier Q
             >
    using TPhysicalQuantityMatrix = PhysicalQuantityMatrix< PhysicalQuantity<ValueType, Dimension, ValueScale>, Columns, Rows, Q >;

} // AE::Math


#define Columns 2
#define Rows    2
#include "PhysicalQuantityMatrixImpl.h"
#undef  Columns
#undef  Rows

#define Columns 2
#define Rows    3
#include "PhysicalQuantityMatrixImpl.h"
#undef  Columns
#undef  Rows

#define Columns 2
#define Rows    4
#include "PhysicalQuantityMatrixImpl.h"
#undef  Columns
#undef  Rows

#define Columns 3
#define Rows    2
#include "PhysicalQuantityMatrixImpl.h"
#undef  Columns
#undef  Rows

#define Columns 3
#define Rows    3
#include "PhysicalQuantityMatrixImpl.h"
#undef  Columns
#undef  Rows

#define Columns 3
#define Rows    4
#include "PhysicalQuantityMatrixImpl.h"
#undef  Columns
#undef  Rows

#define Columns 4
#define Rows    2
#include "PhysicalQuantityMatrixImpl.h"
#undef  Columns
#undef  Rows

#define Columns 4
#define Rows    3
#include "PhysicalQuantityMatrixImpl.h"
#undef  Columns
#undef  Rows

#define Columns 4
#define Rows    4
#include "PhysicalQuantityMatrixImpl.h"
#undef  Columns
#undef  Rows


namespace AE::Math
{
/*
=================================================
    PhysicalQuantity_FromMat
=================================================
*/
    namespace _hidden_
    {
        template <typename       MatType,
                  typename       Dimension,
                  typename       ValueScale,
                  glm::qualifier Q
                 >
        struct _PhysicalQuantity_FromMat;

        template <typename       T,
                  uint           Columns,
                  uint           Rows,
                  typename       Dimension,
                  typename       ValueScale,
                  glm::qualifier Q
                 >
        struct _PhysicalQuantity_FromMat< TMatrix<T,Columns,Rows,Q>, Dimension, ValueScale, Q >
        {
            using type  = TPhysicalQuantityMatrix< Columns, Rows, T, Dimension, ValueScale, Q >;
        };
    }
    template <typename       MatType,
              typename       Dimension,
              typename       ValueScale = PhysicalQuantity_Scale::Integer< typename MatType::Value_t, 1 >,
              glm::qualifier Q          = GLMSimdQualifier
             >
    using PhysicalQuantity_FromMat = typename Math::_hidden_::_PhysicalQuantity_FromMat< MatType, Dimension, ValueScale, Q >::type;

/*
=================================================
    PhysicalQuantityVec_ToScale
=================================================
*
namespace _hidden_
{
    template <typename NewScale, typename ValueType, uint C, uint R, uint Q
              typename Dimension, typename Scale, glm::qualifier Q
             >
    ND_ TMatrix<ValueType, Columns, Rows, Qf>
        PhysicalQuantityMatrix_ToScale (const TPhysicalQuantityMatrix< C, R, ValueType, Dimension, Scale, Qf > &value) __NE___
    {
        const auto  scale = Scale::Value / NewScale::Value;
        return value.GetNonScaled() * TMatrix<ValueType, Columns, Rows, Qf>::FromScalar{ scale };
    }
}
/*
=================================================
    operator * (mat, mat)
=================================================
*
    template <typename ValueType, typename Dimension, uint C, uint R, uint Q
              typename LhsScale, typename RhsScale, glm::qualifier Qf
             >
    ND_ auto  operator * (const TPhysicalQuantityMatrix< Columns, Rows, ValueType, Dimension, LhsScale, Qf >&  lhs,
                          const TPhysicalQuantityMatrix< Columns, Rows, ValueType, Dimension, RhsScale, Qf >&  rhs) __NE___
    {
        using Scale = PhysicalQuantity_Scale::template Mul< LhsScale, RhsScale >;
        using Type  = PhysicalQuantity< ValueType, Dimension, Scale >;

        return PhysicalQuantityMatrix< Type, Columns, Rows, Q >{ Scale::Get( lhs.GetNonScaled(), rhs.GetNonScaled() )};
    }
*/

} // AE::Math


namespace AE::Base
{
    template <typename Qt, uint C, uint R, glm::qualifier Ql>
    struct TMemCopyAvailable< PhysicalQuantityMatrix<Qt,C,R,Ql> >       { static constexpr bool  value = IsMemCopyAvailable<Qt>; };

    template <typename Qt, uint C, uint R, glm::qualifier Ql>
    struct TZeroMemAvailable< PhysicalQuantityMatrix<Qt,C,R,Ql> >       { static constexpr bool  value = IsZeroMemAvailable<Qt>; };

    // 'IsTriviallySerializable< PhysicalQuantityMatrix<> > = false' - because SIMD and packed types has different alignment

} // AE::Base

