#ifndef STARTBUTTON_H
#define STARTBUTTON_H


#include <QLabel>
#include <QPixmap>
#include <QPainter>
#include <QMouseEvent>

class StartButton : public QLabel
{
    Q_OBJECT

public:
    explicit StartButton(QWidget *parent = nullptr);
    void mousePressEvent(QMouseEvent *event);
public slots:
    void buttonOn();
    void buttonOff();
signals:
    void startOn();
    void startOff();
private:
    QPixmap *buttonIcon;
    bool clicked;

    void generateIcon();
};

#endif // STARTBUTTON_H
