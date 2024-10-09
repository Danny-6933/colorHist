#ifndef COLORHISTOGRAM_H
#define COLORHISTOGRAM_H

#include <QtWidgets>
#include "HistogramLabel.h"

class ColorHistogram : public QWidget
{
    Q_OBJECT

    QImage image;
    QVector<int> hist;
    HistogramLabel *histogram;
    QLabel *showColorVal;
    QVector<QPixmap> redSlices;
    QVector<QPixmap> greenSlices;
    QVector<QPixmap> blueSlices;
    int currentColor;
    int thirdColorVal;
    QHBoxLayout *leftSideColor;

    QComboBox *colorPicker;
    QSlider *colorSlider;
    QComboBox *intensity;
    int alpha;
    int pastIndex;


    QLabel *colorCode;
    QLabel *colorDisplay;
public:
    explicit ColorHistogram(const QImage &_image);

private slots:
    void onColorChanged(int index);
    void onIntensityChanged(int index);
    void showColor(QVector<QPixmap> *slices);
    void buildColor();
    void onSliderValueChanged(int value);
    void displayColorToggle(bool checked);
    void updateColorDisplay(int x, int y);

signals:
};

#endif // COLORHISTOGRAM_H
