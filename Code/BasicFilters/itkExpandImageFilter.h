/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkExpandImageFilter.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

Copyright (c) 2001 Insight Consortium
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

 * The name of the Insight Consortium, nor the names of any consortium members,
   nor of any contributors, may be used to endorse or promote products derived
   from this software without specific prior written permission.

  * Modified source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS ``AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/
#ifndef __itkExpandImageFilter_h
#define __itkExpandImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkInterpolateImageFunction.h"
#include "itkLinearInterpolateImageFunction.h"

namespace itk
{

/** \class ExpandImageFilter
 * \brief Expand the size of an image by an integer factor in each
 * dimension.
 *
 * ExpandImageFilter increases the size of an image by an integer
 * factor in each dimension using a interpolation method.
 * The output image size in each dimension is given by:
 *
 * OutputSize[j] = InputSize[j] * ExpandFactors[j]
 *
 * The output values are obtained by interpolating the input image.
 * The default interpolation type used is the LinearInterpolateImageFunction.
 * The user can specify a particular interpolation function via
 * SetInterpolator(). Note that the input interpolator must derive
 * from base class InterpolateImageFunction.
 *
 * When the LargestPossibleRegion is requested, the output image will
 * contain padding at the upper edge of each dimension. The width
 * of padding in the i'th dimension is (ExpandFactors[i] - 1). Users can
 * specify the padding value used by setting the EdgePaddingValue.
 *
 * This filter will produce an output with different pixel spacing
 * that its input image such that:
 *
 * OutputSpacing[j] = InputSpacing[j] / ExpandFactors[j]
 *
 * The filter is templated over the input image type and the output 
 * image type.
 *
 * This filter is implemented as a multithreaded filter and supports
 * streaming.
 *
 * \warning This filter only works for image with scalar pixel types.
 * For vector images use VectorExpandImageFilter.
 *
 * This filter assumes that the input and output image has the same
 * number of dimensions.
 *
 * \sa InterpolateImageFunction
 * \sa LinearInterpolationImageFunction
 * \sa VectorExpandImageFilter
 *
 * \ingroup GeometricTransform
 */
template <
class TInputImage, 
class TOutputImage 
>
class ITK_EXPORT ExpandImageFilter:
  public ImageToImageFilter<TInputImage,TOutputImage>
{
public:

  /**
   * Standard "Self" typedef.
   */
  typedef ExpandImageFilter         Self;

  /**
   * Standard "Superclass" typedef.
   */
  typedef ImageToImageFilter<TInputImage,TOutputImage>  Superclass;

  /** 
   * Smart pointer typedef support.
   */
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /**
   * Method for creation through the object factory.
   */
  itkNewMacro(Self);  

  /**
   * Typedef to describe the output image region type.
   */
  typedef typename TOutputImage::RegionType OutputImageRegionType;

  /** 
   * Run-time type information (and related methods).
   */
  itkTypeMacro(ExpandImageFilter, ImageToImageFilter);

  /**
   * ImageDimension enumeration
   */
  enum { ImageDimension = TInputImage::ImageDimension };

  /**
   * Inherit some types from superclass
   */
  typedef typename Superclass::InputImageType  InputImageType;
  typedef typename Superclass::OutputImageType OutputImageType;
  typedef typename OutputImageType::PixelType  OutputPixelType;

  /**
   * Typedef support for the interpolation function
   */
  typedef InterpolateImageFunction<InputImageType> 
    InterpolatorType;
  typedef typename InterpolatorType::Pointer InterpolatorPointer;
  typedef LinearInterpolateImageFunction<InputImageType> 
    DefaultInterpolatorType;

  /**
   * Set the interpolator function.
   */
  itkSetObjectMacro( Interpolator, InterpolatorType );

  /**
   * Get a pointer to the interpolator function.
   */
  itkGetObjectMacro( Interpolator, InterpolatorType );

  /**
   * Set the expand factors. Values are clamped to 
   * a minimum value of 1. Default is 1 for all dimensions.
   */
  virtual void SetExpandFactors( const unsigned int factors[] );
  virtual void SetExpandFactors( const unsigned int factor );

  /** 
   * Get the expand factors.
   */
  virtual const unsigned int * GetExpandFactors() const
    { return m_ExpandFactors; }

  /**
   * Set the edge padding value. The default is zero.
   */
  itkSetMacro( EdgePaddingValue, OutputPixelType );

  /**
   * Get the edge padding value
   */
  itkGetMacro( EdgePaddingValue, OutputPixelType );

  /**
   * ExpandImageFilter produces an image which is a different resolution and
   * with a different pixel spacing than its input image.  As such,
   * ExpandImageFilter needs to provide an implementation for
   * UpdateOutputInformation() in order to inform the pipeline execution model.
   * The original documentation of this method is below.
   *
   * \sa ProcessObject::GenerateOutputInformaton()
   */
  virtual void GenerateOutputInformation();

  /**
   * ExpandImageFilter needs a smaller input requested region than the output
   * requested region.  As such, ShrinkImageFilter needs to provide an 
   * implementation for GenerateInputRequestedRegion() in order to inform 
   * the pipeline execution model.  
   *
   * \sa ProcessObject::GenerateInputRequestedRegion()
   */
  virtual void GenerateInputRequestedRegion();

protected:

  ExpandImageFilter();
  ~ExpandImageFilter() {};
  ExpandImageFilter(const Self&) {}
  void operator=(const Self&) {}
  void PrintSelf(std::ostream& os, Indent indent) const;

  /**
   * ExpandImageFilter is implemented as a multithreaded filter.  Therefore,
   * this implementation provides a ThreadedGenerateData() routine which
   * is called for each processing thread. The output image data is allocated
   * automatically by the superclass prior to calling ThreadedGenerateData().
   * ThreadedGenerateData can only write to the portion of the output image
   * specified by the parameter "outputRegionForThread"
   *
   * \sa ImageToImageFilter::ThreadedGenerateData(),
   *     ImageToImageFilter::GenerateData()
   */
  virtual
  void ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread,
                            int threadId );
  
  /**
   * This method is used to set the state of the filter before 
   * multi-threading.
   */
  virtual void BeforeThreadedGenerateData();


private:

  unsigned int           m_ExpandFactors[ImageDimension];
  InterpolatorPointer    m_Interpolator;
  OutputPixelType        m_EdgePaddingValue;
 
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkExpandImageFilter.txx"
#endif

#endif
