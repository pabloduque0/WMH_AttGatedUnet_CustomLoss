#include "mainprocessor.h"
#define ARMA_64BIT_WORD
// #define ARMA_NO_DEBUG
#include <armadillo>
//using namespace arma;

#include <QTimer>
#include <itkImageFileWriter.h>
#include <iostream>

MainProcessor::MainProcessor(QCoreApplication* a, QObject *parent) :
                QObject(parent), app(a)
{
    begin = std::chrono::high_resolution_clock::now();
    nifti2Pgm = new Nifti2Pgm();
    edisonWrapper = new EdisonWrapper();
    connect(edisonWrapper, SIGNAL(allProcessesFinished()), this, SLOT(enhanceContrast()));
}

MainProcessor::~MainProcessor()
{
    delete  nifti2Pgm;
    delete edisonWrapper;
    app->quit();
}

void MainProcessor::runProcesses(QFileInfoList imageFiles,
                                 QString edisonPath, QString outDir)
{

    this->imageFiles = imageFiles;

    if(!edisonWrapper->setProgramPath(edisonPath)) {
        std::cout << "Wrong path to Edison " << edisonPath.toStdString() << std::endl;
        return;
    }
    this->outputDir = outDir;

    for(int i = 0; i < 2; i++) {
        nifti2Pgm->setNiftiFile(imageFiles.at(i).absoluteFilePath(), i);
    }

    QList<arma::fcube> cubes = nifti2Pgm->getCubesList();
    FL_org = cubes.at(0);
    nifti2Pgm->normalizeFlair();
    nifti2Pgm->binarizeLB();
    nifti2Pgm->createOutputCubes();
    QString pgmFileName = nifti2Pgm->writePGM(outputDir);

    //edisonWrapper->setParameters(parameters);
    edisonWrapper->setImages(imageFiles);
    edisonWrapper->setOutputPaths(outputDir);
    edisonWrapper->createOutputNames();
    edisonWrapper->createEdisonFile(pgmFileName);
    edisonWrapper->launchStage();
    // enhanceContrast() is launched by signal from edisonWrapper.
}

void MainProcessor::enhanceContrast()
{
    QList<arma::fcube> cubes = nifti2Pgm->getCubesList();
    arma::fcube* FL = &(cubes[0]);
    arma::fcube* LB = &(cubes[1]);
    arma::fmat* im2edison = nifti2Pgm->getIm2edison();
    //arma::fmat* im2pgm = nifti2Pgm->getIm2pgm();
    QString gradientFileName = edisonWrapper->getGradientFile();
    QFileInfo gradientFile(gradientFileName);
    if(!gradientFile.exists()) {
        std::cout << "Gradient file " << gradientFileName.toStdString()
                  << " does not exist" << std::endl;
        return;
    }

    arma::fmat gradientMap;
    gradientMap.load(gradientFileName.toStdString(), arma::raw_ascii);

    FL_en = arma::zeros<arma::fcube>(arma::size(*FL));
//    FL_en = *FL;
    uint bins_g = 1024;
    uint bins_i = 1024;
    arma::fvec gradientMapVec = arma::vectorise(gradientMap);
    arma::fvec im2edisonVec = arma::vectorise(*im2edison);
    arma::umat N = hist3(&gradientMapVec, &im2edisonVec, bins_g, bins_i);
    N.col(0) = arma::zeros<arma::uvec>(bins_g);
    arma::umat NC = cumsum(cumsum(N, 0), 1);

    float max_iwm =im2edison->max();
    arma::fvec i_l = arma::linspace<arma::fvec>(0, max_iwm, bins_i);
    arma::fvec g_l = arma::linspace<arma::fvec>(0, 1, bins_g);
    arma::fvec p = arma::zeros<arma::fvec>(bins_g);
    for(arma::uword gi = 0; gi < bins_g; gi++) {
        unsigned long long sz = sum(NC.col(gi));
        if(sz == 0)
            p(gi) = 0;
        else {
            float sm = sum(NC.col(gi) % g_l);
            p(gi) = sm/static_cast<float>(sz);
        }
    }

    arma::fvec P = cumsum(p % i_l);

    for(arma::uword its = 0; its < FL->n_slices; its++) {
        for(arma::uword i=0; i < FL->n_rows; i++) {
            for(arma::uword j=0; j < FL->n_cols; j ++) {
//                ulong ind = static_cast<ulong>(roundf((*im2pgm)(i + its * FL->n_rows, j) * bins_g));
                ulong ind = static_cast<ulong>(roundf((*im2edison)(i + its * FL->n_rows, j) * bins_g));
                if(ind == 0)
                    FL_en(i, j, its) = 0;
                else
                    FL_en(i, j, its) = (P(ind - 1));
            }
        }
    }
    QPair<float, float> minmax = nifti2Pgm->getMinmaxFlair();
    float maxFL = minmax.second;
    float minFL = minmax.first;
    float minFL_en = FL_en.min();
    float maxFL_en = FL_en.max();
    FL_en = (FL_en - minFL_en) / (maxFL_en - minFL_en)*(maxFL-minFL) + minFL;
    arma::uvec ind = find((*LB) < 0.5);
    FL_en(ind) = FL_org(ind);

    typedef itk::Image< float, 3 > ITKImageType;
    typename ITKImageType::Pointer inputImage;

    std::memcpy(nifti2Pgm->getFlairImage().GetPointer()->GetBufferPointer(),
                FL_en.memptr(), FL_en.n_elem*sizeof(float));

    using ImageType = itk::Image<float, 3>;
    using WriterType = itk::ImageFileWriter<ImageType>;
    WriterType::Pointer writer = WriterType::New();
    QString suffix = imageFiles.at(0).completeSuffix();
    enhancedFileInfo.setFile(QDir(outputDir),
                       imageFiles.at(0).baseName() + "-enhanced." + suffix);
    writer->SetFileName(enhancedFileInfo.absoluteFilePath().toStdString());
    writer->SetInput(nifti2Pgm->getFlairImage().GetPointer());
    writer->Update();
    endMessages();
}

arma::umat MainProcessor::hist3(arma::fvec *X, arma::fvec *Y, uint n_binsX, uint n_binsY)
{
    arma::umat histogram;
    if(X->n_elem != Y->n_elem) {
        std::cout << "Vectors with different number of elements" << std::endl;
        return histogram;
    }

    histogram.zeros(n_binsX, n_binsY);
    arma::uword num_elems = X->n_elem;
    float minX = X->min();
    float maxX = X->max();
    float minY = Y->min();
    float maxY = Y->max();
    float lengthX = maxX - minX;
    float lengthY = maxY - minY;
    uint indexX, indexY;
    for(arma::uword i = 0; i < num_elems; i++) {
        indexX = static_cast<uint>(truncf((X->at(i) - minX) * n_binsX / lengthX));
        indexY = static_cast<uint>(truncf((Y->at(i) - minY) * n_binsY / lengthY));
        if(indexX == n_binsX)
            --indexX;
        if(indexY == n_binsY)
            --indexY;
        histogram(indexX, indexY) += 1;
    }

    return histogram;
}

void MainProcessor::endMessages()
{
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << std::endl << "Images generated:" << std::endl;
    std::cout << enhancedFileInfo.absoluteFilePath().toStdString() << std::endl;
    std::cout << std::endl << QString::number(std::chrono::duration_cast<std::chrono::nanoseconds>
                 (end - begin).count()/1000000.0).toStdString() << "ms." << std::endl;
    std::cout << "Contrast enhancement finished" << std::endl;
    app->quit();
}
