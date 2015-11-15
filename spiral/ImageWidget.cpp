/*
 * Author: Frank Erens
 */

#include "ImageWidget.hpp"

#include "ImageViewer.hpp"

constexpr int kDisplaySize = 128;

ImageWidget::ImageWidget(QWidget* parent) : QLabel{parent} {
    setFixedSize({kDisplaySize, kDisplaySize});
    setStyleSheet("QLabel { border: 1px solid black; background-color:white; }");
    setToolTip("Double click to enlarge image");
}

QImage ImageWidget::image() const {
    return mPixmap.toImage();
}

void ImageWidget::updateImage(const QImage& i) {
    mPixmap = QPixmap::fromImage(i);
    setPixmap(mPixmap.scaled(kDisplaySize, kDisplaySize));
}

bool ImageWidget::loadFromPath(const QString& path) {
    QImage img{path};
    bool correctsize = QList<QSize> {
        {64,64},
        {512,512},
        {4096, 4096}}.contains(img.size());
    if(!correctsize) return false;

    updateImage(img);
    return true;
}

void ImageWidget::saveToPath(const QString& path) const {
    mPixmap.save(path);
}

void ImageWidget::mouseDoubleClickEvent(QMouseEvent* e) {
    (void)e;
    if(pixmap()) {
        (new ImageViewer{mPixmap})->show();
    }
}
