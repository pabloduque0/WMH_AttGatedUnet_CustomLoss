#ifndef EXTERNALPROCESSWRAPPER_H
#define EXTERNALPROCESSWRAPPER_H

#include <QProcess>
#include <QFileInfo>
#include <QDir>
#include <QStringList>
#include <QMap>

class QCoreApplication;

class ExternalProcessWrapper : public QProcess
{
    Q_OBJECT
    
public:
    ExternalProcessWrapper(QObject* parent = Q_NULLPTR);
    ~ExternalProcessWrapper();
    
    virtual bool setParameters(QMap<QString, QString> parameters) = 0;
    virtual bool setProgramPath(QString programPath) = 0;
    
    void setImages(QFileInfoList imageFiles);
    virtual void setOutputPaths(QFileInfo outputDir) = 0;
    virtual void createOutputNames() = 0;
    void setNumThreads(int numThreads);

public slots:
    void launchStage();

protected slots:
    void printOutput();
    void printError();
    void finished(int exitCode, QProcess::ExitStatus exitStatus); 

signals:
    void allProcessesFinished();
    
protected:
    virtual QStringList getArguments() = 0;
    virtual void cleanAfterFinished() = 0;
    
protected:
    QFileInfoList imageFiles;
    int amosProcessRange;
    int countProcess;
    QString m_program;
    QDir outputDirectory;
    int numThreads;
    QStringList outputPaths;

};

#endif // EXTERNALPROCESSWRAPPER_H
