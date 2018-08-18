#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include "view.h"
#include "canvas.h"

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

private:
    Ui::Dialog *ui;
    Canvas *canvas;
    View *view;

};

#endif // DIALOG_H
