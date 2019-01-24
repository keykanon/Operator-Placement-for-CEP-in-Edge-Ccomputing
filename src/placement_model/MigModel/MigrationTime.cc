/*
 * MigrationTime.cpp
 *
 *  Created on: 2017Äê11ÔÂ28ÈÕ
 *      Author: airsola
 */

#include "MigrationTime.h"

Migration_Time::Migration_Time() {
    // TODO Auto-generated constructor stub
    time = 0.0;
}

Migration_Time::Migration_Time(vector<OperatorModel> p, vector<Migration> m, double time){
    placement = p;
    mig = m;
    this->time = time;
}
vector<Migration> Migration_Time::getMig(){
    return mig;
}

void Migration_Time::setMig(const vector<Migration>& mig) {
    this->mig = mig;
}

double Migration_Time::getTime()  {
    return time;
}

vector<OperatorModel> Migration_Time::getPlacement() {
    return placement;
}

void Migration_Time::setPlacement(const vector<OperatorModel>& placement) {
    this->placement = placement;
}

void Migration_Time::setTime(double time) {
    this->time = time;
}

//add a migration
void Migration_Time::addMigration(Migration m){
    mig.push_back(m);
}

Migration_Time::~Migration_Time() {
    // TODO Auto-generated destructor stub
}

