#include "edisonwrapper.h"
#include <QTextStream>
#include <QVector>
#include <iostream>

int EdisonWrapper::getAmosProcessRange()
{
    return 1;
}

EdisonWrapper::EdisonWrapper(QObject* parent) :
    ExternalProcessWrapper(parent)
{
    amosProcessRange = getAmosProcessRange();
}

EdisonWrapper::~EdisonWrapper()
{
    std::cout << "EdisonWrapper deleted" << std::endl;
}

bool EdisonWrapper::setProgramPath(QString programPath)
{
    QFileInfo edisonExe(programPath, "edison");
    if(!edisonExe.exists()) {
        QString text(tr("Edison program not found"));
        std::cout << text.toStdString() << "\n";
        return false;
    }
    m_program = edisonExe.absoluteFilePath();
    return true;
}

void EdisonWrapper::setOutputPaths(QFileInfo outputDir)
{
    outputDirectory = QDir(outputDir.absoluteFilePath());
}

void EdisonWrapper::createOutputNames()
{
    QStringList outputNames;
    QFileInfo gradientPath(QDir(outputDirectory), edisonFiles.gradientMap);
    gradientFile = gradientPath.absoluteFilePath();
    outputPaths << gradientFile;
}

QStringList EdisonWrapper::getArguments()
{
    QStringList arguments;
    arguments.append(edsFile);
    return arguments;
}

void EdisonWrapper::cleanAfterFinished()
{

}

QString EdisonWrapper::getGradientFile() const
{
    return gradientFile;
}

bool EdisonWrapper::setParameters(QMap<QString, QString> /*parameters*/)
{
    return true;
}

bool EdisonWrapper::createEdisonFile(QString pgmFileName)
{
    int numLines;
    if (!edisonFiles.edgeMap.isEmpty() && !edisonFiles.confidenceMap.isEmpty())
        numLines = 18;
    else if (!edisonFiles.edgeMap.isEmpty() || !edisonFiles.confidenceMap.isEmpty())
        numLines = 17;
    else
        numLines = 16;

    QVector<QString> lines(numLines);
    lines[0] = QString("GradientWindowRadius = %1;").arg(edisonParams.GradientWindowRadius);
    lines[1] = QString("MinimumLength = %1;").arg(edisonParams.MinimumLength);
    lines[2] = QString("NmxRank = %1;").arg(edisonParams.NmxRank);
    lines[3] = QString("NmxConf = %1;").arg(edisonParams.NmxConf);
    lines[4] = QString("NmxType = %1;").arg(edisonParams.NmxType);
    lines[5] = QString("HysterisisHighRank = %1;").arg(edisonParams.HysterisisHighRank);
    lines[6] = QString("HysterisisHighConf = %1;").arg(edisonParams.HysterisisHighConf);
    lines[7] = QString("HysterisisHighType = %1;").arg(edisonParams.HysterisisHighType);
    lines[8] = QString("HysterisisLowRank = %1;").arg(edisonParams.HysterisisLowRank);
    lines[9] = QString("HysterisisLowConf = %1;").arg(edisonParams.HysterisisLowConf);
    lines[10] = QString("HysterisisLowType = %1;").arg(edisonParams.HysterisisLowType);
    lines[11] = QString("CustomCurveHystLow = {(%1,%2), (%3,%4)};").
            arg(edisonParams.CustomCurveHystLow[0][0]).arg(edisonParams.CustomCurveHystLow[0][1]).
            arg(edisonParams.CustomCurveHystLow[1][0]).arg(edisonParams.CustomCurveHystLow[1][1]);
    lines[12] = QString("DisplayProgress %1;").arg(edisonParams.DisplayProgress);
    // Load an image to perform edge detection
    lines[13] = QString("Load('%1', IMAGE);").arg(pgmFileName);
    // Selected operation
    lines[14] = QString("EdgeDetect;");

    // Save the resulting maps
    lines[15] = QString("Save('%1', MATLAB_ASCII, GRADIENT_MAP);").
                                arg(gradientFile);
    if(numLines == 18) {
        QFileInfo edgePath(outputDirectory, edisonFiles.edgeMap);
        QFileInfo confidencePath(outputDirectory, edisonFiles.confidenceMap);
        lines[16] = QString("Save('%1', PGM, EDGES);").
                arg(edgePath.absoluteFilePath());
        lines[17] = QString("Save('%1', MATLAB_ASCII, CONFIDENCE_MAP);").
                arg(confidencePath.absoluteFilePath());
    }
    else if(numLines == 17 && !edisonFiles.edgeMap.isEmpty()) {
        QFileInfo edgePath(outputDirectory, edisonFiles.edgeMap);
        lines[16] = QString("Save('%1', PGM, EDGES);").
                arg(edgePath.absoluteFilePath());
    }
    else if(numLines == 17 && !edisonFiles.edgeMap.isEmpty()) {
        QFileInfo confidencePath(outputDirectory, edisonFiles.confidenceMap);
        lines[16] = QString("Save('%1', MATLAB_ASCII, CONFIDENCE_MAP);").
                arg(confidencePath.absoluteFilePath());
    }

    QFileInfo fileInfo(QDir(outputDirectory), "edge_detection.eds");
    edsFile = fileInfo.absoluteFilePath();
    QFile file(edsFile);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        std::cout << "Cannot create gradient file " << edsFile.toStdString()
                  << std::endl;
        return false;
    }
    QTextStream out(&file);
    for(int i = 0; i < numLines; i++)
        out << lines.at(i) << endl;
    out.flush();
    file.close();

    return true;
}
