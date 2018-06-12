#include <QApplication>
#include <QVBoxLayout>
#include <QSlider>
#include <QSpinBox>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QWidget *mainWindow = new QWidget();
    mainWindow->setWindowTitle("Hello World!");

    QSpinBox *spin = new QSpinBox;
    QSlider *slide = new QSlider(Qt::Horizontal);

    spin->setRange(0, 15);
    slide->setRange(0, 15);
    QObject::connect(spin, SIGNAL(valueChanged(int)), slide, SLOT(setValue(int)));
    QObject::connect(slide, SIGNAL(valueChanged(int)), spin, SLOT(setValue(int)));
    spin->setValue(0);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(slide);
    layout->addWidget(spin);
    mainWindow->setLayout(layout);

    mainWindow->show();
    return app.exec();
}
