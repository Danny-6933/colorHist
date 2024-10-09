#include "ColorHistogram.h"
#include "imageviewer.h"
#include "mainwindow.h"
#include "HistogramLabel.h"

#include <QtWidgets>

ColorHistogram::ColorHistogram(const QImage &_image):image(_image),
    hist(1<<24, 0), redSlices(256), greenSlices(256), blueSlices(256), thirdColorVal(0), alpha(255), pastIndex(8) {


    QHBoxLayout *mainLayout = new QHBoxLayout();
    setLayout(mainLayout);

    QVBoxLayout *leftSide = new QVBoxLayout();
    leftSideColor = new QHBoxLayout();
    QHBoxLayout *leftSideOptions = new QHBoxLayout();

    ImageViewer *imageViewer = new ImageViewer(image);

    mainLayout->addWidget(imageViewer);


    histogram = new HistogramLabel();
    histogram->setPixmap(QPixmap(256, 256));
    histogram->setToolTip("Hover over the histogram to see color details");

    connect(histogram, &HistogramLabel::colorHovered, this, &ColorHistogram::updateColorDisplay);

    colorCode = new QLabel();
    colorDisplay = new QLabel();

    leftSideColor->addWidget(colorCode);
    leftSideColor->addWidget(colorDisplay);


    showColorVal = new QLabel();
    showColorVal->setText("0");

    QRadioButton *displayColor = new QRadioButton("Display Color");
    connect(displayColor, &QRadioButton::toggled, this, &ColorHistogram::displayColorToggle);


    leftSideOptions->addWidget(showColorVal);
    leftSideOptions->addWidget(displayColor);

    colorSlider = new QSlider(Qt::Horizontal);
    colorSlider->setRange(0,255);
    colorSlider->setToolTip("Adjusts the intensity level for the selected color channel.");
    connect(colorSlider, &QSlider::valueChanged, this, &ColorHistogram::onSliderValueChanged);

    currentColor = 0;
    colorPicker = new QComboBox();
    colorPicker->addItem("Red");
    colorPicker->addItem("Green");
    colorPicker->addItem("Blue");
    colorPicker->setToolTip("Selects the color channel to display in the histogram.");
    connect(colorPicker, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ColorHistogram::onColorChanged);

    intensity = new QComboBox();
    intensity -> addItem("1");
    intensity -> addItem("2");
    intensity -> addItem("4");
    intensity -> addItem("8");
    intensity -> addItem("16");
    intensity -> addItem("32");
    intensity -> addItem("64");
    intensity -> addItem("128");
    intensity -> addItem("256");
    intensity ->setCurrentIndex(8);
    intensity->setToolTip("Controls how color frequency affects histogram brightness. Higher values make frequent colors appear brighter.");
    connect(intensity, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ColorHistogram::onIntensityChanged);

    colorCode->setVisible(false);
    colorDisplay->setVisible(false);
    leftSide->addWidget(histogram, 0, Qt::AlignCenter);
    leftSide->addLayout(leftSideColor, 0);
    leftSide->addLayout(leftSideOptions, 0);
    leftSide->addWidget(colorSlider);
    leftSide->addWidget(colorPicker, 0, Qt::AlignCenter);
    leftSide->addWidget(intensity, 0, Qt::AlignCenter);

    mainLayout->addLayout(leftSide);

    int totalPixels = image.width() * image.height();

    const QRgb *pixelData = reinterpret_cast<const QRgb *>(image.constBits());

    for (int i = 0; i < totalPixels; ++i) {
        QRgb pixel = pixelData[i];

        int red = (pixel >> 16) & 0xFF;
        int green = (pixel >> 8) & 0xFF;
        int blue = pixel & 0xFF;

        int colorIndex = (red << 16) | (green << 8) | blue;

        hist[colorIndex]++;
    }
    buildColor();
    showColor(&redSlices);
}

void ColorHistogram::displayColorToggle(bool checked) {
    colorCode->setVisible(checked);
    colorDisplay->setVisible(checked);
    histogram->enableMouseTracking(checked);
    if (!checked) {
        colorDisplay->clear();
        colorCode->clear();
    }
}

