#ifndef MENUBUTTON_H
#define MENUBUTTON_H

#include <QToolButton>
#include <QFrame>

class MenuButton : public QToolButton
{
    Q_OBJECT

public:
    MenuButton(QWidget *parent = nullptr, Qt::ArrowType arrow = Qt::LeftArrow);
};

#endif // MENUBUTTON_H
