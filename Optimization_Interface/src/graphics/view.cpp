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
#include <QCheckBox>
#include <QMessageBox>

#include "include/controls/compute_thread.h"

namespace optgui {

View::View(QWidget * parent)
    : QGraphicsView(parent) {
    // set background image
    QStringList background_images = {"lab_indoor_-6_-6_6_6",
            "demo-campus_outdoor_47.65355_-122.30755_120.0905_167.7810",
            "demo-field_outdoor_47.67158_-121.94751_304.6741_372.8843"};

    QString background_image = QInputDialog::getItem(this, tr("Select scene"),
                                      tr("mode"), background_images, 0, false);

    // create new canvas to render
    this->canvas_ = new Canvas(this, background_image);
    this->setScene(this->canvas_);

    // connect canvas selection change to detect curr final point
    connect(this->scene(), SIGNAL(selectionChanged()),
            this, SLOT(setCurrEndpoints()));

    // create controller
    this->controller_ = new Controller(this->canvas_);

    // Set State
    this->state_ = IDLE;

    // Set pen and brush
    dot_pen_ = QPen(Qt::black);
    dot_brush_ = QBrush(Qt::white);

    // enable pinch zoom
    this->viewport()->grabGesture(Qt::PinchGesture);
    this->currentStepScaleFactor_ = 1;
    this->initialZoom_ = 1;

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

View::~View() {
    // Delete temporary markers
    this->clearMarkers();

    // Clear menu widgets
    for (QWidget *button : this->panel_widgets_) {
        delete button;
    }
    this->skyefly_params_table_->clear();
    delete this->skyefly_params_table_;
    delete this->model_params_table_;

    // Delete layout components
    delete this->menu_button_;
    delete this->menu_panel_;
    delete this->expert_menu_button_;
    delete this->expert_panel_;
    delete this->layout();

    // Delete controller and canvas
    delete this->controller_;
    delete this->canvas_;
}

bool View::viewportEvent(QEvent *event) {
    // enable pinch zoom/3 finger zoom
    if (event->type() == QEvent::Gesture) {
        return this->pinchZoom(static_cast<QGestureEvent *>(event));
    }
    return QGraphicsView::viewportEvent(event);
}

void View::setPorts() {
    // set to default state and open network config dialog
    this->setState(IDLE);
    this->controller_->setPorts();
}

void View::saveFile() {
    // save configuration file
    this->controller_->saveFile();
}

void View::loadFile() {
    // save configuration file
    this->controller_->loadFile();
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
    this->layout()->addWidget(this->menu_panel_);
    this->layout()->setAlignment(this->menu_panel_, Qt::AlignRight);

    // initialize menu buttons
    // toggle options
    this->initializeDroneButton(this->menu_panel_);
    this->initializeFinalPointButton(this->menu_panel_);
    this->initializeEllipseButton(this->menu_panel_);
    this->initializeCylinderButton(this->menu_panel_);
    this->initializePolygonButton(this->menu_panel_);
    this->initializePlaneButton(this->menu_panel_);
    this->initializeWaypointButton(this->menu_panel_);
    this->initializeEraserButton(this->menu_panel_);
    this->initializeFlipButton(this->menu_panel_);
    // duplicate button
    this->initializeDuplicateButton(this->menu_panel_);
    // feedback
    this->initializeMessageBox(this->menu_panel_);
    // final time
    this->initializeFreeFinalTimeToggle(this->menu_panel_);
    this->initializeFinaltime(this->menu_panel_);
    // zoom
    this->initializeZoom(this->menu_panel_);
    // stage button
    this->initializeStageButton(this->menu_panel_);

    // add space at the bottom
    this->menu_panel_->menu_layout_->insertStretch(-1, 1);

    // on fly update
    this->initializeTrajLockToggle(this->menu_panel_);

    // on fly update
    this->initializeStageToggle(this->menu_panel_);

    // simulation toggle
    this->initializeSimToggle(this->menu_panel_);

    // exec
    this->initializeExecButton(this->menu_panel_);

    // Connect menu open/close
    connect(this->menu_button_, SIGNAL(clicked()),
            this, SLOT(openMenu()));
    connect(this->menu_panel_->close_button_, SIGNAL(clicked()),
            this, SLOT(closeMenu()));

    // Default to menu closed
    this->closeMenu();
}

void View::initializeExpertPanel() {
    this->expert_panel_ = new MenuPanel(this, false, 180);

    // Create open expert menu button
    this->expert_menu_button_ = new QToolButton(this);
    this->expert_menu_button_->setArrowType(Qt::RightArrow);
    this->expert_menu_button_->setSizePolicy(QSizePolicy::Fixed,
                                             QSizePolicy::Fixed);
    this->expert_menu_button_->setFixedSize(10, 40);
    this->layout()->addWidget(this->expert_menu_button_);
    this->layout()->setAlignment(this->expert_menu_button_, Qt::AlignLeft);

    // Configure expert menu panel parameters
    this->layout()->addWidget(this->expert_panel_);
    this->layout()->setAlignment(this->expert_panel_, Qt::AlignLeft);

    // initialize menu buttons
    this->initializeSkyeFlyParamsTable(this->expert_panel_);
    this->expert_panel_->menu_layout_->insertStretch(-1, 1);
    this->initializeDataCaptureToggle(this->expert_panel_);
    this->initializeModelParamsTable(this->expert_panel_);

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
        case DRONE: {
            this->controller_->addDrone(pos);
            break;
        }
        case POINT: {
            // add a new target
            this->controller_->addFinalPoint(pos);
            break;
        }
        case ELLIPSE: {
            // TODO(bchasnov): disallow user from adding
            // ellipse if it overlaps?

            // add ellipse scaled by zoom factor
            qreal scaling_factor = this->matrix().m11();
            this->controller_->addEllipse(pos,
                                          DEFAULT_RAD / scaling_factor);
            break;
        }
        case CYLINDER: {
            // add cylinder scaled by zoom factor
            qreal scaling_factor = this->matrix().m11();
            this->controller_->addCylinder(pos,
                                          DEFAULT_WIDTH / scaling_factor);
            break;
        }
        case POLYGON: {
            // Add markers until polygon is completed
            if (!this->temp_markers_.isEmpty() &&
                    itemAt(event->pos()) == this->temp_markers_.first()) {
                if (this->temp_markers_.size() >= 3) {
                    QVector<QPointF> poly = QVector<QPointF>();
                    for (QGraphicsItem *dot : this->temp_markers_) {
                        poly.append(QPointF(dot->pos()));
                    }

                    /*
                     * detect if polygon is "inside out" and reverse order of points if so
                     * (philosophical question: should this go in the PolygonModelItem constructor?)
                     */

                    // first get list of edge vectors
                    QVector<QPointF> edge_vecs;
                    for (int i = 0; i < poly.length(); ++i) {
                        edge_vecs.append(poly[(i+1) % poly.length()] - poly[i]);
                    }

                    // compute sum of angles of internal angles,
                    // should be negative
                    qreal tot_angle = 0;
                    for (int i = 0; i < edge_vecs.length(); ++i) {
                        QPointF ahead = edge_vecs[(i+1) % edge_vecs.length()];
                        QPointF behind = edge_vecs[i];

                        // take 2D "pseudo cross product" (behind x ahead)
                        qreal cross_product = behind.rx() * ahead.ry()
                                - behind.ry() * ahead.rx();

                        qreal norm_ahead =
                                sqrt(QPointF::dotProduct(ahead, ahead));
                        qreal norm_behind =
                                sqrt(QPointF::dotProduct(behind, behind));

                        tot_angle +=
                                qAsin(cross_product /
                                      (norm_ahead * norm_behind));
                    }

                    // if poly is "inside out" reverse it
                    if (tot_angle > 0) {
                        std::reverse(poly.begin(), poly.end());
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
                temp_markers_.append(dot);
                dot->setPos(pos);
            }
            break;
        }
        case PLANE: {
            // Add markers to define line
            if (!this->temp_markers_.isEmpty()) {
                QPointF p1 = QPointF(temp_markers_.first()->pos());
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
                temp_markers_.append(dot);
                dot->setPos(pos);
            }
            break;
        }
        case WAYPOINT: {
            if (this->controller_->getNumWaypoints()
                    < skyenet::MAX_WAYPOINTS) {
                this->controller_->addWaypoint(pos);
            }
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
            // regular mouse interaction
            QGraphicsView::mousePressEvent(event);
        }
    }
}

void View::closeMenu() {
    // close menu panel and re-render
    this->menu_panel_->hide();
    this->menu_button_->show();

    this->setState(IDLE);

    this->repaint();
}

void View::closeExpertMenu() {
    // close expert menu panel and re-render
    this->expert_panel_->hide();
    this->expert_menu_button_->show();

    this->setState(IDLE);

    this->repaint();
}

void View::openMenu() {
    // open menu panel and re-render
    this->menu_button_->hide();
    this->menu_panel_->show();
    this->update();
}

void View::openExpertMenu() {
    // open expert menu panel and re-render
    this->expert_menu_button_->hide();
    this->expert_panel_->show();
    this->update();
}

void View::setZoom(qreal value) {
    // set zoom scaling factor
    this->setTransform(QTransform::fromScale(value, value));
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
    // set to default state and try to execute staged traj
    this->clearMarkers();
    this->setState(IDLE);
    this->controller_->execute();
}

void View::stageTraj() {
    // set to default state and try to stage traj
    this->clearMarkers();
    this->setState(IDLE);
    this->controller_->stageTraj();
}

void View::unstageTraj() {
    // set to default state and try to unstage traj
    this->clearMarkers();
    this->setState(IDLE);
    this->controller_->unstageTraj();
}

void View::duplicateSelected() {
    // set to default state and try to duplicate selected ellipse
    this->clearMarkers();
    this->setState(IDLE);
    this->controller_->duplicateSelected();
}

void View::setFinaltime(qreal final_time) {
    // set final time
    this->controller_->setFinaltime(final_time);
}

void View::setClearance(qreal clearance) {
    this->controller_->setClearance(clearance);
}

void View::setSkyeFlyParams() {
    // copy skyefly params from expert panel table to model
    this->controller_->setSkyeFlyParams(this->skyefly_params_table_);
}

void View::clearMarkers() {
    // Clear all temporary markers
    for (QGraphicsItem *dot : this->temp_markers_) {
        this->scene()->removeItem(dot);
        delete dot;
    }
    this->temp_markers_.clear();
}

void View::resizeEvent(QResizeEvent *event) {
    // expand view when windows is resized
    this->expandView();

    QGraphicsView::resizeEvent(event);
}

void View::expandView() {
    // Transpose view
    QRectF oldView = this->viewport()->rect();
    oldView.translate(-oldView.center().x(), -oldView.center().y());

    // Scale canvas out to max size
    QRectF newView = (this->scene()->sceneRect()).united(oldView);
    qreal width = newView.width() / 2 * 50;
    qreal height = newView.height() / 2 * 50;

    // set view and canvas rects
    this->scene()->setSceneRect(
                newView.marginsAdded(QMarginsF(width, height, width, height)));
    this->setSceneRect(this->scene()->sceneRect());
}

bool View::pinchZoom(QGestureEvent *event) {
    // accept pinch zoom gesture
    if (QGesture *pinch = event->gesture(Qt::PinchGesture)) {
        // get gesture
        QPinchGesture *pinchGesture = static_cast<QPinchGesture *>(pinch);
        // gesture started
        if (pinchGesture->state() == Qt::GestureStarted) {
            this->initialZoom_ = this->transform().m11();
            event->accept(Qt::PinchGesture);
        }
        // scale zoom by pinch distance
        QPinchGesture::ChangeFlags changeFlags = pinchGesture->changeFlags();
        if (changeFlags & QPinchGesture::ScaleFactorChanged) {
            this->currentStepScaleFactor_ = pinchGesture->totalScaleFactor();
            this->zoom_slider_->setValue(initialZoom_
                                         * this->currentStepScaleFactor_);
        }
        // gesture finished
        if (pinchGesture->state() == Qt::GestureFinished) {
            this->currentStepScaleFactor_ = 1;
        }
    }
    return true;
}

void View::initializeMessageBox(MenuPanel *panel) {
    this->user_msg_label_ = new QLabel();
    this->user_msg_label_->setText("Feasibility feedback.");
    this->user_msg_label_->setWordWrap(true);
    this->user_msg_label_->setContentsMargins(2, 2, 2, 2);
    this->user_msg_label_->setStyleSheet(
            "QLabel { background-color : black; color : white; }");
    panel->menu_->layout()->addWidget(this->user_msg_label_);
    this->panel_widgets_.append(this->user_msg_label_);

    connect(this->controller_, SIGNAL(updateMessage()),
            this, SLOT(updateFeedbackMessage()));
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
    painter.setBrush(RED);
    painter.drawEllipse(10, 10, 30, 30);
    point_button->setPixmap(pix);
    point_button->setToolTip(tr("Add target point"));
    this->panel_widgets_.append(point_button);
    this->toggle_buttons_.append(point_button);

    panel->menu_->layout()->addWidget(point_button);
    panel->menu_->layout()->setAlignment(point_button,
                                        Qt::AlignTop|Qt::AlignCenter);

    connect(point_button, SIGNAL(changeState(STATE)),
            this, SLOT(setState(STATE)));
}

void View::initializeDroneButton(MenuPanel *panel) {
    MenuButton *drone_button = new MenuButton(DRONE, panel->menu_);
    QPixmap pix(50, 50);
    pix.fill(Qt::transparent);
    QPainter painter(&pix);
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen(Qt::black);
    pen.setWidth(2);
    painter.setPen(pen);
    painter.setBrush(YELLOW);

    QPolygonF poly;
    poly << QPointF(5, 25);
    poly << QPointF(25, 5);
    poly << QPointF(45, 25);
    poly << QPointF(25, 45);
    poly << QPointF(5, 25);
    painter.drawPolygon(poly);

    drone_button->setPixmap(pix);
    drone_button->setToolTip(tr("Add drone"));
    this->panel_widgets_.append(drone_button);
    this->toggle_buttons_.append(drone_button);

    panel->menu_->layout()->addWidget(drone_button);
    panel->menu_->layout()->setAlignment(drone_button,
                                        Qt::AlignTop|Qt::AlignCenter);

    connect(drone_button, SIGNAL(changeState(STATE)),
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

void View::initializeCylinderButton(MenuPanel *panel) {
    MenuButton *cylinder_button = new MenuButton(CYLINDER, panel->menu_);
    QPixmap pix(50, 50);
    pix.fill(Qt::transparent);
    QPainter painter(&pix);
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen(Qt::black);
    pen.setWidth(2);
    painter.setPen(pen);
    painter.setBrush(Qt::green);
    painter.drawRect(1, 12, 48, 24);
    cylinder_button->setPixmap(pix);
    cylinder_button->setToolTip(tr("Add cylinder constraint"));
    this->panel_widgets_.append(cylinder_button);
    this->toggle_buttons_.append(cylinder_button);

    panel->menu_->layout()->addWidget(cylinder_button);
    panel->menu_->layout()->setAlignment(cylinder_button,
                                        Qt::AlignTop|Qt::AlignCenter);

    connect(cylinder_button, SIGNAL(changeState(STATE)),
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
    painter.setBrush(RED);

    painter.drawEllipse(QPointF(7, 7), 6, 6);
    painter.setBrush(Qt::white);
    painter.drawEllipse(QPointF(19, 19), 6, 6);
    painter.drawEllipse(QPointF(31, 31), 6, 6);
    painter.setBrush(GREEN);
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

    // draw icon
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

void View::initializeSkyeFlyParamsTable(MenuPanel *panel) {
    // Create table
    this->skyefly_params_table_ = new QTableWidget(panel->menu_);
    this->skyefly_params_table_->setColumnCount(1);  // fill with spinboxes
    this->skyefly_params_table_->setRowCount(13);  // how many params to edit
        // vertical headers are spinbox labels
    this->skyefly_params_table_->verticalHeader()->setVisible(true);
    this->skyefly_params_table_->verticalHeader()->
            setSectionResizeMode(QHeaderView::Stretch);
    this->skyefly_params_table_->horizontalHeader()->setVisible(false);
    this->skyefly_params_table_->setSizePolicy(QSizePolicy::Expanding,
                                     QSizePolicy::Expanding);
    this->skyefly_params_table_->
            setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->skyefly_params_table_->
            setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        // set size
    this->skyefly_params_table_->setMinimumHeight(450);
        // add table to menu panel
    panel->menu_->layout()->addWidget(this->skyefly_params_table_);
    panel->menu_->layout()->setAlignment(this->skyefly_params_table_,
                                        Qt::AlignTop|Qt::AlignCenter);

    // Default skyefly param values
    skyenet::params default_P;
    quint32 row_index = 0;

     // P.K
    QSpinBox *params_K = new QSpinBox(this->skyefly_params_table_);
    params_K->setRange(skyenet::MIN_HORIZON, skyenet::MAX_HORIZON);
    params_K->setValue(default_P.K);
    connect(params_K, SIGNAL(valueChanged(int)),
            this, SLOT(setSkyeFlyParams()));

    this->skyefly_params_table_->setCellWidget(row_index, 0, params_K);
    this->skyefly_params_table_->
            setVerticalHeaderItem(row_index, new QTableWidgetItem("K"));
    row_index++;

    // P.w_obj
    QDoubleSpinBox *params_w_obj =
            new QDoubleSpinBox(this->skyefly_params_table_);
    params_w_obj->setRange(0, 10000);
    params_w_obj->setValue(default_P.w_obj);
    connect(params_w_obj, SIGNAL(valueChanged(double)),
            this, SLOT(setSkyeFlyParams()));

    this->skyefly_params_table_->setCellWidget(row_index, 0, params_w_obj);
    this->skyefly_params_table_->
            setVerticalHeaderItem(row_index, new QTableWidgetItem("w_obj"));
    row_index++;

    // P.w_trust
    QDoubleSpinBox *params_w_trust =
            new QDoubleSpinBox(this->skyefly_params_table_);
    params_w_trust->setRange(0, 10000);
    params_w_trust->setValue(default_P.w_trust);
    connect(params_w_trust, SIGNAL(valueChanged(double)),
            this, SLOT(setSkyeFlyParams()));

    this->skyefly_params_table_->setCellWidget(row_index, 0, params_w_trust);
    this->skyefly_params_table_->
            setVerticalHeaderItem(row_index, new QTableWidgetItem("w_trust"));
    row_index++;

    // P.w_buffer
    QDoubleSpinBox *params_w_buffer =
            new QDoubleSpinBox(this->skyefly_params_table_);
    params_w_buffer->setRange(0, 10000);
    params_w_buffer->setValue(default_P.w_buff);
    connect(params_w_buffer, SIGNAL(valueChanged(double)),
            this, SLOT(setSkyeFlyParams()));

    this->skyefly_params_table_->setCellWidget(row_index, 0, params_w_buffer);
    this->skyefly_params_table_->
            setVerticalHeaderItem(row_index, new QTableWidgetItem("w_buffer"));
    row_index++;

    // P.eps_cvg
    QDoubleSpinBox *params_eps_cvg =
            new QDoubleSpinBox(this->skyefly_params_table_);
    params_eps_cvg->setRange(0, 10000);
    params_eps_cvg->setValue(default_P.eps_cvg);
    connect(params_eps_cvg, SIGNAL(valueChanged(double)),
            this, SLOT(setSkyeFlyParams()));

    this->skyefly_params_table_->setCellWidget(row_index, 0, params_eps_cvg);
    this->skyefly_params_table_->
            setVerticalHeaderItem(row_index, new QTableWidgetItem("eps_cvg"));
    row_index++;

    // P.eps_subopt
    QDoubleSpinBox *params_eps_subopt =
            new QDoubleSpinBox(this->skyefly_params_table_);
    params_eps_subopt->setRange(0, 10000);
    params_eps_subopt->setValue(default_P.subopt_tol);
    connect(params_eps_subopt, SIGNAL(valueChanged(double)),
            this, SLOT(setSkyeFlyParams()));

    this->skyefly_params_table_->setCellWidget(row_index, 0, params_eps_subopt);
    this->skyefly_params_table_->
            setVerticalHeaderItem(row_index, new QTableWidgetItem("eps_subopt"));
    row_index++;

    // P.scp_iters
    QSpinBox *params_scp_iters =
            new QSpinBox(this->skyefly_params_table_);
    params_scp_iters->setRange(0, 100);
    params_scp_iters->setValue(default_P.scp_iters);
    connect(params_scp_iters, SIGNAL(valueChanged(int)),
            this, SLOT(setSkyeFlyParams()));

    this->skyefly_params_table_->setCellWidget(row_index, 0, params_scp_iters);
    this->skyefly_params_table_->
            setVerticalHeaderItem(row_index, new QTableWidgetItem("scp_iters"));
    row_index++;

    // P.tf_max
    QDoubleSpinBox *params_tf_max =
            new QDoubleSpinBox(this->skyefly_params_table_);
    params_tf_max->setRange(0,100);
    params_tf_max->setValue(default_P.tf_max);
    connect(params_tf_max, SIGNAL(valueChanged(double)),
            this, SLOT(setSkyeFlyParams()));

    this->skyefly_params_table_->setCellWidget(row_index, 0, params_tf_max);
    this->skyefly_params_table_->
            setVerticalHeaderItem(row_index, new QTableWidgetItem("tf_max"));
    row_index++;

    // P.dt_min
    QDoubleSpinBox *params_dt_min =
            new QDoubleSpinBox(this->skyefly_params_table_);
    params_dt_min->setRange(0,100);
    params_dt_min->setValue(default_P.dt_min);
    connect(params_dt_min, SIGNAL(valueChanged(double)),
            this, SLOT(setSkyeFlyParams()));

    this->skyefly_params_table_->setCellWidget(row_index, 0, params_dt_min);
    this->skyefly_params_table_->
            setVerticalHeaderItem(row_index, new QTableWidgetItem("dt_min"));
    row_index++;

    // P.dt_max
    QDoubleSpinBox *params_dt_max =
            new QDoubleSpinBox(this->skyefly_params_table_);
    params_dt_max->setRange(0,100);
    params_dt_max->setValue(default_P.dt_max);
    connect(params_dt_max, SIGNAL(valueChanged(double)),
            this, SLOT(setSkyeFlyParams()));

    this->skyefly_params_table_->setCellWidget(row_index, 0, params_dt_max);
    this->skyefly_params_table_->
            setVerticalHeaderItem(row_index, new QTableWidgetItem("dt_max"));
    row_index++;

    // P.a_min
    QDoubleSpinBox *params_a_min =
            new QDoubleSpinBox(this->skyefly_params_table_);
    params_a_min->setRange(0, 10000);
    params_a_min->setValue(default_P.a_min);
    params_a_min->setSingleStep(0.1);
    connect(params_a_min, SIGNAL(valueChanged(double)),
            this, SLOT(setSkyeFlyParams()));

    connect(params_a_min, SIGNAL(valueChanged(double)),
            this, SLOT(constrainAccel()));
    this->a_min_row = row_index;

    this->skyefly_params_table_->setCellWidget(row_index, 0, params_a_min);
    this->skyefly_params_table_->
            setVerticalHeaderItem(row_index, new QTableWidgetItem("a_min"));
    row_index++;
    */

    // P.a_max
    QDoubleSpinBox *params_a_max =
            new QDoubleSpinBox(this->skyefly_params_table_);
    params_a_max->setRange(0, 10000);
    params_a_max->setSingleStep(0.1);
    params_a_max->setValue(default_P.a_max);
    connect(params_a_max, SIGNAL(valueChanged(double)),
            this, SLOT(setSkyeFlyParams()));

    connect(params_a_max, SIGNAL(valueChanged(double)),
            this, SLOT(constrainAccel()));
    this->a_max_row = row_index;

    this->skyefly_params_table_->setCellWidget(row_index, 0, params_a_max);
    this->skyefly_params_table_->
            setVerticalHeaderItem(row_index, new QTableWidgetItem("a_max"));
    row_index++;

    // P.v_max
    QDoubleSpinBox *params_v_max =
            new QDoubleSpinBox(this->skyefly_params_table_);
    params_v_max->setRange(0, 10000);
    params_v_max->setSingleStep(0.1);
    params_v_max->setValue(default_P.v_max);
    connect(params_v_max, SIGNAL(valueChanged(double)),
            this, SLOT(setSkyeFlyParams()));

    this->skyefly_params_table_->setCellWidget(row_index, 0, params_v_max);
    this->skyefly_params_table_->
            setVerticalHeaderItem(row_index, new QTableWidgetItem("v_max"));
    row_index++;

    // P.theta_max
    QDoubleSpinBox *params_theta_max =
            new QDoubleSpinBox(this->skyefly_params_table_);
    params_theta_max->setRange(0, 10000);
    params_theta_max->setSingleStep(0.1);
    params_theta_max->setValue(default_P.theta_max);
    connect(params_theta_max, SIGNAL(valueChanged(double)),
            this, SLOT(setSkyeFlyParams()));

    this->skyefly_params_table_->setCellWidget(row_index, 0, params_theta_max);
    this->skyefly_params_table_->
            setVerticalHeaderItem(row_index,
                                  new QTableWidgetItem("theta_max"));
    row_index++;
}

void View::constrainWpIdx(int value) {
    // constraint wp index by K when K is changed
    QSpinBox *params_wp_idx = qobject_cast<QSpinBox *>(
                this->skyefly_params_table_->cellWidget(this->wp_idx_row, 0));
    params_wp_idx->setMaximum(value - 1);
}

void View::constrainAccel() {
    // constraint a_min and a_max by each other when either is changed
    QDoubleSpinBox *params_a_min = qobject_cast<QDoubleSpinBox *>(
                this->skyefly_params_table_->cellWidget(this->a_min_row, 0));
    QDoubleSpinBox *params_a_max = qobject_cast<QDoubleSpinBox *>(
                this->skyefly_params_table_->cellWidget(this->a_max_row, 0));
    params_a_min->setMaximum(params_a_max->value());
    params_a_max->setMinimum(params_a_min->value());
}

void View::setCurrEndpoints() {

    QList<QGraphicsItem *> all_items = this->scene()->items();
    QList<QGraphicsItem *> selected_items = this->scene()->selectedItems();
    bool target_found = false;
    bool drone_found = false;

    for (QGraphicsItem * item : selected_items) {

        // Obtain selected drone
        if (item->type() == GRAPHICS_TYPE::DRONE_GRAPHIC && !drone_found) {
            DroneGraphicsItem *drone_graphic =
                    qgraphicsitem_cast<DroneGraphicsItem *>(item);
            // set model as curr selected drone
            this->controller_->setCurrDrone(drone_graphic->model_);
            // found, dont flag other selected points
            drone_found = true;
        }

        // Obtain selected target
        if (item->type() == GRAPHICS_TYPE::POINT_GRAPHIC && !target_found) {
            this->controller_->setCurrFinalPoint(
                        qgraphicsitem_cast<PointGraphicsItem *>(item)->model_);
            // found, dont flag other selected points
            target_found = true;
        }
    }

    // Obtain all existing targets
    PointModelItem *targets[skyenet::MAX_TARGETS];
    quint32 num_targ = 0;
    for (QGraphicsItem * item : all_items) {
        if (item->type() == GRAPHICS_TYPE::POINT_GRAPHIC) {
            // this->controller_->addPooledFinalPoint(
            //             qgraphicsitem_cast<PointGraphicsItem *>(item)->model_);
            targets[num_targ] = qgraphicsitem_cast<PointGraphicsItem *>(item)->model_;
            num_targ++;
        }
    }
    this->controller_->setPooledFinalPoints(targets, num_targ);
}

void View::toggleSim(int state) {
    // toggle between simulate execution or actual execution
    this->controller_->setSimulated(state == Qt::Checked);
}

void View::toggleStage(int state) {
    // toggle between simulate execution or actual execution
    this->controller_->setStageBool(state == Qt::Checked);
}

void View::toggleTrajLock(int state) {
    this->controller_->setTrajLock(state == Qt::Checked);
}

void View::toggleFreeFinalTime(int state) {
    this->controller_->setFreeFinalTime(state == Qt::Checked);
}

void View::toggleDataCapture(int state) {
    this->controller_->setDataCapture(state == Qt::Checked);
}

void View::initializeModelParamsTable(MenuPanel *panel) {
    // Create table
    this->model_params_table_ = new QTableWidget(panel->menu_);
    this->model_params_table_->setColumnCount(1);  // fill with spinboxes
    this->model_params_table_->setRowCount(1);  // how many params to edit
        // vertical headers are spinbox labels
    this->model_params_table_->verticalHeader()->setVisible(true);
    this->model_params_table_->verticalHeader()->
            setSectionResizeMode(QHeaderView::Stretch);
    this->model_params_table_->horizontalHeader()->setVisible(false);
    this->model_params_table_->setSizePolicy(QSizePolicy::Expanding,
                                     QSizePolicy::Fixed);
    this->model_params_table_->
            setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->model_params_table_->
            setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        // set size
    this->model_params_table_->setMaximumHeight(30);
        // add table to menu panel
    panel->menu_->layout()->addWidget(this->model_params_table_);
    panel->menu_->layout()->setAlignment(this->model_params_table_,
                                        Qt::AlignBottom|Qt::AlignCenter);

    quint32 row_index = 0;

    // ellipse clearance
    QDoubleSpinBox *clearance = new QDoubleSpinBox(this->model_params_table_);
    clearance->setRange(0, 10);
    clearance->setSingleStep(0.1);
    clearance->setValue(INIT_CLEARANCE);
    connect(clearance, SIGNAL(valueChanged(double)),
            this, SLOT(setClearance(double)));

    this->model_params_table_->setCellWidget(row_index, 0, clearance);
    this->model_params_table_->
            setVerticalHeaderItem(row_index, new QTableWidgetItem("clearance"));
    row_index++;
}

void View::initializeFinaltime(MenuPanel *panel) {
    QDoubleSpinBox *opt_finaltime = new QDoubleSpinBox(panel->menu_);
    opt_finaltime->setSizePolicy(QSizePolicy::Expanding,
                                      QSizePolicy::Minimum);
    opt_finaltime->setSingleStep(1.0);
    opt_finaltime->setRange(0.0, 100.0);
    opt_finaltime->setSuffix("s");
    opt_finaltime->setValue(skyenet::params().tf);
    opt_finaltime->setToolTip(tr("Set final time"));
    opt_finaltime->setMinimumHeight(40);
    opt_finaltime->setStyleSheet("QDoubleSpinBox::up-button "
                               "{ width: 35px; height: 20px; } "
                               "QDoubleSpinBox::down-button "
                               "{ width: 35px; height: 20px; }");

    QLabel *opt_finaltime_label = new QLabel();
    opt_finaltime_label->setText("Final time");

    panel->menu_->layout()->addWidget(opt_finaltime_label);
    panel->menu_->layout()->addWidget(opt_finaltime);
    panel->menu_->layout()->setAlignment(opt_finaltime,
                                        Qt::AlignBottom);

    this->panel_widgets_.append(opt_finaltime);
    this->panel_widgets_.append(opt_finaltime_label);

    // free final time updates text box and P.tf
    connect(this->controller_, SIGNAL(finalTime(double)),
            this, SLOT(setFinaltime(qreal)));
    connect(this->controller_, SIGNAL(finalTime(double)),
            opt_finaltime, SLOT(setValue(qreal)));
    // manual change updates P.tf
    connect(opt_finaltime, SIGNAL(valueChanged(double)),
            this, SLOT(setFinaltime(qreal)));


    // Set table size
    this->skyefly_params_table_->resizeColumnsToContents();
}

void View::initializeStageToggle(MenuPanel *panel) {
    QCheckBox *stage_toggle = new QCheckBox("No Stage", panel->menu_);
    stage_toggle->
            setToolTip(tr("Disable Staging of Trajectory"));
    stage_toggle->setMinimumHeight(35);
    stage_toggle->setCheckState(Qt::Unchecked);
    panel->menu_->layout()->addWidget(stage_toggle);
    panel->menu_->layout()->setAlignment(stage_toggle, Qt::AlignBottom);

    this->panel_widgets_.append(stage_toggle);

    // Connect execute button
    connect(stage_toggle, SIGNAL(stateChanged(int)),
            this, SLOT(toggleStage(int)));
}

void View::initializeSimToggle(MenuPanel *panel) {
    QCheckBox *sim_toggle = new QCheckBox("Simulate", panel->menu_);
    sim_toggle->
            setToolTip(tr("Toggle simulate trajectory"));
    sim_toggle->setMinimumHeight(35);
    sim_toggle->setCheckState(Qt::Unchecked);
    panel->menu_->layout()->addWidget(sim_toggle);
    panel->menu_->layout()->setAlignment(sim_toggle, Qt::AlignBottom);

    this->panel_widgets_.append(sim_toggle);

    // Connect execute button
    connect(sim_toggle, SIGNAL(stateChanged(int)),
            this, SLOT(toggleSim(int)));
}

void View::initializeTrajLockToggle(MenuPanel *panel) {
    QCheckBox *traj_lock_toggle = new QCheckBox("Traj Lock", panel->menu_);
    traj_lock_toggle->
            setToolTip(tr("Lock Executed Trajectory"));
    traj_lock_toggle->setMinimumHeight(35);
    traj_lock_toggle->setCheckState(Qt::Unchecked);
    panel->menu_->layout()->addWidget(traj_lock_toggle);
    panel->menu_->layout()->setAlignment(traj_lock_toggle, Qt::AlignBottom);

    this->panel_widgets_.append(traj_lock_toggle);

    // Connect execute button
    connect(traj_lock_toggle, SIGNAL(stateChanged(int)),
            this, SLOT(toggleTrajLock(int)));
}

void View::initializeDataCaptureToggle(MenuPanel *panel) {
    QCheckBox *data_capture_toggle =
            new QCheckBox("Data Capture", panel->menu_);
    data_capture_toggle->
            setToolTip(tr("Save trajectory data to csv"));
    data_capture_toggle->setMinimumHeight(35);
    data_capture_toggle->setCheckState(Qt::Checked);
    panel->menu_->layout()->addWidget(data_capture_toggle);
    panel->menu_->layout()->setAlignment(
                data_capture_toggle, Qt::AlignBottom);

    this->panel_widgets_.append(data_capture_toggle);

    // Connect execute button
    connect(data_capture_toggle, SIGNAL(stateChanged(int)),
            this, SLOT(toggleDataCapture(int)));
}

void View::initializeFreeFinalTimeToggle(MenuPanel *panel) {
    QCheckBox *free_final_time_toggle =
            new QCheckBox("Free Final Time", panel->menu_);
    free_final_time_toggle->
            setToolTip(tr("Toggle simulate trajectory"));
    free_final_time_toggle->setMinimumHeight(35);
    free_final_time_toggle->setCheckState(Qt::Unchecked);
    panel->menu_->layout()->addWidget(free_final_time_toggle);
    panel->menu_->layout()->setAlignment(
                free_final_time_toggle, Qt::AlignBottom);

    this->panel_widgets_.append(free_final_time_toggle);

    // Connect execute button
    connect(free_final_time_toggle, SIGNAL(stateChanged(int)),
            this, SLOT(toggleFreeFinalTime(int)));
}

void View::initializeExecButton(MenuPanel *panel) {
    QPushButton *exec_button = new QPushButton("Exec", panel->menu_);
    exec_button->
            setToolTip(tr("Execute staged trajectory"));
    exec_button->setMinimumHeight(35);
    panel->menu_->layout()->addWidget(exec_button);
    panel->menu_->layout()->setAlignment(exec_button, Qt::AlignBottom);

    this->panel_widgets_.append(exec_button);

    // Connect execute button
    connect(exec_button, SIGNAL(clicked(bool)),
            this, SLOT(execute()));
}

void View::initializeStageButton(MenuPanel *panel) {
    QPushButton *stage_button = new QPushButton("Stage", panel->menu_);
    stage_button->
            setToolTip(tr("Stage current trajectory"));
    stage_button->setMinimumHeight(35);

    QPushButton *unstage_button = new QPushButton("Unstage", panel->menu_);
    unstage_button->
            setToolTip(tr("Unstage staged trajectory"));
    unstage_button->setMinimumHeight(35);

    panel->menu_->layout()->addWidget(stage_button);
    panel->menu_->layout()->setAlignment(stage_button, Qt::AlignBottom);
    panel->menu_->layout()->addWidget(unstage_button);
    panel->menu_->layout()->setAlignment(unstage_button, Qt::AlignBottom);

    this->panel_widgets_.append(stage_button);
    this->panel_widgets_.append(unstage_button);

    // Connect stage + unstage buttons
    connect(stage_button, SIGNAL(clicked(bool)),
            this, SLOT(stageTraj()));
    connect(unstage_button, SIGNAL(clicked(bool)),
            this, SLOT(unstageTraj()));
}

void View::initializeDuplicateButton(MenuPanel *panel) {
    QPushButton *duplicate_button = new QPushButton("Duplicate", panel->menu_);
    duplicate_button->setToolTip(tr("Duplicate Selected Constraint"));
    duplicate_button->setMinimumHeight(35);
    panel->menu_->layout()->addWidget(duplicate_button);
    panel->menu_->layout()->
            setAlignment(duplicate_button, Qt::AlignBottom);

    this->panel_widgets_.append(duplicate_button);

    connect(duplicate_button, SIGNAL(clicked(bool)),
            this, SLOT(duplicateSelected()));
}

void View::initializeZoom(MenuPanel *panel) {
    this->zoom_slider_ = new QDoubleSpinBox(panel->menu_);
    this->zoom_slider_->setSizePolicy(QSizePolicy::Expanding,
                                      QSizePolicy::Minimum);
    this->zoom_slider_->setSingleStep(0.1);
    this->zoom_slider_->setRange(0.01, 2.0);
    this->zoom_slider_->setSuffix("x");
    this->zoom_slider_->setValue(1.0);
    this->zoom_slider_->setToolTip(tr("Set zoom level"));
    this->zoom_slider_->setMinimumHeight(40);
    this->zoom_slider_->setStyleSheet("QDoubleSpinBox::up-button "
                               "{ width: 35px; height: 20px; } "
                               "QDoubleSpinBox::down-button "
                               "{ width: 35px; height: 20px; }");
    panel->menu_->layout()->addWidget(this->zoom_slider_);
    panel->menu_->layout()->setAlignment(this->zoom_slider_, Qt::AlignBottom);

    this->panel_widgets_.append(this->zoom_slider_);

    connect(this->zoom_slider_, SIGNAL(valueChanged(double)),
            this, SLOT(setZoom(double)));
}

void View::updateFeedbackMessage() {
    // get codes from model
    FEASIBILITY_CODE feasibility_code =
            this->controller_->getIsValidTraj();
    INPUT_CODE input_code =
            this->controller_->getIsValidInput();

    // update message from feasibility codes
    if (input_code == INPUT_CODE::VALID_INPUT) {
        switch (feasibility_code) {
        case FEASIBLE: {
            this->user_msg_label_->setText("Trajectory remains feasible");
            break;
        }
        case INFEASIBLE: {
            this->user_msg_label_->
                setText("Increase final time to regain feasibility");
            break;
        }
        }
    } else {
        switch (input_code) {
        case OBS_OVERLAP: {
            this->user_msg_label_->setText("Obstacles cannot overlap");
            break;
        }
        case DRONE_OVERLAP: {
            this->user_msg_label_->setText("Vehicle cannot be in obstacle");
            break;
        }
        case FINAL_POS_OVERLAP: {
            this->user_msg_label_->setText("Target cannot be in obstacle");
            break;
        }
        case VALID_INPUT: {
            // should never enter here
        }
        }
    }
}

}  // namespace optgui
