/*LICENSE_START*/
/*
 *  Copyright 1995-2002 Washington University School of Medicine
 *
 *  http://brainmap.wustl.edu
 *
 *  This file is part of CARET.
 *
 *  CARET is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  CARET is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CARET; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
/*LICENSE_END*/

#include <QDir>
#include <QRegExp>

#define __COMMAND_BASE_MAIN__
#include "CommandBase.h"
#undef __COMMAND_BASE_MAIN__
#include "CommandException.h"
#include "FileUtilities.h"
#include "GiftiNodeDataFile.h"
#include "NameIndexSort.h"
#include "NodeAttributeFile.h"
#include "ProgramParameters.h"
#include "StringUtilities.h"
#include "VolumeFile.h"

#include "CommandCaretFileCopy.h"
#include "CommandCaretFileNamingUnitTesting.h"  
#include "CommandCaretHelpCreateHtmlIndexFile.h"
#include "CommandCiftiCorrelationMatrix.h"
#include "CommandColorFileAddColor.h"
#include "CommandColorFileCreateMissingColors.h"
#include "CommandConvertDataFileToCaret6.h"
#include "CommandConvertSpecFileToCaret6.h"
#include "CommandCreateCiftiDenseTimeseries.h"
#include "CommandDataFileCompare.h"
#include "CommandDeformationMapApply.h"
#include "CommandDeformationMapApplyGenericNames.h"
#include "CommandDeformationMapCreate.h"
#include "CommandDeformationMapPathUpdate.h"
#include "CommandExtend.h"
#include "CommandFileConvert.h"
#include "CommandFileReadTime.h"
#include "CommandFileSubstitution.h"
#include "CommandGiftiInfo.h"
#include "CommandHelp.h"
#include "CommandHelpFull.h"
#include "CommandHelpGlobalOptions.h"
#include "CommandHelpHTML.h"
#include "CommandHelpPDF.h"
#include "CommandHelpSearch.h"
#include "CommandImageCombine.h"
#include "CommandImageCompare.h"
#include "CommandImageFormatConvert.h"
#include "CommandImageInsertText.h"
#include "CommandImageResize.h"
#include "CommandImageToWebPage.h"
#include "CommandImageView.h"
#include "CommandMetricClustering.h"
#include "CommandMetricComposite.h"
#include "CommandMetricCompositeIdentifiedColumns.h"
#include "CommandMetricCorrelationCoefficientMap.h"
#include "CommandMetricExtrema.h"
#include "CommandMetricFileCreate.h"
#include "CommandMetricGradient.h"
#include "CommandMetricGradientAll.h"
#include "CommandMetricROIGradient.h"
#include "CommandMetricROIMask.h"
#include "CommandMetricROISmoothing.h"
#include "CommandMetricInGroupDifference.h"
#include "CommandMetricInformation.h"
#include "CommandMetricMath.h"
#include "CommandMetricMathPostfix.h"
#include "CommandMetricMultipleCorrelationCoefficientMap.h"
#include "CommandMetricSetColumnName.h"
#include "CommandMetricSetColumnToScalar.h"
#include "CommandMetricSmoothing.h"
#include "CommandMetricStatisticsAnovaOneWay.h"
#include "CommandMetricStatisticsAnovaTwoWay.h"
#include "CommandMetricStatisticsCoordinateDifference.h"
#include "CommandMetricCorrelationMatrix.h"
#include "CommandMetricStatisticsInterhemisphericClusters.h"
#include "CommandMetricStatisticsKruskalWallis.h"
#include "CommandMetricStatisticsLeveneMap.h"
#include "CommandMetricStatisticsNormalization.h"
#include "CommandMetricStatisticsOneSampleTTest.h"
#include "CommandMetricStatisticsPairedTTest.h"
#include "CommandMetricStatisticsShuffledCrossCorrelationMaps.h"
#include "CommandMetricStatisticsShuffledTMap.h"
#include "CommandMetricStatisticsSubtraceGroupAverage.h"
#include "CommandMetricStatisticsTMap.h"
#include "CommandMetricStatisticsTwoSampleTTest.h"
#include "CommandMetricStatisticsZMap.h"
#include "CommandMetricTranspose.h"
#include "CommandMetricTwinComparison.h"
#include "CommandMetricTwinPairedDataDiffs.h"
#include "CommandPaintAddColumns.h"
#include "CommandPaintAssignNodes.h"
#include "CommandPaintAssignNodesRelativeToLine.h"
#include "CommandPaintComposite.h"
#include "CommandPaintDilation.h"
#include "CommandPaintFileCreate.h"
#include "CommandPaintLabelNameUpdate.h"
#include "CommandPaintSetColumnName.h"
#include "CommandPreferencesFileSettings.h"
#include "CommandSceneCreate.h"
#include "CommandScriptComment.h"
#include "CommandScriptConvert.h"
#include "CommandScriptRun.h"
#include "CommandScriptVariableRead.h"
#include "CommandScriptVariableSet.h"
#include "CommandShowScene.h"
#include "CommandShowSurface.h"
#include "CommandShowVolume.h"
#include "CommandSpecFileAdd.h"
#include "CommandSpecFileChangeResolution.h"
#include "CommandSpecFileClean.h"
#include "CommandSpecFileCopy.h"
#include "CommandSpecFileCreate.h"
#include "CommandSpecFileDirectoryClean.h"
#include "CommandSpecFileZip.h"
#include "CommandStatisticSetRandomSeed.h"
#include "CommandStatisticalUnitTesting.h"
#include "CommandStereotaxicSpaces.h"
#include "CommandStudyMetaDataFileDuplicates.h"
#include "CommandStudyMetaDataFileValidate.h"
#include "CommandSurfaceAffineRegression.h"
#include "CommandSurfaceAlignToStandardOrientation.h"
#include "CommandSurfaceApplyTransformationMatrix.h"
#include "CommandSurfaceAverage.h"
#include "CommandSurfaceBankStraddling.h"
#include "CommandSurfaceBorderCreateAverage.h"
#include "CommandSurfaceBorderCreateParallelBorder.h"
#include "CommandSurfaceBorderCutter.h"
#include "CommandSurfaceBorderDelete.h"
#include "CommandSurfaceBorderDrawAroundROI.h"
#include "CommandSurfaceBorderDrawGeodesic.h"
#include "CommandSurfaceBorderDrawMetric.h"
#include "CommandSurfaceBorderFileMerge.h"
#include "CommandSurfaceBorderToPaint.h"
#include "CommandSurfaceBorderIntersection.h"
#include "CommandSurfaceBorderLandmarkIdentification.h"
#include "CommandSurfaceBorderMerge.h"
#include "CommandSurfaceBorderNibbler.h"
#include "CommandSurfaceBorderProjection.h"
#include "CommandSurfaceBorderResample.h"
#include "CommandSurfaceBorderReverse.h"
#include "CommandSurfaceBorderLengths.h"
#include "CommandSurfaceBorderLinkToFocus.h"
#include "CommandSurfaceBorderSetVariability.h"
#include "CommandSurfaceBorderToMetric.h"
#include "CommandSurfaceBorderUnprojection.h"
#include "CommandSurfaceBorderVariability.h"
#include "CommandSurfaceCellCreate.h"
#include "CommandSurfaceCellProjection.h"
#include "CommandSurfaceCellUnprojection.h"
#include "CommandSurfaceCrossoverCheck.h"
#include "CommandSurfaceCurvature.h"
#include "CommandSurfaceDistortion.h"
#include "CommandSurfaceFlatMultiResMorphing.h"
#include "CommandSurfaceFlatten.h"
#include "CommandSurfaceFociAttributeAssignment.h"
#include "CommandSurfaceFociCreate.h"
#include "CommandSurfaceFociDelete.h"
#include "CommandSurfaceFociProjection.h"
#include "CommandSurfaceFociProjectionPals.h"
#include "CommandSurfaceFociReassignStudyNames.h"
#include "CommandSurfaceFociStudyValidate.h"
#include "CommandSurfaceFociUnprojection.h"
#include "CommandSurfaceGenerateInflated.h"
#include "CommandSurfaceGeodesic.h"
#include "CommandSurfaceInflate.h"
#include "CommandSurfaceInformation.h"
#include "CommandSurfaceNormals.h"
#include "CommandSurfacePlaceFociAtLimits.h"
#include "CommandSurfacePlaceFociAtExtremum.h"
#include "CommandSurfaceRegionOfInterestSelection.h"
#include "CommandSurfaceRegistrationPrepareSlits.h"
#include "CommandSurfaceRegistrationSpherical.h"
#include "CommandSurfaceRegistrationSphericalSpecOnly.h"
#include "CommandSurfaceRoiCoordReport.h"
#include "CommandSurfaceRoiFoldingMeasures.h"
#include "CommandSurfaceRoiNodeAreas.h"
#include "CommandSurfaceRoiShapeMeasures.h"
#include "CommandSurfaceRoiStatisticalReport.h"
#include "CommandSurfaceSmoothing.h"
#include "CommandSurfaceSphere.h"
#include "CommandSurfaceSphereProjectUnproject.h"
#include "CommandSurfaceSphericalMultiResMorphing.h"
#include "CommandSurfaceSulcalDepth.h"
#include "CommandSurfaceSulcalIdentificationProbabilistic.h"
#include "CommandSurfaceIdentifySulci.h"
#include "CommandSurfaceToCArrays.h"
#include "CommandSurfaceToCerebralHull.h"
#include "CommandSurfaceToSegmentationVolume.h"
#include "CommandSurfaceToVolume.h"
#include "CommandSurfaceTopologyDisconnectNodes.h"
#include "CommandSurfaceTopologyFixOrientation.h"
#include "CommandSurfaceTopologyNeighbors.h"
#include "CommandSurfaceTopologyReport.h"
#include "CommandSurfaceTransformToStandardView.h"
#include "CommandSurfacesToSegmentationVolumeMask.h"
#include "CommandSystemCommandExecute.h"
#include "CommandSystemFileCopy.h"
#include "CommandSystemFileDelete.h"
#include "CommandTransformationMatrixCreate.h"
#include "CommandVerify.h"
#include "CommandVersion.h"
#include "CommandVolumeAnatomyPeaks.h"
#include "CommandVolumeAtlasResamplingAndSmoothing.h"
#include "CommandVolumeBiasCorrection.h"
#include "CommandVolumeBlur.h"
#include "CommandVolumeClassifyIntensities.h"
#include "CommandVolumeConvertVectorToVolume.h"
#include "CommandVolumeCreate.h"
#include "CommandVolumeCreateCorpusCallosumSlice.h"
#include "CommandVolumeCreateInStereotaxicSpace.h"
#include "CommandVolumeDilate.h"
#include "CommandVolumeDilateErode.h"
#include "CommandVolumeDilateErodeWithinMask.h"
#include "CommandVolumeErode.h"
#include "CommandVolumeEulerCount.h"
#include "CommandVolumeFileCombine.h"
#include "CommandVolumeFileMerge.h"
#include "CommandVolumeFillBiggestObject.h"
#include "CommandVolumeFillHoles.h"
#include "CommandVolumeFillSlice.h"
#include "CommandVolumeFindLimits.h"
#include "CommandVolumeFloodFill.h"
#include "CommandVolumeGradient.h"
#include "CommandVolumeSegmentationToCerebralHull.h"
#include "CommandVolumeHistogram.h"
#include "CommandVolumeImportRawFile.h"
#include "CommandVolumeInformation.h"
#include "CommandVolumeInformationNifti.h"
#include "CommandVolumeMakePlane.h"
#include "CommandVolumeMakeRectangle.h"
#include "CommandVolumeMakeShell.h"
#include "CommandVolumeMakeSphere.h"
#include "CommandVolumeMapToSurface.h"
#include "CommandVolumeMapToSurfacePALS.h"
#include "CommandVolumeMapToSurfaceROIFile.h"
#include "CommandVolumeMapToVtkModel.h"
#include "CommandVolumeMaskVolume.h"
#include "CommandVolumeMaskWithVolume.h"
#include "CommandVolumeNearToPlane.h"
#include "CommandVolumePadVolume.h"
#include "CommandVolumeProbAtlasToFunctional.h"
#include "CommandVolumeRemoveIslands.h"
#include "CommandVolumeReplaceVectorMagnitudeWithVolume.h"
#include "CommandVolumeReplaceVoxelsWithVectorMagnitude.h"
#include "CommandVolumeResample.h"
#include "CommandVolumeRescaleVoxels.h"
#include "CommandVolumeResize.h"
#include "CommandVolumeROIGradient.h"
#include "CommandVolumeROIMinima.h"
#include "CommandVolumeROISmoothing.h"
#include "CommandVolumeScale0to255.h"
#include "CommandVolumeScalePercent0to255.h"
#include "CommandVolumeSculpt.h"
#include "CommandVolumeSegmentation.h"
#include "CommandVolumeSegmentationLigase.h"
#include "CommandVolumeSegmentationStereotaxicSpace.h"
#include "CommandVolumeSetOrientation.h"
#include "CommandVolumeSetOrigin.h"
#include "CommandVolumeSetSpacing.h"
#include "CommandVolumeShiftAxis.h"
#include "CommandVolumeSmearAxis.h"
#include "CommandVolumeTFCE.h"
#include "CommandVolumeThreshold.h"
#include "CommandVolumeThresholdDual.h"
#include "CommandVolumeThresholdInverse.h"
#include "CommandVolumeFslToVector.h"
#include "CommandVolumeTopologyCorrector.h"
#include "CommandVolumeTopologyGraph.h"
#include "CommandVolumeTopologyReport.h"
#include "CommandVolumeVectorCombine.h"

