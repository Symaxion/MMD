/*
 * Author: Frank Erens
 */

#include "MainWindow.hpp"

#include "ImageWidget.hpp"
#include "Spiralify.hpp"

#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

#include <iostream>

MainWindow::MainWindow() {
    setWindowTitle("Color spiral");

    // TODO prevent resizing
    setSizePolicy({});

    // Main Layout
    mLayout = new QHBoxLayout{this};

    mImagePre = new ImageWidget{this};
    mLayout->addWidget(mImagePre);

    mImagePost = new ImageWidget{this};
    mLayout->addWidget(mImagePost);

    mSideWidget = new QWidget{this};
    mLayout->addWidget(mSideWidget);

    // Sidebar layout
    mSideLayout = new QVBoxLayout{mSideWidget};

    mLoadButton = new QPushButton{"Load ...", this};
    mSideLayout->addWidget(mLoadButton);
    connect(mLoadButton, &QPushButton::clicked, this, &MainWindow::loadImage);

    mSaveButton = new QPushButton{"Save ...", this};
    mSideLayout->addWidget(mSaveButton);
    connect(mSaveButton, &QPushButton::clicked, this, &MainWindow::saveImage);

    mRunButton = new QPushButton{"Run!", this};
    mRunButton->setEnabled(false);
    mSideLayout->addWidget(mRunButton);
    connect(mRunButton, &QPushButton::clicked, this, &MainWindow::run);

    mSideLayout->addStretch();

}

void MainWindow::loadImage() {
    QString fileName = QFileDialog::getOpenFileName(this,
            "Load Image", "", "Image files (*.png *.jpg *.bmp)");
    if(!fileName.isEmpty()) {
        bool result = mImagePre->loadFromPath(fileName);
        if(!result) {
            QMessageBox::critical(this, "Incorrect image size",
                    "Image size needs to be either 64x64, 512x512 or "
                    "4096x4096.");
            return;
        }
        mImagePost->clear();
    }

    mRunButton->setEnabled(true);
}

void MainWindow::saveImage() {
    QString fileName = QFileDialog::getSaveFileName(this,
            "Save Image", "", "Image Files (*.png *jpg *.bmp)");
    if(!fileName.isEmpty()) {
        mImagePost->saveToPath(fileName);
    }
}

void MainWindow::run() {
    QImage from = mImagePre->image();

    unsigned colordepth = QHash<int, unsigned>{
        {64, 4},
        {512, 6},
        {4096, 8},
    }[from.height()];

    mImagePost->updateImage(spiralify(from, colordepth));
}
