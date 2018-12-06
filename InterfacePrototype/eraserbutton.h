#ifndef ERASERBUTTON_H
#define ERASERBUTTON_H

#include <QLabel>
#include <QPixmap>
#include <QMouseEvent>

class EraserButton : public QLabel
{
    Q_OBJECT

public:
    explicit EraserButton(QWidget *parent = nullptr);
    void mousePressEvent(QMouseEvent *event);
public slots:
    void buttonOff();
    void buttonOn();
signals:
    void eraserOn();
    void eraserOff();
private:
    QPixmap *buttonIcon;
    bool clicked;

    void generateIcon();
};

#endif // ERASERBUTTON_H
