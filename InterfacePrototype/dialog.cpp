#include "dialog.h"
#include "ui_dialog.h"

#include <QGraphicsScene>
#include <QPainter>

#include <QDebug>
#include <obstacle.h>

Dialog::Dialog(QWidget *parent)
    : QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Window);

    this->canvas = new Canvas(this);
    this->view = new View(canvas, this);
    this->layout()->addWidget(view);
}

Dialog::~Dialog()
{
    delete ui;
}
