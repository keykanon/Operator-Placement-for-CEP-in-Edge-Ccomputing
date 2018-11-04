//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "SpeedOperator.h"

Define_Module(SpeedOperator);

/*Event* SpeedOperator::setMessage(Event* e){
    Event* event = new Event();
    event->setMarker(e->getMarker());
    event->setTime(e->getTime());
    event->setVehicleID(e->getVehicleID());
    event->setSpeed(e->getSpeed());
    event->setLaneID(e->getLaneID());

    return event;
}

//set marker message
Event* SpeedOperator::setMarker(int time){
    Event* event = new Event();
    event->setMarker(true);
    event->setTime(time);

    return event;
}*/

void SpeedOperator::init(){
    destAddress = rand()% destAddresses[1];//destAddresses[intuniform(0, destAddresses.size()-1)];
}
void SpeedOperator::initialize()
{
    // TODO - Generated method body

    numSent = numReceived = 0;
    WATCH(numSent);
    WATCH(numReceived);

    //fsm.setName("fsm");





    destAddresses = cStringTokenizer(par("destAddresses").stdstringValue().c_str()).asIntVector();
    myAddress = par("address").longValue();
    packetLengthBytes = &par("packetLength");

    endToEndDelaySignal = registerSignal("endToEndDelay");
    hopCountSignal = registerSignal("hopCount");
    sourceAddressSignal = registerSignal("sourceAddress");

    pkCounter = 0;
    WATCH(pkCounter);  // always put watches in initialize(), NEVER in handleMessage()
    //startStopBurst = new cMessage("startStopBurst");
    //sendMessage = new cMessage("sendMessage");
    init();
    //destAddresses = cStringTokenizer(par("destAddresses").stdstringValue().c_str()).asIntVector();

}
void SpeedOperator::processMessage(cMessage* msg){

    clock_t process_begin = clock();
    clock_t process_end = clock();


    EventPacket* event = (EventPacket*)msg;

    if(event->getMarkerMessage()){

        EventPacket* ep = generateMarker(event->getDestAddrs(),2,event);
        send(ep, "accident");
    }
    else{
        numReceived ++;
        //find map
        std::vector<double>* vehSpeeds;
        std::map<int, std::vector<double>*>::iterator mit = speeds.find(event->getVehicleID());
        if(mit == speeds.end()){
            vehSpeeds = new std::vector<double>();
        }
        else{
            vehSpeeds = mit->second;
            int size = vehSpeeds->size();
            if(size >= 2 &&
                    event->getSpeed() <= (*vehSpeeds)[size-1] &&
                    (*vehSpeeds)[size-1] <=  (*vehSpeeds)[size-2]){ //&&
                   // (*vehSpeeds)[size-2] <=  (*vehSpeeds)[size-3] &&
                   // (*vehSpeeds)[size-3] <=  (*vehSpeeds)[size-4]){


                EventPacket* ep = generatePacket(event->getDestAddrs(),2, event);

                sendDelayed(ep,(double(process_end - process_begin))/1000.0,"accident");
                //send(ep, "accident");
            }
        }
        vehSpeeds->push_back(event->getSpeed());
        speeds[event->getVehicleID()] = vehSpeeds;
    }

    delete msg;
}

void SpeedOperator::handleMessage(cMessage *msg)
{
    // TODO - Generated method body
    processMessage(msg);
}

EventPacket* SpeedOperator::generatePacket(const char* destAddrs, int port){
    // generate and send out a packet

    //first calculate the address
   //int destAddress = destAddresses[intuniform(0, destAddresses.size()-1)];

   std::vector<int> destAddresses = cStringTokenizer(destAddrs).asIntVector();

   char pkname[40];
   sprintf(pkname, "pk-%d-to-%d-#%d", myAddress, destAddresses[2], pkCounter++);
   //EV << "generating packet " << pkname << endl;

   EventPacket *pk = new EventPacket(pkname);
   pk->setByteLength(packetLengthBytes->longValue());
   pk->setSrcAddr(myAddress);
   pk->setDestAddr(destAddresses[2]);
   //pk->setDestAddrs(destAddrs);
   pk->setPort(port);

   pk->setMarkerMessage(false);
   pk->setMonitorFlag(false);
   pk->setMonitorMessage(false);
   pk->setReMarkerMessage(false);
   pk->setUsed(true);

   return pk;

}

EventPacket* SpeedOperator::generatePacket(const char* destAddrs, int port, EventPacket* ep){
    EventPacket* pk = generatePacket(destAddrs, port);

    pk->setMarkerMessage(ep->getMarkerMessage());
    pk->setTime(ep->getTime());
    pk->setVehicleID(ep->getVehicleID());
    pk->setSpeed(ep->getSpeed());
    pk->setLaneID(ep->getLaneID());
    pk->setAppNum(ep->getAppNum());
    pk->setMarkerMessage(ep->getMarkerMessage());
    pk->setMonitorFlag(ep->getMonitorFlag());
    pk->setMonitorMessage(ep->getMonitorMessage());
    pk->setReMarkerMessage(ep->getReMarkerMessage());
    pk->setUsed(ep->getUsed());
    return pk;
}

EventPacket* SpeedOperator::generateMarker(const char* destAddrs, int port, EventPacket* marker){
    EventPacket* pk = generatePacket(destAddrs, port);
    pk->setAppNum(marker->getAppNum());
    pk->setMarkerMessage(true);
    pk->setMarkerID(marker->getMarkerID());

    pk->setTime(marker->getTime());
    pk->setReMarkerMessage(true);
    for(int i = 0; i < marker->getOperatorTypeArraySize(); i ++){
        pk->setOperatorType(i,marker->getOperatorType(i));
    }
    pk->setEventNum(numReceived);
    pk->setUsed(marker->getUsed());
    numReceived = 0;

    pk->setSendTime(marker->getSendTime());
    pk->setProcessTime(marker->getProcessTime() );
    pk->setTransmissionTime(marker->getTransmissionTime());
    pk->setQueueTime(marker->getQueueTime());
    pk->setQueueBeginTime(marker->getQueueBeginTime());
    pk->setTransmissionBeginTime(marker->getTransmissionBeginTime());

    return pk;
}
