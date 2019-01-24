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

#include "Router.h"
#include "../placement_model/OperatorPlacementManager.h"

Define_Module(Router);

void Router::initialize(){
    networkUsage=  0.0;
    managerNetworkUsage = 0.0;

    myAddress = getParentModule()->par("Naddress");

    dropSignal = registerSignal("drop");
    outputIfSignal = registerSignal("outputIf");

    int this_address = getParentModule()->par("Naddress");
    int this_capacity =  getParentModule()->par("capacity");
    fognode = new FogNode(this_address, this_capacity);

    this->monitor = false;
    monitorID = 0;

    MONITOR_MESSAGE = new cMessage("Monitor");

    capacity = &(getParentModule()->par("capacity"));
    nodeCapacity = getParentModule()->par("capacity").longValue();

    scheduleAt(simTime(), MONITOR_MESSAGE);
    //
// Brute force approach -- every node does topology discovery on its own,
// and finds routes to all other nodes independently, at the beginning
// of the simulation. This could be improved: (1) central routing database,
// (2) on-demand route calculation
//
    cTopology *topo = new cTopology("topo");

    std::vector<std::string> nedTypes;
    nedTypes.push_back(getParentModule()->getNedTypeName());
    topo->extractByNedTypeName(nedTypes);
    //EV << "cTopology found " << topo->getNumNodes() << " nodes\n";

    cTopology::Node *thisNode = topo->getNodeFor(getParentModule());



    // find and store next hops
    for (int i = 0; i < topo->getNumNodes(); i++) {
        if (topo->getNode(i) == thisNode)
            continue;  // skip ourselves
        topo->calculateUnweightedSingleShortestPathsTo(topo->getNode(i));

        if (thisNode->getNumPaths() == 0)
            continue;  // not connected

        cGate *parentModuleGate = thisNode->getPath(0)->getLocalGate();
        int gateIndex = parentModuleGate->getIndex();
        int address = topo->getNode(i)->getModule()->par("Naddress");
        int destCapacity = topo->getNode(i)->getModule()->par("capacity");
        rtable[address] = gateIndex;

        //set fognode
        char edgeID[30];
        sprintf(edgeID, "%d-%d", fognode->getNodeID(), address );
        FogNode* fd = new FogNode(address,destCapacity );
        FogEdge* edge = new FogEdge(edgeID, fognode, fd);
        fognode->addEdge(edge);

        //set node message
        //nodeMessage.neighborNodeID[neighborNum] = fd->getNodeID();
        //nodeMessage.neighborCapacity[neighborNum] = fd->getCapacity();
        //nodeMessage.transmission_rate[neighborNum] = edge->getTransmission_rate();


        //EV << "  towards address " << address << " gateIndex is " << gateIndex << endl;
    }
    delete topo;



}

