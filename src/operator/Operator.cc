
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

#include "Operator.h"

Define_Module(Operator);

void Operator::init(){

}

void Operator::processMessage(cMessage* msg){

}

void Operator::initialize()
{
    // TODO - Generated method body
    EV << "Operator Initial" << endl;
    numSent = numReceived = 0;
    WATCH(numSent);
    WATCH(numReceived);

    //fsm.setName("fsm");

    destAddresses = cStringTokenizer(par("destAddresses").stdstringValue().c_str()).asIntVector();
    myAddress = par("address").longValue();
    lambda = par("lambda").doubleValue();
    sleepTime = &par("sleepTime");
    burstTime = &par("burstTime");
    sendIATime = &par("sendIaTime");
    packetLengthBytes = &par("packetLength");

    endToEndDelaySignal = registerSignal("endToEndDelay");
    hopCountSignal = registerSignal("hopCount");
    sourceAddressSignal = registerSignal("sourceAddress");

    pkCounter = 0;
    WATCH(pkCounter);  // always put watches in initialize(), NEVER in handleMessage()
    //startStopBurst = new cMessage("startStopBurst");
    //sendMessage = new cMessage("sendMessage");
    init();
}

void Operator::handleMessage(cMessage *msg)
{
    // TODO - Generated method body
    processPacket((EventPacket*)msg);
}

EventPacket* Operator::generatePacket(const char* destAddrs, int port){
    // generate and send out a packet

    //first calculate the address
   //int destAddress = destAddresses[intuniform(0, destAddresses.size()-1)];

   //EV << "generating packet " << pkname << endl;

   EventPacket *pk = new EventPacket();
   pk->setByteLength(packetLengthBytes->longValue());
   pk->setSrcAddr(myAddress);

   pk->setDestAddrs(destAddrs);
   pk->setPort(port);
   pk->setMarkerMessage(false);
   pk->setMonitorFlag(false);
   pk->setMonitorMessage(false);
   pk->setReMarkerMessage(false);
   pk->setPlacementMessage(false);
   pk->setUsed(true);



   return pk;

}

EventPacket* Operator::generatePacket(const char* destAddrs, EventPacket* ep){
    EventPacket* pk = generatePacket(destAddrs, ep->getPort());

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

    pk->setHopCount(ep->getHopCount()+1);

    std::vector<int> destAddresses = cStringTokenizer(destAddrs).asIntVector();
    char pkname[40];
    sprintf(pkname, "pk-%d-to-%d-#%d", myAddress, destAddresses[ep->getHopCount()+1], pkCounter++);
    int destAddress = -1;
    if(ep->getHopCount()+1 < destAddresses.size()){
        destAddress = destAddresses[ep->getHopCount()+1];
    }
    pk->setDestAddr(destAddress);

    return pk;
}

EventPacket* Operator::generateMarker(const char* destAddrs,  EventPacket* marker){
    EventPacket* pk = generatePacket(destAddrs, marker->getPort());


    pk->setMarkerMessage(true);
    pk->setTime(marker->getTime());
    pk->setAppNum(marker->getAppNum());
    pk->setMarkerID(marker->getMarkerID());

    pk->setUsed(marker->getUsed());
    pk->setReMarkerMessage(true);
    for(int i = 0; i < marker->getOperatorTypeArraySize();i  ++){
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

    std::vector<int> destAddresses = cStringTokenizer(destAddrs).asIntVector();
    //char pkname[40];
    //sprintf(pkname, "pk-%d-to-%d-#%d", myAddress, destAddresses[ep->getHopCount()+1], pkCounter++);
    int destAddress = -1;
    if(marker->getHopCount()+1 < destAddresses.size()){
        destAddress = destAddresses[marker->getHopCount()+1];
    }
    pk->setDestAddr(destAddress);

    numReceived = 0;
    return pk;
}

void Operator::processPacket(EventPacket *pk)
{



    clock_t process_begin = clock();
    clock_t process_end = clock();

    EventPacket* event = pk;

    //judge the event is marker
    if(event->getMarkerMessage()){

        EventPacket* marker = generateMarker(event->getDestAddrs(),  event);
        send(marker,"out");
    }
    else{
        numReceived++;
        if(rand() % 4 == 0){
            EventPacket* ep = generatePacket( event->getDestAddrs(),event);
            process_end = clock();
            send(ep, "out");
            //sendDelayed(ep, lambda, "out");
        }
    }



    delete pk;
}

void Operator::refreshDisplay() const
{
    // update status string above icon
    //char txt[64];
    //sprintf(txt, "sent:%d received:%d", numSent, numReceived);
    //getDisplayString().setTagArg("t", 0, txt);
}
