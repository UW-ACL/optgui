#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include "controlbox.h"
#include "view.h"
#include "canvas.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    Canvas *canvas;
    View *view;
    ControlBox *controls;

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
