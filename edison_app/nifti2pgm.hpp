#ifndef NIFTY2PGM_H
#define NIFTY2PGM_H

#include <QObject>
#include <QList>
#include <itkImageFileReader.hxx>
#include <itkCastImageFilter.hxx>

#define ARMA_64BIT_WORD
// #define ARMA_NO_DEBUG
#include <armadillo>
//using namespace arma;

class Nifti2Pgm : public QObject
{
    Q_OBJECT
public:
    explicit Nifti2Pgm(QObject *parent = nullptr);
    ~Nifti2Pgm();
    void setNiftiFile(const QString &fileName, int index);
    void readNifti();
    void normalizeFlair();
    void binarizeLB();
    void createOutputCubes();
    QString writePGM(QString outputDir);

    QList<arma::fcube>& getCubesList();
    itk::Image< float, 3 >::Pointer getFlairImage() const;

    arma::fmat* getIm2pgm();
    arma::fmat* getIm2edison();

    QPair<float, float> getMinmaxFlair();

    itk::MetaDataDictionary getFlairDict() const;

signals:

private:
    template <class TImage>
    int ReadImage(const char * fileName, typename TImage::Pointer image)
    {
        using ImageType = TImage;
        using ImageReaderType = itk::ImageFileReader<ImageType>;

        typename ImageReaderType::Pointer reader = ImageReaderType::New();
        reader->SetFileName(fileName);

        try
        {
            reader->Update();
        }
        catch (itk::ExceptionObject & e)
        {
            std::cerr << e.what() << std::endl;
            return EXIT_FAILURE;
        }

        image->Graft(reader->GetOutput());

        return EXIT_SUCCESS;
    }

