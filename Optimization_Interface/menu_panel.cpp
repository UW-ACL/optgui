// TITLE:   Optimization_Interface/menu_panel.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "menu_panel.h"

#include <QHBoxLayout>
#include <QPainter>
#include <QtMath>

#include "view.h"

namespace interface {

//constructor
MenuPanel::MenuPanel(QWidget *parent, Qt::WindowFlags flags)
    : QFrame(parent, flags) {
    this->initialize();
}

//destructor
MenuPanel::~MenuPanel() {
    // delete menu controls
    for (MenuButton *button : *this->menu_buttons_) {
        delete button;
    }
    delete this->menu_buttons_;
    delete this->sim_button_;
    delete this->exec_button_;
    delete this->sim_button_;
    delete this->zoom_slider_;
    delete this->menu_->layout();

    // delete menu panel components
    delete this->menu_;
    delete this->close_button_;
    delete this->layout();
}

//initializes values
void MenuPanel::initialize() {
    // Set color
    this->setAutoFillBackground(true);
    QPalette palette = this->palette();
    QColor background = QWidget::palette().window().color();
    background.setAlpha(200);
    palette.setColor(QPalette::Base, background);
    this->setPalette(palette);

    // Set frame
    this->setFrameShape(QFrame::Panel);
    this->setFrameShadow(QFrame::Raised);

    // Set layout
    this->setLayout(new QHBoxLayout(this));
    this->layout()->setContentsMargins(1, 1, 1, 1);
    this->layout()->setSpacing(1);

    // Create menu close button
    this->close_button_ = new QToolButton(this);
    this->close_button_->setArrowType(Qt::RightArrow);
    this->close_button_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    this->close_button_->setFixedSize(10, 40);
    this->layout()->addWidget(this->close_button_);
    this->layout()->setAlignment(this->close_button_, Qt::AlignLeft);

    // Create menu
    this->menu_ = new QWidget(this);
    this->menu_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->menu_->setLayout(new QVBoxLayout(this->menu_));
    this->menu_->layout()->setContentsMargins(0, 10, 0, 10);
    this->menu_->layout()->setSpacing(0);
    this->layout()->addWidget(this->menu_);

    // Create menu buttons
    this->menu_buttons_ = new QVector<MenuButton*>();
    this->initializePointButton();
    this->initializeEllipseButton();
    this->initializePolygonButton();
    this->initializePlaneButton();
    this->initializeWaypointButton();
    this->initializeEraserButton();
    this->initializeFlipButton();

    // Show menu buttons
    for (MenuButton *button : *this->menu_buttons_) {
        this->menu_->layout()->addWidget(button);
        this->menu_->layout()->setAlignment(button,
                                            Qt::AlignTop|Qt::AlignCenter);
    }
    this->initializeAddEllipseButton();
    this->initializeHorizonSlider();
    this->initializeFinaltimeSlider();

    //Add stretch between sliders and dialogue box
    this->menu_->layout()->addItem(new QSpacerItem(2,10));


    //Create dialogue box for the user
    this->user_msg_label_ = new QLabel();
    this->user_msg_label_->setText("User dialogue box output.");
    this->user_msg_label_->setWordWrap(true);
    this->user_msg_label_->setMargin(2);
    this->user_msg_label_->setStyleSheet("QLabel { background-color : black; color : white; }");
    this->menu_->layout()->addWidget(this->user_msg_label_);

    // Create simulate button
    this->initializeSimButton();

    // Create execute button
    this->initializeExecButton();

    // Create zoom slider
    this->initializeZoomSlider();

}

//initializes point button
void MenuPanel::initializePointButton() {
    MenuButton *point_button = new MenuButton(POINT, this->menu_);
    QPixmap pix(50, 50);
    pix.fill(Qt::transparent);
    QPainter painter(&pix);
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen(Qt::red);
    pen.setWidth(2);
    painter.setPen(pen);
    painter.setBrush(Qt::red);
    painter.drawEllipse(15, 15, 20, 20);
    point_button->setPixmap(pix);
    point_button->setToolTip(tr("Add point"));
    this->menu_buttons_->append(point_button);
}

//initializes ellipse button
void MenuPanel::initializeEllipseButton() {
    MenuButton *ellipse_button = new MenuButton(ELLIPSE, this->menu_);
    QPixmap pix(50, 50);
    pix.fill(Qt::transparent);
    QPainter painter(&pix);
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen(Qt::black);
    pen.setWidth(2);
    painter.setPen(pen);
    painter.setBrush(Qt::gray);
    painter.drawEllipse(1, 1, 48, 48);
    ellipse_button->setPixmap(pix);
    ellipse_button->setToolTip(tr("Add ellipse constraint"));
    this->menu_buttons_->append(ellipse_button);
}

//initializes polygon button
void MenuPanel::initializePolygonButton() {
    MenuButton *polygon_button = new MenuButton(POLYGON, this->menu_);
    QPixmap pix(50, 50);
    pix.fill(Qt::transparent);
    QPainter painter(&pix);
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen(Qt::black);
    pen.setWidth(2);
    painter.setPen(pen);
    painter.setBrush(Qt::gray);

    QPolygonF poly;
    poly << QPointF(25, 1);
    poly << QPointF(49, 21);
    poly << QPointF(40, 49);
    poly << QPointF(10, 49);
    poly << QPointF(1, 21);
    painter.drawPolygon(poly);

    polygon_button->setPixmap(pix);
    polygon_button->setToolTip(tr("Add polygon constraint"));
    this->menu_buttons_->append(polygon_button);
}

//initializes plane button
void MenuPanel::initializePlaneButton() {
    MenuButton *plane_button = new MenuButton(PLANE, this->menu_);
    QPixmap pix(50, 50);
    pix.fill(Qt::transparent);
    QPainter painter(&pix);
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen(Qt::black);
    pen.setWidth(3);
    painter.setPen(pen);
    painter.setBrush(Qt::gray);
    painter.drawLine(QPointF(10, 40), QPointF(40, 10));

    plane_button->setPixmap(pix);
    plane_button->setToolTip(tr("Add plane constraint"));
    this->menu_buttons_->append(plane_button);
}

//initializes eraser button
void MenuPanel::initializeEraserButton() {
    MenuButton *eraser_button = new MenuButton(ERASER, this->menu_);
    QPixmap pix(":/images/erasericon.png");
    pix = pix.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    eraser_button->setPixmap(pix);
    eraser_button->setToolTip(tr("Delete item"));
    this->menu_buttons_->append(eraser_button);
}

//initializes waypoint button
void MenuPanel::initializeWaypointButton() {
    MenuButton *waypoint_button = new MenuButton(WAYPOINT, this->menu_);
    QPixmap pix(50, 50);
    pix.fill(Qt::transparent);
    QPainter painter(&pix);
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen(Qt::black);
    pen.setWidth(1);
    painter.setPen(pen);
    painter.setBrush(Qt::red);

    painter.drawEllipse(QPointF(7, 7), 6, 6);
    painter.setBrush(Qt::white);
    painter.drawEllipse(QPointF(19, 19), 6, 6);
    painter.drawEllipse(QPointF(31, 31), 6, 6);
    painter.setBrush(Qt::green);
    painter.drawEllipse(QPointF(42, 42), 6, 6);

    waypoint_button->setPixmap(pix);
    waypoint_button->setToolTip(tr("Add waypoint"));
    this->menu_buttons_->append(waypoint_button);
}

//initializes constraint flipping button
void MenuPanel::initializeFlipButton() {
    MenuButton *flip_button = new MenuButton(FLIP, this->menu_);
    QPixmap pix(50, 50);
    pix.fill(Qt::transparent);
    QPainter painter(&pix);
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen(Qt::black);
    pen.setWidth(3);
    painter.setPen(pen);
    painter.setBrush(Qt::gray);

    QLineF line(QPointF(10, 40), QPointF(40, 10));

    QPolygonF poly;
    poly << line.p1();
    poly << line.p2();
    poly << line.normalVector().translated(
                line.dx(), line.dy()).pointAt(10.0 / line.length());
    poly << line.normalVector().pointAt(10.0 / line.length());

    painter.setPen(Qt::NoPen);
    painter.drawPolygon(poly);
    painter.setPen(pen);
    painter.drawLine(line);

    flip_button->setPixmap(pix);
    flip_button->setToolTip(tr("Flip constraint directionity"));
    this->menu_buttons_->append(flip_button);
}

//initializes slider for number of iterations allowed (horizon length)
void MenuPanel::initializeHorizonSlider() {
    this->opt_horizon_slider_ = new QSlider(Qt::Vertical, this->menu_);
    this->opt_horizon_slider_->setSizePolicy(QSizePolicy::Expanding,
                                      QSizePolicy::Minimum);
    this->opt_horizon_slider_->setTickInterval(1);
    this->opt_horizon_slider_->setTickPosition(QSlider::TicksAbove);
    this->opt_horizon_slider_->setMinimum(16);
    this->opt_horizon_slider_->setMaximum(32);
    this->opt_horizon_slider_->setValue(this->horizonlength_init_);
    this->opt_horizon_slider_->setToolTip(tr("Set horizon length"));

    this->opt_horizon_label_ = new QLabel();
    this->opt_horizon_label_->setText("T=" + QString::number(horizonlength_init_));

    this->menu_->layout()->addWidget(this->opt_horizon_label_);
    this->menu_->layout()->addWidget(this->opt_horizon_slider_);
    this->menu_->layout()->setAlignment(this->opt_horizon_slider_, Qt::AlignBottom);
}

//initializes slider for final time constraints
void MenuPanel::initializeFinaltimeSlider() {
    QWidget *wid = new QWidget(this->menu_);
    wid->setLayout(new QHBoxLayout(wid));
    wid->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    this->opt_finaltime_slider_ = new QSlider(Qt::Vertical, this->menu_);
    this->opt_finaltime_slider_->setSizePolicy(QSizePolicy::Expanding,
                                      QSizePolicy::Minimum);
    this->opt_finaltime_slider_->setTickInterval(1);
    this->opt_finaltime_slider_->setTickPosition(QSlider::TicksAbove);
    this->opt_finaltime_slider_->setMinimum(20);
    this->opt_finaltime_slider_->setMaximum(200);
    this->opt_finaltime_slider_->setValue(this->finaltime_init_*10); //make sure this points to the right member
    this->opt_finaltime_slider_->setToolTip(tr("Set final time"));

    this->opt_finaltime_label_ = new QLabel();
    this->opt_finaltime_label_->setText("T=" + QString::number(finaltime_init_));
    wid->layout()->addWidget(this->opt_finaltime_slider_);
    wid->layout()->addWidget(this->opt_finaltime_label_);


    this->menu_->layout()->addWidget(this->opt_finaltime_label_);
    this->menu_->layout()->addWidget(this->opt_finaltime_slider_);
    this->menu_->layout()->setAlignment(wid, Qt::AlignBottom);
}

//initializes button for executing traj to vehicle
void MenuPanel::initializeExecButton() {
    this->exec_button_ = new QPushButton("Exec", this->menu_);
    this->exec_button_->setToolTip(tr("Execute optimization with constraints"));
    this->menu_->layout()->addWidget(this->exec_button_);
    this->menu_->layout()->setAlignment(this->exec_button_, Qt::AlignBottom);
}

//initializes button for simulating traj on tablet
void MenuPanel::initializeSimButton() {
    this->sim_button_ = new QPushButton("Sim", this->menu_);
    this->sim_button_->setToolTip(tr("Simulate the optimization with constraints"));
    this->menu_->layout()->addWidget(this->sim_button_);
    this->menu_->layout()->setAlignment(this->sim_button_, Qt::AlignBottom);
}

void MenuPanel::initializeAddEllipseButton() {
    this->add_ellipse_button_ = new QPushButton("Add ellipse", this->menu_);
    this->add_ellipse_button_->setToolTip(tr("Add ellipse"));
    this->menu_->layout()->addWidget(this->add_ellipse_button_);
    this->menu_->layout()->setAlignment(this->add_ellipse_button_, Qt::AlignBottom);
}

/* Initializes button for freezing vehicle (TODO: not yet used)
void MenuPanel::initializeFreezeButton() {
    this->freeze_button_ = new MenuButton(FREEZE, this->menu_);
    this->freeze_button_->setText("Freeze");
    this->freeze_button_->setToolTip(tr("Freeze optimization"));
    this->menu_->layout()->addWidget(this->freeze_button_);
    this->menu_->layout()->setAlignment(this->freeze_button_, Qt::AlignBottom);
    this->freeze_button_->setCheckable(true);
    this->menu_buttons_->append(this->freeze_button_);
}
*/

//initializes slider for map scaling (TODO: link to controller at init)
void MenuPanel::initializeZoomSlider() {
    this->zoom_slider_ = new QSlider(Qt::Horizontal, this->menu_);
    this->zoom_slider_->setSizePolicy(QSizePolicy::Expanding,
                                      QSizePolicy::Minimum);
    this->zoom_slider_->setTickInterval(1);
    this->zoom_slider_->setTickPosition(QSlider::TicksAbove);
    this->zoom_slider_->setMinimum(1);
    this->zoom_slider_->setMaximum(49);
    this->zoom_slider_->setValue(this->zoom_init_);
    this->zoom_slider_->setToolTip(tr("Set zoom level"));
    this->menu_->layout()->addWidget(this->zoom_slider_);
    this->menu_->layout()->setAlignment(this->zoom_slider_, Qt::AlignBottom);
}

}  // namespace interface
