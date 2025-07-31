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
        : imageList(images), videoView(videoView), count(0) {}

signals:
    void imagesLoaded(std::vector<QPixmap> loadedImages);

public slots:
    void loadImages() {
        std::vector<QPixmap> loaded;
        for (const QString& path : imageList) {
            QPixmap pixmap(path);
            if (!pixmap.isNull()) {
                QPixmap scaledPixmap = pixmap.scaled(
                    videoView->size(),
                    Qt::KeepAspectRatio,
                    Qt::SmoothTransformation
                    );

                loaded.push_back(scaledPixmap);
                count++;

                if(loaded.size() == 50 || (count == imageList.size()))
                {
                    emit imagesLoaded(loaded);
                    loaded.clear();
                }
            }
        }
        // emit imagesLoaded(loaded);
    }

private:
    QStringList imageList;
    QLabel* videoView;
    int count;

};
