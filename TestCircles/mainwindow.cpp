#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "obstacle.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->canvas = new QGraphicsScene(this);
    obstacle *circle = new obstacle(nullptr);
    this->canvas->addItem(circle);

    ui->graphicsView->setScene(this->canvas);
}

MainWindow::~MainWindow()
{
    delete ui;
}
