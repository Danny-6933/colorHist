#include "mainwindow.h"

#include <QtWidgets>

#include "ColorHistogram.h"

MainWindow *mainWindow;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {

    QAction *openFileAct = new QAction("Open image file");
    connect(openFileAct, &QAction::triggered, this, &MainWindow::openFileSlot);

    openFileAct->setShortcut(Qt::CTRL | Qt::Key_O);

    QMenu *fileMenu = new QMenu("File");
    fileMenu->addAction(openFileAct);
    menuBar()->addMenu(fileMenu);


    QSettings settings("Very Real Company LLC", "Graphics1");
    lastDir = settings.value("lastDir", "").toString();

    mainWindow = this;

}

MainWindow::~MainWindow() {
    QSettings settings("Very Real Company LLC", "Graphics1");
    settings.setValue("lastDir", lastDir);
}

int Waiter::count = 0;

Waiter::Waiter() {
    if (++count == 1) {
        mainWindow->setEnabled(false);
        mainWindow->statusBar()->showMessage("patience is a virtue...");
        mainWindow->statusBar()->show();
        QApplication::processEvents();
    }
}

Waiter::~Waiter(){
    if (--count == 0) {
        mainWindow->setEnabled(true);
        mainWindow->statusBar()->clearMessage();
        mainWindow->statusBar()->hide();
        QApplication::processEvents();
    }
}


void MainWindow::openFileSlot() {
    qDebug() << "clickéd";

    QString fName = QFileDialog::getOpenFileName(this, "Select image file", lastDir, "Image files (*.png *.jpeg *.jpg *.bmp)");
    if (fName.isEmpty()) return;

    lastDir = QFileInfo(fName).absolutePath();
    // qDebug() << lastDir;


    QImage image(fName);
    if (image.isNull()) return;



    ColorHistogram *colorHistogram = new ColorHistogram(image);

    setCentralWidget(colorHistogram);
}
