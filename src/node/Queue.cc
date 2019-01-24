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

#include "Queue.h"

Define_Module(Queue);

void Queue::initialize()
{
    // TODO - Generated method body
    SEND_MESSAGE = new cMessage("send message");
    sendFlag = false;
    network_usage = 0.0;
    time = simTime();
}

void Queue::handleMessage(cMessage *msg)
{
    // TODO - Generated method body
    if(msg == SEND_MESSAGE){
        if(!pk_queue.isEmpty()){
            EventPacket* pk = (EventPacket*)pk_queue.pop();
            double queue_time = pk->getQueueTime() + simTime().dbl() - pk->getQueueBeginTime();

            pk->setTransmissionBeginTime(simTime().dbl());
            pk->setQueueTime(queue_time);

            send(pk,"line$o");

            simtime_t endTransmission = gate("line$o")->getTransmissionChannel()->getTransmissionFinishTime();

            if(endTransmission < simTime()){
                endTransmission = simTime();
            }
            scheduleAt(endTransmission, SEND_MESSAGE);
        }
        else{
            sendFlag = false;
        }

    }
    else if(msg->arrivedOn("line$i")){
        EventPacket* pk = (EventPacket*) msg;
        double transmission_time = pk->getTransmissionTime() + simTime().dbl() - pk->getTransmissionBeginTime();
        pk->setTransmissionTime(transmission_time);

        double datarate = msg->getSenderGate()->getTransmissionChannel()->getNominalDatarate() / 1024.0 / 1024.0;
        //EV << "datarate = " << datarate << "-- transmission time " << transmission_time << endl;
        network_usage += datarate * transmission_time;
        //EV << "network usage = " << datarate * transmission_time << endl;
        pk->setNetworkUsage(datarate * transmission_time);
        //network_usage = 0.0;

        send(msg, "router$o");
    }
    else{
        EventPacket* pk = (EventPacket*) msg;
        pk->setQueueBeginTime(simTime().dbl());
        pk_queue.insert(pk);

        if(!sendFlag){
            scheduleAt(simTime(), SEND_MESSAGE);
            sendFlag = true;
        }

    }
}