    int ReadScalarImage(const char * inputFileName, const itk::ImageIOBase::IOComponentType componentType)
    {
        switch (componentType)
        {
        default:
        case itk::ImageIOBase::UNKNOWNCOMPONENTTYPE:
            std::cerr << "Unknown and unsupported component type!" << std::endl;
            return EXIT_FAILURE;

        case itk::ImageIOBase::UCHAR:
        {
            using PixelType = unsigned char;
            using ImageType = itk::Image<PixelType, 3>;

            typename ImageType::Pointer image = ImageType::New();

            if (ReadImage<ImageType>(inputFileName, image) == EXIT_FAILURE)
            {
                return EXIT_FAILURE;
            }

            castImage<ImageType, PixelType>(image, imageList[indexFile]);
            // std::cout << imageList.at(indexFile) << std::endl;
            break;
        }

        case itk::ImageIOBase::CHAR:
        {
            using PixelType = char;
            using ImageType = itk::Image<PixelType, 3>;

            typename ImageType::Pointer image = ImageType::New();

            if (ReadImage<ImageType>(inputFileName, image) == EXIT_FAILURE)
            {
                return EXIT_FAILURE;
            }

            castImage<ImageType, PixelType>(image, imageList[indexFile]);
            // std::cout << imageList.at(indexFile) << std::endl;
            break;
        }

        case itk::ImageIOBase::USHORT:
        {
            using PixelType = unsigned short;
            using ImageType = itk::Image<PixelType, 3>;

            typename ImageType::Pointer image = ImageType::New();

            if (ReadImage<ImageType>(inputFileName, image) == EXIT_FAILURE)
            {
                return EXIT_FAILURE;
            }

            castImage<ImageType, PixelType>(image, imageList[indexFile]);
            // std::cout << imageList.at(indexFile) << std::endl;
            break;
        }

        case itk::ImageIOBase::SHORT:
        {
            using PixelType = short;
            using ImageType = itk::Image<PixelType, 3>;

            typename ImageType::Pointer image = ImageType::New();

            if (ReadImage<ImageType>(inputFileName, image) == EXIT_FAILURE)
            {
                return EXIT_FAILURE;
            }
            castImage<ImageType, PixelType>(image, imageList[indexFile]);
            // std::cout << imageList.at(indexFile) << std::endl;
            break;
        }

        case itk::ImageIOBase::UINT:
        {
            using PixelType = unsigned int;
            using ImageType = itk::Image<PixelType, 3>;

            typename ImageType::Pointer image = ImageType::New();

            if (ReadImage<ImageType>(inputFileName, image) == EXIT_FAILURE)
            {
                return EXIT_FAILURE;
            }

            castImage<ImageType, PixelType>(image, imageList[indexFile]);
            // std::cout << imageList.at(indexFile) << std::endl;
            break;
        }

        case itk::ImageIOBase::INT:
        {
            using PixelType = int;
            using ImageType = itk::Image<PixelType, 3>;

            typename ImageType::Pointer image = ImageType::New();

            if (ReadImage<ImageType>(inputFileName, image) == EXIT_FAILURE)
            {
                return EXIT_FAILURE;
            }

            castImage<ImageType, PixelType>(image, imageList[indexFile]);
            // std::cout << imageList.at(indexFile) << std::endl;
            break;
        }

        case itk::ImageIOBase::ULONG:
        {
            using PixelType = unsigned long;
            using ImageType = itk::Image<PixelType, 3>;

            typename ImageType::Pointer image = ImageType::New();

            if (ReadImage<ImageType>(inputFileName, image) == EXIT_FAILURE)
            {
                return EXIT_FAILURE;
            }

            castImage<ImageType, PixelType>(image, imageList[indexFile]);
            // std::cout << imageList.at(indexFile) << std::endl;
            break;
        }

        case itk::ImageIOBase::LONG:
        {
            using PixelType = long;
            using ImageType = itk::Image<PixelType, 3>;

            typename ImageType::Pointer image = ImageType::New();

            if (ReadImage<ImageType>(inputFileName, image) == EXIT_FAILURE)
            {
                return EXIT_FAILURE;
            }

            castImage<ImageType, PixelType>(image, imageList[indexFile]);
            // std::cout << imageList.at(indexFile) << std::endl;
            break;
        }

        case itk::ImageIOBase::FLOAT:
        {
            using PixelType = float;
            using ImageType = itk::Image<PixelType, 3>;

            typename ImageType::Pointer image = ImageType::New();

            if (ReadImage<ImageType>(inputFileName, image) == EXIT_FAILURE)
            {
                return EXIT_FAILURE;
            }

            castImage<ImageType, PixelType>(image, imageList[indexFile]);
            // std::cout << imageList.at(indexFile) << std::endl;
            break;
        }

        case itk::ImageIOBase::DOUBLE:
        {
            using PixelType = double;
            using ImageType = itk::Image<PixelType, 3>;

            typename ImageType::Pointer image = ImageType::New();

            if (ReadImage<ImageType>(inputFileName, image) == EXIT_FAILURE)
            {
                return EXIT_FAILURE;
            }

            castImage<ImageType, PixelType>(image, imageList[indexFile]);
            // std::cout << imageList.at(indexFile) << std::endl;
            break;
        }
        }
        return EXIT_SUCCESS;
    }
    template <class TImage, typename DataType> void
         castImage(typename TImage::Pointer inputImage,
                   itk::SmartPointer<itk::Image<float, 3> > outputImage) {

        typedef typename itk::CastImageFilter< itk::Image<DataType, 3>, itk::Image<float, 3> > CastFilterType;
        typedef typename CastFilterType::Pointer CastImageFilterPtr;
        CastImageFilterPtr filter = CastFilterType::New();
        filter->SetInput(inputImage);
        filter->Update();
        outputImage->Graft(filter->GetOutput());

    }

    template <typename DataType> void
    createCubeFromImageAccesor(arma::Cube<DataType>* cube, typename itk::Image< DataType, 3 >::Pointer itkImage,
                               bool writeAccess = true, bool copy_aux_mem = false,
                               bool strict = true)
    {

        if(writeAccess) {
//            void* vPointer = itkImage->GetBufferPointer();
//            DataType *mPointer = static_cast<DataType *>(vPointer);
            DataType* mPointer = itkImage->GetBufferPointer();
            itk::Size<3> size = itkImage->GetLargestPossibleRegion().GetSize();
            *cube = arma::Cube<DataType>(mPointer, size[0], size[1], size[2], copy_aux_mem, strict);
        }
        else {
            const void* vPointer = itkImage->GetBufferPointer();
            DataType *mPointer = const_cast<DataType*>(static_cast<const DataType *>(vPointer));
            itk::Size<3> size = itkImage->GetLargestPossibleRegion().GetSize();
            *cube = arma::Cube<DataType>(mPointer, size[0], size[1], size[2], copy_aux_mem, strict);
        }
    }

    template <typename InType, typename OutType>
    void castCube2NumType(arma::Cube<InType>* cube, arma::Cube<OutType>* outCube,
                          bool round = false,  bool create = true)
    {

        if(round)
            roundArma(cube, outCube, create);
        else
            copyCastingArma(cube, outCube, create);

    }