/**
 * constructor.
 */
CommandBase::CommandBase(const QString& operationSwitchIn,
                         const QString& shortDescriptionIn)
{
   operationSwitch  = operationSwitchIn;
   shortDescription = shortDescriptionIn;
   parameters       = getEmptyParameters();

   exitCode = 0;
}

/**
 * destructor.
 */
CommandBase::~CommandBase()
{
}

/**
 * get all commands.
 */
void 
CommandBase::getAllCommandsUnsorted(std::vector<CommandBase*>& commandsOut)
{
   commandsOut.clear();
   
   commandsOut.push_back(new CommandCaretFileCopy);
   commandsOut.push_back(new CommandCaretFileNamingUnitTesting);
   commandsOut.push_back(new CommandCaretHelpCreateHtmlIndexFile);
   commandsOut.push_back(new CommandCiftiCorrelationMatrix);
   commandsOut.push_back(new CommandColorFileAddColor);
   commandsOut.push_back(new CommandColorFileCreateMissingColors);
   commandsOut.push_back(new CommandConvertDataFileToCaret6);
   commandsOut.push_back(new CommandConvertSpecFileToCaret6);
   commandsOut.push_back(new CommandCreateCiftiDenseTimeseries);
   commandsOut.push_back(new CommandDataFileCompare);
   commandsOut.push_back(new CommandDeformationMapApply);
   commandsOut.push_back(new CommandDeformationMapApplyGenericNames);
   commandsOut.push_back(new CommandDeformationMapCreate);
   commandsOut.push_back(new CommandDeformationMapPathUpdate);
   commandsOut.push_back(new CommandExtend);
   commandsOut.push_back(new CommandFileConvert);
   commandsOut.push_back(new CommandFileReadTime);
   commandsOut.push_back(new CommandFileSubstitution);
   commandsOut.push_back(new CommandGiftiInfo);
   commandsOut.push_back(new CommandHelp);
   commandsOut.push_back(new CommandHelpFull);
   commandsOut.push_back(new CommandHelpGlobalOptions);
   commandsOut.push_back(new CommandHelpPDF);
   commandsOut.push_back(new CommandHelpHTML);
   commandsOut.push_back(new CommandHelpSearch);
   commandsOut.push_back(new CommandImageCombine);
   commandsOut.push_back(new CommandImageCompare);
   commandsOut.push_back(new CommandImageFormatConvert);
   commandsOut.push_back(new CommandImageInsertText);
   commandsOut.push_back(new CommandImageResize);
   commandsOut.push_back(new CommandImageToWebPage);
   commandsOut.push_back(new CommandImageView);
   commandsOut.push_back(new CommandMetricClustering);
   commandsOut.push_back(new CommandMetricComposite);
   commandsOut.push_back(new CommandMetricCompositeIdentifiedColumns);
   commandsOut.push_back(new CommandMetricCorrelationCoefficientMap);
   commandsOut.push_back(new CommandMetricCorrelationMatrix);
   commandsOut.push_back(new CommandMetricExtrema);
   commandsOut.push_back(new CommandMetricFileCreate);
   commandsOut.push_back(new CommandMetricGradient);
   commandsOut.push_back(new CommandMetricGradientAll);
   commandsOut.push_back(new CommandMetricInGroupDifference);
   commandsOut.push_back(new CommandMetricInformation);
   commandsOut.push_back(new CommandMetricMath);
   commandsOut.push_back(new CommandMetricMathPostfix);
   commandsOut.push_back(new CommandMetricMultipleCorrelationCoefficientMap);
   commandsOut.push_back(new CommandMetricROIGradient);
   commandsOut.push_back(new CommandMetricROIMask);
   commandsOut.push_back(new CommandMetricROISmoothing);
   commandsOut.push_back(new CommandMetricSetColumnName);
   commandsOut.push_back(new CommandMetricSetColumnToScalar);
   commandsOut.push_back(new CommandMetricSmoothing);
   commandsOut.push_back(new CommandMetricStatisticsAnovaOneWay);
   commandsOut.push_back(new CommandMetricStatisticsAnovaTwoWay);
   commandsOut.push_back(new CommandMetricStatisticsCoordinateDifference);
   commandsOut.push_back(new CommandMetricStatisticsInterhemisphericClusters);
   commandsOut.push_back(new CommandMetricStatisticsKruskalWallis);
   commandsOut.push_back(new CommandMetricStatisticsLeveneMap);
   commandsOut.push_back(new CommandMetricStatisticsNormalization);
   commandsOut.push_back(new CommandMetricStatisticsOneSampleTTest);
   commandsOut.push_back(new CommandMetricStatisticsPairedTTest);
   commandsOut.push_back(new CommandMetricStatisticsShuffledCrossCorrelationMaps);
   commandsOut.push_back(new CommandMetricStatisticsShuffledTMap);
   commandsOut.push_back(new CommandMetricStatisticsSubtraceGroupAverage);
   commandsOut.push_back(new CommandMetricStatisticsTMap);
   commandsOut.push_back(new CommandMetricStatisticsTwoSampleTTest);
   commandsOut.push_back(new CommandMetricStatisticsZMap);
   commandsOut.push_back(new CommandMetricTranspose);
   commandsOut.push_back(new CommandMetricTwinComparison);
   commandsOut.push_back(new CommandMetricTwinPairedDataDiffs);
   commandsOut.push_back(new CommandPaintAddColumns);
   commandsOut.push_back(new CommandPaintAssignNodes);
   commandsOut.push_back(new CommandPaintAssignNodesRelativeToLine);
   commandsOut.push_back(new CommandPaintComposite);
   commandsOut.push_back(new CommandPaintDilation);
   commandsOut.push_back(new CommandPaintFileCreate);
   commandsOut.push_back(new CommandPaintLabelNameUpdate);
   commandsOut.push_back(new CommandPaintSetColumnName);
   commandsOut.push_back(new CommandPreferencesFileSettings);
   commandsOut.push_back(new CommandSceneCreate);
   commandsOut.push_back(new CommandScriptComment);
   commandsOut.push_back(new CommandScriptConvert);
   commandsOut.push_back(new CommandScriptRun);
   commandsOut.push_back(new CommandScriptVariableRead);
   commandsOut.push_back(new CommandScriptVariableSet);
   commandsOut.push_back(new CommandShowScene);
   commandsOut.push_back(new CommandShowSurface);
   commandsOut.push_back(new CommandShowVolume);
   commandsOut.push_back(new CommandSpecFileAdd);
   commandsOut.push_back(new CommandSpecFileChangeResolution);
   commandsOut.push_back(new CommandSpecFileClean);
   commandsOut.push_back(new CommandSpecFileCopy);
   commandsOut.push_back(new CommandSpecFileCreate);
   commandsOut.push_back(new CommandSpecFileDirectoryClean);
   commandsOut.push_back(new CommandSpecFileZip);
   commandsOut.push_back(new CommandStatisticSetRandomSeed);
   commandsOut.push_back(new CommandStatisticalUnitTesting);
   commandsOut.push_back(new CommandStereotaxicSpaces);
   commandsOut.push_back(new CommandStudyMetaDataFileDuplicates);
   commandsOut.push_back(new CommandStudyMetaDataFileValidate);
   commandsOut.push_back(new CommandSurfaceAffineRegression);
   commandsOut.push_back(new CommandSurfaceAlignToStandardOrientation);
   commandsOut.push_back(new CommandSurfaceApplyTransformationMatrix);
   commandsOut.push_back(new CommandSurfaceAverage);
   commandsOut.push_back(new CommandSurfaceBankStraddling);
   commandsOut.push_back(new CommandSurfaceBorderCreateAverage);
   commandsOut.push_back(new CommandSurfaceBorderCreateParallelBorder);
   commandsOut.push_back(new CommandSurfaceBorderCutter);
   commandsOut.push_back(new CommandSurfaceBorderDelete);
   commandsOut.push_back(new CommandSurfaceBorderDrawAroundROI);
   commandsOut.push_back(new CommandSurfaceBorderDrawGeodesic);
   commandsOut.push_back(new CommandSurfaceBorderDrawMetric);
   commandsOut.push_back(new CommandSurfaceBorderFileMerge);
   commandsOut.push_back(new CommandSurfaceBorderToPaint);
   commandsOut.push_back(new CommandSurfaceBorderIntersection);
   commandsOut.push_back(new CommandSurfaceBorderLandmarkIdentification);
   commandsOut.push_back(new CommandSurfaceBorderMerge);
   commandsOut.push_back(new CommandSurfaceBorderNibbler);
   commandsOut.push_back(new CommandSurfaceBorderProjection);
   commandsOut.push_back(new CommandSurfaceBorderResample);
   commandsOut.push_back(new CommandSurfaceBorderReverse);
   commandsOut.push_back(new CommandSurfaceBorderLengths);
   commandsOut.push_back(new CommandSurfaceBorderLinkToFocus);
   commandsOut.push_back(new CommandSurfaceBorderSetVariability);
   commandsOut.push_back(new CommandSurfaceBorderToMetric);
   commandsOut.push_back(new CommandSurfaceBorderUnprojection);
   commandsOut.push_back(new CommandSurfaceBorderVariability);
   commandsOut.push_back(new CommandSurfaceCrossoverCheck);
   commandsOut.push_back(new CommandSurfaceCellCreate);
   commandsOut.push_back(new CommandSurfaceCellProjection);
   commandsOut.push_back(new CommandSurfaceCellUnprojection);
   commandsOut.push_back(new CommandSurfaceCurvature);
   commandsOut.push_back(new CommandSurfaceDistortion);
   commandsOut.push_back(new CommandSurfaceFlatMultiResMorphing);
   commandsOut.push_back(new CommandSurfaceFlatten);
   commandsOut.push_back(new CommandSurfaceFociAttributeAssignment);
   commandsOut.push_back(new CommandSurfaceFociCreate);
   commandsOut.push_back(new CommandSurfaceFociDelete);
   commandsOut.push_back(new CommandSurfaceFociProjection);
   commandsOut.push_back(new CommandSurfaceFociProjectionPals);
   commandsOut.push_back(new CommandSurfaceFociReassignStudyNames);
   commandsOut.push_back(new CommandSurfaceFociStudyValidate);
   commandsOut.push_back(new CommandSurfaceFociUnprojection);
   commandsOut.push_back(new CommandSurfaceGenerateInflated);
   commandsOut.push_back(new CommandSurfaceGeodesic);
   commandsOut.push_back(new CommandSurfaceIdentifySulci);
   commandsOut.push_back(new CommandSurfaceInflate);
   commandsOut.push_back(new CommandSurfaceInformation);
   commandsOut.push_back(new CommandSurfaceNormals);
   commandsOut.push_back(new CommandSurfacePlaceFociAtExtremum);
   commandsOut.push_back(new CommandSurfacePlaceFociAtLimits);
   commandsOut.push_back(new CommandSurfaceRegionOfInterestSelection);
   commandsOut.push_back(new CommandSurfaceRegistrationPrepareSlits);
   commandsOut.push_back(new CommandSurfaceRegistrationSpherical);
   commandsOut.push_back(new CommandSurfaceRegistrationSphericalSpecOnly);
   commandsOut.push_back(new CommandSurfaceRoiCoordReport);
   commandsOut.push_back(new CommandSurfaceRoiFoldingMeasures);
   commandsOut.push_back(new CommandSurfaceRoiNodeAreas);
   commandsOut.push_back(new CommandSurfaceRoiShapeMeasures);
   commandsOut.push_back(new CommandSurfaceRoiStatisticalReport);
   commandsOut.push_back(new CommandSurfaceSphere());
   commandsOut.push_back(new CommandSurfaceSulcalDepth);
   commandsOut.push_back(new CommandSurfaceSulcalIdentificationProbabilistic);
   commandsOut.push_back(new CommandSurfaceToCArrays);
   commandsOut.push_back(new CommandSurfaceToCerebralHull);
   commandsOut.push_back(new CommandSurfaceToSegmentationVolume);
   commandsOut.push_back(new CommandSurfaceToVolume);
   commandsOut.push_back(new CommandSurfaceSmoothing);
   commandsOut.push_back(new CommandSurfaceSphereProjectUnproject);
   commandsOut.push_back(new CommandSurfaceSphericalMultiResMorphing);
   commandsOut.push_back(new CommandSurfaceTopologyFixOrientation);
   commandsOut.push_back(new CommandSurfaceTopologyDisconnectNodes);
   commandsOut.push_back(new CommandSurfaceTopologyNeighbors);
   commandsOut.push_back(new CommandSurfaceTopologyReport);
   commandsOut.push_back(new CommandSurfaceTransformToStandardView);
   commandsOut.push_back(new CommandSurfacesToSegmentationVolumeMask);
   commandsOut.push_back(new CommandSystemCommandExecute);
   commandsOut.push_back(new CommandSystemFileCopy);
   commandsOut.push_back(new CommandSystemFileDelete);
   commandsOut.push_back(new CommandTransformationMatrixCreate);
   commandsOut.push_back(new CommandVerify);
   commandsOut.push_back(new CommandVersion);
   commandsOut.push_back(new CommandVolumeAnatomyPeaks);
   commandsOut.push_back(new CommandVolumeAtlasResamplingAndSmoothing);
   commandsOut.push_back(new CommandVolumeBiasCorrection);
   commandsOut.push_back(new CommandVolumeBlur);
   commandsOut.push_back(new CommandVolumeClassifyIntensities);
   commandsOut.push_back(new CommandVolumeConvertVectorToVolume);
   commandsOut.push_back(new CommandVolumeCreate);
   commandsOut.push_back(new CommandVolumeCreateCorpusCallosumSlice);
   commandsOut.push_back(new CommandVolumeCreateInStereotaxicSpace);
   commandsOut.push_back(new CommandVolumeDilate);
   commandsOut.push_back(new CommandVolumeDilateErode);
   commandsOut.push_back(new CommandVolumeDilateErodeWithinMask);
   commandsOut.push_back(new CommandVolumeErode);
   commandsOut.push_back(new CommandVolumeEulerCount);
   commandsOut.push_back(new CommandVolumeFileCombine);
   commandsOut.push_back(new CommandVolumeFileMerge);
   commandsOut.push_back(new CommandVolumeFillBiggestObject);
   commandsOut.push_back(new CommandVolumeFillHoles);
   commandsOut.push_back(new CommandVolumeFillSlice);
   commandsOut.push_back(new CommandVolumeFindLimits);
   commandsOut.push_back(new CommandVolumeFloodFill);
   commandsOut.push_back(new CommandVolumeGradient);
   commandsOut.push_back(new CommandVolumeHistogram);
   commandsOut.push_back(new CommandVolumeSegmentationToCerebralHull);
   commandsOut.push_back(new CommandVolumeImportRawFile);
   commandsOut.push_back(new CommandVolumeInformation);
   commandsOut.push_back(new CommandVolumeInformationNifti);
   commandsOut.push_back(new CommandVolumeMapToSurface);
   commandsOut.push_back(new CommandVolumeMapToSurfacePALS);
   commandsOut.push_back(new CommandVolumeMapToSurfaceROIFile);
   commandsOut.push_back(new CommandVolumeMapToVtkModel);
   commandsOut.push_back(new CommandVolumeMakePlane);
   commandsOut.push_back(new CommandVolumeMakeRectangle);
   commandsOut.push_back(new CommandVolumeMakeShell);
   commandsOut.push_back(new CommandVolumeMakeSphere);
   commandsOut.push_back(new CommandVolumeMaskVolume);
   commandsOut.push_back(new CommandVolumeMaskWithVolume);
   commandsOut.push_back(new CommandVolumeNearToPlane);
   commandsOut.push_back(new CommandVolumePadVolume);
   commandsOut.push_back(new CommandVolumeProbAtlasToFunctional);
   commandsOut.push_back(new CommandVolumeRemoveIslands);
   commandsOut.push_back(new CommandVolumeReplaceVectorMagnitudeWithVolume);
   commandsOut.push_back(new CommandVolumeReplaceVoxelsWithVectorMagnitude);
   commandsOut.push_back(new CommandVolumeResample);
   commandsOut.push_back(new CommandVolumeRescaleVoxels);
   commandsOut.push_back(new CommandVolumeResize);
   commandsOut.push_back(new CommandVolumeROIGradient);
   commandsOut.push_back(new CommandVolumeROIMinima);
   commandsOut.push_back(new CommandVolumeROISmoothing);
   commandsOut.push_back(new CommandVolumeScale0to255);
   commandsOut.push_back(new CommandVolumeScalePercent0to255);
   commandsOut.push_back(new CommandVolumeSculpt);
   commandsOut.push_back(new CommandVolumeSegmentation);
   commandsOut.push_back(new CommandVolumeSegmentationLigase);
   commandsOut.push_back(new CommandVolumeSegmentationStereotaxicSpace);
   commandsOut.push_back(new CommandVolumeSetOrientation);
   commandsOut.push_back(new CommandVolumeSetOrigin);
   commandsOut.push_back(new CommandVolumeSetSpacing);
   commandsOut.push_back(new CommandVolumeShiftAxis);
   commandsOut.push_back(new CommandVolumeSmearAxis);
   commandsOut.push_back(new CommandVolumeTFCE);
   commandsOut.push_back(new CommandVolumeThreshold);
   commandsOut.push_back(new CommandVolumeThresholdDual);
   commandsOut.push_back(new CommandVolumeThresholdInverse);
   commandsOut.push_back(new CommandVolumeFslToVector);
   commandsOut.push_back(new CommandVolumeTopologyCorrector);
   commandsOut.push_back(new CommandVolumeTopologyGraph);
   commandsOut.push_back(new CommandVolumeTopologyReport);
   commandsOut.push_back(new CommandVolumeVectorCombine);
}
                            
