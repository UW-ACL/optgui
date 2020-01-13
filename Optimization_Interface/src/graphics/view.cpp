// TITLE:   Optimization_Interface/src/graphics/view.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "include/graphics/view.h"

#include <QHBoxLayout>
#include <QTimer>
#include <QScrollBar>
#include <QInputDialog>
#include <QDebug>

namespace interface {

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

    // Create MenuPanel (This was moved from initialize())
    this->menu_panel_ = new MenuPanel(this);

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

    this->initialize();
    this->compute_timer_.start();
}

View::~View() {
    // Delete temporary markers
    this->clearMarkers();
    delete this->temp_markers_;

    // Delete layout components
    delete this->menu_button_;
    delete this->menu_panel_;
    delete this->layout();

    // Delete controller and canvas
    delete this->controller_;
    delete this->canvas_;
}

void View::loadFile() {
    // Clear graphics
    this->controller_->clearWaypointsGraphic();
    this->controller_->clearPathGraphic();
    this->controller_->clearDroneGraphic();

    // Create new canvas
    Canvas *new_canvas = new Canvas(this);
    this->setScene(new_canvas);
    delete this->canvas_;
    this->canvas_ = new_canvas;

    // Pass new canvas to controller
    this->controller_->setCanvas(new_canvas);

    // Set state to idle
    this->setState(IDLE);

    // Load file onto canvas
    this->controller_->loadFile();

    // Expand scene
    this->expandView();
    this->canvas_->expandScene();
}

void View::saveFile() {
    this->setState(IDLE);
    this->controller_->saveFile();
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
    this->layout()->setMargin(0);

    // Set scroll preferences
    this->setDragMode(QGraphicsView::ScrollHandDrag);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setResizeAnchor(QGraphicsView::AnchorViewCenter);

    // Set rendering preference
    this->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    // Create open menu button
    this->menu_button_ = new QToolButton(this);
    this->menu_button_->setArrowType(Qt::LeftArrow);
    this->menu_button_->setSizePolicy(QSizePolicy::Fixed,
                                      QSizePolicy::Fixed);
    this->menu_button_->setFixedSize(10, 40);
    this->layout()->addWidget(this->menu_button_);
    this->layout()->setAlignment(this->menu_button_, Qt::AlignRight);

    // Configure menu panel parameters
    // TODO(bchasnov): Might want to move this to the constructor
    // where menu_panel_ is initialized?
    this->menu_panel_->setSizePolicy(QSizePolicy::Expanding,
                                     QSizePolicy::Expanding);
    this->menu_panel_->setFixedWidth(100);
    this->layout()->addWidget(this->menu_panel_);
    this->layout()->setAlignment(this->menu_panel_, Qt::AlignRight);

    // Default to menu closed
    this->closeMenu();

    // Set render hint
    this->setRenderHint(QPainter::Antialiasing);

    // Connect menu open/close
    connect(this->menu_button_, SIGNAL(clicked()),
            this, SLOT(openMenu()));
    connect(this->menu_panel_->close_button_, SIGNAL(clicked()),
            this, SLOT(closeMenu()));

    // Connect menu buttons
    for (MenuButton *button : *this->menu_panel_->menu_buttons_) {
        connect(button, SIGNAL(changeState(STATE)),
                this, SLOT(setState(STATE)));
    }

    // Connect sliders and zoom
    this->setZoom(this->menu_panel_->zoom_init_);
    connect(this->menu_panel_->zoom_slider_, SIGNAL(valueChanged(int)),
            this, SLOT(setZoom(int)));

    connect(this->menu_panel_->opt_horizon_slider_, SIGNAL(valueChanged(int)),
            this, SLOT(setHorizon(int)));

    connect(this->menu_panel_->opt_finaltime_slider_, SIGNAL(valueChanged(int)),
            this, SLOT(setFinaltime(int)));

    // Connect execute button
    connect(this->menu_panel_->exec_button_, SIGNAL(clicked(bool)),
            this, SLOT(execute()));

    // Connect simulate button
    connect(this->menu_panel_->sim_button_, SIGNAL(clicked(bool)),
            this, SLOT(toggleSim()));

    connect(this->menu_panel_->duplicate_button_, SIGNAL(clicked(bool)),
            this, SLOT(duplicateSelected()));

    timer_sim_ = new QTimer(this);
    connect(this->timer_sim_, SIGNAL(timeout()), this, SLOT(stepSim()));

    // Expand view to fill screen
    this->expandView();

    this->view_tick_ = 0;
}

void View::mousePressEvent(QMouseEvent *event) {
    // Grab event position in scene coordinates
    if (this->controller_->isFrozen()) {
        qDebug() << "Frozen! Cannot take mouse event";
    }
    QPointF pos = this->mapToScene(event->pos());

    switch (this->state_) {
        case FREEZE: {
            // do nothing
            break;
        }
        case POINT: {
            this->controller_->updateFinalPosition(pos);
            break;
        }
        case ELLIPSE: {
            // TODO(bchasnov): disallow user from adding
            // ellipse if it overlaps?
            qreal scaling_factor = this->matrix().m11();
            this->controller_->addEllipse(new QPointF(pos),
                                          DEFAULT_RAD / scaling_factor);
            break;
        }
        case POLYGON: {
            // Add markers until polygon is completed
            if (!this->temp_markers_->isEmpty() &&
                    itemAt(event->pos()) == this->temp_markers_->first()) {
                if (this->temp_markers_->size() >= 3) {
                    QVector<QPointF*> *poly = new QVector<QPointF*>();
                    for (QGraphicsItem *dot : *this->temp_markers_) {
                        poly->append(new QPointF(dot->pos().x(),
                                                 dot->pos().y()));
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
                QPointF *p1 = new QPointF(temp_markers_->first()->pos());
                this->controller_->addPlane(p1, new QPointF(pos));
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
            this->controller_->addWaypoint(new QPointF(pos));
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

    if (this->simulating_ == 0) {
        this->controller_->compute();
    }
}

void View::closeMenu() {
    this->menu_panel_->hide();
    this->menu_button_->show();

    this->setState(IDLE);

    this->repaint();
}

void View::openMenu() {
    this->menu_button_->hide();
    this->menu_panel_->show();
    this->update();
}

void View::setZoom(int value) {
    qreal scaleFactor = value / 100.0;
    qDebug() << scaleFactor;
    this->resetMatrix();
    this->scale(scaleFactor, scaleFactor);
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
    for (MenuButton *button : *this->menu_panel_->menu_buttons_) {
        if (set_idle || button->getButtonType() != button_type) {
            button->setFrameShadow(QFrame::Raised);
        }
    }
}

void View::updatePath() {
    this->controller_->updatePath();
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

// TODO(bchasnov): does this belong in view? probably not..
void View::stepSim() {
    if (this->controller_->simDrone(this->simulating_)) {
        ++this->simulating_;
        QTimer::singleShot(this->controller_->getTimeInterval() * 1000, this,
                           SLOT(stepSim()));
    } else {
        this->simulating_ = 0;
    }
}

void View::toggleSim() {
    this->simulating_ = 0;
    stepSim();
}


void View::setFinaltime(int final_time) {
    float float_final_time = final_time / 10.0;
    this->controller_->setFinaltime(float_final_time);
    this->menu_panel_->opt_finaltime_label_->
            setText("T = " + QString::number(final_time));
}

void View::setHorizon(int horizon) {
    this->controller_->setHorizonLength(horizon);
    this->menu_panel_->opt_horizon_label_->
            setText("N = " + QString::number(horizon));
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

}  // namespace interface
