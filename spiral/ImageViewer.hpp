/*
 * Author: Frank Erens
 */

#ifndef IMAGEVIEWER_HPP_
#define IMAGEVIEWER_HPP_

#include <QtWidgets/QScrollArea>

class QLabel;

class ImageViewer : public QScrollArea {
public:
    ImageViewer(QPixmap& px);
private:
    QLabel* mLabel;
};

#endif /* IMAGEVIEWER_HPP_ */
