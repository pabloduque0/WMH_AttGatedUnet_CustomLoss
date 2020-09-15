#include "nifti2pgm.hpp"
#include <itkNiftiImageIOFactory.h>
#include <itkNiftiImageIO.h>
#include <QFileInfo>
#include <QDir>

Nifti2Pgm::Nifti2Pgm(QObject *parent) : QObject(parent)
{
    itk::NiftiImageIOFactory::RegisterOneFactory();
    using ImageType =  itk::Image< float, 3 >;
    for(int i = 0; i < 2; i++) {
        imageList.append(ImageType::New());
        arma::fcube cube;
        cubesList.append(cube);
    }
}

Nifti2Pgm::~Nifti2Pgm()
{
//    delete image;
}

void Nifti2Pgm::setNiftiFile(const QString &fileName, int index)
{
    niftiFile = fileName;
    indexFile = index;
    if(indexFile == 0)
        flairFile = niftiFile;
    readNifti();
}

void Nifti2Pgm::readNifti()
{
    itk::NiftiImageIO::Pointer imageIO = itk::NiftiImageIO::New();
    imageIO->SetFileName(niftiFile.toStdString());
    imageIO->ReadImageInformation();

    pixelType = imageIO->GetPixelType();

    std::cout << "Pixel Type is " << itk::ImageIOBase::GetPixelTypeAsString(pixelType) << std::endl;

    componentType = imageIO->GetComponentType();

    std::cout << "Component Type is " << imageIO->GetComponentTypeAsString(componentType) << std::endl;

    const unsigned int imageDimension = imageIO->GetNumberOfDimensions();

    std::cout << "Image Dimension is " << imageDimension << std::endl;

    switch (pixelType) {
        case itk::ImageIOBase::SCALAR: {
            ReadScalarImage(niftiFile.toStdString().c_str(), componentType);
        }
        break;

        default:
            std::cerr << "not implemented yet!" << std::endl;
//        return EXIT_FAILURE;
    }

    createCubeFromImageAccesor<float>(&cubesList[indexFile], imageList[indexFile]);
    if(indexFile == 0) {
        minmaxFlair.first = cubesList.at(0).min();
        minmaxFlair.second = cubesList.at(0).max();
        flairDict = imageIO->GetMetaDataDictionary();
        imageList.at(0).GetPointer()->SetMetaDataDictionary(flairDict);
    }
}

void Nifti2Pgm::normalizeFlair()
{
    normalizeArma<arma::fcube>(&cubesList[0]);
}

void Nifti2Pgm::binarizeLB()
{
    bool create = true;
    binarizeArma< arma::fcube, arma::fcube >(&cubesList[1], &lbBinary, 1, -1.f, create);
}

void Nifti2Pgm::createOutputCubes()
{
//    arma::fcube flairLB = cubesList[0];
    arma::fcube flairLB = cubesList[0] % lbBinary;
    arma::uword rows = cubesList.at(0).n_rows;
    arma::uword cols = cubesList.at(0).n_cols;
    arma::uword slices = cubesList.at(0).n_slices;
    im2edison.zeros(rows*slices, cols);
    im2pgm.zeros(rows*slices, cols);

    arma::uword initrow = 0;
    arma::fmat im2, im;
    for(arma::uword its = 0; its < slices; its++) {
        im2 = cubesList.at(0).slice(its);
        im = flairLB.slice(its);
        im2edison.rows(initrow, initrow + rows - 1) = im; //.t()
        im2pgm.rows(initrow, initrow + rows - 1) = im2; //
        initrow += rows;
    }
}

QString Nifti2Pgm::writePGM(QString outputDir)
{
    arma::Mat<uchar> pgmData;
    arma::Mat<float> pgmDataF = im2edison * 255;
    roundArma(&pgmDataF, &pgmData);
    QFileInfo inInfo(QDir(outputDir), flairFile);
    QFileInfo outInfo(QDir(outputDir), inInfo.baseName() + ".pgm");
    pgmData.save(outInfo.absoluteFilePath().toStdString(), arma::pgm_binary);

    return outInfo.absoluteFilePath();
}

QList<arma::fcube>& Nifti2Pgm::getCubesList()
{
    return cubesList;
}

itk::Image< float, 3 >::Pointer Nifti2Pgm::getFlairImage() const
{
    return imageList.at(0);
}

arma::fmat* Nifti2Pgm::getIm2pgm()
{
    return &im2pgm;
}

arma::fmat* Nifti2Pgm::getIm2edison()
{
    return &im2edison;
}

QPair<float, float> Nifti2Pgm::getMinmaxFlair()
{
    return minmaxFlair;
}

itk::MetaDataDictionary Nifti2Pgm::getFlairDict() const
{
    return flairDict;
}



