#pragma once
#include <QObject>
#include <QStringList>
#include <QPixmap>
#include <vector>
#include <QLabel>



class ImageLoader : public QObject {
    Q_OBJECT
public:
    explicit ImageLoader(const QStringList& images, QLabel* videoView)
        : imageList(images), videoView(videoView), count(0)
    {

    }

signals:
    void imagesLoaded(std::vector<QPixmap> loadedImages, std::vector<QPixmap> scaledImages);

public slots:
    void loadImages() {
        std::vector<QPixmap> loaded;
        std::vector<QPixmap> saledLoaded;
        for (const QString& path : imageList) {
            QPixmap pixmap(path);
            if (!pixmap.isNull()) {

                loaded.push_back(pixmap);
                QPixmap scaledPixmap = pixmap.scaled( videoView->size(),
                                                     Qt::KeepAspectRatio,
                                                     Qt::SmoothTransformation);
                saledLoaded.push_back(scaledPixmap);
            }
        }
        emit imagesLoaded(loaded, saledLoaded);
    }


private:
    QStringList imageList;
    QLabel* videoView;
    int count;

};
