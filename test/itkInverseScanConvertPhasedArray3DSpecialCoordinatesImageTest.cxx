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

#include <iostream>
#include <iomanip>

#include "itkMath.h"
#include "itkPhasedArray3DSpecialCoordinatesImage.h"
#include "itkResampleImageFilter.h"
#include "itkGaborImageSource.h"
#include "itkImageFileWriter.h"

int itkInverseScanConvertPhasedArray3DSpecialCoordinatesImageTest( int argc, char* argv[] )
{
  if( argc < 2 )
    {
    std::cerr << "Usage: " << argv[0] << "outputImageFile" << std::endl;
    return EXIT_FAILURE;
    }
  const char * outputImageFile = argv[1];

  const unsigned int Dimension = 3;
  typedef float                                            PixelType;
  typedef itk::Image< PixelType, Dimension >                     InputImageType;
  typedef itk::PhasedArray3DSpecialCoordinatesImage< PixelType > OutputImageType;
  typedef double                                                 CoordRepType;


  typedef itk::GaborImageSource< InputImageType > SourceType;
  SourceType::Pointer source = SourceType::New();

  SourceType::ArrayType sigma;
  sigma[0] = 5.0;
  sigma[1] = 5.0;
  sigma[2] = 8.0;
  source->SetSigma( sigma );

  SourceType::ArrayType mean;
  mean.Fill( 0.0 );
  source->SetMean( mean );

  InputImageType::SizeType size;
  size.Fill( 128 );
  source->SetSize( size );

  InputImageType::SpacingType spacing;
  spacing[0] = 0.2;
  spacing[1] = 0.2;
  spacing[2] = 0.4;
  source->SetSpacing( spacing );

  InputImageType::PointType origin;
  for( unsigned int ii = 0; ii < Dimension; ++ii )
    {
    origin[ii] = -1 * spacing[ii] * size[ii]  / 2;
    }
  source->SetOrigin( origin );

  source->SetFrequency( 0.2 );

  source->SetCalculateImaginaryPart( true );

  try
    {
    source->Update();
    }
  catch( itk::ExceptionObject & error )
    {
    std::cerr << "Error while generating Gabor input: " << error << std::endl;
    return EXIT_FAILURE;
    }
  InputImageType::Pointer gaborInput = source->GetOutput();
  gaborInput->DisconnectPipeline();

  typedef itk::ResampleImageFilter< InputImageType, OutputImageType > ResamplerType;
  ResamplerType::Pointer resampler = ResamplerType::New();
  resampler->SetInput( gaborInput );

  OutputImageType::Pointer phasedArraySampledData = resampler->GetOutput();
  phasedArraySampledData->SetAzimuthAngularSeparation( 5.0*2.0*vnl_math::pi/360.0 );
  phasedArraySampledData->SetElevationAngularSeparation( 5.0*2.0*vnl_math::pi/360.0 );
  phasedArraySampledData->SetRadiusSampleSize( 0.5 );
  phasedArraySampledData->SetFirstSampleDistance( 2 );

  OutputImageType::SizeType outputSize;
  outputSize[0] = 64;
  outputSize[1] = 32;
  outputSize[2] = 16;
  resampler->SetSize( outputSize );

 typedef itk::ImageFileWriter< OutputImageType > WriterType;
 WriterType::Pointer writer = WriterType::New();
 writer->SetFileName( outputImageFile );
 writer->SetInput( resampler->GetOutput() );
 writer->Update();

 return EXIT_SUCCESS;

}