void ColorHistogram::updateColorDisplay(int x, int y) {
    int r = 0, g = 0, b = 0;

    switch (currentColor) {
    case 0:  // red selected
        r = thirdColorVal;
        g = y;
        b = x;
        break;
    case 1:  // green selected
        r = x;
        g = thirdColorVal;
        b = y;
        break;
    case 2:  // blue selected
        r = x;
        g = y;
        b = thirdColorVal;
        break;
    }

    QColor hoverColor(r, g, b);
    QPixmap colorPixmap(20, 20);
    colorPixmap.fill(hoverColor);
    colorDisplay->setPixmap(colorPixmap);
    colorCode->setText(hoverColor.name());
}



void ColorHistogram::onColorChanged(int index) {
    QString selectedColor = colorPicker->itemText(index);
    qDebug() << index;
    currentColor = index;

    if (selectedColor == "Red") {
        qDebug() << "red is selected";
        showColor(&redSlices);
    } else if (selectedColor == "Green") {
        qDebug() << "green is selected";
        showColor(&greenSlices);
    } else if (selectedColor == "Blue") {
        qDebug() << "blue is selected";
        showColor(&blueSlices);
    }
}

void ColorHistogram::onIntensityChanged(int index) {
    if (QMessageBox::question(this, "Confirm", "This requires some calculations, and may take a second. Do you want to proceed?")==QMessageBox::No) {
        intensity->blockSignals(true);
        intensity->setCurrentIndex(pastIndex);
        intensity->blockSignals(false);
        QMessageBox::information(this, "Cancelling", "You're impatient, I get it");
        return;
    } else {
        pastIndex = index;
        alpha = 2 << (index);
        buildColor();
    }
}

void ColorHistogram::onSliderValueChanged(int value) {
    QVector<QPixmap> *slices = nullptr;

    if (currentColor == 0) {
        slices = &redSlices;
    } else if (currentColor == 1) {
        slices = &greenSlices;
    } else if (currentColor == 2) {
        slices = &blueSlices;
    }
    showColorVal->setText(QString::number(value));

    histogram->setPixmap((*slices)[value]);
}

void ColorHistogram::showColor(QVector<QPixmap> *slices) {
    histogram->setPixmap((*slices)[thirdColorVal]);
}



void ColorHistogram::buildColor() {
    qDebug() << "pre-crash";
    Waiter waiter;
    // make red
    for (int c = 0; c < 256; ++c) {
        QPixmap pixmap(256, 256);
        pixmap.fill(Qt::white);
        QApplication::processEvents();

        QPainter painter(&pixmap);

        for (int x = 0; x < 256; ++x) {
            for (int y = 0; y < 256; ++y) {
                int colorIndex = (c << 16) | (y << 8) | x;

                int scale = alpha * hist[colorIndex];
                if (scale > 255) scale = 255;

                painter.setPen(hist[colorIndex] > 0 ? 0x010101*scale : Qt::black);
                painter.drawPoint(y, x); // had these x,y to begin but made this quick switch after using the test file you gave us
            }
        }

        (redSlices)[c] = pixmap;
    }


    // make green
    for (int c = 0; c < 256; ++c) {
        QPixmap pixmap(256, 256);
        pixmap.fill(Qt::white);
        QApplication::processEvents();

        QPainter painter(&pixmap);

        for (int x = 0; x < 256; ++x) {
            for (int y = 0; y < 256; ++y) {

                int colorIndex = (y << 16) | (c << 8) | x;
                int scale = alpha * hist[colorIndex];
                if (scale > 255) scale = 255;

                painter.setPen(hist[colorIndex] > 0 ? 0x010101*scale : Qt::black);
                painter.drawPoint(y, x); // had these x,y to begin but made this quick switch after using the test file you gave us
            }
        }

        (greenSlices)[c] = pixmap;
    }


    // make blue
    for (int c = 0; c < 256; ++c) {
        QPixmap pixmap(256, 256);
        pixmap.fill(Qt::white);
        QApplication::processEvents();

        QPainter painter(&pixmap);

        for (int x = 0; x < 256; ++x) {
            for (int y = 0; y < 256; ++y) {
                int colorIndex = (x << 16) | (y << 8) | c;

                int scale = alpha * hist[colorIndex];
                if (scale > 255) scale = 255;

                painter.setPen(hist[colorIndex] > 0 ? 0x010101*scale : Qt::black);
                painter.drawPoint(x, y);
            }
        }

        (blueSlices)[c] = pixmap;
    }
}

