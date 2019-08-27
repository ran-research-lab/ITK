/*=========================================================================
 *
 *  Copyright Insight Software Consortium
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
#include "itkLevelSetContainer.h"
#include "itkLevelSetEquationChanAndVeseExternalTerm.h"
#include "itkLevelSetEquationTermContainer.h"
#include "itkLevelSetEquationContainer.h"
#include "itkSinRegularizedHeavisideStepFunction.h"
#include "itkLevelSetEvolution.h"
#include "itkBinaryImageToLevelSetImageAdaptor.h"
#include "itkLevelSetEquationCurvatureTerm.h"
#include "itkLevelSetEvolutionNumberOfIterationsStoppingCriterion.h"

int
itkSingleLevelSetWhitakerImage2DWithCurvatureTest(int argc, char * argv[])
{
  if (argc < 2)
  {
    std::cerr << "Missing Arguments" << std::endl;
    return EXIT_FAILURE;
  }

  constexpr unsigned int Dimension = 2;

  using InputPixelType = unsigned short;
  using InputImageType = itk::Image<InputPixelType, Dimension>;
  using InputIteratorType = itk::ImageRegionIteratorWithIndex<InputImageType>;
  using ReaderType = itk::ImageFileReader<InputImageType>;

  using PixelType = float;
  using OffsetType = InputImageType::OffsetType;

  using SparseLevelSetType = itk::WhitakerSparseLevelSetImage<PixelType, Dimension>;
  using BinaryToSparseAdaptorType = itk::BinaryImageToLevelSetImageAdaptor<InputImageType, SparseLevelSetType>;

  using IdentifierType = itk::IdentifierType;

  using LevelSetContainerType = itk::LevelSetContainer<IdentifierType, SparseLevelSetType>;

  using ChanAndVeseInternalTermType =
    itk::LevelSetEquationChanAndVeseInternalTerm<InputImageType, LevelSetContainerType>;
  using ChanAndVeseExternalTermType =
    itk::LevelSetEquationChanAndVeseExternalTerm<InputImageType, LevelSetContainerType>;
  using CurvatureTermType = itk::LevelSetEquationCurvatureTerm<InputImageType, LevelSetContainerType>;
  using TermContainerType = itk::LevelSetEquationTermContainer<InputImageType, LevelSetContainerType>;

  using EquationContainerType = itk::LevelSetEquationContainer<TermContainerType>;

  using LevelSetEvolutionType = itk::LevelSetEvolution<EquationContainerType, SparseLevelSetType>;

  using LevelSetOutputRealType = SparseLevelSetType::OutputRealType;
  using HeavisideFunctionBaseType =
    itk::SinRegularizedHeavisideStepFunction<LevelSetOutputRealType, LevelSetOutputRealType>;
  using InputIteratorType = itk::ImageRegionIteratorWithIndex<InputImageType>;

  // load binary mask
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(argv[1]);
  reader->Update();
  InputImageType::Pointer input = reader->GetOutput();

  // Binary initialization
  InputImageType::Pointer binary = InputImageType::New();
  binary->SetRegions(input->GetLargestPossibleRegion());
  binary->CopyInformation(input);
  binary->Allocate();
  binary->FillBuffer(itk::NumericTraits<InputPixelType>::ZeroValue());

  InputImageType::RegionType region;
  InputImageType::IndexType  index;
  InputImageType::SizeType   size;

  index.Fill(10);
  size.Fill(30);

  region.SetIndex(index);
  region.SetSize(size);

  InputIteratorType iIt(binary, region);
  iIt.GoToBegin();
  while (!iIt.IsAtEnd())
  {
    iIt.Set(itk::NumericTraits<InputPixelType>::OneValue());
    ++iIt;
  }

  // Convert binary mask to sparse level set
  BinaryToSparseAdaptorType::Pointer adaptor = BinaryToSparseAdaptorType::New();
  adaptor->SetInputImage(binary);
  adaptor->Initialize();
  std::cout << "Finished converting to sparse format" << std::endl;

  SparseLevelSetType::Pointer level_set = adaptor->GetModifiableLevelSet();


  input->TransformPhysicalPointToIndex(binary->GetOrigin(), index);
  OffsetType offset;
  for (unsigned int i = 0; i < Dimension; i++)
  {
    offset[i] = index[i];
  }
  level_set->SetDomainOffset(offset);

  // Define the Heaviside function
  HeavisideFunctionBaseType::Pointer heaviside = HeavisideFunctionBaseType::New();
  heaviside->SetEpsilon(1.0);

  // Insert the levelsets in a levelset container
  LevelSetContainerType::Pointer lscontainer = LevelSetContainerType::New();
  lscontainer->SetHeaviside(heaviside);

  bool LevelSetNotYetAdded = lscontainer->AddLevelSet(0, level_set, false);
  if (!LevelSetNotYetAdded)
  {
    return EXIT_FAILURE;
  }
  std::cout << "Level set container created" << std::endl;

  // **************** CREATE ALL TERMS ****************

  // -----------------------------
  // *** 1st Level Set phi ***

  // Create ChanAndVese internal term for phi_{1}
  ChanAndVeseInternalTermType::Pointer cvInternalTerm0 = ChanAndVeseInternalTermType::New();
  cvInternalTerm0->SetInput(input);
  cvInternalTerm0->SetCoefficient(1.0);
  std::cout << "LevelSet 1: CV internal term created" << std::endl;

  // Create ChanAndVese external term for phi_{1}
  ChanAndVeseExternalTermType::Pointer cvExternalTerm0 = ChanAndVeseExternalTermType::New();
  cvExternalTerm0->SetInput(input);
  cvExternalTerm0->SetCoefficient(1.0);
  std::cout << "LevelSet 1: CV external term created" << std::endl;

  // Create ChanAndVese curvature term for phi_{1}
  CurvatureTermType::Pointer curvatureTerm0 = CurvatureTermType::New();
  curvatureTerm0->SetInput(input);
  curvatureTerm0->SetCoefficient(1.0);
  std::cout << "LevelSet 1: Curvature term created" << std::endl;

  // **************** CREATE ALL EQUATIONS ****************

  // Create Term Container
  TermContainerType::Pointer termContainer0 = TermContainerType::New();
  termContainer0->SetInput(input);
  termContainer0->SetCurrentLevelSetId(0);
  termContainer0->SetLevelSetContainer(lscontainer);

  termContainer0->AddTerm(0, cvInternalTerm0);
  termContainer0->AddTerm(1, cvExternalTerm0);
  termContainer0->AddTerm(2, curvatureTerm0);
  std::cout << "Term container 0 created" << std::endl;

  EquationContainerType::Pointer equationContainer = EquationContainerType::New();
  equationContainer->SetLevelSetContainer(lscontainer);
  equationContainer->AddEquation(0, termContainer0);

  using StoppingCriterionType = itk::LevelSetEvolutionNumberOfIterationsStoppingCriterion<LevelSetContainerType>;
  StoppingCriterionType::Pointer criterion = StoppingCriterionType::New();
  criterion->SetNumberOfIterations(5);

  LevelSetEvolutionType::Pointer evolution = LevelSetEvolutionType::New();
  evolution->SetEquationContainer(equationContainer);

  if (evolution->GetEquationContainer() != equationContainer)
  {
    std::cerr << "evolution->GetEquationContainer() != equationContainer" << std::endl;

    return EXIT_FAILURE;
  }
  std::cout << "evolution->GetEquationContainer() == equationContainer" << std::endl;

  evolution->SetStoppingCriterion(criterion);

  if (evolution->GetStoppingCriterion() != criterion)
  {
    std::cerr << "evolution->GetStoppingCriterion( ) != criterion" << std::endl;

    return EXIT_FAILURE;
  }
  std::cout << "evolution->GetStoppingCriterion( ) == criterion" << std::endl;

  evolution->SetLevelSetContainer(lscontainer);

  if (evolution->GetLevelSetContainer() != lscontainer)
  {
    std::cerr << "evolution->GetLevelSetContainer( ) != lscontainer" << std::endl;

    return EXIT_FAILURE;
  }
  std::cerr << "evolution->GetLevelSetContainer( ) == lscontainer" << std::endl;


  try
  {
    evolution->Update();
  }
  catch (itk::ExceptionObject & err)
  {
    std::cout << err << std::endl;
    return EXIT_FAILURE;
  }

  if (evolution->GetNumberOfIterations() != 5)
  {
    std::cerr << "evolution->GetNumberOfIterations() != 5" << std::endl;
    std::cerr << "evolution->GetNumberOfIterations() = " << evolution->GetNumberOfIterations() << std::endl;

    return EXIT_FAILURE;
  }

  if (itk::Math::NotAlmostEquals(evolution->GetAlpha(), 0.9))
  {
    std::cerr << "evolution->GetAlpha() != 0.9" << std::endl;

    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
