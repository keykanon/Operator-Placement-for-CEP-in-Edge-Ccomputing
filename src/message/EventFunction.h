
#ifndef EVENT_FUNCTION_H
#define EVENT_FUNCTION_H

#include "../sumo/vehicleMessage/VehicleStatus.h"
#include "Event_m.h"
#include <omnetpp.h>

//set normal message
Event* setMessage(vehicleStatus* vehs, int time){
    Event* event = new Event();
    event->setMarker(false);
    event->setTime(time);
    event->setVehicleID(std::atoi(vehs->getVehicleID().c_str()));
    event->setSpeed(vehs->getSpeed());
    event->setLaneID(vehs->getLaneID().c_str());

    return event;
}

//set marker message
Event* setMarker(int time){
    Event* event = new Event();
    event->setMarker(true);
    event->setTime(time);

    return event;
}

Event* setMessage(Event* e){
    Event* event = new Event();
    event->setMarker(e->getMarker());
    event->setTime(e->getTime());
    event->setVehicleID(e->getVehicleID());
    event->setSpeed(e->getSpeed());
    event->setLaneID(e->getLaneID());

    return event;
}
#endif
