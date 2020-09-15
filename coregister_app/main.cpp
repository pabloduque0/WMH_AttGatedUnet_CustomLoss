#include <QCoreApplication>
#include "resamplingslicerwrapper.h"
#include <iostream>

int parseCmd(int argc, char *argv[]) {
    int ret = 0;
    if(argc == 2) {
        if (strcmp(argv[1], "-help") == 0) {
            std::cout << "coregister_app -3dspath <path to 3DSlicer> " << std::endl <<
                         "\t-indir <input images directory" << std::endl <<
                         "\t-images <Flair file name> <T1 file name> <T1 bet-mask file name> " << std::endl <<
                         "\t-outdir <output images directory>" << std::endl <<
                         "\t-dbc <do bias correction> (optional)" << std::endl << std::endl <<
                         "If -dbc is not used Flair image is copied to output directory"  << std::endl;
            ret = 1;
        }

    }
    else if(argc < 11 || argc > 12) {
        std::cout << "Invalid arguments number try coregister_app -help" << std::endl;
        ret = 2;
    }
    else if (argc <= 12) {
        if(strcmp(argv[1], "-3dspath") != 0) {
            std::cout << "Invalid first argument try coregister_app -help" << std::endl;
            ret = 3;
        }
        if(strcmp(argv[3], "-indir") != 0) {
            std::cout << "Invalid third argument try coregister_app -help" << std::endl;
            ret = 4;
        }
        if(strcmp(argv[5], "-images") != 0) {
            std::cout << "Invalid fith argument try coregister_app -help" << std::endl;
            ret = 5;
        }
        if(strcmp(argv[9], "-outdir") != 0) {
            std::cout << "Invalid nineth argument try coregister_app -help" << std::endl;
            ret = 6;
        }
        if (argc == 12 && strcmp(argv[11], "-dbc") != 0) {
            std::cout << "Invalid last argument try coregister_app -help" << std::endl;
            ret = 7;
        }
    }
    else {
        std::cout << "Invalid arguments try coregister_app -help" << std::endl;
        ret = 8;
    }
    return ret;
}

bool getImagesFiles(QString inDirName, QStringList imageNames, QFileInfoList& imageFiles) {
    bool ret = true;
    for(int i = 0; i < 3; i++) {
        QFileInfo fileInfoF(QDir(inDirName), imageNames.at(i));
        if(fileInfoF.exists())
            imageFiles.append(fileInfoF);
        else {
            std::cout << fileInfoF.filePath().toStdString() << " does not exist" << std::endl;
            ret = ret && false;
        }
    }
    return ret;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    ResamplingSlicerWrapper *process = new ResamplingSlicerWrapper(&a);

    int ret = parseCmd(argc, argv);
    if( ret != 0)
        return ret;
    QString slicerPath = argv[2];
    QStringList imageNames;
    QString inDirName = argv[4];
    QFileInfo inDir(inDirName);
    if(!inDir.exists()) {
        std::cout << inDirName.toStdString() << " does not exists" << std::endl;
        return 9;
    }
    QFileInfoList imageFiles;
    for(int i = 0; i < 3; i++)
        imageNames.append(argv[6 + i]);

    QString outDirName = argv[10];
    QFileInfo outDir;
    outDir.setFile(outDirName);
    if(!outDir.exists()) {
        if(!QDir::root().mkdir(outDir.absoluteFilePath())) {
            std::cout << "Can not create output directory  " << outDirName.toStdString() << std::endl;
            return 10;
        }
    }

    bool doBiasCorrection = argc == 12 ? true : false;

    if(!getImagesFiles(inDirName, imageNames, imageFiles))
        return 11;
    QMap<QString, QString> parameters;
    parameters["do bias correction"] = QString::number(doBiasCorrection);

    if(!process->setProgramPath(slicerPath))
        return 10;
    process->setParameters(parameters);
    process->setImages(imageFiles);
    process->setOutputPaths(outDir);
    process->createOutputNames();
    process->launchStage();

    return a.exec();
}
