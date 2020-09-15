#ifndef MAINPROCESSOR_H
#define MAINPROCESSOR_H

#include <QCoreApplication>
#include <QObject>

#include "nifti2pgm.hpp"
#include "edisonwrapper.h"

class MainProcessor : public QObject
{
    Q_OBJECT
public:
    MainProcessor(QCoreApplication* a, QObject * parent = nullptr);
    virtual ~MainProcessor();

    void runProcesses(QFileInfoList imageFiles, QString edisonPath, QString outDir);
    void endMessages();

private slots:
    void enhanceContrast();

private:
    arma::umat hist3(arma::fvec *XY, arma::fvec *Y, uint n_binsX, uint n_binsY);    

private:
    QString outputDir;
    Nifti2Pgm* nifti2Pgm;
    EdisonWrapper* edisonWrapper;
    QCoreApplication* app;
    QFileInfoList imageFiles;
    QFileInfo enhancedFileInfo;
    arma::fcube FL_en;
    arma::fcube FL_org;
    std::chrono::time_point<std::chrono::high_resolution_clock> begin;
};

#endif // MAINPROCESSOR_H
