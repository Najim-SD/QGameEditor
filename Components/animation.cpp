#include "animation.h"

Animation::Animation(QString animationName, QString filePath, QString projectPath, AnimationFileType fType, int hf, int vf, int fps, bool transpPixel)
{
    name = animationName;
    QString fullName = Helper::getFileNameWithExtension(filePath);
    QString fileName = Helper::getNameWithOutExtension(fullName);
    QString extension = Helper::getExtension(fullName);

    fileType = fType;
    horizontalFrames = hf;
    verticalFrames = vf;
    frameRate = fps;
    firstPixelAsTransparent = transpPixel;

    switch (fileType) {
    case SingleFile:{
        QString dataFilePath = projectPath + "/data/" + fullName;
        QFile::copy(filePath, dataFilePath);
        filesPaths.append(dataFilePath);

        if(horizontalFrames == 1 && verticalFrames == 1){
            if(extension == "gif"){
                // use QMovie to extract frames
                QMovie * gifMovie = new QMovie(dataFilePath);
                int framesCount = gifMovie->frameCount();
                for(int i = 0; i < framesCount; i++){
                    Frame *frame = new Frame;
                    gifMovie->jumpToFrame(i);
                    if(firstPixelAsTransparent){
                        QImage img = gifMovie->currentImage();
                        frame->pixmap = drawClippedImage(img, img.pixel(0,0));
                    }else{
                        frame->pixmap = new QPixmap(gifMovie->currentPixmap());
                    }
                    frames.append(frame);
                }

            }else{
                Frame *frame = new Frame;
                if(firstPixelAsTransparent){
                    QImage img(dataFilePath);
                    frame->pixmap = drawClippedImage(img, img.pixel(0,0));
                }else{
                    frame->pixmap = new QPixmap(dataFilePath);
                }
                frames.append(frame);
            }
        }
        else{
            // divide by hf and vf
            QImage sheet = QImage(dataFilePath);
            int framesCount = horizontalFrames * verticalFrames;
            int imgWidth = sheet.width() / horizontalFrames;
            int imgHeight = sheet.height() / verticalFrames;

            for(int i = 0; i < framesCount; i++){
                Frame *frame = new Frame;
                int xp = i%hf * imgWidth;
                int yp = i/hf * imgHeight;
                if(firstPixelAsTransparent){
                    QImage img = sheet.copy(xp,yp, imgWidth, imgHeight);
                    frame->pixmap = drawClippedImage(img, sheet.pixel(0,0));
                }else{
                    frame->pixmap = new QPixmap(QPixmap::fromImage(sheet.copy(xp,yp, imgWidth, imgHeight)));
                }
                frames.append(frame);
            }
        }
    break;
    }       // end SingleFile
    case MultipleFiles:{
        QString digitLess = fileName;
        while(digitLess.at(digitLess.size()-1).isDigit()) digitLess.remove(digitLess.size()-1,1);
        QString directory = filePath.left(filePath.lastIndexOf("/"));
        QDir dir(directory);
        dir.setFilter(QDir::Files);
        QStringList files = dir.entryList();

        for(int i = 0; i < files.size(); i++){
            QString fileDigitLess = Helper::getNameWithOutExtension(files[i]);
            while(fileDigitLess.at(fileDigitLess.size()-1).isDigit()) fileDigitLess.remove(fileDigitLess.size()-1,1);
            if(digitLess == fileDigitLess){
                QString dataFilePath = projectPath + "/data/" + files[i];
                QString originalFilePath = directory + "/" + files[i];
                QFile::copy(originalFilePath, dataFilePath);
                filesPaths.append(dataFilePath);

                // Load the dataFilePath into a frame
                // --------------------------------------------------------
                Frame *frame = new Frame;
                if(firstPixelAsTransparent){
                    QImage img(dataFilePath);
                    frame->pixmap = drawClippedImage(img, img.pixel(0,0));
                }else{
                    frame->pixmap = new QPixmap(dataFilePath);
                }
                frames.append(frame);
            }
        }
    break;
    }       // end MultipleFiles
    }   // end switch
}

// Draws the image to a new QPixmap with the maskColor as transparent pixels
QPixmap * Animation::drawClippedImage(QImage &img, QRgb maskColor)
{
    QBitmap mask = QBitmap::fromImage(img.createMaskFromColor(maskColor));
    QPixmap * pix = new QPixmap(img.width(), img.height());
    pix->fill(QColor(0,0,0,0));
    QPainter p(pix);
    p.setClipRegion(QRegion(mask));
    p.drawImage(img.rect(), img, img.rect());

    return pix;
}
