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

#ifndef __MCEP_PLACEMENT_SPEEDOPERATOR_H_
#define __MCEP_PLACEMENT_SPEEDOPERATOR_H_

#include <omnetpp.h>
#include <map>
#include <string.h>
#include <vector>
#include "Operator.h"
//#include "../message/Event_m.h"
//#include "../message/EventFunction.h"

//using namespace std;
using namespace omnetpp;

/**
 * TODO - Generated class
 */
class SpeedOperator : public cSimpleModule
{
private:
    int destAddress;
    //cPar * capacity;



    int myAddress;
    std::vector<int> destAddresses;
    cPar *packetLengthBytes;

    int pkCounter;
    int numSent;
    int numReceived;

    // signals
    simsignal_t endToEndDelaySignal;
    simsignal_t hopCountSignal;
    simsignal_t sourceAddressSignal;
  protected:


    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;

    virtual void init() ;
    virtual void processMessage(cMessage* msg) ;

    std::map<int, std::vector<double>*> speeds;

    //set marker message
    //Event* setMarker(int time);
    //Event* setMessage(Event* e);
    virtual EventPacket* generatePacket(const char* destAddrs, int port);
    virtual EventPacket* generatePacket(const char* destAddrs, int port, EventPacket* ep);
    virtual EventPacket* generateMarker(const char* destAddrs, int port, EventPacket* marker);
};

#endif
