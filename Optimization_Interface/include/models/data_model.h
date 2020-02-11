// TITLE:   Optimization_Interface/include/models/data_model.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Abstract data model for item with port

#ifndef DATA_MODEL_H_
#define DATA_MODEL_H_

#include <QtMath>

namespace optgui {

class DataModel {
 public:
    DataModel() {}
    virtual ~DataModel() {}
    quint16 port_;
};

}  // namespace optgui

#endif  // DATA_MODEL_H_
