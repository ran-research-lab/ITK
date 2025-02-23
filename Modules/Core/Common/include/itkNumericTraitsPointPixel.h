/*=========================================================================
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef itkNumericTraitsPointPixel_h
#define itkNumericTraitsPointPixel_h

#include "itkNumericTraits.h"
#include "itkPoint.h"

namespace itk
{
/**
 * \brief NumericTraits for Point
 * \tparam T Component type for Point
 * \tparam D Dimension of the space
 */
template <typename T, unsigned int D>
class NumericTraits<Point<T, D>>
{
private:
  using ElementAbsType = typename NumericTraits<T>::AbsType;
  using ElementAccumulateType = typename NumericTraits<T>::AccumulateType;
  using ElementFloatType = typename NumericTraits<T>::FloatType;
  using ElementPrintType = typename NumericTraits<T>::PrintType;
  using ElementRealType = typename NumericTraits<T>::RealType;

public:
  /** Return the type of the native component type. */
  using ValueType = T;
  using Self = Point<T, D>;

  /** Unsigned component type */
  using AbsType = Point<ElementAbsType, D>;

  /** Accumulation of addition and multiplication. */
  using AccumulateType = Point<ElementAccumulateType, D>;

  /** Typedef for operations that use floating point instead of real precision
   */
  using FloatType = Point<ElementFloatType, D>;

  /** Return the type that can be printed. */
  using PrintType = Point<ElementPrintType, D>;

  /** Type for real-valued scalar operations. */
  using RealType = Point<ElementRealType, D>;

  /** Type for real-valued scalar operations. */
  using ScalarRealType = ElementRealType;

  /** Measurement vector type */
  using MeasurementVectorType = Self;

  /** Component wise defined element
   *
   * \note minimum value for floating pointer types is defined as
   * minimum positive normalize value.
   */
  static const Self
  max(const Self &)
  {
    return MakeFilled<Self>(NumericTraits<T>::max());
  }

  static const Self
  min(const Self &)
  {
    return MakeFilled<Self>(NumericTraits<T>::min());
  }

  static const Self
  max()
  {
    return MakeFilled<Self>(NumericTraits<T>::max());
  }

  static const Self
  min()
  {
    return MakeFilled<Self>(NumericTraits<T>::min());
  }

  static const Self
  NonpositiveMin()
  {
    return MakeFilled<Self>(NumericTraits<T>::NonpositiveMin());
  }

  static const Self
  ZeroValue()
  {
    return MakeFilled<Self>(NumericTraits<T>::ZeroValue());
  }

  static const Self
  OneValue()
  {
    return MakeFilled<Self>(NumericTraits<T>::OneValue());
  }

  static const Self
  NonpositiveMin(const Self &)
  {
    return NonpositiveMin();
  }

  static const Self
  ZeroValue(const Self &)
  {
    return ZeroValue();
  }

  static const Self
  OneValue(const Self &)
  {
    return OneValue();
  }

  static constexpr bool IsSigned = std::is_signed<ValueType>::value;
  static constexpr bool IsInteger = std::is_integral<ValueType>::value;
  static constexpr bool IsComplex = NumericTraits<ValueType>::IsComplex;

  /** Fixed length vectors cannot be resized, so an exception will
   *  be thrown if the input size is not valid.  If the size is valid
   *  the vector will be filled with zeros. */
  static void
  SetLength(Point<T, D> & m, const unsigned int s)
  {
    if (s != D)
    {
      itkGenericExceptionMacro(<< "Cannot set the size of a Point of length " << D << " to " << s);
    }
    m.Fill(NumericTraits<T>::ZeroValue());
  }

  /** Return the dimensionality of the point. */
  static unsigned int
  GetLength(const Point<T, D> &)
  {
    return D;
  }

  /** Return the dimensionality of the point. */
  static unsigned int
  GetLength()
  {
    return D;
  }

  static void
  AssignToArray(const Self & v, MeasurementVectorType & mv)
  {
    mv = v;
  }

  template <typename TArray>
  static void
  AssignToArray(const Self & v, TArray & mv)
  {
    for (unsigned int i = 0; i < D; ++i)
    {
      mv[i] = v[i];
    }
  }

  /** \note: the functions are preferred over the member variables as
   * they are defined for all partial specialization
   */
  static const Self ITKCommon_EXPORT Zero;
  static const Self ITKCommon_EXPORT One;
};
} // end namespace itk

#endif // itkNumericTraitsPointPixel_h
