// TITLE:   Optimization_Interface/controller.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "controller.h"

#include "ellipse_graphics_item.h"
#include "polygon_graphics_item.h"
#include "plane_graphics_item.h"
#include "path_graphics_item.h"
#include "globals.h"

namespace interface {

Controller::Controller(Canvas *canvas)
{
    this->canvas_ = canvas;
    this->model_ = new ConstraintModel();
}

Controller::~Controller()
{
    delete this->model_;
}

void Controller::addEllipse(QPointF *point)
{
    EllipseModelItem *item_model = new EllipseModelItem(point);
    EllipseGraphicsItem *item_graphic = new EllipseGraphicsItem(item_model);
    this->canvas_->addItem(item_graphic);
    this->model_->addEllipse(item_model);
    this->canvas_->bringToFront(item_graphic);
    item_graphic->expandScene();
}

void Controller::removeItem(QGraphicsItem *item)
{
    switch (item->type()) {
        case ELLIPSE_GRAPHIC: {
            EllipseGraphicsItem *ellipse = qgraphicsitem_cast<EllipseGraphicsItem *>(item);
            EllipseModelItem *model = ellipse->model_;
            this->model_->removeEllipse(model);
            this->canvas_->removeItem(ellipse);
            delete model;
            delete item;
            break;
        }
        case POLYGON_GRAPHIC: {
            PolygonGraphicsItem *polygon = qgraphicsitem_cast<PolygonGraphicsItem *>(item);
            PolygonModelItem *model = polygon->model_;
            this->model_->removePolygon(model);
            this->canvas_->removeItem(polygon);
            delete model;
            delete item;
            break;
        }
        case PLANE_GRAPHIC: {
            PlaneGraphicsItem *plane = qgraphicsitem_cast<PlaneGraphicsItem *>(item);
            PlaneModelItem *model = plane->model_;
            this->model_->removePlane(model);
            this->canvas_->removeItem(plane);
            delete model;
            delete item;
            break;
        }
        case PATH_GRAPHIC: {
            PathGraphicsItem *path_point = qgraphicsitem_cast<PathGraphicsItem *>(item);
            QPointF *model = path_point->point_;
            this->model_->removePathPoint(model);
            this->canvas_->removeItem(path_point);
            this->canvas_->update();
            delete model;
            delete path_point;
            break;
        }
    }
}

void Controller::flipConvex(QGraphicsItem *item)
{
    switch (item->type()) {
        case ELLIPSE_GRAPHIC: {
            EllipseGraphicsItem *ellipse = qgraphicsitem_cast<EllipseGraphicsItem *>(item);
            ellipse->flipConvex();
            break;
        }
        case POLYGON_GRAPHIC: {
            PolygonGraphicsItem *polygon = qgraphicsitem_cast<PolygonGraphicsItem *>(item);
            polygon->flipConvex();
            break;
        }
        case PLANE_GRAPHIC: {
            PlaneGraphicsItem *plane = qgraphicsitem_cast<PlaneGraphicsItem *>(item);
            plane->flipConvex();
            break;
        }
    }
}

void Controller::addPolygon(QVector<QPointF *> *points)
{
    PolygonModelItem *item_model = new PolygonModelItem(points);
    PolygonGraphicsItem *item_graphic = new PolygonGraphicsItem(item_model);
    this->canvas_->addItem(item_graphic);
    this->model_->addPolygon(item_model);
    this->canvas_->bringToFront(item_graphic);
    item_graphic->expandScene();
}

void Controller::addPlane(QPointF *p1, QPointF *p2)
{
    PlaneModelItem *item_model = new PlaneModelItem(p1, p2);
    PlaneGraphicsItem *item_graphic = new PlaneGraphicsItem(item_model);
    this->canvas_->addItem(item_graphic);
    this->model_->addPlane(item_model);
    this->canvas_->bringToFront(item_graphic);
    item_graphic->expandScene();
}

void Controller::addPath(QPointF *point)
{
    PathGraphicsItem *item_graphic = new PathGraphicsItem(point, this->model_->getPathOrdering());
    this->canvas_->addItem(item_graphic);
    this->model_->addPathPoint(point);
    this->canvas_->bringToFront(item_graphic);
    item_graphic->expandScene();
}

}  // namespace
