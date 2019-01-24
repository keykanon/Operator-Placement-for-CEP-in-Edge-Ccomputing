/*
 * Migration.cpp
 *
 *  Created on: 2017Äê11ÔÂ28ÈÕ
 *      Author: airsola
 */

#include "Migration.h"

Migration::Migration() {
    // TODO Auto-generated constructor stub

}
Migration::Migration(OperatorModel* op, FogNode* s, FogNode* d, double time, double delay){
    this->op = op;
    this->source = s;
    this->dest = d;
    this->time =time;
    map<int,FogEdge*> edges = s->getEdges();
    if(edges.count(d->getNodeID()) > 0){
        this->cost = s->getEdges()[d->getNodeID()]->getTransmission_rate() * delay * time;
    }
}

double Migration::getCost() const {
    return cost;
}

void Migration::setCost(double cost) {
    this->cost = cost;
}

FogNode* Migration::getDest() const {
    return dest;
}

void Migration::setDest(FogNode* dest) {
    this->dest = dest;
}

FogNode* Migration::getSource() const {
    return source;
}

 OperatorModel* Migration::getOp() {
    return op;
}

void Migration::setOp(OperatorModel* op) {
    this->op = op;
}

double Migration::getTime() const {
    return time;
}

void Migration::setTime(double time) {
    this->time = time;
}

void Migration::setSource(FogNode* source) {
    this->source = source;
}

Migration::~Migration() {
    // TODO Auto-generated destructor stub
}


