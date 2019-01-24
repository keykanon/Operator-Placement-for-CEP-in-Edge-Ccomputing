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

#include "AccidentOperator.h"

Define_Module(AccidentOperator);

void AccidentOperator::initialize()
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

    //destAddresses = cStringTokenizer(par("destAddresses").stdstringValue().c_str()).asIntVector();
    destAddress = destAddresses[intuniform(0, destAddresses.size()-1)];
}

void AccidentOperator::handleMessage(cMessage *msg)
{
    SimTime t = simTime();
    EV << "time = " << t.str().c_str() << endl;
    // TODO - Generated method body
    EventPacket* event = (EventPacket*)msg;
    //marker message
    if(event->getMarkerMessage()){
        //vector<int> destAddress = this->destAddresses[1];
        event->setDestAddr(-1);
        event->setEventNum(numReceived);
        //event->setOperatorType(3);

        send(event,"user");
        numReceived = 0;
    }
    //not marker message
    else{
        numReceived++;
        //get the speed dropped event
        if(strcmp(msg->getArrivalGate()->getName(), "speedIn") == 0){
            speedDroppedRecord[event->getVehicleID()] = event->getTime();
        }
        //get the lane switch event
        else{
            //a accident event
            if(speedDroppedRecord.count(event->getVehicleID()) > 0 &&
                    event->getTime() - speedDroppedRecord[event->getVehicleID()] == 1){
                EV << "traffic accident!" << endl;
            }
        }
        delete msg;
    }

}
