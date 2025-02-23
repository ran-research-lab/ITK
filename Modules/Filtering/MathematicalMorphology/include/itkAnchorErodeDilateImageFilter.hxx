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
#ifndef itkAnchorErodeDilateImageFilter_hxx
#define itkAnchorErodeDilateImageFilter_hxx


#include "itkAnchorUtilities.h"
namespace itk
{
template <typename TImage, typename TKernel, typename TFunction1>
AnchorErodeDilateImageFilter<TImage, TKernel, TFunction1>::AnchorErodeDilateImageFilter()
  : m_Boundary(NumericTraits<InputImagePixelType>::ZeroValue())
{
  this->DynamicMultiThreadingOn();
}

template <typename TImage, typename TKernel, typename TFunction1>
void
AnchorErodeDilateImageFilter<TImage, TKernel, TFunction1>::DynamicThreadedGenerateData(
  const InputImageRegionType & outputRegionForThread)
{
  // check that we are using a decomposable kernel
  if (!this->GetKernel().GetDecomposable())
  {
    itkExceptionMacro("Anchor morphology only works with decomposable structuring elements");
  }
  // TFunction1 will be < for erosions
  // TFunction2 will be <=

  AnchorLineType AnchorLine;

  // the initial version will adopt the methodology of loading a line
  // at a time into a buffer vector, carrying out the opening or
  // closing, and then copy the result to the output. Hopefully this
  // will improve cache performance when working along non raster
  // directions.

  InputImageConstPointer input = this->GetInput();

  InputImageRegionType IReg = outputRegionForThread;
  IReg.PadByRadius(this->GetKernel().GetRadius());
  IReg.Crop(this->GetInput()->GetRequestedRegion());

  // allocate an internal buffer
  auto internalbuffer = InputImageType::New();
  internalbuffer->SetRegions(IReg);
  internalbuffer->Allocate();
  InputImagePointer output = internalbuffer;

  // get the region size
  InputImageRegionType OReg = outputRegionForThread;
  // maximum buffer length is sum of dimensions
  unsigned int bufflength = 0;
  for (unsigned int i = 0; i < TImage::ImageDimension; ++i)
  {
    bufflength += IReg.GetSize()[i];
  }

  // compat
  bufflength += 2;

  std::vector<InputImagePixelType> buffer(bufflength);
  std::vector<InputImagePixelType> inbuffer(bufflength);

  // iterate over all the structuring elements
  typename KernelType::DecompType decomposition = this->GetKernel().GetLines();
  BresType                        BresLine;

  for (unsigned int i = 0; i < decomposition.size(); ++i)
  {
    typename KernelType::LType     ThisLine = decomposition[i];
    typename BresType::OffsetArray TheseOffsets = BresLine.BuildLine(ThisLine, bufflength);

    using KernelLType = typename KernelType::LType;

    unsigned int SELength = GetLinePixels<KernelLType>(ThisLine);

    // want lines to be odd
    if (!(SELength % 2))
    {
      ++SELength;
    }

    InputImageRegionType BigFace = MakeEnlargedFace<InputImageType, KernelLType>(input, IReg, ThisLine);

    AnchorLine.SetSize(SELength);

    DoAnchorFace<TImage, BresType, AnchorLineType, KernelLType>(
      input, output, m_Boundary, ThisLine, AnchorLine, TheseOffsets, inbuffer, buffer, IReg, BigFace);
    // after the first pass the input will be taken from the output
    input = internalbuffer;
  }

  // copy internal buffer to output
  using IterType = ImageRegionIterator<InputImageType>;
  IterType oit(this->GetOutput(), OReg);
  IterType iit(internalbuffer, OReg);
  for (oit.GoToBegin(), iit.GoToBegin(); !oit.IsAtEnd(); ++oit, ++iit)
  {
    oit.Set(iit.Get());
  }
}

template <typename TImage, typename TKernel, typename TFunction1>
void
AnchorErodeDilateImageFilter<TImage, TKernel, TFunction1>::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Boundary: " << m_Boundary << std::endl;
}

} // end namespace itk

#endif
