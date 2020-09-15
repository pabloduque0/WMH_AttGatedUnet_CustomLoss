#include "resamplingslicerwrapper.h"
#include <iostream>

int ResamplingSlicerWrapper::getAmosProcessRange()
{
    return 5;
}

ResamplingSlicerWrapper::ResamplingSlicerWrapper(QCoreApplication* a, QObject* parent) :
    ExternalProcessWrapper(a, parent)
{
    begin = std::chrono::high_resolution_clock::now();
    amosProcessRange = getAmosProcessRange();
    doBiasCorrection = true;
}

ResamplingSlicerWrapper::~ResamplingSlicerWrapper()
{
    std::cout << "ResamplingSlicerWrapper deleted" << std::endl;
}

bool ResamplingSlicerWrapper::setProgramPath(QString programPath)
{
    QFileInfo slicerExe(programPath, "Slicer");
    if(!slicerExe.exists()) {
        QString text(tr("Slicer program not found"));
        std::cout << text.toStdString() << "\n";
        return false;
    }
    m_program = slicerExe.absoluteFilePath();
    return true;
}

void ResamplingSlicerWrapper::setOutputPaths(QFileInfo outputDir)
{
    this->outputDirectory = QDir(outputDir.absoluteFilePath());
}

void ResamplingSlicerWrapper::createOutputNames()
{
    QString TXName = "TX_FL.tfm";
    QFileInfo TXInfo(outputDirectory, TXName);
    QStringList outputNames;
    QString name = imageFiles.at(0).baseName();
    QString suffix = imageFiles.at(0).completeSuffix();
    name = name + "_bcN4" + "." + suffix;
    outputNames.append(name);
    for(int i = 1; i < imageFiles.size(); i++) {
        name = imageFiles.at(i).baseName();
        suffix = imageFiles.at(i).completeSuffix();
        name = name + "_rsfl" + "." + suffix;
        outputNames.append(name);
    }
    TXPath = TXInfo.absoluteFilePath();

    QFileInfo flairOutPath(outputDirectory, outputNames.at(0));
    flairBCPath = flairOutPath.absoluteFilePath();
    QFileInfo T1OutPath(outputDirectory, outputNames.at(1));
    T1RSFLPath = T1OutPath.absoluteFilePath();
    QFileInfo LBOutPath(outputDirectory, outputNames.at(2));
    LBRSFLPath = LBOutPath.absoluteFilePath();

    if(doBiasCorrection) {
        QFileInfo T1TempBCPath(outputDirectory, "T1_bcN4_temp.nii");
        outputPaths << flairBCPath << T1BCPath << T1RSFLPath << LBRSFLPath << TXPath;
    }
    else {
        outputPaths<< flairBCPath << T1RSFLPath << LBRSFLPath << TXPath;
    }
}

QStringList ResamplingSlicerWrapper::getArguments()
{
    QStringList arguments;
    arguments << "-launch";
    int countRealProcess;
        
    if(doBiasCorrection)
        countRealProcess = countProcess;
    else {
        // starting at BRAINSFit
        countRealProcess = countProcess + 2;
        if( countProcess == 1) {
            flairBCPath = imageFiles.at(0).absoluteFilePath();
            T1BCPath = imageFiles.at(1).absoluteFilePath();
        }
    }
    
    if(countRealProcess == 1 || countRealProcess == 2) {
        QString N4Path("N4ITKBiasFieldCorrection");
        arguments << N4Path;
        if(countRealProcess == 1) {
            arguments << imageFiles.at(0).absoluteFilePath() << flairBCPath;
            std::cout << "Flair Bias Corr." << std::endl;
        }
        else if(countRealProcess == 2) {
            arguments << imageFiles.at(1).absoluteFilePath() << T1BCPath;
            std::cout << "T1 Bias Corr." << std::endl;
        }
    } 
    else if(countRealProcess == 3) {
        QString BRAINSFit("BRAINSFit");
        arguments << BRAINSFit;

        if(doBiasCorrection)
            arguments << "--fixedVolume" << flairBCPath;
        else
            arguments << "--fixedVolume" << imageFiles.at(0).absoluteFilePath();
        arguments << "--movingVolume" << T1BCPath;
        arguments << "--linearTransform" << TXPath;
        arguments << "--samplingPercentage" << QString::number(0.8);
        arguments << "--maskProcessingMode" << "ROI";
        arguments << "--movingBinaryVolume" << imageFiles.at(2).absoluteFilePath();
        arguments << "--transformType" << "ScaleVersor3D";
        arguments << "--interpolationMode" << "BSpline";
        arguments << "--outputVolumePixelType" << "short";
        arguments << "--numberOfThreads" << QString::number(numThreads);

        std::cout << "Fitting" << std::endl;
    }
    else if(countRealProcess == 4 || countRealProcess == 5) {
        QString BRAINSResample("BRAINSResample");
        arguments << BRAINSResample;

        if(doBiasCorrection)
            arguments << "--referenceVolume" << flairBCPath;
        else
            arguments << "--referenceVolume" << imageFiles.at(0).absoluteFilePath();
        arguments << "--warpTransform" << TXPath;
        if(countRealProcess == 4) {
            arguments << "--inputVolume" << T1BCPath;
            arguments << "--outputVolume" << T1RSFLPath;
            arguments << "--interpolationMode" << "BSpline";
            std::cout << "T1 Resampling" << std::endl;
        }
        else if(countRealProcess == 5) {
            arguments << "--inputVolume" << imageFiles.at(2).absoluteFilePath();
            arguments << "--outputVolume" << LBRSFLPath;
            arguments << "--interpolationMode" << "NearestNeighbor";
            std::cout << "LB Resampling" << std::endl;
        }
        arguments << "--pixelType" << "short";        
        arguments << "--defaultValue" << QString::number(0);
        arguments << "--numberOfThreads" << QString::number(numThreads);
    }
    return arguments;
}

void ResamplingSlicerWrapper::cleanAfterFinished()
{
    QFile file(T1BCPath);
    if(doBiasCorrection)
        file.remove();
    file.setFileName(TXPath);
    file.remove();
    outputPaths.removeOne(T1BCPath);
    QFileInfo TX(TXPath);
    QString inverseName = TX.baseName();
    inverseName = inverseName  + "_Inverse.h5";
    QFileInfo TXinverse(outputDirectory, inverseName);
    file.setFileName(TXinverse.absoluteFilePath());
    file.remove();
    outputPaths.removeOne(TXPath);

    if(!doBiasCorrection) {
        // copy original Flair to output dir
        if(imageFiles.at(0).absolutePath() != outputDirectory.absolutePath()) {
            QFile file(imageFiles.at(0).absoluteFilePath());
            QFileInfo flairOutPath(outputPaths.at(0));
            QFileInfo flairOut(flairOutPath.absolutePath(), imageFiles.at(0).fileName());
            if(flairOut.exists())
                QFile::remove(flairOut.absoluteFilePath());
            file.copy(flairOut.absoluteFilePath());
            outputPaths[0] = flairOut.absoluteFilePath();
        }
        else
            outputPaths.removeAt(0);
    }

}

bool ResamplingSlicerWrapper::setParameters(QMap<QString, QString> parameters)
{
    if(parameters.contains("do bias correction"))
        doBiasCorrection = parameters["do bias correction"] == QString::number(true) ? true : false;
    if(!doBiasCorrection)
        amosProcessRange = 3;
    return true;
}
