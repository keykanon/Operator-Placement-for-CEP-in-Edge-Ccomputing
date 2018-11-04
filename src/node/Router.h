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

#ifndef __NETWORK_TEST_ROUTER_H_
#define __NETWORK_TEST_ROUTER_H_

#include <map>

#include <omnetpp.h>
//#include "Packet_m.h"
#include "../message/EventPacket_m.h"

#include "../placement_model/FogModel/FogNode.h"
#include "../placement_model/FogModel/FogEdge.h"

using namespace omnetpp;

/**
 * TODO - Generated class
 */
class Router : public cSimpleModule
{
 private:
    int myAddress;

    typedef std::map<int, int> RoutingTable;  // destaddr -> gateindex
    RoutingTable rtable;

    FogNode* fognode;

    cMessage* MONITOR_MESSAGE;

    bool monitor;
    int monitorID;

    cPar* capacity;
    int nodeCapacity;
    map<int,map<int, int>*> appRecords;

    simsignal_t dropSignal;
    simsignal_t outputIfSignal;

    double networkUsage;
    double managerNetworkUsage;
  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);

    int getPort(EventPacket* pk);
    int setPlacement(Placement p);
    void resetPlacement(Placement p);

    EventPacket* monitorNode();

};

#endif