/**
 * get all commands.
 */
void 
CommandBase::getAllCommandsSortedBySwitch(std::vector<CommandBase*>& commandsOut)
{
   commandsOut.clear();
   
   //
   // Get the commands
   //
   std::vector<CommandBase*> commands;
   getAllCommandsUnsorted(commands);
   
   // 
   // Sort the commands by indices
   //
   const int numCommands = static_cast<int>(commands.size());
   NameIndexSort nis;
   for (int i = 0; i < numCommands; i++) {
      nis.add(i, commands[i]->getOperationSwitch());
   }
   nis.sortByNameCaseSensitive();
   
   //
   // Loop through the commands
   //
   const int numItems = nis.getNumberOfItems();
   for (int i = 0; i < numItems; i++) {
      //
      // Get the command and add it to output commands
      //
      int commandIndex;
      QString commandName;
      nis.getSortedNameAndIndex(i, commandIndex, commandName);
      commandsOut.push_back(commands[commandIndex]);
   }
}

/**
 * get all commands.
 */
void 
CommandBase::getAllCommandsSortedByDescription(std::vector<CommandBase*>& commandsOut)
{
   commandsOut.clear();
   
   //
   // Get the commands
   //
   std::vector<CommandBase*> commands;
   getAllCommandsUnsorted(commands);
   
   // 
   // Sort the commands by indices
   //
   const int numCommands = static_cast<int>(commands.size());
   NameIndexSort nis;
   for (int i = 0; i < numCommands; i++) {
      nis.add(i, commands[i]->getShortDescription());
   }
   nis.sortByNameCaseSensitive();
   
   //
   // Loop through the commands
   //
   const int numItems = nis.getNumberOfItems();
   for (int i = 0; i < numItems; i++) {
      //
      // Get the command and add it to output commands
      //
      int commandIndex;
      QString commandName;
      nis.getSortedNameAndIndex(i, commandIndex, commandName);
      commandsOut.push_back(commands[commandIndex]);
   }
}                            

