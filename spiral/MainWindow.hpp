/*
 * Author: Frank Erens
 */

#ifndef MAINWINDOW_HPP_
#define MAINWINDOW_HPP_

#include <QtWidgets/QWidget>

class ImageWidget;

class QHBoxLayout;
class QVBoxLayout;
class QPushButton;

class MainWindow : public QWidget {
public:
    MainWindow();

public slots:
    void loadImage();
    void saveImage();
    void run();

private:
    QHBoxLayout* mLayout;
    QWidget* mSideWidget;
    QVBoxLayout* mSideLayout;
    QPushButton* mLoadButton;
    QPushButton* mSaveButton;
    QPushButton* mRunButton;
    ImageWidget* mImagePre;
    ImageWidget* mImagePost;

};

#endif /* MAINWINDOW_HPP_ */
