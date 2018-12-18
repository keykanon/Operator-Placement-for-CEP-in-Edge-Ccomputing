//\\\
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

#ifndef __NETWORK_TEST_OPERATOR_H_
#define __NETWORK_TEST_OPERATOR_H_

#include <omnetpp.h>
#include "../message/EventPacket_m.h"

using namespace omnetpp;



/**
 * TODO - Generated class
 */
class Operator : public cSimpleModule
{
  protected:
    //configuration
    int myAddress;
    double lambda;
    std::vector<int> destAddresses;
    cPar *sleepTime;
    cPar *burstTime;
    cPar *sendIATime;
    cPar *packetLengthBytes;

    int pkCounter;
    cMessage *startStopBurst;
    cMessage *sendMessage;
    int numSent;
    int numReceived;

    // signals
    simsignal_t endToEndDelaySignal;
    simsignal_t hopCountSignal;
    simsignal_t sourceAddressSignal;

    //functions
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override ;

    virtual void init() ;
    virtual void processMessage(cMessage* msg) ;
    EventPacket* generatePacket(const char* destAddrs, int port);
    EventPacket* generatePacket(const char* destAddrs,  EventPacket* ep);
    EventPacket* generateMarker(const char* destAddrs,  EventPacket* marker);
    virtual void processPacket(EventPacket *pk);
    virtual void refreshDisplay() const override;
};

#endif