void Router::handleMessage(cMessage *msg)
{
    clock_t processBegin = clock();
    clock_t processEnd;


    //Monitor Message
    if(msg == MONITOR_MESSAGE){
        if(monitor == true){
            EventPacket* monitor_message = monitorNode();


            send(monitor_message, "eventStorage$o");
            //scheduleAt(simTime() + 5, MONITOR_MESSAGE);
        }
        return;
    }

    EventPacket *pk = check_and_cast<EventPacket *>(msg);

    //set network usage
    networkUsage += pk->getNetworkUsage();
//-------------------Monitor this node-------------------------------
    if(pk->getMonitorFlag()){
        managerNetworkUsage += pk->getNetworkUsage();
        if(pk->getMonitorH() == -1){
            monitor = false;
        }
        else{

            if(pk->getMonitorID() > monitorID){
                monitorID = pk->getMonitorID();
                map<int,map<int, int>*>::iterator it;

                it = appRecords.begin();

                while(it != appRecords.end())
                {
                    //it->first;
                    delete it->second;
                    it->second = NULL;
                    it ++;
                }

                appRecords.clear();
            }
            if(pk->getMonitorH() == 0){
                delete pk;
                return;
            }
            if(!this->monitor){
                this->monitor = true;


                //if(MONITOR_MESSAGE->getArrivalTime() == 0){
                //    scheduleAt(simTime() , MONITOR_MESSAGE);
                //}

            }

            EventPacket* monitor_message = monitorNode();
            monitor_message->setDestAddr(pk->getDestAddr());
            send(monitor_message, "eventStorage$o");

            if(pk->getMonitorH() > 0){

                map<int, FogEdge*>::iterator it;

                map<int, FogEdge*> edges = fognode->getEdges();

                it = edges.begin();

                while(it != edges.end())
                {
                    EventPacket* monitorFlag = new EventPacket();
                    monitorFlag->setMonitorFlag(true);
                    monitorFlag->setMonitorH(pk->getMonitorH() - 1);
                    monitorFlag->setMonitorID(pk->getMonitorID());
                    monitorFlag->setDestAddr(pk->getDestAddr());
                    //it->first;
                    FogNode* dest = it->second->getDest(fognode);
                    int neighborAddress = dest->getNodeID();

                    RoutingTable::iterator rit = rtable.find(neighborAddress);
                    if (rit == rtable.end()) {
                        //EV << "address " << destAddr << " unreachable, discarding packet " << pk->getName() << endl;
                        //emit(dropSignal, (long)pk->getByteLength());
                        delete pk;
                        return;
                    }

                    int outGateIndex = (*rit).second;


                    cGate* outGate = gateHalf("gate" , cGate::OUTPUT, outGateIndex);
                    //endTransmission = outGate->getTransmissionChannel()->getTransmissionFinishTime();
                    //simtime_t tnow = simTime();
                    //if(endTransmission < tnow){
                    send(monitorFlag, outGate);
                    //}
                    //else{
                    //    sendDelayed(monitorFlag, endTransmission-tnow, outGate);
                    //}

                    //send(monitorFlag, outGate);
                    it ++;
                }



            }
            delete pk;
        }
        return;
    }

//-------------------------reply marker message----------------------
    if(pk->getReMarkerMessage()){

        EventPacket* remarker = new EventPacket();
        remarker->setReMarkerMessage(true);
        remarker->setEventNum(pk->getEventNum());

        for(int i = 0; i < pk->getOperatorTypeArraySize(); i ++){
            remarker->setOperatorType(i,pk->getOperatorType(i));
        }
        remarker->setHopCount(pk->getHopCount());
        remarker->setAppNum(pk->getAppNum());
        remarker->setDestAddr(pk->getDestAddr());
        remarker->setTimestamp(pk->getTimestamp());

        remarker->setTime(pk->getTime());
        remarker->setMarkerID(pk->getTime());

        processEnd = clock();
        remarker->setSendTime(pk->getSendTime());
        remarker->setProcessTime(pk->getProcessTime() + (double)(processEnd - processBegin));



        double transmission_time = simTime().dbl() - pk->getTransmissionBeginTime() + pk->getTransmissionTime();

        remarker->setTransmissionTime(transmission_time);
        remarker->setQueueTime(pk->getQueueTime());
        remarker->setQueueBeginTime(pk->getQueueBeginTime());
        remarker->setTransmissionBeginTime(pk->getTransmissionBeginTime());

        remarker->setNetworkUsage(networkUsage+ pk->getNetworkUsage());
        networkUsage = 0;
        remarker->setManagerNetworkUsage(managerNetworkUsage + pk->getManagerNetworkUsage());
        managerNetworkUsage = 0;



        if(remarker->getDestAddr() == -1){
            send(remarker,"eventStorage$o");
            return;
        }

    }

    //--------------------placement packet----------------------------
    if(pk->getPlacementMessage()){
        Placement p = pk->getPlacement();
        if(pk->getDestAddr() == -1){
            resetPlacement(p);
            return;
        }

        if(setPlacement(pk->getPlacement()) == -1){

            p.placementNum = nodeCapacity;
            pk->setPlacement(p);
            pk->setSrcAddr(myAddress);
            send(pk, "eventStorage$o");
        }
        else{
            delete pk;
        }
        return;
    }



//-------------------------event parket-----------------------------

    int destAddr = pk->getDestAddr();
    //event to this node.
    if (destAddr == myAddress) {
        //EV << "local delivery of packet " << pk->getName() << endl;

        int port = -1;
        while(port == -1){
            if(pk->getUsed()){
                port = getPort(pk);
            }
            else{
                port = 0;//pk->getPort();
            }
            if(port == -1){
                port = 0;
            }
        }
        send(pk, "localOut", port);
        //emit(outputIfSignal, -1);  // -1: local
        return;
    }

    //event to other node
    RoutingTable::iterator it = rtable.find(destAddr);
    if (it == rtable.end()) {
        //EV << "address " << destAddr << " unreachable, discarding packet " << pk->getName() << endl;
        //emit(dropSignal, (long)pk->getByteLength());
        delete pk;
        return;
    }

    int outGateIndex = (*it).second;
    //EV << "forwarding packet " << pk->getName() << " on gate index " << outGateIndex << endl;
    //pk->setHopCount(pk->getHopCount()+1);
    //emit(outputIfSignal, outGateIndex);

    cGate* outGate = gateHalf("gate" , cGate::OUTPUT, outGateIndex);
    //send(pk, outGate);
    //simtime_t endTransmission = outGate->getTransmissionChannel()->getTransmissionFinishTime();
    //simtime_t tnow = simTime();
    //if(endTransmission < tnow){
    send(pk, outGate);
    //}
    //else{
    //    sendDelayed(pk, endTransmission-tnow, outGate);
    //}


}

