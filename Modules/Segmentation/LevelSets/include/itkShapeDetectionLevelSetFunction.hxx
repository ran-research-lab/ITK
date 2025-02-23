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
#ifndef itkShapeDetectionLevelSetFunction_hxx
#define itkShapeDetectionLevelSetFunction_hxx

#include "itkImageRegionIterator.h"
#include "itkImageAlgorithm.h"

namespace itk
{
template <typename TImageType, typename TFeatureImageType>
void
ShapeDetectionLevelSetFunction<TImageType, TFeatureImageType>::CalculateSpeedImage()
{
  /* copy the feature image into the speed image */
  ImageAlgorithm::Copy(this->GetFeatureImage(),
                       this->GetSpeedImage(),
                       this->GetFeatureImage()->GetRequestedRegion(),
                       this->GetFeatureImage()->GetRequestedRegion());
}
} // end namespace itk

#endif
