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

#include "LaneOperator.h"

Define_Module(LaneOperator);

//set marker message
/*Event* LaneOperator::setMarker(int time){
    Event* event = new Event();
    event->setMarker(true);
    event->setTime(time);

    return event;
}

Event* LaneOperator::setMessage(Event* e){
    Event* event = new Event();
    event->setMarker(e->getMarker());
    event->setTime(e->getTime());
    event->setVehicleID(e->getVehicleID());
    event->setSpeed(e->getSpeed());
    event->setLaneID(e->getLaneID());

    return event;
}*/
void LaneOperator::init(){
    destAddress = rand() % destAddresses[1];//destAddresses[intuniform(0, destAddresses.size()-1)];
}

void LaneOperator::initialize()
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

    init();
}

void LaneOperator::processMessage(cMessage* msg){

    clock_t process_begin = clock();
    clock_t process_end = clock();

    EventPacket* event = (EventPacket*)msg;

    //judge the event is marker
    if(event->getMarkerMessage()){
        EventPacket* marker = generateMarker(event->getDestAddrs(), 3, event);
        send(marker,"accident");
    }
    else{
        //find map
       numReceived ++;
       std::vector<opp_string>* vehLanes;
       std::map<int, std::vector<opp_string>*>::iterator mit = lanes.find(event->getVehicleID());
       if(mit == lanes.end()){
           vehLanes = new std::vector<opp_string>();
       }
       else{
           vehLanes = mit->second;
           int size = vehLanes->size();
           if(size >= 1 &&
                   (strcmp(event->getLaneID(), (*vehLanes)[size-1].c_str()) != 0)){

               EventPacket* ep = generatePacket( event->getDestAddrs(),3,event);
               process_end = clock();
               sendDelayed(ep,(double(process_end - process_begin))/1000.0, "accident");
           }
       }
       vehLanes->push_back(event->getLaneID());
       lanes[event->getVehicleID()] = vehLanes;


    }
    delete msg;
}

void LaneOperator::handleMessage(cMessage *msg)
{
    // TODO - Generated method body
    processMessage(msg);
}

EventPacket* LaneOperator::generatePacket(const char* destAddrs, int port){
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

EventPacket* LaneOperator::generatePacket(const char* destAddrs, int port, EventPacket* ep){
    EventPacket* pk = generatePacket(destAddrs, port);

    pk->setMarkerMessage(ep->getMarkerMessage());
    pk->setTime(ep->getTime());
    pk->setVehicleID(ep->getVehicleID());
    pk->setSpeed(ep->getSpeed());
    pk->setLaneID(ep->getLaneID());
    pk->setAppNum(ep->getAppNum());
    pk->setMarkerMessage(ep->getMarkerMessage());
    pk->setMonitorMessage(ep->getMonitorMessage());
    pk->setReMarkerMessage(ep->getReMarkerMessage());
    pk->setUsed(ep->getUsed());

    return pk;
}

EventPacket* LaneOperator::generateMarker(const char* destAddrs, int port, EventPacket* marker){
    EventPacket* pk = generatePacket(destAddrs, port);

    pk->setMarkerMessage(true);
    pk->setTime(marker->getTime());
    pk->setAppNum(marker->getAppNum());
    pk->setMarkerID(marker->getMarkerID());

    pk->setUsed(marker->getUsed());
    pk->setReMarkerMessage(true);
    for(int i = 0; i < marker->getOperatorTypeArraySize(); i ++){
        pk->setOperatorType(i,marker->getOperatorType(i));
    }
    pk->setEventNum(numReceived);

    pk->setSendTime(marker->getSendTime());
    pk->setProcessTime(marker->getProcessTime() );
    pk->setTransmissionTime(marker->getTransmissionTime());
    pk->setQueueTime(marker->getQueueTime());
    pk->setQueueBeginTime(marker->getQueueBeginTime());
    pk->setTransmissionBeginTime(marker->getTransmissionBeginTime());

    pk->setHopCount(marker->getHopCount()+1);

    numReceived = 0;
    return pk;
}
