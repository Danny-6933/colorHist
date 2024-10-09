#ifndef HISTOGRAMLABEL_H
#define HISTOGRAMLABEL_H

#include <QLabel>
#include <QColor>
#include <QMouseEvent>

class HistogramLabel : public QLabel {
    Q_OBJECT

public:
    explicit HistogramLabel(QWidget *parent = nullptr);
    void enableMouseTracking(bool enable);

signals:
    void colorHovered(int x, int y);

protected:
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    bool mouseTrackingEnabled;
};

#endif // HISTOGRAMLABEL_H
