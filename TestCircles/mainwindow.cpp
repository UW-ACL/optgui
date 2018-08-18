#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "obstacle.h"
#include "view.h"

#include <QtDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //this->setAttribute(Qt::WA_TranslucentBackground, true);

    ui->setupUi(this);

    this->canvas = new Canvas(this);
    this->view = new View(this);


    this->view->setRenderHint(QPainter::Antialiasing);
    this->view->setScene(this->canvas);

    //this->centralWidget()->layout()->addWidget(this->view);
    this->setCentralWidget(this->view);

    this->view->setMinimumWidth(600);
    this->view->setMinimumHeight(300);

    //this->canvas->setSceneRect(this->view->rect());
    this->canvas->setSceneRect(this->view->rect());

    this->controls = new ControlBox(this);
    this->addToolBar(Qt::RightToolBarArea, controls);

//    QPen mypen = QPen(Qt::red);
//    mypen.setWidth(10);

//    QLineF topLine(canvas->sceneRect().topLeft(), canvas->sceneRect().topRight());
//    QLineF leftLine(canvas->sceneRect().topLeft(), canvas->sceneRect().bottomLeft());
//    QLineF rightLine(canvas->sceneRect().topRight(), canvas->sceneRect().bottomRight());
//    QLineF bottomLine(canvas->sceneRect().bottomLeft(), canvas->sceneRect().bottomRight());

//    canvas->addLine(topLine, mypen);
//    canvas->addLine(rightLine, mypen);
//    canvas->addLine(leftLine, mypen);
//    canvas->addLine(bottomLine, mypen);

    Obstacle *circle = new Obstacle(this->canvas, 50.0);
    this->canvas->addItem(circle);

    qDebug() << "does this work";
}

MainWindow::~MainWindow()
{
    delete ui;
}
