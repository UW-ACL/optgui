// TITLE:   Optimization_Interface/controller.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "controller.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

#include "ellipse_graphics_item.h"
#include "polygon_graphics_item.h"
#include "plane_graphics_item.h"
#include "path_graphics_item.h"
#include "globals.h"

namespace interface {

Controller::Controller(Canvas *canvas) {
    this->canvas_ = canvas;
    this->model_ = new ConstraintModel();

    // initialize path graphic
    this->path_graphic_ = new PathGraphicsItem(this->model_->path_);
    this->canvas_->addItem(this->path_graphic_);
}

Controller::~Controller() {
    // deinitialize path graphic
    this->clearPath();
    delete this->path_graphic_;

    // clean up model
    delete this->model_;
}

void Controller::setCanvas(Canvas *canvas) {
    this->canvas_ = canvas;
}

void Controller::removeItem(QGraphicsItem *item) {
    switch (item->type()) {
        case ELLIPSE_GRAPHIC: {
            EllipseGraphicsItem *ellipse = qgraphicsitem_cast<
                    EllipseGraphicsItem *>(item);
            EllipseModelItem *model = ellipse->model_;
            this->model_->removeEllipse(model);
            this->canvas_->removeItem(ellipse);
            delete model;
            delete item;
            break;
        }
        case POLYGON_GRAPHIC: {
            PolygonGraphicsItem *polygon = qgraphicsitem_cast<
                    PolygonGraphicsItem *>(item);
            PolygonModelItem *model = polygon->model_;
            this->model_->removePolygon(model);
            this->canvas_->removeItem(polygon);
            delete model;
            delete item;
            break;
        }
        case PLANE_GRAPHIC: {
            PlaneGraphicsItem *plane = qgraphicsitem_cast<
                    PlaneGraphicsItem *>(item);
            PlaneModelItem *model = plane->model_;
            this->model_->removePlane(model);
            this->canvas_->removeItem(plane);
            delete model;
            delete item;
            break;
        }
        case HANDLE_GRAPHIC: {
            if (item->parentItem() &&
                    item->parentItem()->type() == PATH_GRAPHIC) {
                PolygonResizeHandle *point_handle =
                        dynamic_cast<PolygonResizeHandle *>(item);
                QPointF *point_model = point_handle->getPoint();
                qgraphicsitem_cast<PathGraphicsItem *>
                        (point_handle->parentItem())->removeHandle(point_handle);
                this->model_->removePathPoint(point_model);
                this->canvas_->removeItem(point_handle);
                this->canvas_->update();
                delete point_model;
                delete point_handle;
            }
            break;
        }
    }
}

void Controller::flipDirection(QGraphicsItem *item) {
    switch (item->type()) {
        case ELLIPSE_GRAPHIC: {
            EllipseGraphicsItem *ellipse = qgraphicsitem_cast<
                    EllipseGraphicsItem *>(item);
            ellipse->flipDirection();
            break;
        }
        case POLYGON_GRAPHIC: {
            PolygonGraphicsItem *polygon = qgraphicsitem_cast<
                    PolygonGraphicsItem *>(item);
            polygon->flipDirection();
            break;
        }
        case PLANE_GRAPHIC: {
            PlaneGraphicsItem *plane = qgraphicsitem_cast<
                    PlaneGraphicsItem *>(item);
            plane->flipDirection();
            break;
        }
    }
}

void Controller::addEllipse(QPointF *point) {
    EllipseModelItem *item_model = new EllipseModelItem(point);
    this->loadEllipse(item_model);
}

void Controller::addPolygon(QVector<QPointF *> *points) {
    PolygonModelItem *item_model = new PolygonModelItem(points);
    this->loadPolygon(item_model);
}

void Controller::addPlane(QPointF *p1, QPointF *p2) {
    PlaneModelItem *item_model = new PlaneModelItem(p1, p2);
    this->loadPlane(item_model);
}

void Controller::addPath(QPointF *point) {
    this->loadPath(point);
}


void Controller::loadEllipse(EllipseModelItem *item_model) {
    EllipseGraphicsItem *item_graphic = new EllipseGraphicsItem(item_model);
    this->canvas_->addItem(item_graphic);
    this->model_->addEllipse(item_model);
    this->canvas_->bringToFront(item_graphic);
    item_graphic->expandScene();
}

void Controller::loadPolygon(PolygonModelItem *item_model) {
    PolygonGraphicsItem *item_graphic = new PolygonGraphicsItem(item_model);
    this->canvas_->addItem(item_graphic);
    this->model_->addPolygon(item_model);
    this->canvas_->bringToFront(item_graphic);
    item_graphic->expandScene();
}

void Controller::loadPlane(PlaneModelItem *item_model) {
    PlaneGraphicsItem *item_graphic = new PlaneGraphicsItem(item_model);
    this->canvas_->addItem(item_graphic);
    this->model_->addPlane(item_model);
    this->canvas_->bringToFront(item_graphic);
    item_graphic->expandScene();
}

void Controller::loadPath(QPointF *point) {
    this->model_->addPathPoint(point);
    this->canvas_->update();
}

void Controller::writeEllipse(EllipseModelItem *model, QDataStream *out) {
    *out << (bool)model->direction_;
    *out << *model->pos_;
    *out << (double)model->radius_;
}

void Controller::writePolygon(PolygonModelItem *model, QDataStream *out) {
    *out << (bool)model->direction_;
    *out << (quint32)model->points_->size();
    for (QPointF *point : *model->points_) {
        *out << *point;
    }
}

void Controller::writePlane(PlaneModelItem *model, QDataStream *out) {
    *out << (bool)model->direction_;
    *out << *model->p1_;
    *out << *model->p2_;
}

void Controller::writePath(QPointF *point, QDataStream *out) {
    *out << *point;
}

EllipseModelItem *Controller::readEllipse(QDataStream *in) {
    bool direction;
    *in >> direction;
    QPointF pos;
    *in >> pos;
    double radius;
    *in >> radius;

    EllipseModelItem *model = new EllipseModelItem(new QPointF(pos));
    model->direction_ = direction;
    model->radius_ = radius;

    return model;
}

PolygonModelItem *Controller::readPolygon(QDataStream *in) {
    bool direction;
    *in >> direction;
    quint32 size;
    *in >> size;
    QVector<QPointF *> *points = new QVector<QPointF *>();
    points->reserve(size);
    for (quint32 i = 0; i < size; i++) {
        QPointF point;
        *in >> point;
        points->append(new QPointF(point));
    }
    PolygonModelItem *model = new PolygonModelItem(points);
    model->direction_ = direction;

    return model;
}

PlaneModelItem *Controller::readPlane(QDataStream *in) {
    bool direction;
    *in >> direction;
    QPointF p1;
    *in >> p1;
    QPointF p2;
    *in >> p2;

    PlaneModelItem *model = new PlaneModelItem(new QPointF(p1),
                                               new QPointF(p2));
    model->direction_ = direction;

    return model;
}

QPointF *Controller::readPath(QDataStream *in) {
    QPointF point;
    *in >> point;
    return new QPointF(point);
}

void Controller::saveFile() {
    QString file_name = QFileDialog::getSaveFileName(nullptr,
        QFileDialog::tr("Save Constraint Layout"), "",
        QFileDialog::tr("Constraint Layout (*.cst);;All Files (*)"));

    if (file_name.isEmpty()) {
        return;
    } else {
        QFile *file = new QFile(file_name);
        if (!file->open(QIODevice::WriteOnly)) {
            QMessageBox::information(nullptr,
                                     QFileDialog::tr("Unable to open file"),
                                     file->errorString());
            delete file;
            return;
        }

        QDataStream *out = new QDataStream(file);
        out->setVersion(QDataStream::Qt_5_11);

        // Write ellipses
        quint32 num_ellipses = this->model_->ellipses_->size();
        *out << num_ellipses;
        for (EllipseModelItem *model : *this->model_->ellipses_) {
            this->writeEllipse(model, out);
        }

        // Write polygons
        quint32 num_polygons = this->model_->polygons_->size();
        *out << num_polygons;
        for (PolygonModelItem *model : *this->model_->polygons_) {
            this->writePolygon(model, out);
        }

        // Write planes
        quint32 num_planes = this->model_->planes_->size();
        *out << num_planes;
        for (PlaneModelItem *model : *this->model_->planes_) {
            this->writePlane(model, out);
        }

        // Write ellipses
        quint32 num_paths = this->model_->path_->size();
        *out << num_paths;
        for (QPointF *model : *this->model_->path_) {
            this->writePath(model, out);
        }

        // Clean up IO
        delete out;
        delete file;
    }
}

void Controller::loadFile() {
    QString file_name = QFileDialog::getOpenFileName(nullptr,
        QFileDialog::tr("Open Constraint Layout"), "",
        QFileDialog::tr("Constraint Layout (*.cst);;All Files (*)"));

    if (file_name.isEmpty()) {
        return;
    } else {
        QFile *file = new QFile(file_name);

        if (!file->open(QIODevice::ReadOnly)) {
            QMessageBox::information(nullptr,
                                     QFileDialog::tr("Unable to open file"),
                                     file->errorString());
            delete file;
            return;
        }

        QDataStream *in = new QDataStream(file);

        // Reset model
        delete this->model_;
        this->model_ = new ConstraintModel();

        // Reset path graphic
        delete this->path_graphic_;

        this->path_graphic_ = new PathGraphicsItem(this->model_->path_);
        this->canvas_->addItem(this->path_graphic_);

        // Read ellipses
        quint32 num_ellipses;
        *in >> num_ellipses;
        for (quint32 i = 0; i < num_ellipses; i++) {
            this->loadEllipse(this->readEllipse(in));
        }

        // Read polygons
        quint32 num_polygons;
        *in >> num_polygons;
        for (quint32 i = 0; i < num_polygons; i++) {
            this->loadPolygon(this->readPolygon(in));
        }

        // Read planes
        quint32 num_planes;
        *in >> num_planes;
        for (quint32 i = 0; i < num_planes; i++) {
            this->loadPlane(this->readPlane(in));
        }

        // Read paths
        quint32 num_paths;
        *in >> num_paths;
        for (quint32 i = 0; i < num_paths; i++) {
            this->loadPath(this->readPath(in));
        }

        // clean up IO
        delete in;
        delete file;
    }
}

void Controller::execute() {
    qDebug() << "executing";
}

void Controller::clearPath() {
    this->canvas_->removeItem(this->path_graphic_);
}

}  // namespace interface
