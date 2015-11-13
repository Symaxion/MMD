/*
 * Author: Frank Erens
 */

#include "ImageViewer.hpp"

#include <QtWidgets/QLabel>

ImageViewer::ImageViewer(QPixmap& px) {
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle("Image Viewer");
    setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    mLabel = new QLabel{this};
    mLabel->setPixmap(px);
    mLabel->setScaledContents(true);

    setWidget(mLabel);
}
