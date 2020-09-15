#ifndef RESAMPLINGSLICERWRAPPER_H
#define RESAMPLINGSLICERWRAPPER_H

#include "externalprocesswrapper.h"

class EdisonWrapper : public ExternalProcessWrapper
{
    Q_OBJECT
    
public:
    
    EdisonWrapper(QObject* parent = Q_NULLPTR);
    virtual ~EdisonWrapper() Q_DECL_OVERRIDE;
    
    static int getAmosProcessRange();
    bool setProgramPath(QString programPath) Q_DECL_OVERRIDE;
    void setOutputPaths(QFileInfo outputDir) Q_DECL_OVERRIDE;
    void createOutputNames() Q_DECL_OVERRIDE;
    
    bool setParameters(QMap<QString, QString> parameters) Q_DECL_OVERRIDE;

    bool createEdisonFile(QString pgmFileName);
    
protected:
    QStringList getArguments() Q_DECL_OVERRIDE;
    void cleanAfterFinished() Q_DECL_OVERRIDE;

public:
    // Specify the edge detection parameters
    struct EdisonParams {
        int GradientWindowRadius = 5;
        int MinimumLength = 5;
        double NmxRank = 0.5;
        double NmxConf = 0.5;
        QString NmxType = "ARC";
        double HysterisisHighRank = 0.9;
        double HysterisisHighConf = 0.9;
        QString HysterisisHighType = "BOX";
        double HysterisisLowRank = 0.8;
        double HysterisisLowConf = 0.8;
        QString HysterisisLowType = "CUSTOM";
        double CustomCurveHystLow[2][2] = {{0.4,0.7}, {0.6,0.3}};
        QString DisplayProgress = "ON";
    };

    struct EdisonFiles {
        QString gradientMap = "gradientmap.txt";
        // leave empty if don't need some of following maps
        //QString edgeMap = "";
        QString confidenceMap = "";
        QString edgeMap = "edgesmap.pgm";
        // QString confidenceMap = "confidencemap.pgm";
    };
    
    QString getGradientFile() const;

protected:
    EdisonParams edisonParams;
    EdisonFiles edisonFiles;
    QString gradientFile;
    QString edsFile;
};

#endif // RESAMPLINGSLICERWRAPPER_H