/**
 * execute the command (returns true if successful).
 */
bool 
CommandBase::execute(QString& errorMessageOut)
{
   errorMessageOut = "";
   
   if (parameters == NULL) {
      errorMessageOut = (QString("PROGRAM ERROR: parameters have not been set for ")
                         + getOperationSwitch());
      return false;
   }
   
   errorMessageOut = getShortDescription() + " ERROR: ";
   
/*
   //
   // See if directory should be changed
   //
   const int chdirIndex = parameters->getIndexOfParameterWithValue("-CHDIR");
   if (chdirIndex >= 0) {
      const int dirNameIndex = chdirIndex + 1;
      if (dirNameIndex < parameters->getNumberOfParameters()) {
         const QString directoryName = parameters->getParameterAtIndex(dirNameIndex);
         if (directoryName.isEmpty() == false) {
            //
            // Is specified directory valid?
            //
            QDir dir(directoryName);
            if (dir.exists() == false) {
               errorMessageOut += (" directory for -CHDIR \""
                                   + directoryName
                                   + "\" is invalid.");
               return false;
            }

            //
            // Change to the specified directory
            //
            QDir::setCurrent(directoryName);
            
            //
            // Remove the "-CHDIR" and directory name parameters
            // Remember, remove largest index first since array shrinks
            //
            parameters->removeParameterAtIndex(dirNameIndex);
            parameters->removeParameterAtIndex(chdirIndex);
         }
      }
      else {
         errorMessageOut += "directory name missing for \"-CHDIR\" option";
         return false;
      }
   }
*/   
   try {
      executeCommand();
   }
   catch (BrainModelAlgorithmException& bmae) {
      errorMessageOut += bmae.whatQString();
      return false;
   }
   catch (CommandException& ce) {
      errorMessageOut += ce.whatQString();
      return false;
   }
   catch (FileException& fe) {
      errorMessageOut += fe.whatQString();
      return false;
   }
   catch (ProgramParametersException& ppe) {
      errorMessageOut += ppe.whatQString();
      return false;
   }
   catch (StatisticException& se) {
      errorMessageOut += StringUtilities::fromStdString(se.whatStdString());
      return false;
   }
   catch (std::exception& e) {
      errorMessageOut += QString(e.what());
      return false;
   }
   
   errorMessageOut = "";
   
   return true;
}
      
