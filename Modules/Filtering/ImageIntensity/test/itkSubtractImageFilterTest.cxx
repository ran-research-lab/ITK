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

#include "itkSubtractImageFilter.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkMath.h"
#include "itkTestingMacros.h"


int
itkSubtractImageFilterTest(int, char *[])
{

  // Define the dimension of the images
  constexpr unsigned int Dimension = 3;

  // Declare the pixel types of the images
  using PixelType = float;

  // Declare the types of the images
  using InputImageType1 = itk::Image<PixelType, Dimension>;
  using InputImageType2 = itk::Image<PixelType, Dimension>;
  using OutputImageType = itk::Image<PixelType, Dimension>;

  // Declare appropriate Iterator types for each image
  using OutputImageIteratorType = itk::ImageRegionIteratorWithIndex<OutputImageType>;


  // Declare the type of the index to access images
  using IndexType = itk::Index<Dimension>;

  // Declare the type of the size
  using SizeType = itk::Size<Dimension>;

  // Declare the type of the region
  using RegionType = itk::ImageRegion<Dimension>;

  // Create two images
  auto inputImageA = InputImageType1::New();
  auto inputImageB = InputImageType2::New();

  // Define their size, and start index
  SizeType size;
  size[0] = 2;
  size[1] = 2;
  size[2] = 2;

  IndexType start;
  start[0] = 0;
  start[1] = 0;
  start[2] = 0;

  RegionType region;
  region.SetIndex(start);
  region.SetSize(size);

  // Initialize Image A
  inputImageA->SetLargestPossibleRegion(region);
  inputImageA->SetBufferedRegion(region);
  inputImageA->SetRequestedRegion(region);
  inputImageA->Allocate();

  // Initialize Image B
  inputImageB->SetLargestPossibleRegion(region);
  inputImageB->SetBufferedRegion(region);
  inputImageB->SetRequestedRegion(region);
  inputImageB->Allocate();

  // Initialize the content of Image A
  constexpr InputImageType1::PixelType valueA = 2.0;
  inputImageA->FillBuffer(valueA);

  // Initialize the content of Image B
  constexpr InputImageType2::PixelType valueB = 3.0;
  inputImageB->FillBuffer(valueB);


  // Declare the type for the itk::SubtractImageFilter
  using FilterType = itk::SubtractImageFilter<InputImageType1, InputImageType2, OutputImageType>;

  // Create the filter
  auto filter = FilterType::New();

  ITK_EXERCISE_BASIC_OBJECT_METHODS(filter, SubtractImageFilter, BinaryGeneratorImageFilter);

  // Set the input images
  filter->SetInput1(inputImageA);
  filter->SetInput2(inputImageB);


  // Execute the filter
  ITK_TRY_EXPECT_NO_EXCEPTION(filter->Update());


  // Get the filter output
  OutputImageType::Pointer outputImage = filter->GetOutput();


  // Create an iterator for going through the image output
  OutputImageIteratorType oIt(outputImage, outputImage->GetBufferedRegion());

  // Check the content of the result image
  //
  const auto expectedValue = static_cast<OutputImageType::PixelType>(valueA - valueB);
  while (!oIt.IsAtEnd())
  {
    if (!itk::Math::ExactlyEquals(oIt.Get(), expectedValue))
    {
      std::cerr << "Test failed!" << std::endl;
      std::cerr << "Error in pixel value at index [" << oIt.GetIndex() << "]" << std::endl;
      std::cerr << "Expected: " << expectedValue << ", but got: " << oIt.Get() << std::endl;
      return EXIT_FAILURE;
    }
    ++oIt;
  }


  // All objects should be automatically destroyed at this point
  std::cout << "Test finished." << std::endl;
  return EXIT_SUCCESS;
}
