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

#include "itkImageFileReader.h"
#include "itkCentralDifferenceImageFunction.h"
#include "itkTestingMacros.h"

int
itkOrientedImage3DTest(int argc, char * argv[])
{

  if (argc < 20)
  {
    std::cerr << "Usage: " << itkNameOfTestExecutableMacro(argv) << " InputImage  "
              << "corner1x corner1y corner1z "
              << "corner2x corner2y corner2z "
              << "corner3x corner3y corner3z "
              << "corner4x corner4y corner4z "
              << "derivative1x derivative1y derivative1z "
              << "derivative2x derivative2y derivative2z" << std::endl;
    return EXIT_FAILURE;
  }

  constexpr unsigned int Dimension = 3;
  using PixelType = unsigned char;

  using ImageType = itk::Image<PixelType, Dimension>;
  using ReaderType = itk::ImageFileReader<ImageType>;

  using IndexType = ImageType::IndexType;
  using IndexValueType = IndexType::IndexValueType;

  auto reader = ReaderType::New();

  reader->SetFileName(argv[1]);

  try
  {
    reader->Update();
  }
  catch (const itk::ExceptionObject & e)
  {
    std::cerr << e << std::endl;
    return EXIT_FAILURE;
  }

  ImageType::ConstPointer image = reader->GetOutput();

  ImageType::DirectionType directionCosines = image->GetDirection();

  std::cout << directionCosines << std::endl;

  unsigned int element = 2;
  const double tolerance = 1e-3;

  ImageType::RegionType region = image->GetLargestPossibleRegion();
  ImageType::SizeType   size = region.GetSize();

  constexpr int numberOfPointsToTest = 4;

  ImageType::IndexType index[numberOfPointsToTest];
  ImageType::PointType physicalPoint;

  index[0][0] = 0;
  index[0][1] = 0;
  index[0][2] = 0;

  index[1][0] = size[0];
  index[1][1] = 0;
  index[1][2] = 0;

  index[2][0] = 0;
  index[2][1] = size[1];
  index[2][2] = 0;

  index[3][0] = 0;
  index[3][1] = 0;
  index[3][2] = size[2];


  image->Print(std::cout);
  std::cout << std::endl;
  std::cout << std::endl;

  for (int pointId = 0; pointId < numberOfPointsToTest; ++pointId)
  {

    image->TransformIndexToPhysicalPoint(index[pointId], physicalPoint);

    std::cout << index[pointId] << " : " << physicalPoint << std::endl;

    for (unsigned int dim = 0; dim < Dimension; ++dim)
    {
      const double expectedValue = std::stod(argv[element++]);
      const double currentValue = physicalPoint[dim];
      const double difference = currentValue - expectedValue;
      if (itk::Math::abs(difference) > tolerance)
      {
        std::cerr << "Error: " << std::endl;
        std::cerr << "in Point # " << pointId << std::endl;
        std::cerr << "Expected      = " << expectedValue << std::endl;
        std::cerr << "Read          = " << currentValue << std::endl;
        std::cerr << "Index         = " << index[pointId] << std::endl;
        std::cerr << "PhysicalPoint = " << physicalPoint << std::endl;
      }
    }
  }

  //
  // Select a point in the middle of the image and compute its
  // derivative using the image orientation.
  //
  IndexType centralIndex;
  centralIndex[0] = static_cast<IndexValueType>(size[0] / 2.0);
  centralIndex[1] = static_cast<IndexValueType>(size[1] / 2.0);
  centralIndex[2] = static_cast<IndexValueType>(size[2] / 2.0);

  using CentralDifferenceImageFunctionType = itk::CentralDifferenceImageFunction<ImageType, double>;

  auto gradientHelper1 = CentralDifferenceImageFunctionType::New();
  gradientHelper1->SetInputImage(image);

  std::cout << std::endl;
  std::cout << std::endl;
  std::cout << "Image Direction" << std::endl;
  std::cout << image->GetDirection() << std::endl;

  { // Compute gradient value without taking image direction into account
    gradientHelper1->UseImageDirectionOff();
    CentralDifferenceImageFunctionType::OutputType gradient1a = gradientHelper1->EvaluateAtIndex(centralIndex);

    std::cout << "Gradient without Direction" << std::endl;
    std::cout << gradient1a << std::endl;

    for (unsigned int dim = 0; dim < Dimension; ++dim)
    {
      const double expectedValue = std::stod(argv[element++]);
      const double currentValue = gradient1a[dim];
      const double difference = currentValue - expectedValue;
      if (itk::Math::abs(difference) > tolerance)
      {
        std::cerr << "Error: " << std::endl;

        std::cerr << "Expected      = " << expectedValue << std::endl;
        std::cerr << "Read          = " << currentValue << std::endl;
        return EXIT_FAILURE;
      }
    }
  }


  { // Compute gradient value taking image direction into account
    gradientHelper1->UseImageDirectionOn();
    CentralDifferenceImageFunctionType::OutputType gradient1b = gradientHelper1->EvaluateAtIndex(centralIndex);

    std::cout << std::endl;
    std::cout << "Gradient with Direction" << std::endl;
    std::cout << gradient1b << std::endl;

    for (unsigned int dim = 0; dim < Dimension; ++dim)
    {
      const double expectedValue = std::stod(argv[element++]);
      const double currentValue = gradient1b[dim];
      const double difference = currentValue - expectedValue;
      if (itk::Math::abs(difference) > tolerance)
      {
        std::cerr << "Error: " << std::endl;
        std::cerr << "Expected      = " << expectedValue << std::endl;
        std::cerr << "Read          = " << currentValue << std::endl;
        return EXIT_FAILURE;
      }
    }
  }

  return EXIT_SUCCESS;
}
