#include "externalprocesswrapper.h"
#include <iostream>
#include <QCoreApplication>

using namespace std;

ExternalProcessWrapper::ExternalProcessWrapper(QObject* parent) :
     QProcess(parent)
{
    connect (this, SIGNAL(readyReadStandardOutput()), this, SLOT(printOutput()));
    connect (this, SIGNAL(readyReadStandardError()), this, SLOT(printError()));
    connect (this, SIGNAL(finished(int , QProcess::ExitStatus )), this, SLOT(finished(int , QProcess::ExitStatus ))); 
    
    amosProcessRange = 0;
    countProcess = 1;
}

ExternalProcessWrapper::~ExternalProcessWrapper()
{    
}

void ExternalProcessWrapper::setNumThreads(int numThreads)
{
    this->numThreads = numThreads;
}

void ExternalProcessWrapper::launchStage()
{
    
    QStringList arguments = getArguments();
    
    start(m_program, arguments);
}

void ExternalProcessWrapper::printOutput()
{
    QByteArray byteArray = readAllStandardOutput();
    QStringList strLines = QString(byteArray).split("\n");

    foreach (QString line, strLines){
        cout<< line.toStdString() << endl;
    }
}

void ExternalProcessWrapper::printError()
{
    QByteArray byteArray = readAllStandardOutput();
    QStringList strLines = QString(byteArray).split("\n");

    foreach (QString line, strLines){
        cout << line.toStdString() << endl;
    }
}

void ExternalProcessWrapper::setImages(QFileInfoList imageFiles)
{
    this->imageFiles = imageFiles;  
}

void ExternalProcessWrapper::finished(int exitCode, QProcess::ExitStatus exitStatus)
{
    cout << "----------------------------------------------------" << endl;
    cout << "exitCode " << exitCode << " exitStatus " << exitStatus << endl;

    if(countProcess < amosProcessRange) {
        countProcess++;
        launchStage();
    }
    else {
        cleanAfterFinished();
        cout << "Images generated:" << endl;
        for(int i = 0; i < outputPaths.size(); i++)
            cout << outputPaths.at(i).toStdString() << endl;
        cout << endl << "Edison execution finished " << endl;
        emit allProcessesFinished();
    }
}