/**
 * set the parameters for the command.
 */
void 
CommandBase::setParameters(ProgramParameters* parametersIn)
{
   parameters = parametersIn;
}

/**
 * get the name and label from a volume file name.
 */
void 
CommandBase::splitOutputVolumeNameIntoNameAndLabel(QString& nameInOut,
                                                   QString& labelOut) const
{
   static const QString outputVolumeDelimeter(":::");
   
   labelOut = "";
   int colonPos = nameInOut.indexOf(outputVolumeDelimeter);
   if (colonPos != -1) {
      labelOut = nameInOut.mid(0, colonPos);
      nameInOut = nameInOut.mid(colonPos + outputVolumeDelimeter.length());
   }
}
                                     
/**
 * read a volume file.
 */
void 
CommandBase::readVolumeFile(VolumeFile& vf,
                            const QString& name) throw (FileException)
{
   try {
      vf.readFile(name);
   }
   catch (FileException& e) {
      const QString msg("ERROR reading: " 
                        + name
                        + " "
                        + e.whatQString());
      throw FileException(msg);
   }
}
                    
/**
 * write a volume file.
 */
void 
CommandBase::writeVolumeFile(VolumeFile& vf,
                             const QString& name,
                             const QString& label) throw (FileException)
{
   try {
      if (label.isEmpty() == false) {
         vf.setDescriptiveLabel(label);
      }
      
      //
      // Always write float since some operations have results ranging 0.0 to 1.0
      //
      vf.setVoxelDataType(VolumeFile::VOXEL_DATA_TYPE_FLOAT);
      
      vf.writeFile(name);
   }
   catch (FileException& e) {
      const QString msg("ERROR writing: " 
                        + name
                        + " "
                        + e.whatQString());
      throw FileException(msg);
   }
}                           