    template <typename ArmaTypeIn, typename ArmaTypeOut> void
    roundArma(ArmaTypeIn* armaIn, ArmaTypeOut* roundedArmaIn, bool create = true)
    {
        if(create)
            roundedArmaIn->copy_size(*armaIn);
        typename ArmaTypeIn::const_iterator a1 = armaIn->begin();
        typename ArmaTypeIn::const_iterator b1 = armaIn->end();
        typename ArmaTypeOut::iterator a2 = roundedArmaIn->begin();
        typename ArmaTypeIn::const_iterator i = a1;
        typename ArmaTypeOut::iterator j = a2;

        for( ;i != b1; ++i, ++j) {
            *j = round(*i);
        }
    }

    template <typename ArmaTypeIn, typename ArmaTypeOut>
    void copyCastingArma(ArmaTypeIn* armaIn, ArmaTypeOut* changedArmaIn,
                         bool create = true)
    {
        if(create)
            changedArmaIn->copy_size(*armaIn);
        typename ArmaTypeIn::const_iterator a1 = armaIn->begin();
        typename ArmaTypeIn::const_iterator b1 = armaIn->end();
        typename ArmaTypeOut::iterator a2 = changedArmaIn->begin();
        typename ArmaTypeIn::const_iterator i = a1;
        typename ArmaTypeOut::iterator j = a2;

        for( ;i != b1; ++i, ++j) {
            *j = *i;
        }
    }

    template <typename ArmaTypeIn, typename DataType = float>
    static void normalizeArma(ArmaTypeIn* armaIn)
    {
//        auto begin = chrono::high_resolution_clock::now();

        // 	typename ArmaTypeIn::const_iterator a1 = armaIn->begin();
        typename ArmaTypeIn::const_iterator b1 = armaIn->end();
        typename ArmaTypeIn::iterator i = armaIn->begin();

        DataType maxIn = -arma::datum::inf;
        DataType minIn = arma::datum::inf;
//        DataType maxIn = armaIn->max();
//        DataType minIn = armaIn->min();

        for( ;i != b1; ++i) {
            maxIn = *i > maxIn ? *i : maxIn;
            minIn = *i < minIn ? *i : minIn;
        }

        DataType range = maxIn - minIn;
        if(range <= vnl_math::eps)
            return;

        i = armaIn->begin();
        for( ;i != b1; ++i)
            *i = (*i - minIn) / range;

//        auto end = chrono::high_resolution_clock::now();
//        if(showTime && logSender) {
//            logSender->logText("normalizeArma " + QString::number(chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0) + "ms.");
//            // 	    cout << "normalizeArma " << chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0 << "ms" << endl;
//        }
    }

    template <typename ArmaTypeIn, typename ArmaTypeOut>
    static void binarizeArma(ArmaTypeIn* armaIn, ArmaTypeOut* binaryArmaIn,
                             float lowerThreshold, float upperThreshold = -1, bool create = false)
    {
//        auto begin = chrono::high_resolution_clock::now();

        if(create)
            binaryArmaIn->copy_size(*armaIn);
        typename ArmaTypeIn::const_iterator a1 = armaIn->begin();
        typename ArmaTypeIn::const_iterator b1 = armaIn->end();
        typename ArmaTypeOut::iterator a2 = binaryArmaIn->begin();
        //typename ArmaTypeOut::const_iterator b2 = binaryArmaIn->end();
        typename ArmaTypeIn::const_iterator i = a1;
        typename ArmaTypeOut::iterator j = a2;

        if(upperThreshold < 0) {
            for( ;i != b1; ++i, ++j) {
                if(*i >= lowerThreshold)
                    *j = 1;
                else
                    *j = 0;
            }
        }
        else {
            for( ;i != b1; ++i, ++j) {
                if(*i >= lowerThreshold && *i <= upperThreshold)
                    *j = 1;
                else
                    *j = 0;
            }
        }

//        auto end = chrono::high_resolution_clock::now();
//        if(showTime && logSender) {
//            logSender->logText("binarizeArma " + QString::number(chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0) + "ms.");
//            // 	    cout << "binarizeArma " << chrono::duration_cast<chrono::nanoseconds>(end-begin).count()/1000000.0 << "ms" << endl;
//        }
    }

private:
    QString niftiFile;
    QString flairFile;
    itk::ImageIOBase::IOPixelType pixelType;

    using IOComponentType = itk::ImageIOBase::IOComponentType;
    IOComponentType componentType;

    QList < typename itk::Image< float, 3 >::Pointer > imageList;
    QList < arma::fcube > cubesList;
    arma::fcube lbBinary;
    arma::fmat im2edison;
    arma::fmat im2pgm;
    int indexFile;
    QPair<float, float> minmaxFlair;
    itk::MetaDataDictionary flairDict;
};

#endif // NIFTY2PGM_H
