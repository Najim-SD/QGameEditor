#include "animation.h"

Animation::Animation(QString filePath, QString projectPath, AnimationFileType fType, int hf, int vf, int fps)
{
    QString fullName = filePath.right(filePath.length() - filePath.lastIndexOf("/") - 1);
    name = fullName.left(fullName.indexOf("."));
    QString extension = fullName.right(fullName.size() - name.size() - 1);

    fileType = fType;
    horizontalFrames = hf;
    verticalFrames = vf;
    frameRate = fps;

    switch (fileType) {
    case SingleFile:{
        QString dataFilePath = projectPath + "/data/" + filePath.right(filePath.length() - filePath.lastIndexOf("/") - 1);
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
                    frame->pixmap = new QPixmap(gifMovie->currentPixmap());
                    frames.append(frame);
                }

            }else{
                Frame *frame = new Frame;
                frame->pixmap = new QPixmap(dataFilePath);
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
                frame->pixmap = new QPixmap(QPixmap::fromImage(sheet.copy(xp,yp, imgWidth, imgHeight)));
                frames.append(frame);
            }
        }
    break;
    }       // end SingleFile
    case MultipleFiles:{

    break;
    }       // end MultipleFiles
    }
}
