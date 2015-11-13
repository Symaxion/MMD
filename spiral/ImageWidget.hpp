/*
 * Author: Frank Erens
 */

#ifndef IMAGEWIDGET_HPP_
#define IMAGEWIDGET_HPP_

#include <QtGui/QImage>
#include <QtWidgets/QLabel>

class ImageWidget : public QLabel {
public:
    ImageWidget(QWidget* parent = nullptr);

    QImage image() const;
    void updateImage(const QImage& i);

    void loadFromPath(const QString& path);
    void saveToPath(const QString& path) const;

    void mouseDoubleClickEvent(QMouseEvent* e) override;

private:
    // QImage mImage;
    QPixmap mPixmap;
};


#endif /* IMAGEWIDGET_HPP_ */
