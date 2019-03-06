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
#include <cmath>
#include <fstream>
#include "../placement_model/OperatorPlacementManager.h"
#include "../placement_model/CEPModel/StreamPath.h"


using namespace omnetpp;





/**
 * TODO - Generated class
 * 发送事件
 * 监控附近的边缘节点
 * 得到数据
 * 假设部署在最近的边缘节点上，图上之所以用连线的方式，主要是为了实验方便，（假设部署在节点e时，只是用该连线，其他连线无用）
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
    vector<double> RT_MAX_CONSTRAINTS = {10,10,10};
    vector<int> type = {5,5,5};//,4,5,3,4,5};
    const int OGNUM = 3;
    map<int, vector<int>> edgeCepMap;
    ofstream out;

    //strategy
    int strategy = 0;
    int algorithm =  3;

    int sendDelayType = 0;
    int poisson_lambda = 30;
    int rl_type = 1; // 0 for train, 1 for test
    //bool first_monte_carlo_policy = true;

    const int TOTALSENDTIME = 2;
    vector<int> sendTime = {1,1,1};
    double sendDelay =  1200 * intensiveAddrNum;
    double delayChange = 50 * OGNUM * intensiveAddrNum;
    int intensiveNodeID = 7;
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
    int app_num = 0;
    vector<string> vehicleIDs = {"0","2","4"};//,"8", "9","17","19","22","28"};//,"28","29"};
    int sumoStart = 110, sumoEnd = 150;
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
    cQueue markerQueue;
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

    vector<int> roundTimeRecord;
    vector<double> avg_response_time_record;
    map<int,std::map<int, double>> response_time_record;
    map<int, std::map<int, double>> process_time_record;
    map<int,std::map<int, double>> queue_time_record;
    map<int,std::map<int, double>> transmission_time_record;
    map<int, std::map<int, double>> predicted_response_time_record;
    map<int, double> avg_eventNumber_record;
    map<int, std::map<int, double>> eventNumber_record;

    double totalNetworkUsage;
    std::map<int, double> networkUsage;
    std::map<int, double> managerNetworkUsage;

    int monitorMessageNum;

    int numReceived;
    cPar *packetLengthBytes;

    //Reinforcement Learning learner

    // signals
    simsignal_t endToEndDelaySignal;
    simsignal_t hopCountSignal;
    simsignal_t sourceAddressSignal;


    //calculate average record between last time period
    void avgRecordCal(map<int, map<int, double>>& record, vector<double>& avgRecord);
    //update record
    void updateRecord(map<int,map<int, double>>& record, int tid, int appIndex, double time);
    void printRecord( map<int,map<int, double>>& record);
  protected:
    double fac(int k){
        double ret = 1;
        for(int i = 2; i <= k; ++ i){
            ret *= i;
        }
        return ret;
    }

    vector<double> getLastInputRate(){
        vector<double> ret;
        map<int, double> rt = eventNumber_record[sim_time-1];
        for(int i = 0; i < OGNUM; ++ i){
            ret.push_back(rt[i]);
        }
        return ret;
    }
    vector<double> getLastRecord(map<int, map<int,double>>& record){
        vector<double> ret;

        //此处得到的数据是当前时间往前推10-5秒的结果，所以至少大10秒
        //此时若没有得到数据，说明响应时间较高，按最大响应时间计算
        if(sim_time < 10){
            return ret;
        }

        for(int time = sim_time - 10; time < sim_time-5; ++ time){
            if(record.count(time) <= 0){
                for(int i = 0; i < OGNUM; ++ i){
                    ret.push_back(sim_time - time);
                }
            }
            else{
                map<int, double> rt = record[time];
                for(int i = 0; i < OGNUM; ++ i){
                    ret.push_back(rt[i]);
                }
            }

        }
        return ret;
    }



     virtual void initialize() override;
     virtual void handleMessage(cMessage *msg) override;
     virtual void finish() override;

     void clear_queue(){

     }
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
