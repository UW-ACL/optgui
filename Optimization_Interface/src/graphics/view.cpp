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
    this->controller_ = new Controller(this->canvas_);

    // Set State
    this->state_ = IDLE;

    // Set pen and brush
    dot_pen_ = QPen(Qt::black);
    dot_brush_ = QBrush(Qt::white);

    // Create poly vector
    this->temp_markers_ = new QVector<QGraphicsItem*>();

    // enable pinch zoom
    this->viewport()->grabGesture(Qt::PinchGesture);
    this->currentStepScaleFactor_ = 1;
    this->initialZoom_ = 1;

    // set graphical settings menu panels
    this->initialize();
}

View::~View() {
    // Delete temporary markers
    this->clearMarkers();
    delete this->temp_markers_;

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
    if (event->type() == QEvent::Gesture) {
        return this->pinchZoom(static_cast<QGestureEvent *>(event));
    }
    return QGraphicsView::viewportEvent(event);
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
    this->layout()->addWidget(this->menu_panel_);
    this->layout()->setAlignment(this->menu_panel_, Qt::AlignRight);

    // initialize menu buttons
    // toggle options
    this->initializeFinalPointButton(this->menu_panel_);
    this->initializeEllipseButton(this->menu_panel_);
    this->initializePolygonButton(this->menu_panel_);
    this->initializePlaneButton(this->menu_panel_);
    this->initializeWaypointButton(this->menu_panel_);
    this->initializeEraserButton(this->menu_panel_);
    this->initializeFlipButton(this->menu_panel_);
    // duplicate
    this->initializeDuplicateButton(this->menu_panel_);
    // feedback + final time
    this->initializeMessageBox(this->menu_panel_);
    this->initializeFinaltime(this->menu_panel_);
    // zoom
    this->initializeZoom(this->menu_panel_);
    // stage
    this->initializeStageButton(this->menu_panel_);

    // add space at the bottom
    this->menu_panel_->menu_layout_->insertStretch(-1, 1);

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

                    /*
                     * detect if polygon is "inside out" and reverse order of points if so
                     * (philosophical question: should this go in the PolygonModelItem constructor?)
                     */

                    // first get list of edge vectors
                    QVector<QPointF> edge_vecs;
                    for (int i = 0; i < poly.length(); ++i) {
                        edge_vecs.append(poly[(i+1) % poly.length()] - poly[i]);
                    }

                    // compute sum of angles of internal angles; should be negative
                    qreal tot_angle = 0;
                    for (int i = 0; i < edge_vecs.length(); ++i) {
                        QPointF ahead = edge_vecs[(i+1) % edge_vecs.length()];
                        QPointF behind = edge_vecs[i];

                        // take 2D "pseudo cross product" (behind x ahead)
                        qreal cross_product = behind.rx() * ahead.ry() - behind.ry() * ahead.rx();

                        qreal norm_ahead = sqrt(QPointF::dotProduct(ahead, ahead));
                        qreal norm_behind = sqrt(QPointF::dotProduct(behind, behind));

                        tot_angle += qAsin(cross_product/(norm_ahead * norm_behind));
                    }

                    // if poly is "inside out" reverse it
                    if (tot_angle > 0){
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
            // TODO(dtsull16): support up to skyenet::MAX_WAYPOINTS
            if (this->controller_->model_->getNumWaypoints() < 1) {
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
    this->clearMarkers();
    this->setState(IDLE);
    this->controller_->execute();
}

void View::stageTraj() {
    this->clearMarkers();
    this->setState(IDLE);
    this->controller_->stageTraj();
}

void View::unstageTraj() {
    this->clearMarkers();
    this->setState(IDLE);
    this->controller_->unstageTraj();
}

void View::duplicateSelected() {
    this->setState(IDLE);
    this->controller_->duplicateSelected();
}

void View::setFinaltime(qreal final_time) {
    this->controller_->setFinaltime(final_time);
}

void View::setClearance(qreal clearance) {
    this->controller_->model_->setClearance(clearance);
}

void View::setSkyeFlyParams() {
    this->controller_->setSkyeFlyParams(this->skyefly_params_table_);
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
    if (QGesture *pinch = event->gesture(Qt::PinchGesture)) {
        QPinchGesture *pinchGesture = static_cast<QPinchGesture *>(pinch);

        if (pinchGesture->state() == Qt::GestureStarted) {
            this->initialZoom_ = this->transform().m11();
            event->accept(Qt::PinchGesture);
        }
        QPinchGesture::ChangeFlags changeFlags = pinchGesture->changeFlags();
        if (changeFlags & QPinchGesture::ScaleFactorChanged) {
            this->currentStepScaleFactor_ = pinchGesture->totalScaleFactor();
            this->zoom_slider_->setValue(initialZoom_
                                         * this->currentStepScaleFactor_);
        }
        if (pinchGesture->state() == Qt::GestureFinished) {
            this->currentStepScaleFactor_ = 1;
        }
    }
    return true;
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
    painter.setBrush(RED);
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
    this->skyefly_params_table_->setRowCount(17);  // how many params to edit
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
    skyenet::params default_P = skyenet::getDefaultP();
    // TODO(dtsull16): get min and max values for all params
    quint32 row_index = 0;

    // P.K
    QSpinBox *params_K = new QSpinBox(this->skyefly_params_table_);
    // TODO(dtsull16): replace with skyenet::MIN_HORIZON
    params_K->setRange(5, skyenet::MAX_HORIZON);
    params_K->setValue(default_P.K);
    connect(params_K, SIGNAL(valueChanged(int)),
            this, SLOT(setSkyeFlyParams()));

    this->skyefly_params_table_->setCellWidget(row_index, 0, params_K);
    this->skyefly_params_table_->
            setVerticalHeaderItem(row_index, new QTableWidgetItem("K"));
    row_index++;

    // P.dK
    QSpinBox *params_dK = new QSpinBox(this->skyefly_params_table_);
    params_dK->setRange(0, 1000);
    params_dK->setSingleStep(0.1);
    params_dK->setValue(default_P.dK);
    connect(params_dK, SIGNAL(valueChanged(int)),
            this, SLOT(setSkyeFlyParams()));

    this->skyefly_params_table_->setCellWidget(row_index, 0, params_dK);
    this->skyefly_params_table_->
            setVerticalHeaderItem(row_index, new QTableWidgetItem("dK"));
    row_index++;

    // P.n_recalcs
    QSpinBox *params_n_recalcs = new QSpinBox(this->skyefly_params_table_);
    params_n_recalcs->setRange(0, 1000);
    params_n_recalcs->setValue(default_P.n_recalcs);
    connect(params_n_recalcs, SIGNAL(valueChanged(int)),
            this, SLOT(setSkyeFlyParams()));

    this->skyefly_params_table_->setCellWidget(row_index, 0, params_n_recalcs);
    this->skyefly_params_table_->
            setVerticalHeaderItem(row_index,
                                  new QTableWidgetItem("n_recalcs"));
    row_index++;

    // P.a_min
    QDoubleSpinBox *params_a_min =
            new QDoubleSpinBox(this->skyefly_params_table_);
    params_a_min->setRange(-1000, 1000);
    params_a_min->setValue(default_P.a_min);
    params_a_min->setSingleStep(0.1);
    connect(params_a_min, SIGNAL(valueChanged(double)),
            this, SLOT(setSkyeFlyParams()));

    this->skyefly_params_table_->setCellWidget(row_index, 0, params_a_min);
    this->skyefly_params_table_->
            setVerticalHeaderItem(row_index, new QTableWidgetItem("a_min"));
    row_index++;

    // P.a_max
    QDoubleSpinBox *params_a_max =
            new QDoubleSpinBox(this->skyefly_params_table_);
    params_a_max->setRange(-1000, 1000);
    params_a_max->setSingleStep(0.1);
    params_a_max->setValue(default_P.a_max);
    connect(params_a_max, SIGNAL(valueChanged(double)),
            this, SLOT(setSkyeFlyParams()));

    this->skyefly_params_table_->setCellWidget(row_index, 0, params_a_max);
    this->skyefly_params_table_->
            setVerticalHeaderItem(row_index, new QTableWidgetItem("a_max"));
    row_index++;

    // P.theta_max
    QDoubleSpinBox *params_theta_max =
            new QDoubleSpinBox(this->skyefly_params_table_);
    params_theta_max->setRange(-1000, 1000);
    params_theta_max->setSingleStep(0.1);
    params_theta_max->setValue(default_P.theta_max);
    connect(params_theta_max, SIGNAL(valueChanged(double)),
            this, SLOT(setSkyeFlyParams()));

    this->skyefly_params_table_->setCellWidget(row_index, 0, params_theta_max);
    this->skyefly_params_table_->
            setVerticalHeaderItem(row_index,
                                  new QTableWidgetItem("theta_max"));
    row_index++;

    // P.q_max
    QDoubleSpinBox *params_q_max =
            new QDoubleSpinBox(this->skyefly_params_table_);
    params_q_max->setRange(-1000, 1000);
    params_q_max->setValue(default_P.q_max);
    params_q_max->setSingleStep(0.01);
    connect(params_q_max, SIGNAL(valueChanged(double)),
            this, SLOT(setSkyeFlyParams()));

    this->skyefly_params_table_->setCellWidget(row_index, 0, params_q_max);
    this->skyefly_params_table_->
            setVerticalHeaderItem(row_index, new QTableWidgetItem("q_max"));
    row_index++;

    // P.max_iter
    QSpinBox *params_max_iter = new QSpinBox(this->skyefly_params_table_);
    params_max_iter->setRange(0, 1000);
    params_max_iter->setValue(default_P.max_iter);
    connect(params_max_iter, SIGNAL(valueChanged(int)),
            this, SLOT(setSkyeFlyParams()));

    this->skyefly_params_table_->setCellWidget(row_index, 0, params_max_iter);
    this->skyefly_params_table_->
            setVerticalHeaderItem(row_index, new QTableWidgetItem("max_iter"));
    row_index++;

    // P.Delta_i
    QDoubleSpinBox *params_delta_i =
            new QDoubleSpinBox(this->skyefly_params_table_);
    params_delta_i->setRange(0, 1000);
    params_delta_i->setValue(default_P.Delta_i);
    connect(params_delta_i, SIGNAL(valueChanged(double)),
            this, SLOT(setSkyeFlyParams()));

    this->skyefly_params_table_->setCellWidget(row_index, 0, params_delta_i);
    this->skyefly_params_table_->
            setVerticalHeaderItem(row_index, new QTableWidgetItem("delta_i"));
    row_index++;

    // P.lambda
    QDoubleSpinBox *params_lambda =
            new QDoubleSpinBox(this->skyefly_params_table_);
    params_lambda->setRange(0, 1000000);
    params_lambda->setValue(default_P.lambda);
    params_lambda->setSingleStep(10000);
    connect(params_lambda, SIGNAL(valueChanged(double)),
            this, SLOT(setSkyeFlyParams()));

    this->skyefly_params_table_->setCellWidget(row_index, 0, params_lambda);
    this->skyefly_params_table_->
            setVerticalHeaderItem(row_index, new QTableWidgetItem("lambda"));
    row_index++;

    // P.alpha
    QDoubleSpinBox *params_alpha =
            new QDoubleSpinBox(this->skyefly_params_table_);
    params_alpha->setRange(-1000, 1000);
    params_alpha->setValue(default_P.alpha);
    params_alpha->setSingleStep(0.1);
    connect(params_alpha, SIGNAL(valueChanged(double)),
            this, SLOT(setSkyeFlyParams()));

    this->skyefly_params_table_->setCellWidget(row_index, 0, params_alpha);
    this->skyefly_params_table_->
            setVerticalHeaderItem(row_index, new QTableWidgetItem("alpha"));
    row_index++;

    // P.dL_tol
    QDoubleSpinBox *params_dL_tol =
            new QDoubleSpinBox(this->skyefly_params_table_);
    params_dL_tol->setRange(0, 1000);
    params_dL_tol->setValue(default_P.dL_tol);
    params_dL_tol->setSingleStep(0.01);
    connect(params_dL_tol, SIGNAL(valueChanged(double)),
            this, SLOT(setSkyeFlyParams()));

    this->skyefly_params_table_->setCellWidget(row_index, 0, params_dL_tol);
    this->skyefly_params_table_->
            setVerticalHeaderItem(row_index, new QTableWidgetItem("dL_tol"));
    row_index++;

    // P.rho_0
    QDoubleSpinBox *params_rho_0 =
            new QDoubleSpinBox(this->skyefly_params_table_);
    params_rho_0->setRange(-1000, 1000);
    params_rho_0->setValue(default_P.rho_0);
    connect(params_rho_0, SIGNAL(valueChanged(double)),
            this, SLOT(setSkyeFlyParams()));

    this->skyefly_params_table_->setCellWidget(row_index, 0, params_rho_0);
    this->skyefly_params_table_->
            setVerticalHeaderItem(row_index, new QTableWidgetItem("rho_0"));
    row_index++;

    // P.rho_1
    QDoubleSpinBox *params_rho_1 =
            new QDoubleSpinBox(this->skyefly_params_table_);
    params_rho_1->setRange(-1000, 1000);
    params_rho_1->setValue(default_P.rho_1);
    connect(params_rho_1, SIGNAL(valueChanged(double)),
            this, SLOT(setSkyeFlyParams()));

    this->skyefly_params_table_->setCellWidget(row_index, 0, params_rho_1);
    this->skyefly_params_table_->
            setVerticalHeaderItem(row_index, new QTableWidgetItem("rho_1"));
    row_index++;

    // P.rho_2
    QDoubleSpinBox *params_rho_2 =
            new QDoubleSpinBox(this->skyefly_params_table_);
    params_rho_2->setRange(-1000, 1000);
    params_rho_2->setValue(default_P.rho_2);
    connect(params_rho_2, SIGNAL(valueChanged(double)),
            this, SLOT(setSkyeFlyParams()));

    this->skyefly_params_table_->setCellWidget(row_index, 0, params_rho_2);
    this->skyefly_params_table_->
            setVerticalHeaderItem(row_index, new QTableWidgetItem("rho_2"));
    row_index++;

    // P.rirelax
    QDoubleSpinBox *params_rirelax =
            new QDoubleSpinBox(this->skyefly_params_table_);
    params_rirelax->setRange(0, 10000);
    params_rirelax->setValue(default_P.rirelax);
    connect(params_rirelax, SIGNAL(valueChanged(double)),
            this, SLOT(setSkyeFlyParams()));

    this->skyefly_params_table_->setCellWidget(row_index, 0, params_rirelax);
    this->skyefly_params_table_->
            setVerticalHeaderItem(row_index, new QTableWidgetItem("rirelax"));
    row_index++;

    // P.rfrelax
    QDoubleSpinBox *params_rfrelax =
            new QDoubleSpinBox(this->skyefly_params_table_);
    params_rfrelax->setRange(0, 10000);
    params_rfrelax->setValue(default_P.rfrelax);
    connect(params_rfrelax, SIGNAL(valueChanged(double)),
            this, SLOT(setSkyeFlyParams()));

    this->skyefly_params_table_->setCellWidget(row_index, 0, params_rfrelax);
    this->skyefly_params_table_->
            setVerticalHeaderItem(row_index, new QTableWidgetItem("rfrelax"));
    row_index++;
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
    opt_finaltime->setRange(2.0, 20.0);
    opt_finaltime->setSuffix("s");
    opt_finaltime->setValue(skyenet::getDefaultP().tf);
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

    connect(opt_finaltime, SIGNAL(valueChanged(double)),
            this, SLOT(setFinaltime(qreal)));


    // Set table size
    this->skyefly_params_table_->resizeColumnsToContents();
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

}  // namespace optgui
