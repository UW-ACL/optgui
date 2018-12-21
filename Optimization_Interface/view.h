// TITLE:   Optimization_Interface/view.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#ifndef VIEW_H
#define VIEW_H

#include <QGraphicsView>
#include <QToolButton>
#include <QMouseEvent>
#include <QGraphicsItem>

#include "canvas.h"
#include "menu_panel.h"
#include "globals.h"
#include "controller.h"

namespace interface {

const qreal DOT_SIZE = 20;

class View : public QGraphicsView
{
    Q_OBJECT

public:
    explicit View(QWidget *parent);
    ~View();
protected:
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
private slots:
    void openMenu();
    void closeMenu();
    void setZoom(int value);
    void setState(STATE button_type);
private:
    void initialize();
    QToolButton *menu_button_;
    MenuPanel *menu_panel_;
    STATE state_;
    Canvas *canvas_;
    Controller *controller_;
    QVector<QGraphicsItem*> *temp_markers_;
    QPen dot_pen_;
    QBrush dot_brush_;
};

}  // namespace

#endif // VIEW_H
