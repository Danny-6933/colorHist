#include "HistogramLabel.h"

HistogramLabel::HistogramLabel(QWidget *parent) : QLabel(parent), mouseTrackingEnabled(false) {
    setMouseTracking(true);
}

void HistogramLabel::enableMouseTracking(bool enable) {
    mouseTrackingEnabled = enable;
}

void HistogramLabel::mouseMoveEvent(QMouseEvent *event) {
    if (!mouseTrackingEnabled || !pixmap() || !rect().contains(event->pos())) {
        return;
    }

    QPoint pos = event->pos();
    emit colorHovered(pos.x(), pos.y());
}