/**
 * checks for excessive parameters and throws exception if any are found.
 */
void 
CommandBase::checkForExcessiveParameters() throw (CommandException)
{
   if (parameters->getParametersAvailable()) {
      throw CommandException("Too many parameters provided for operation "
                             + getOperationSwitch());
   }
}      

/**
 * get pointer to empty parameters.
 */
ProgramParameters* 
CommandBase::getEmptyParameters()
{
/*
   static const char* progName = "caret_command";
   static char* argvDummy[1] = { progName };
   static ProgramParameters emptyParameters(1, argvDummy);
   return &emptyParameters;
*/   
   static ProgramParameters emptyParameters("caret_command", QStringList());
   return &emptyParameters;
}
      
/**
 * get general help information.
 */
QString 
CommandBase::getGeneralHelpInformation()
{
   QString helpInfo =
      "ADDING ADDITIONAL COMMANDS\n"
      "   To add additional commands:\n"
      "      * Download and build the caret source code and required libraries  \n"
      "        (which is not a simple task).\n"
      "      * Derive a new class for the command from the class CommandBase\n"
      "        located in the caret_source/caret_command_operations directory.\n"
      "      * Add the name of the new command's \".h\" and \".cxx\" files to\n"
      "        the \"caret_command_operations.pro file\".  In CommandBase.cxx,\n"
      "        add the include for the new command's header file and create a \n"
      "        new instance of the command in the method \n"
      "        getAllCommandsUnsorted().\n"
      "      * In the \"caret_source\" directory, run the command \"make build\"\n"
      "        to update caret and caret_command.\n"
      "   \n"
      "EXECUTING COMMANDS THROUGH THE GRAPHICAL USER INTERFACE\n"
      "   A graphical-user interface is provided in the Caret program for \n"
      "   executing all of the commands.  From Caret's Window Menu, select \n"
      "   \"Caret Command Executor\".  \n"
      "   \n"
      "METRIC FILES\n"
      "   Many of the metric commands have a metric column identifier as one\n"
      "   of the parameters.  This metric column may be either the number of \n"
      "   the metric column (which starts at one) or the name of the metric \n"
      "   column.  The name has priority over a number, so, if you happen to\n"
      "   name a column \"3\", you will not be able to access column 3 by its\n"
      "   column number.  If a column name contains spaces, the name must be\n"
      "   enclosed in double quotes.\n"
      " \n"
      "REQUESTING ADDITIONAL COMMANDS\n"
      "   Post requests for additional command line operations to the Caret\n"
      "   User's Mailing List.  Information about the list is available at\n"
      "      http://brainmap.wustl.edu/resources/caretnew.html#Help\n"
      " \n"
      "RETURN VALUE\n"
      "   If a command is successful, the program's return code will be zero or\n"
      "   greater.  A negative return code indicates that an error has occurred.\n"
      " \n"
      "SURFACE SHAPE FILES\n"
      "   Surface Shape and Metric files are the same file format.  So, any\n"
      "   command that accepts a metric file will also accept a surface shape\n"
      "   file.\n";
   
   return helpInfo;
}
      
      
