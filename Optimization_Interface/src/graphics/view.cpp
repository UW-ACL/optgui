// TITLE:   Optimization_Interface/src/graphics/view.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "include/graphics/view.h"

#include <QHBoxLayout>
#include <QScrollBar>
#include <QInputDialog>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QMessageBox>

#include "include/controls/compute_thread.h"

namespace optgui {

View::View(QWidget * parent)
    : QGraphicsView(parent) {
    // Create Canvas
    QStringList background_images = {"lab_indoor_-6_-6_6_6",
            "demo-campus_outdoor_47.65355_-122.30755_120.0905_167.7810",
            "demo-field_outdoor_47.67158_-121.94751_304.6741_372.8843"};

    QString background_image = QInputDialog::getItem(this, tr("Select scene"),
                                      tr("mode"), background_images, 0, false);

    this->canvas_ = new Canvas(this, background_image);
    this->setScene(this->canvas_);

    // Create Controller
    // added menu panel to construction
    this->controller_ = new Controller(this->canvas_, this->menu_panel_);

    // Set State
    this->state_ = IDLE;

    // Set pen and brush
    dot_pen_ = QPen(Qt::black);
    dot_brush_ = QBrush(Qt::white);

    // Create poly vector
    this->temp_markers_ = new QVector<QGraphicsItem*>();

    // set graphical settings menu panels
    this->initialize();
}

View::~View() {
    // Delete temporary markers
    this->clearMarkers();
    delete this->temp_markers_;

    for (QWidget *button : this->panel_widgets_) {
        delete button;
    }

    // Delete layout components
    delete this->menu_button_;
    delete this->menu_panel_;
    delete this->layout();

    // Delete controller and canvas
    delete this->controller_;
    delete this->canvas_;
}

void View::setPorts() {
    this->setState(IDLE);
    this->controller_->setPorts();
}

void View::initialize() {
    // Set color
    this->setAutoFillBackground(true);
    QPalette palette = this->palette();
    QColor background = QWidget::palette().window().color();
    background.setAlpha(200);
    palette.setColor(QPalette::Base, background);
    this->setPalette(palette);

    // Set Layout
    this->setLayout(new QHBoxLayout(this));
    this->layout()->setContentsMargins(0, 0, 0, 0);

    // Set scroll preferences
    this->setDragMode(QGraphicsView::ScrollHandDrag);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setResizeAnchor(QGraphicsView::AnchorViewCenter);

    // Set rendering preference
    this->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    // Set render hint
    this->setRenderHint(QPainter::Antialiasing);

    this->initializeExpertPanel();
    this->initializeMenuPanel();

    // Expand view to fill screen
    this->expandView();
}

void View::initializeMenuPanel() {
    this->menu_panel_ = new MenuPanel(this, true);

    // Create open menu button
    this->menu_button_ = new QToolButton(this);
    this->menu_button_->setArrowType(Qt::LeftArrow);
    this->menu_button_->setSizePolicy(QSizePolicy::Fixed,
                                      QSizePolicy::Fixed);
    this->menu_button_->setFixedSize(10, 40);
    this->layout()->addWidget(this->menu_button_);
    this->layout()->setAlignment(this->menu_button_, Qt::AlignRight);

    // Configure menu panel parameters
    this->menu_panel_->setSizePolicy(QSizePolicy::Expanding,
                                     QSizePolicy::Expanding);
    this->menu_panel_->setFixedWidth(100);
    this->layout()->addWidget(this->menu_panel_);
    this->layout()->setAlignment(this->menu_panel_, Qt::AlignRight);

    // initialize menu buttons
    this->initializeFinalPointButton(this->menu_panel_);
    this->initializeEllipseButton(this->menu_panel_);
    this->initializePolygonButton(this->menu_panel_);
    this->initializePlaneButton(this->menu_panel_);
    this->initializeWaypointButton(this->menu_panel_);
    this->initializeEraserButton(this->menu_panel_);
    this->initializeFlipButton(this->menu_panel_);
    this->initializeMessageBox(this->menu_panel_);
    this->initializeFinaltime(this->menu_panel_);
    this->initializeDuplicateButton(this->menu_panel_);
    this->initializeExecButton(this->menu_panel_);
    this->initializeZoom(this->menu_panel_);

    // add space at the bottom
    this->menu_panel_->menu_layout_->insertStretch(-1, 1);

    // Connect menu open/close
    connect(this->menu_button_, SIGNAL(clicked()),
            this, SLOT(openMenu()));
    connect(this->menu_panel_->close_button_, SIGNAL(clicked()),
            this, SLOT(closeMenu()));

    // Default to menu closed
    this->closeMenu();
}

void View::initializeExpertPanel() {
    this->expert_panel_ = new MenuPanel(this, false);

    // Create open expert menu button
    this->expert_menu_button_ = new QToolButton(this);
    this->expert_menu_button_->setArrowType(Qt::RightArrow);
    this->expert_menu_button_->setSizePolicy(QSizePolicy::Fixed,
                                             QSizePolicy::Fixed);
    this->expert_menu_button_->setFixedSize(10, 40);
    this->layout()->addWidget(this->expert_menu_button_);
    this->layout()->setAlignment(this->expert_menu_button_, Qt::AlignLeft);

    // Configure expert menu panel parameters
    this->expert_panel_->setSizePolicy(QSizePolicy::Expanding,
                                     QSizePolicy::Expanding);
    this->expert_panel_->setFixedWidth(100);
    this->layout()->addWidget(this->expert_panel_);
    this->layout()->setAlignment(this->expert_panel_, Qt::AlignLeft);

    // initialize menu buttons
    this->initializeHorizon(this->expert_panel_);

    // add space at the bottom
    this->expert_panel_->menu_layout_->insertStretch(-1, 1);

    // Connect menu open/close
    connect(this->expert_menu_button_, SIGNAL(clicked()),
            this, SLOT(openExpertMenu()));
    connect(this->expert_panel_->close_button_, SIGNAL(clicked()),
            this, SLOT(closeExpertMenu()));

    // Default to menu closed
    this->closeExpertMenu();
}

void View::mousePressEvent(QMouseEvent *event) {
    // Grab event position in scene coordinates
    QPointF pos = this->mapToScene(event->pos());

    switch (this->state_) {
        case POINT: {
            this->controller_->updateFinalPosition(pos);
            break;
        }
        case ELLIPSE: {
            // TODO(bchasnov): disallow user from adding
            // ellipse if it overlaps?
            qreal scaling_factor = this->matrix().m11();
            this->controller_->addEllipse(pos,
                                          DEFAULT_RAD / scaling_factor);
            break;
        }
        case POLYGON: {
            // Add markers until polygon is completed
            if (!this->temp_markers_->isEmpty() &&
                    itemAt(event->pos()) == this->temp_markers_->first()) {
                if (this->temp_markers_->size() >= 3) {
                    QVector<QPointF> poly = QVector<QPointF>();
                    for (QGraphicsItem *dot : *this->temp_markers_) {
                        poly.append(QPointF(dot->pos()));
                    }
                    this->controller_->addPolygon(poly);
                }
                // Clean up markers
                this->clearMarkers();
            } else {
                // Add temporary marker
                qreal dotSize = DOT_SIZE / this->matrix().m11();
                QGraphicsItem *dot =
                        this->scene()->addEllipse(-dotSize, -dotSize,
                                                  dotSize * 2, dotSize * 2,
                                                  dot_pen_, dot_brush_);
                temp_markers_->append(dot);
                dot->setPos(pos);
            }
            break;
        }
        case PLANE: {
            // Add markers to define line
            if (!this->temp_markers_->isEmpty()) {
                QPointF p1 = QPointF(temp_markers_->first()->pos());
                this->controller_->addPlane(p1, pos);
                // Clean up markers
                this->clearMarkers();
            } else {
                // Add temporary marker
                qreal dotSize = DOT_SIZE / this->matrix().m11();
                QGraphicsItem *dot =
                        this->scene()->addEllipse(-dotSize, -dotSize,
                                                  dotSize * 2, dotSize * 2,
                                                  dot_pen_, dot_brush_);
                temp_markers_->append(dot);
                dot->setPos(pos);
            }
            break;
        }
        case WAYPOINT: {
            this->controller_->addWaypoint(pos);
            break;
        }
        case ERASER: {
            // Delete clicked item if possible
            if (itemAt(event->pos())) {
                this->controller_->removeItem(itemAt(event->pos()));
            }
            break;
        }
        case FLIP: {
            // Flip clicked item if possible
            if (itemAt(event->pos())) {
                this->controller_->flipDirection(itemAt(event->pos()));
            }
            break;
        }
        default: {
            QGraphicsView::mousePressEvent(event);
        }
    }
}

void View::closeMenu() {
    this->menu_panel_->hide();
    this->menu_button_->show();

    this->setState(IDLE);

    this->repaint();
}

void View::closeExpertMenu() {
    this->expert_panel_->hide();
    this->expert_menu_button_->show();

    this->setState(IDLE);

    this->repaint();
}

void View::openMenu() {
    this->menu_button_->hide();
    this->menu_panel_->show();
    this->update();
}

void View::openExpertMenu() {
    this->expert_menu_button_->hide();
    this->expert_panel_->show();
    this->update();
}

void View::setZoom(qreal value) {
    this->resetMatrix();
    this->scale(value, value);
}

void View::setState(STATE button_type) {
    // Clear all temporary markers
    this->clearMarkers();

    // Deselect items
    // this->canvas_->clearSelection();

    // Go to idle or switch to new state
    bool set_idle = false;
    if (button_type == this->state_) {
        this->state_ = IDLE;
        set_idle = true;
    } else {
        this->state_ = button_type;
    }

    // Unclick menu buttons
    for (MenuButton *button : this->toggle_buttons_) {
        if (set_idle || button->getButtonType() != button_type) {
            button->setFrameShadow(QFrame::Raised);
        }
    }
}

void View::execute() {
    this->clearMarkers();
    this->setState(IDLE);
    this->controller_->execute();
}

void View::duplicateSelected() {
    this->setState(IDLE);
    this->controller_->duplicateSelected();
}

void View::setFinaltime(qreal final_time) {
    this->controller_->setFinaltime(final_time);
}

void View::setHorizon(int horizon_length) {
    this->controller_->setHorizonLength(horizon_length);
}

void View::clearMarkers() {
    // Clear all temporary markers
    for (QGraphicsItem *dot : *this->temp_markers_) {
        this->scene()->removeItem(dot);
        delete dot;
    }
    this->temp_markers_->clear();
}

void View::resizeEvent(QResizeEvent *event) {
    this->expandView();

    QGraphicsView::resizeEvent(event);
}

void View::expandView() {
    // Expand scene to viewable area
    QRectF oldView = this->viewport()->rect();
    oldView.translate(-oldView.center().x(), -oldView.center().y());

    this->scene()->setSceneRect((this->scene()->sceneRect()).united(oldView));
    this->setSceneRect(this->scene()->sceneRect());
}

void View::initializeMessageBox(MenuPanel *panel) {
    QLabel *user_msg_label = new QLabel();
    user_msg_label->setText("User dialogue box output.");
    user_msg_label->setWordWrap(true);
    user_msg_label->setContentsMargins(2, 2, 2, 2);
    user_msg_label->setStyleSheet(
            "QLabel { background-color : black; color : white; }");
    panel->menu_->layout()->addWidget(user_msg_label);
    this->panel_widgets_.append(user_msg_label);

    connect(this->controller_->compute_thread_, SIGNAL(setMessage(QString)),
            user_msg_label, SLOT(setText(QString)));
}

void View::initializeFinalPointButton(MenuPanel *panel) {
    MenuButton *point_button = new MenuButton(POINT, panel->menu_);
    QPixmap pix(50, 50);
    pix.fill(Qt::transparent);
    QPainter painter(&pix);
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen(Qt::black);
    pen.setWidth(2);
    painter.setPen(pen);
    painter.setBrush(Qt::red);
    painter.drawEllipse(15, 15, 20, 20);
    point_button->setPixmap(pix);
    point_button->setToolTip(tr("Set final point"));
    this->panel_widgets_.append(point_button);
    this->toggle_buttons_.append(point_button);

    panel->menu_->layout()->addWidget(point_button);
    panel->menu_->layout()->setAlignment(point_button,
                                        Qt::AlignTop|Qt::AlignCenter);

    connect(point_button, SIGNAL(changeState(STATE)),
            this, SLOT(setState(STATE)));
}

void View::initializeEllipseButton(MenuPanel *panel) {
    MenuButton *ellipse_button = new MenuButton(ELLIPSE, panel->menu_);
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
    this->panel_widgets_.append(ellipse_button);
    this->toggle_buttons_.append(ellipse_button);

    panel->menu_->layout()->addWidget(ellipse_button);
    panel->menu_->layout()->setAlignment(ellipse_button,
                                        Qt::AlignTop|Qt::AlignCenter);

    connect(ellipse_button, SIGNAL(changeState(STATE)),
            this, SLOT(setState(STATE)));
}

void View::initializePolygonButton(MenuPanel *panel) {
    MenuButton *polygon_button = new MenuButton(POLYGON, panel->menu_);
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
    this->panel_widgets_.append(polygon_button);
    this->toggle_buttons_.append(polygon_button);

    panel->menu_->layout()->addWidget(polygon_button);
    panel->menu_->layout()->setAlignment(polygon_button,
                                        Qt::AlignTop|Qt::AlignCenter);

    connect(polygon_button, SIGNAL(changeState(STATE)),
            this, SLOT(setState(STATE)));
}

void View::initializePlaneButton(MenuPanel *panel) {
    MenuButton *plane_button = new MenuButton(PLANE, panel->menu_);

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

    plane_button->setPixmap(pix);
    plane_button->setToolTip(tr("Add plane constraint"));
    this->panel_widgets_.append(plane_button);
    this->toggle_buttons_.append(plane_button);

    panel->menu_->layout()->addWidget(plane_button);
    panel->menu_->layout()->setAlignment(plane_button,
                                        Qt::AlignTop|Qt::AlignCenter);

    connect(plane_button, SIGNAL(changeState(STATE)),
            this, SLOT(setState(STATE)));
}

void View::initializeEraserButton(MenuPanel *panel) {
    MenuButton *eraser_button = new MenuButton(ERASER, panel->menu_);
    QPixmap pix(":/assets/erasericon.png");
    pix = pix.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    eraser_button->setPixmap(pix);
    eraser_button->setToolTip(tr("Delete item"));
    this->panel_widgets_.append(eraser_button);
    this->toggle_buttons_.append(eraser_button);

    panel->menu_->layout()->addWidget(eraser_button);
    panel->menu_->layout()->setAlignment(eraser_button,
                                        Qt::AlignTop|Qt::AlignCenter);

    connect(eraser_button, SIGNAL(changeState(STATE)),
            this, SLOT(setState(STATE)));
}

void View::initializeWaypointButton(MenuPanel *panel) {
    MenuButton *waypoint_button = new MenuButton(WAYPOINT, panel->menu_);
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
    this->panel_widgets_.append(waypoint_button);
    this->toggle_buttons_.append(waypoint_button);

    panel->menu_->layout()->addWidget(waypoint_button);
    panel->menu_->layout()->setAlignment(waypoint_button,
                                        Qt::AlignTop|Qt::AlignCenter);

    connect(waypoint_button, SIGNAL(changeState(STATE)),
            this, SLOT(setState(STATE)));
}

void View::initializeFlipButton(MenuPanel *panel) {
    MenuButton *flip_button = new MenuButton(FLIP, panel->menu_);

    QPixmap pix(50, 50);
    pix.fill(Qt::transparent);
    QPainter painter(&pix);
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen(Qt::black);
    pen.setWidth(3);
    painter.setPen(pen);
    painter.setBrush(Qt::gray);

    QLineF line(QPointF(10, 40), QPointF(40, 10));
    QLineF normal = line.normalVector();

    QPolygonF poly;
    poly << normal.translated(
                line.dx(), line.dy()).pointAt(-10.0 / line.length());
    poly << normal.translated(
                line.dx(), line.dy()).pointAt(10.0 / line.length());
    poly << normal.pointAt(10.0 / line.length());
    poly << normal.pointAt(-10.0 / line.length());

    painter.setPen(Qt::NoPen);
    painter.drawPolygon(poly);

    painter.setPen(pen);
    painter.drawLine(line);

    QLineF shiftedNormal = normal.translated(line.dx() / 2, line.dy() / 2);
    QPointF topArrowPoint = shiftedNormal.pointAt(16.0 / line.length());
    QPointF bottomArrowPoint = shiftedNormal.pointAt(-16.0 / line.length());

    painter.drawLine(QLineF(shiftedNormal.pointAt(14.0 / line.length()),
                            shiftedNormal.pointAt(5.0 / line.length())));
    painter.drawLine(topArrowPoint,
                     QPointF(topArrowPoint.x(), topArrowPoint.y() + 6));
    painter.drawLine(topArrowPoint,
                     QPointF(topArrowPoint.x() + 6, topArrowPoint.y()));

    painter.drawLine(QLineF(shiftedNormal.pointAt(-14.0 / line.length()),
                            shiftedNormal.pointAt(-5.0 / line.length())));
    painter.drawLine(bottomArrowPoint,
                     QPointF(bottomArrowPoint.x(), bottomArrowPoint.y() - 6));
    painter.drawLine(bottomArrowPoint,
                     QPointF(bottomArrowPoint.x() - 6, bottomArrowPoint.y()));

    flip_button->setPixmap(pix);
    flip_button->setToolTip(tr("Flip constraint directionity"));
    this->panel_widgets_.append(flip_button);
    this->toggle_buttons_.append(flip_button);

    panel->menu_->layout()->addWidget(flip_button);
    panel->menu_->layout()->setAlignment(flip_button,
                                        Qt::AlignTop|Qt::AlignCenter);

    connect(flip_button, SIGNAL(changeState(STATE)),
            this, SLOT(setState(STATE)));
}

void View::initializeHorizon(MenuPanel *panel) {
    QSpinBox *opt_horizon = new QSpinBox(panel->menu_);
    opt_horizon->setSizePolicy(QSizePolicy::Expanding,
                                      QSizePolicy::Minimum);
    opt_horizon->setSingleStep(1);
    opt_horizon->setRange(16, MAX_HORIZON);
    opt_horizon->setValue(MAX_HORIZON);
    opt_horizon->setToolTip(tr("Set horizon length"));
    opt_horizon->setMinimumHeight(30);
    opt_horizon->setStyleSheet("QSpinBox::up-button "
                               "{ width: 30px; height: 15px; } "
                               "QSpinBox::down-button "
                               "{ width: 30px; height: 15px; }");

    QLabel *opt_horizon_label = new QLabel();
    opt_horizon_label->setText("Iterations");

    panel->menu_->layout()->addWidget(opt_horizon_label);
    panel->menu_->layout()->setAlignment(opt_horizon_label,
                                        Qt::AlignTop);
    panel->menu_->layout()->addWidget(opt_horizon);
    panel->menu_->layout()->setAlignment(opt_horizon,
                                        Qt::AlignTop);

    this->panel_widgets_.append(opt_horizon);
    this->panel_widgets_.append(opt_horizon_label);

    connect(opt_horizon, SIGNAL(valueChanged(int)),
            this, SLOT(setHorizon(int)));
}

void View::initializeFinaltime(MenuPanel *panel) {
    QDoubleSpinBox *opt_finaltime = new QDoubleSpinBox(panel->menu_);
    opt_finaltime->setSizePolicy(QSizePolicy::Expanding,
                                      QSizePolicy::Minimum);
    opt_finaltime->setSingleStep(1.0);
    opt_finaltime->setRange(2.0, 20.0);
    opt_finaltime->setSuffix("s");
    opt_finaltime->setValue(3.0);
    opt_finaltime->setToolTip(tr("Set final time"));
    opt_finaltime->setMinimumHeight(30);
    opt_finaltime->setStyleSheet("QDoubleSpinBox::up-button "
                               "{ width: 30px; height: 15px; } "
                               "QDoubleSpinBox::down-button "
                               "{ width: 30px; height: 15px; }");

    QLabel *opt_finaltime_label = new QLabel();
    opt_finaltime_label->setText("Final time");

    panel->menu_->layout()->addWidget(opt_finaltime_label);
    panel->menu_->layout()->addWidget(opt_finaltime);
    panel->menu_->layout()->setAlignment(opt_finaltime,
                                        Qt::AlignBottom);

    this->panel_widgets_.append(opt_finaltime);
    this->panel_widgets_.append(opt_finaltime_label);

    connect(opt_finaltime, SIGNAL(valueChanged(double)),
            this, SLOT(setFinaltime(qreal)));
}

void View::initializeExecButton(MenuPanel *panel) {
    QPushButton *exec_button = new QPushButton("Exec", panel->menu_);
    exec_button->
            setToolTip(tr("Execute optimization with constraints"));
    panel->menu_->layout()->addWidget(exec_button);
    panel->menu_->layout()->setAlignment(exec_button, Qt::AlignBottom);

    this->panel_widgets_.append(exec_button);

    // Connect execute button
    connect(exec_button, SIGNAL(clicked(bool)),
            this, SLOT(execute()));
}

void View::initializeDuplicateButton(MenuPanel *panel) {
    QPushButton *duplicate_button = new QPushButton("Duplicate", panel->menu_);
    duplicate_button->setToolTip(tr("Duplicate Selected Constraint"));
    panel->menu_->layout()->addWidget(duplicate_button);
    panel->menu_->layout()->
            setAlignment(duplicate_button, Qt::AlignBottom);

    this->panel_widgets_.append(duplicate_button);

    connect(duplicate_button, SIGNAL(clicked(bool)),
            this, SLOT(duplicateSelected()));
}

void View::initializeZoom(MenuPanel *panel) {
    QDoubleSpinBox *zoom = new QDoubleSpinBox(panel->menu_);
    zoom->setSizePolicy(QSizePolicy::Expanding,
                                      QSizePolicy::Minimum);
    zoom->setSingleStep(0.1);
    zoom->setRange(0.1, 2.0);
    zoom->setSuffix("x");
    zoom->setValue(1.0);
    zoom->setToolTip(tr("Set zoom level"));
    zoom->setMinimumHeight(30);
    zoom->setStyleSheet("QDoubleSpinBox::up-button "
                               "{ width: 30px; height: 15px; } "
                               "QDoubleSpinBox::down-button "
                               "{ width: 30px; height: 15px; }");
    panel->menu_->layout()->addWidget(zoom);
    panel->menu_->layout()->setAlignment(zoom, Qt::AlignBottom);

    this->panel_widgets_.append(zoom);

    connect(zoom, SIGNAL(valueChanged(double)),
            this, SLOT(setZoom(double)));
}

}  // namespace optgui