void Router::resetPlacement(Placement p){
    nodeCapacity = capacity->longValue();
    for(int i = 0; i < p.placementNum; i ++){
        if(appRecords.count(p.OperatorGraphIndex[i]) > 0){
            //nodeCapacity += appRecords[p.OperatorGraphIndex[i]]->size();
            //if(nodeCapacity > capacity->longValue()){
            //    nodeCapacity = capacity->longValue();
            //}
            delete appRecords[p.OperatorGraphIndex[i]];
            appRecords.erase(p.OperatorGraphIndex[i]);
        }
    }
}

int Router::setPlacement(Placement p){

    //resetCapacity
    resetPlacement(p);


    //have no capacity

    if(p.placementNum > nodeCapacity){
        return -1;
    }
    else{
        for(int i = 0; i < p.placementNum; i ++){
            if(appRecords.count(p.OperatorGraphIndex[i]) > 0){
                map<int, int>* appRecord = appRecords[p.OperatorGraphIndex[i]];
                if(appRecord->count(p.operatorID[i]) == 0){
                    (*appRecord)[p.operatorID[i]] = (capacity->longValue()-nodeCapacity);
                    nodeCapacity --;
                }
            }
            else{
                map<int, int>* appRecord = new map<int, int>();
                (*appRecord)[p.operatorID[i]] = (capacity->longValue()-nodeCapacity);
                appRecords[p.OperatorGraphIndex[i]] = appRecord;
                nodeCapacity -- ;
            }
        }
        return 1;
    }
}

int Router::getPort(EventPacket* pk){
//----------deal with the message of capacity-----


    int port = -1;
    int app_num = pk->getAppNum();
    //if appRecords has no key app_num
    if(appRecords.count(app_num) == 0 ){

        return -1;

    }
    else{
        map<int, int>* appRecord = appRecords[app_num];
        if(appRecord->count(pk->getPort()) > 0){

            port = (*appRecord)[pk->getOperatorType(pk->getHopCount())];
        }
        else{
            /*if(nodeCapacity > 0){
                port = (capacity->longValue()-nodeCapacity);
                (*appRecord)[pk->getPort()] = port;

                nodeCapacity --;
            }
            else{*/
            return -1;
            //}
        }
        /*switch(pk->getPort()){
        case 0:
            if(appRecord->count(0) == 0){
                if(nodeCapacity == 0){
                    return -1;
                }
                (*appRecord)[0] = pk->getPort() + (capacity->longValue()-nodeCapacity)*4;
                port = pk->getPort() + (capacity->longValue()-nodeCapacity)*4;
                nodeCapacity --;
            }
            else{
                port = (*appRecord)[0];
            }
            break;
        case 1:
            if(appRecord->count(1) == 0){
                if(nodeCapacity == 0){
                    return -1;
                }
                (*appRecord)[1] = pk->getPort() + (capacity->longValue()-nodeCapacity)*4;
                port = pk->getPort() + (capacity->longValue()-nodeCapacity)*4;
                nodeCapacity --;
            }
            else{
                port = (*appRecord)[1];
            }
            break;
        case 2:
        case 3:
            if(appRecord->count(2) == 0){
                if(nodeCapacity == 0){
                    return -1;
                }
                (*appRecord)[2] = pk->getPort() + (capacity->longValue()-nodeCapacity)*4;
                port = pk->getPort() + (capacity->longValue()-nodeCapacity)*4;
                nodeCapacity --;
            }
            else{
                port = (*appRecord)[2];
            }

            break;
        default:
            break;
        }*/
    }

    return port;
    //-----------------------------------------
}

