#ifndef RESAMPLINGSLICERWRAPPER_H
#define RESAMPLINGSLICERWRAPPER_H

#include "externalprocesswrapper.h"

class ResamplingSlicerWrapper : public ExternalProcessWrapper
{
    Q_OBJECT
    
public:
    
    ResamplingSlicerWrapper(QCoreApplication* a, QObject* parent = Q_NULLPTR);
    virtual ~ResamplingSlicerWrapper() Q_DECL_OVERRIDE;
    
    static int getAmosProcessRange();
    bool setProgramPath(QString programPath) Q_DECL_OVERRIDE;
    void setOutputPaths(QFileInfo outputDir) Q_DECL_OVERRIDE;
    void createOutputNames() Q_DECL_OVERRIDE;
    
    bool setParameters(QMap<QString, QString> parameters) Q_DECL_OVERRIDE;
    
protected:
    QStringList getArguments() Q_DECL_OVERRIDE;
    void cleanAfterFinished() Q_DECL_OVERRIDE;
    
protected:
    QString flairBCPath;
    QString T1BCPath;
    QString T1RSFLPath;
    QString LBRSFLPath;
    QString TXPath;
    bool doBiasCorrection;    
};

#endif // RESAMPLINGSLICERWRAPPER_H
