#include <QCoreApplication>
#include "mainprocessor.h"
#include <iostream>

int parseCmd(int argc, char *argv[]) {
    int ret = 0;
    if(argc == 2) {
        if (strcmp(argv[1], "-help") == 0) {
            std::cout << "edison_app -edisonpath <path to Edison> " << std::endl <<
                         "\t-indir <input images directory> " << std::endl <<
                         "\t-images <Flair file name> <T1 bet-mask-rsfl file name> " << std::endl <<
                         "\t-outdir <output images directory>" << std::endl;
            ret = 1;
        }

    }
    else if(argc != 10) {
        std::cout << "Invalid arguments number try edison_app -help" << std::endl;
        ret = 2;
    }
    else {
        if(strcmp(argv[1], "-edisonpath") != 0) {
            std::cout << "Invalid first argument try edison_app -help" << std::endl;
            ret = 3;
        }
        if(strcmp(argv[3], "-indir") != 0) {
            std::cout << "Invalid third argument try edison_app -help" << std::endl;
            ret = 4;
        }
        if(strcmp(argv[5], "-images") != 0) {
            std::cout << "Invalid fith argument try edison_app -help" << std::endl;
            ret = 5;
        }
        if(strcmp(argv[8], "-outdir") != 0) {
            std::cout << "Invalid nineth argument try edison_app -help" << std::endl;
            ret = 6;
        }
    }
    return ret;
}

bool getImagesFiles(QString inDirName, QStringList imageNames, QFileInfoList& imageFiles) {
    bool ret = true;
    for(int i = 0; i < 2; i++) {
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
    MainProcessor* processor = new MainProcessor(&a);

    int ret = parseCmd(argc, argv);
    if( ret != 0)
        return ret;
    QString edisonPath = argv[2];
    QStringList imageNames;
    QString inDirName = argv[4];
    QFileInfo inDir(inDirName);
    if(!inDir.exists()) {
        std::cout << inDirName.toStdString() << " does not exists" << std::endl;
        return 7;
    }
    QFileInfoList imageFiles;
    for(int i = 0; i < 2; i++)
        imageNames.append(argv[6 + i]);

    QString outDirName = argv[9];
    QFileInfo outDir;
    outDir.setFile(outDirName);
    if(!outDir.exists()) {
        if(!QDir::root().mkdir(outDir.absoluteFilePath())) {
            std::cout << "Can not create output directory  " << outDirName.toStdString() << std::endl;
            return 8;
        }
    }

    if(!getImagesFiles(inDirName, imageNames, imageFiles))
        return 9;

    processor->runProcesses(imageFiles, edisonPath, outDir.absoluteFilePath());

    return a.exec();
}