EventPacket* Router::monitorNode(){
    //
  // Brute force approach -- every node does topology discovery on its own,
  // and finds routes to all other nodes independently, at the beginning
  // of the simulation. This could be improved: (1) central routing database,
  // (2) on-demand route calculation
  //
    EventPacket* monitorMessage = new EventPacket();
    monitorMessage->setPlacementMessage(false);

    fognode->clearEdge();

    NodeMessage nodeMessage;
    nodeMessage.nodeID = fognode->getNodeID();
    nodeMessage.capacity = nodeCapacity;

    int neighborNum = 0;

    cTopology *topo = new cTopology("topo");

    std::vector<std::string> nedTypes;
    nedTypes.push_back(getParentModule()->getNedTypeName());
    topo->extractByNedTypeName(nedTypes);
    //EV << "cTopology found " << topo->getNumNodes() << " nodes\n";

    cTopology::Node *thisNode = topo->getNodeFor(getParentModule());



    // find and store next hops
    for (int i = 0; i < topo->getNumNodes(); i++) {
        if (topo->getNode(i) == thisNode)
            continue;  // skip ourselves
        topo->calculateUnweightedSingleShortestPathsTo(topo->getNode(i));

        if (thisNode->getNumPaths() == 0)
            continue;  // not connected

        cGate *parentModuleGate = thisNode->getPath(0)->getLocalGate();
        int gateIndex = parentModuleGate->getIndex();
        int address = topo->getNode(i)->getModule()->par("Naddress");
        //int destCapacity = topo->getNode(i)->getModule()->par("capacity");
        rtable[address] = gateIndex;
        //EV << "  towards address " << address << " gateIndex is " << gateIndex << endl;
    }
    neighborNum = thisNode->getNumOutLinks();
    for(int i = 0; i < thisNode->getNumOutLinks(); i ++){
        cTopology::Node *neighborNode = thisNode->getLinkOut(i)->getRemoteNode();

        //not event storage
        if(strcmp(neighborNode->getModule()->getFullName(),"eventStorage") == 0){
            continue;
        }
        if(neighborNode == thisNode){
            continue;
        }
        //set fognode
        char edgeID[30];
        int address = neighborNode->getModule()->par("Naddress");
        int destCapacity = neighborNode->getModule()->par("capacity");
        sprintf(edgeID, "%d-%d", fognode->getNodeID(), address );
        FogNode* fd = new FogNode(address,destCapacity );

        double transmission_rate = 0.0;
        int outGateIndex = 0;
        //event to other node
        RoutingTable::iterator it = rtable.find(address);
        //if (it != rtable.end()) {
            //EV << "address " << destAddr << " unreachable, discarding packet " << pk->getName() << endl;
            //emit(dropSignal, (long)pk->getByteLength());

                    //EV << "forwarding packet " << pk->getName() << " on gate index " << outGateIndex << endl;


        //}

        outGateIndex = (*it).second;

        cGate* outGate = getParentModule()->gateHalf("g" , cGate::OUTPUT, outGateIndex);
        //cGate* outGate = gateHalf("gate" , cGate::OUTPUT, outGateIndex);
        transmission_rate = outGate->getTransmissionChannel()->getNominalDatarate();
        FogEdge* edge = new FogEdge(edgeID, fognode, fd,transmission_rate);
        fognode->addEdge(edge);

        //set node message
        nodeMessage.neighborNodeID[i] = fd->getNodeID();
        nodeMessage.neighborCapacity[i] = fd->getCapacity();
        nodeMessage.transmission_rate[i] = transmission_rate;
    }
    delete topo;
    nodeMessage.neighborNum = neighborNum;
    monitorMessage->setFognode(nodeMessage);
    monitorMessage->setMonitorMessage(true);
    monitorMessage->setSrcAddr(fognode->getNodeID());


    return monitorMessage;

}
