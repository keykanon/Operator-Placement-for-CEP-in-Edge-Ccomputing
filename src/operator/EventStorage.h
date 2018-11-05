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

#ifndef __MCEP_PLACEMENT_EVENTSTORAGE_H_
#define __MCEP_PLACEMENT_EVENTSTORAGE_H_

#include <omnetpp.h>
//#include "../message/EventFunction.h"
#include "../sumo/io/SumotrInputHandle.h"
#include "../sumo/io/NetInputHandle.h"
#include "../sumo/vehicleMessage/TrafficLogs.h"

//#include "../message/Event_m.h"
#include "../message/EventPacket_m.h"
#include <vector>
#include <map>
#include <ctime>
#include <fstream>
#include "../placement_model/OperatorPlacementManager.h"
#include "../placement_model/CEPModel/StreamPath.h"

using namespace omnetpp;

/**
 * TODO - Generated class
 */
class EventStorage : public cSimpleModule
{
  private:
    //operator graph numbers
    int networkSize ;


    //operator graph response time constraints
    vector<int> intensiveAddr;
    int intensiveAddrNum = 1;
    vector<double> RT_CONSTRAINTS = {1,1,1};//,1,1,1,1,1,1};
    vector<int> type = {10,10,10};//,4,5,3,4,5};
    const int OGNUM = RT_CONSTRAINTS.size();
    map<int, vector<int>> edgeCepMap;
    ofstream out;

    //strategy
    int strategy = 0;
    int algorithm =  0;

    double sendDelay =  4000 * intensiveAddrNum;
//record result
    cOutVector endToEndDelayVec;
    cStdDev eedStats;


    cMessage* SENDMESSAGE;
    NetInputHandle* net;
    TrafficMap* trafficmap;
    SumotrInputHandle* sumotr;
    TrafficLogs* trafficLogs;
    //int temp;
    //double delta;
    int app_num;
    vector<string> vehicleIDs = {"0","2","4"};//,"8", "9","17","19","22","28"};//,"28","29"};
    int sumoStart = 110, sumoEnd = 130;
    const int TIME_INIT = 110;
    vector<int> time ;//= {110,110,110,110};//,150,150};
    vector<int> timeEnd ;//= {170,170,170,170};//,250,250};
    vector<bool> replace;

    int sim_time;
    int timestamp;
    //vector<int> timeMarker = {5,15,25,35,45};

    vector<bool> app_active;

    //vector<bool> s_or_l;
    //vector<bool> smarker;
    //vector<bool> lmarker;

    vector<vector<vehicleStatus*>*> vehicles;

    //Operator Placement Model
    map<int, OperatorPlacementManager*> opm;
    map<int,vector<vector<StreamPath*>>> placement;
    int placementIndex;

    //vector<vector<int>> simple_placement;

    bool first = true;
    int monitor_interval = 5;

    //basic
    cQueue queue;
    int myAddress;
    std::vector<int> destAddresses;
    std::vector<int> destAddress;
    int pkCounter;

    SimTime simtime;
    SimTime monitorTime;
    double processTime;



    vector<int> numSent;
    vector<int> totalNumSent;

    vector<std::map<int, int>> eventsRecord;

    vector<double> algorithm_time;

    map<int,std::map<int, double>> response_time_record;
    map<int, std::map<int, double>> process_time_record;
    map<int,std::map<int, double>> queue_time_record;
    map<int,std::map<int, double>> transmission_time_record;

    double totalNetworkUsage;
    std::map<int, double> networkUsage;
    std::map<int, double> managerNetworkUsage;

    int monitorMessageNum;

    int numReceived;
    cPar *packetLengthBytes;

    // signals
    simsignal_t endToEndDelaySignal;
    simsignal_t hopCountSignal;
    simsignal_t sourceAddressSignal;

    //update record
    void updateRecord(map<int,map<int, double>>& record, int tid, int appIndex, double time);
    void printRecord( map<int,map<int, double>>& record);
  protected:

     virtual void initialize() override;
     virtual void handleMessage(cMessage *msg) override;
     virtual void finish() override;
  public:
     //get the destination addresses
     std::string getDestAddrs(int app_num, int pathIndex, bool&);
    void init();
    void processMessage(cMessage* ep) ;
    bool isAllPlaced(vector<StreamPath*>);
    EventPacket* setEventPacket(vehicleStatus* vehs, string destAddrs, int port, int time, int app_num, bool used, vector<OperatorModel*>);
    EventPacket* setEventMarker(string destAddrs, int port, int time,  int app_num, bool used, vector<OperatorModel*>);
    void sendPlacementPacket(int destAddr,int baseNode,OperatorPlacementManager* nodeOpm);
};

#endif
