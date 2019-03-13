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

#include "EventStorage.h"

Define_Module(EventStorage);

void EventStorage::finish(){
    eedStats.record();

    /*for(int i = 0; i < OGNUM; i ++){
        vector<OperatorModel*> ops = opm->getOperatorGraph(i)->getOperatorModel();
        out << "operator graph:" << i << endl;
        for(int j = 0; j < ops.size(); j ++){
            vector<int> historical = ops[j]->getHistoricalEventNum();
            out << "op = " << j << endl;
            for(int k = 0; k < historical.size(); k ++){
                out << "time = " << k << ":" << historical[k] << endl;
            }
            out << "-------------" << endl;
        }
        out << "=============================" << endl;

    }*/

    //----------------result output---------------------
   //-----------event number record -----------------
    //vector<double> throughputs;
    vector<int> sendTime;
    for(int i = 0; i < OGNUM; i ++){
        //throughputs.push_back(0);
        sendTime.push_back(0);
    }

//  for(int i = 0; i < eventsRecord.size(); i ++){
//      map<int, int > record = eventsRecord[i];
//     // out << "time = " << i << endl;
//      for(int j = 0; j < OGNUM; j ++){
//         // out << "-- app " << j << " sent " << record[j] << "events"  << endl;
//          if(record[j] > 0){
//              //throughputs[j] = throughputs[j] + record[j];
//              sendTime[j] = sendTime[j] + 1;
//          }
//      }
//      out << "-----------------input_rate-------------------" << endl;
//  }
   out << "-----------------input_rate-------------------" << endl;
   out << eventsRecord.size() << endl;
   for(int i = 0; i < eventsRecord.size(); i ++){
        map<int, int > record = eventsRecord[i];
        int num = 0;
        for(int j = 0; j < OGNUM; j ++){
            num += record[j];
            out <<  record[j] << " "  ;
            if(record[j] > 0){
                sendTime[j] + 1;
            }

        }
        out << num << endl;
   }

  double tnow = simTime().dbl();
  //---------------total algorithm time---------------
  out << "------------------algorithm time----------------------" << endl;
  double avg_alg_time = 0;
  for(int i = 0; i < algorithm_time.size(); i ++){
      avg_alg_time += algorithm_time[i];
      out << "algorithm_time " << i << " = " << algorithm_time[i] << endl;
  }
  avg_alg_time /= algorithm_time.size();
  out << "avg algorithm time = " << avg_alg_time << endl;
  out << "-------------------------------------------------" << endl;

   //---------------total process time --------------
   out << "total time = " << tnow << endl;
   for(int i = 0; i < OGNUM; i ++){
       out  << "app " << i << " total events number " << totalNumSent[i] << ", throughput " << double(totalNumSent[i])/double(sendTime[i]) << endl;
   }
   out << "process time = " << processTime << " -- avg = " << processTime/tnow << endl;

   //-------------network usage output-----------
   out << "total network usage = " << totalNetworkUsage << endl;
   double netUseAvg = 0.0;
   double managerUse = 0.0;
   for(int i = 0; i < 0 + sim_time; i ++){
       if(networkUsage.count(i) > 0){
           out  << networkUsage[i] ;
           netUseAvg += networkUsage[i];
       }
       if(managerNetworkUsage.count(i) > 0){
           managerUse += managerNetworkUsage[i];
           //out << " manager net use -- " << managerNetworkUsage[i] ;
       }
       out << endl;
   }
   netUseAvg /= networkUsage.size();
   out << "network usage average = " << netUseAvg << endl;
   out << "manager usage = " << managerUse << endl;


   out << "------------event number record---------------" << endl;
   printRecord(0, eventNumber_record , out);
   //---------response time record-------------------------
   out << "---------response time---------------" << endl;
   printRecord(0, response_time_record, out);

   out << "---------round avg response time----------" << endl;
   avgRecordCal(response_time_record, avg_response_time_record);
   for(int i = 0; i < avg_response_time_record.size(); ++ i){
       out << avg_response_time_record[i] << endl;
   }

   out << "-----------round end sim time-------------" << endl;
   for(int i = 0; i < roundTimeRecord.size(); ++ i){
       out << roundTimeRecord[i] << endl;
   }
   //out << "---------predicted response time----------" << endl;
   //printRecord(predicted_response_time_record);

   out << "---------process_time_record---------------" << endl;
   printRecord(0, process_time_record, out);

   out << "---------transmission_time_record---------------" << endl;
   printRecord(0, transmission_time_record, out);

   out << "---------queue_time_record---------------" << endl;
   printRecord(0, queue_time_record, out);

   out << "------------------END-----------------------" <<endl;

   map<int, vector<int>>::iterator it = edgeCepMap.begin();
   while(it != edgeCepMap.end()){
       out << "H = " << opm[it->first]->getH() << endl;
       it ++;
   }

   out << "sim_time = " << sim_time << endl;

   out.flush();

   if(strategy == 0 && algorithm >= 2){
       char mc_filename[1024];
       sprintf(mc_filename, "%d_t%d", algorithm, TOTALSENDTIME);
       opm[intensiveNodeID]->RL_output(mc_filename);
   }

}

//set normal message
EventPacket* EventStorage::setEventPacket(vehicleStatus* vehs, string destAddrs, int port,int time, int app_num, bool used, vector<OperatorModel*> operatorType){
    EventPacket* event = new EventPacket();
    event->setMarkerMessage(false);
    event->setMonitorFlag(false);
    event->setMonitorMessage(false);
    event->setReMarkerMessage(false);
    event->setPlacementMessage(false);
    event->setUsed(used);

    event->setTime(time);
    event->setVehicleID(std::atoi(vehs->getVehicleID().c_str()));
    event->setSpeed(vehs->getSpeed());
    event->setLaneID(vehs->getLaneID().c_str());
    event->setAppNum(app_num);

    //set time record
    event->setHopCount(1);
    //set operator type array
    for(int i = operatorType.size()-1; i >= 0; i --){//for(int i = 0; i < operatorType.size(); i ++){
        const char* name = operatorType[i]->getOperatorID().c_str();
        int opType = 0;
        if(strcmp(name,"es") != 0){
            opType = atoi(name)+1;
        }
        event->setOperatorType(operatorType.size()-1-i,opType);

    }

    //calculate destAddress according placement algorithm
    //int destAddress = destAddresses[intuniform(0, destAddresses.size()-1)];
    std::vector<int> destAddresses = cStringTokenizer(destAddrs.c_str()).asIntVector();
    char pkname[40];
    sprintf(pkname, "pk-%d-to-%d-#%d", myAddress, destAddress[app_num], pkCounter++);
    //EV << "generating packet " << pkname << endl;


    //set destAddress
    event->setByteLength(packetLengthBytes->longValue());
    event->setSrcAddr(destAddresses[0]);
    event->setDestAddr(destAddresses[1]);
    event->setDestAddrs(destAddrs.c_str());
    event->setPort(port);

    return event;
}

//set marker message
EventPacket* EventStorage::setEventMarker(string destAddrs, int port, int time,  int app_num, bool used, vector<OperatorModel*> operatorType){
    EventPacket* event = new EventPacket();

    //set flags
    event->setMarkerMessage(true);
    event->setMonitorFlag(false);
    event->setMonitorMessage(false);
    event->setReMarkerMessage(false);
    event->setPlacementMessage(false);
    event->setMarkerID(time);


    //set time record
    event->setSendTime(simTime().dbl());
    event->setProcessTime(0.0);
    event->setQueueTime(0.0);
    event->setTransmissionTime(0.0);
    event->setTransmissionBeginTime(simTime().dbl());
    event->setQueueBeginTime(simTime().dbl());

//set event message
    event->setTime(time);
    event->setAppNum(app_num);
    event->setUsed(used);
    //calculate destAddress according placement algorithm
    //int destAddress = destAddresses[intuniform(0, destAddresses.size()-1)];

    std::vector<int> destAddresses = cStringTokenizer(destAddrs.c_str()).asIntVector();

    char pkname[40];
    sprintf(pkname, "marker-%d-to-%d-#%d", myAddress, destAddresses[1], pkCounter++);
    //EV << "generating packet " << pkname << endl;

    //set operator type array
    for(int i = operatorType.size()-1; i >= 0; i --){//operatorType.size(); i ++){
        const char* name = operatorType[i]->getOperatorID().c_str();
        int opType = 0;
        if(strcmp(name,"es") != 0){
            opType = atoi(name)+1;
        }
        event->setOperatorType(operatorType.size()-1-i,opType);
    }

    //set destAddress
    event->setByteLength(packetLengthBytes->longValue());
    event->setSrcAddr(destAddresses[0]);
    event->setDestAddr(destAddresses[1]);
    event->setDestAddrs(destAddrs.c_str());
    event->setPort(port);
    event->setHopCount(1);

    return event;
}



void EventStorage::sendPlacementPacket(int destAddr, int baseNode, OperatorPlacementManager* nodeOpm){
    map<int, Placement> placementM;

    //get all placement message
    vector<int> ogvec = edgeCepMap[baseNode];
    for(int ogIndex = 0; ogIndex < ogvec.size(); ogIndex ++){
        if(!replace[ogvec[ogIndex]]){
            continue;
        }
       OperatorGraphModel* ogm = nodeOpm->getOperatorGraph(ogvec[ogIndex]);
       vector<OperatorModel*> ops = ogm->getOperatorModel();

       for(int opIndex = 1; opIndex < ops.size(); opIndex ++){
           if(ops[opIndex]->getFogNode() != NULL ){
               int nodeId = ops[opIndex]->getFogNode()->getNodeID();
               Placement p ;
               p.placementNum = 0;
               if(placementM.count(nodeId) > 0){
                   p = placementM[nodeId];
               }
               int pnum = p.placementNum;
               p.OperatorGraphIndex[pnum] = ogvec[ogIndex];
               p.operatorID[pnum] = opIndex;
               p.placementNum ++;
               placementM[nodeId] = p;
           }
       }
   }

    //set placement message
    map<int, Placement>::iterator pit = placementM.begin();
    while(pit != placementM.end()){
        EventPacket* placement_message = new EventPacket();
        placement_message->setMarkerMessage(false);
        placement_message->setMonitorFlag(false);
        placement_message->setReMarkerMessage(false);

        placement_message->setPlacementMessage(true);
        placement_message->setPlacement(pit->second);
        placement_message->setSrcAddr(destAddr);
        placement_message->setDestAddr(destAddr);

        cGate* outGate = gateHalf("gate" , cGate::OUTPUT, pit->first);
        send(placement_message,outGate);

        pit ++;

    }

}


void EventStorage::init(){
    app_num = 0;

    placementIndex = 0;
    // TODO - Generated method body
    //temp = -1;

    // TODO - Generated method body
    //get sumo data
    net = new NetInputHandle("D:\\study\\Placement_of_MCEP\\sumoData\\map.net.xml");
    trafficmap = net->getMap();
    sumotr = new SumotrInputHandle("D:\\study\\Placement_of_MCEP\\sumoData\\map.sumo.tr", trafficmap);


    //get traffic logs
    trafficLogs = sumotr->getTrafficLogs();

    EV << "Event Storage Initial Finish" << endl;

    /*for(int i = 0; i < OGNUM;i ++){
        s_or_l.push_back(true);
        smarker.push_back(true);
        lmarker.push_back(true);
    }*/
    for(int i = 0; i < OGNUM; i ++){
        time.push_back(sumoStart);
        timeEnd.push_back(sumoEnd);
    }

    for(int i = 0; i < OGNUM; i ++){
        numSent.push_back(0);
        totalNumSent.push_back(0);
        replace.push_back(true);

        app_active.push_back(timestamp >= time[i]);
    }



    //set UpdateStrategies
      UpdateStrategies update;
      update.updateStrategies = 2;
      update.temporal = 1;
      update.distance = 1;
      update.x = 1000000;
      update.y = 1000000;
      update.isUpdate = true;
      update.updateMeters = 1;
      update.updateTemporal = 1;
      double timeSpan = 300;
      //string consumerID = "0";
      double spatial = 500;

      vehicleStatus* csmStatus = NULL;

      for(int i = 0; i < OGNUM; i ++){
          vehicles.push_back(trafficLogs->getVehicles((double)time[i],update, timeSpan, vehicleIDs[i], spatial, &csmStatus));
      }


      simtime_t sendTime;

      map<int, OperatorPlacementManager*>::iterator it = opm.begin();
      while(it != opm.end()){
      //for(int i = 0; i < opm.size(); i ++){
          EventPacket* monitorFlag = new EventPacket();
          monitorFlag->setMonitorFlag(true);
          monitorFlag->setMonitorH(opm[it->first]->getH());
          monitorFlag->setSrcAddr(it->first);
          monitorFlag->setDestAddr(it->first);
          opm[it->first]->setMonitorId(opm[it->first]->getMonitorId()+1);
          opm[it->first]->setMonitorIncrease(true);

          cGate* outGate = gateHalf("gate" , cGate::OUTPUT, opm[it->first]->getES()->getNodeID());
          send(monitorFlag, outGate);
          sendTime = monitorFlag->getArrivalTime();

          it ++;
      }

      SENDMESSAGE = new cMessage("start");

      sim_time = 0;
      simtime = simTime();
      monitorTime = simTime();


      scheduleAt(sendTime+5, SENDMESSAGE);
}
//处理数据
void EventStorage::processMessage(cMessage* msg){
    app_num = (app_num + 1)% OGNUM;

    //only the time of event in the application request can be sent
    while(timestamp != time[app_num] && !app_active[app_num]){
        app_num = (app_num + 1)% OGNUM;
    }



    //sumo数据集当前时间点还有数据，并且模拟时间在时间范围中
    while(vehicles[app_num]->empty() && time[app_num] < timeEnd[app_num]){

        //set UpdateStrategies
          time[app_num] = time[app_num]+1;
          /*destAddress[0] = rand() % destAddresses[1];
          opm->resetES(destAddress[0]);*/

          //whether to increase time stamp
          bool timeIncrease = true;
          for(int i = 0; i < OGNUM; i ++){
              if(time[i] == timestamp ){//&& time[i] < timeEnd[i]){
                  timeIncrease = false;
              }
          }

          //所有的operator graph的数据集时间都到达下一个时间点
          if(timeIncrease){
              timestamp ++;

              //一轮数据集结束
              if(timestamp >= sumoEnd){
                  //输出本轮record
                  outputRecord();
                  vector<int> type = this->type;
                  //根据测试模式，改变参数
                  switch(test_type){
                  case 0:
                      //10组3个operator, 30组4个operator，60组5个operator

                      if(sendTime[app_num] > 10){
                          type = {4,4,4};

                      }
                      else if(sendTime[app_num] > 40){
                          type = {5,5,5};
                      }
                      sendDelay = initial_send_delay;
                      opm[intensiveNodeID]->resetOG(type);
                      break;
                  case 1:
                      intensiveNodeID = (intensiveNodeID + 1) % 10;
                      break;
                  case 2:
                      break;

                  default:
                      break;
                  }

                  //record the end time of this round
                  roundTimeRecord.push_back(sim_time);

                  replacement_decision();

//                  if(TOTALSENDTIME > 1){
//                      queue.clear();
//                  }
                  for(int i = 0; i < sendTime.size(); ++ i){
                      if(sendTime[i] < TOTALSENDTIME){
                          timestamp = sumoStart;

                          break;
                      }
                  }
                  if(strategy == 0 && algorithm >= 2 ){
                      opm[intensiveNodeID]->RL_increase_round_time();
                  }
              }

          }

              //********************************************
          UpdateStrategies update;
          update.updateStrategies = 2;
          update.temporal = 1;
          update.distance = 1;
          update.x = 1000000;
          update.y = 1000000;
          update.isUpdate = true;
          update.updateMeters = 1;
          update.updateTemporal = 1;
          double timeSpan = 300;
          //string consumerID = "0";
          double spatial = 300;

          vehicleStatus* csmStatus = NULL;

          vehicles[app_num] = trafficLogs->getVehicles((double)time[app_num],update, timeSpan, vehicleIDs[app_num], spatial, &csmStatus);

          //EV << "time:" << time[app_num] << endl;

          //send(setMarker( destAddress, 0, time), "out", destAddress);
          //send(setMarker( destAddress, 1, time), "out", destAddress);
    }
    string destAddrs;

   if(time[app_num] < timeEnd[app_num]){
       if(queue.getLength() > 10000){
           return;
       }
       vehicleStatus* vehs = (*vehicles[app_num])[vehicles[app_num]->size()-1];



       //simtime_t endTransmission = simTime();

       bool used = true;
       //if(s_or_l[app_num]){
           //cGate* outGate = gateHalf("gate" , cGate::OUTPUT, destAddress[0]);


       OperatorGraphModel* og = opm[destAddress[app_num]]->getOperatorGraph(app_num);
       vector<StreamPath*> paths = og->getStreamPath();


       for(int i = 0; i < paths.size(); i ++){
           destAddrs = getDestAddrs(app_num, i, used);
           StreamPath* path = paths[i];

           EventPacket* pk = setEventPacket(vehs,destAddrs, i, time[app_num], app_num, used,path->getOperators());

           queue.insert(pk);
       }


       vehicles[app_num]->pop_back();
   }
   else{
       if(time[app_num] == timeEnd[app_num]){
           if(sendTime[app_num] >= TOTALSENDTIME){
               time[app_num] = time[app_num]+1;
               app_active[app_num] = false;
               sendTime[app_num] ++;


              //***********send monitor message**************************
              for(int i = 0; i < destAddresses[1]; i ++){
                  EventPacket* monitorFlag = new EventPacket();
                  monitorFlag->setDestAddr(i);
                  monitorFlag->setSrcAddr(i);
                  monitorFlag->setMonitorFlag(true);
                  monitorFlag->setMonitorH(-1);
                  //monitorFlag->setSrcAddr(i);
                  //cGate* outGate = gateHalf("gate" , cGate::OUTPUT, i);
                  //send(monitorFlag, outGate);
                  queue.insert(monitorFlag);
              }
              return;
           }

           time[app_num] = sumoStart;
           sendTime[app_num] ++;

       }
   }

}
void EventStorage::initialize()
{
    eedStats.setName("End-to-End Delay");
    endToEndDelayVec.setName("End-to-End Delay");



    numReceived = 0;

    timestamp = TIME_INIT;

    //fsm.setName("fsm");
    networkSize = par("size").longValue();

    destAddresses = cStringTokenizer(par("destAddresses").stdstringValue().c_str()).asIntVector();


    //************set the event storage location *********
    for(int i = 0; i < intensiveAddrNum; i ++){
        srand(i);
        intensiveAddr.push_back(rand()%destAddresses[1]);
    }
    intensiveAddr[0] = intensiveNodeID;

    for(int i = 0; i < OGNUM; i ++){
        srand(i);
        //destAddress.push_back(2);
        destAddress.push_back(intensiveAddr[rand()%intensiveAddrNum]);//destAddresses[intuniform(0, destAddresses.size()-1)];
        vector<int> ogmap;
        if(edgeCepMap.count(destAddress[i]) > 0){
            ogmap = edgeCepMap[destAddress[i]];
        }
        ogmap.push_back(i);
        edgeCepMap[destAddress[i]] = ogmap;

    }

    map<int, vector<int>>::iterator it = edgeCepMap.begin();
    while(it != edgeCepMap.end()){

        FogNode* es = new FogNode(it->first);
        vector<int> ogvec = edgeCepMap[it->first];
        vector<double> rtc;
        vector<int> ognum;
        for(int vecIndex = 0; vecIndex < ogvec.size(); vecIndex ++){
            rtc.push_back(RT_CONSTRAINTS[ogvec[vecIndex]]);
            ognum.push_back(type[ogvec[vecIndex]]);
        }

        OperatorPlacementManager* opm_i = new OperatorPlacementManager(es, rtc, ognum,it->second);
        opm[it->first] = opm_i;

        it ++;
    }
    //destAddress[0] = 1;

    //for(int i = 0; i < opm.size(); i ++){
    map<int, OperatorPlacementManager*>::iterator opmIt = opm.begin();
    while(opmIt != opm.end()){
        out << "edge node id = " << opmIt->first << endl;
        for(int ogIndex = 0; ogIndex < opm[opmIt->first]->getOperatorGraphNum(); ogIndex ++){
            opm[opmIt->first]->getOperatorGraph(edgeCepMap[opmIt->first][ogIndex])->printPaths(out);
        }
        opmIt ++;
    }

    //***********send monitor message**************************
    //EventPacket* monitorFlag = new EventPacket();
    //monitorFlag->setMonitorFlag(true);
    //monitorFlag->setMonitorH(opm->getH());

    //cGate* outGate = gateHalf("gate" , cGate::OUTPUT, destAddress[0]);
    //send(monitorFlag, outGate);


    //********************************************

    //open result file
        char filename[1024];
        memset(filename, 0 , 1024);
        sprintf(filename, "result_c5_n3_og%d_r%d_%d_%d_dr%d_%d_%d_mt%d_e%d_sam%d.txt",type[0], opm[7]->getOperatorGraph(0)->randSeed ,  strategy, algorithm, sendDelayType, poisson_lambda, TOTALSENDTIME, rl_type,EPSILON_TYPE,STATE_ACTION_MODEL);
        out.open(filename, ios::out);



    myAddress = par("address").longValue();
    //sleepTime = &par("sleepTime");
    //burstTime = &par("burstTime");
    //sendIATime = &par("sendIaTime");
    packetLengthBytes = &par("packetLength");

    endToEndDelaySignal = registerSignal("endToEndDelay");
    hopCountSignal = registerSignal("hopCount");
    sourceAddressSignal = registerSignal("sourceAddress");

    totalNetworkUsage = 0.0;


    pkCounter = 0;
    WATCH(pkCounter);  // always put watches in initialize(), NEVER in handleMessage()
    //startStopBurst = new cMessage("startStopBurst");
    //sendMessage = new cMessage("sendMessage");
    init();
}



void EventStorage::handleMessage(cMessage *msg)
{
    simtime_t tnow = simTime();
    clock_t process_begin = clock();
    clock_t process_end = clock();


//manager part
    if(tnow - simtime >= 1){
        map<int, int> record;

        for(int i = 0; i < OGNUM; i ++){
            EV_INFO  << "app " << i << " sent " << numSent[i] << "events." << endl;

            //update the event number record
            updateRecord(eventNumber_record, sim_time, i,  numSent[i]);

            record[i] = numSent[i];
            numSent[i] = 0;
            simtime = tnow;

            //send marker message
            string destAddrs;
            bool used = true;

            OperatorGraphModel* og = opm[destAddress[i]]->getOperatorGraph(i);
            vector<StreamPath*> paths = og->getStreamPath();
            int size = og->getStreamPathSize();
            for(int pindex = 0; pindex < size; pindex ++){
                destAddrs = getDestAddrs(i, pindex, used);
                StreamPath* path = paths[pindex];
                EventPacket* pkl = setEventMarker(destAddrs, pindex, sim_time, i, used, path->getOperators());

                //queue.insert(pkl);
                int nodeIndex = pkl->getSrcAddr();
                cGate* outGate = gateHalf("gate" , cGate::OUTPUT, nodeIndex);
                pkl->setSendTime(simTime().dbl());
                double msg_processTime = (double)(process_end-process_begin);
                processTime += msg_processTime;
                pkl->setProcessTime(msg_processTime);
                pkl->setTransmissionBeginTime(simTime().dbl());
                send(pkl, outGate);
            }

            /*destAddrs = getDestAddrs(i, 1, used);
            EventPacket* pkr = setEventMarker(destAddrs, 1, sim_time, used);
            queue.insert(pkr);
                 */
        }
        switch(sendDelayType){
        case 0:{
            int randNum = rand() % 100000;
            double total_probability = 0;
            for(int k = 1; k < 2.5 * poisson_lambda; ++ k){
                double p = pow(poisson_lambda, k) / fac(k) * exp(-poisson_lambda);
                total_probability += p;
                if(randNum < p*100000){
                    sendDelay = k * 100;
                }
            }

            break;
        }
        case 1:{
            if(sendDelay >  6000){
                //sendDelayType = 2;
                break;
            }
            sendDelay += delayChange;
            break;
        }
        case 2:{
            if(sendDelay < 1000){
                //sendDelayType = 1;
                break;
            }
            sendDelay -= delayChange;
            break;
        }
        default:
            break;
        }
        //
        eventsRecord.push_back(record);

//        for(int appIndex = 0; appIndex < appNum; ++ appIndex){
//            opm[intensiveNodeID]->updateEventNumber(app_num, operatorType, time, eventNum)
//        }


        sim_time ++;

    }


    if(tnow - monitorTime >= monitor_interval ){
        //set monitor interval
        if(first){
            if(strategy == 0 && algorithm >= 2 && rl_type == 0){
                monitor_interval = 1;
            }
            else{
                monitor_interval = 5;
            }
        }
        //timeMarker[app_num] = time[app_num] + 5;

        //***********send monitor message**************************
        map<int, OperatorPlacementManager*>::iterator opmIt = opm.begin();
        while(opmIt != opm.end()){
            EventPacket* monitorFlag = new EventPacket();

            monitorFlag->setSrcAddr(opmIt->first);
            monitorFlag->setDestAddr(opmIt->first);

            monitorFlag->setMonitorFlag(true);
            monitorFlag->setMonitorH(opmIt->second->getH());
            monitorFlag->setMonitorID(opmIt->second->getMonitorId());

            opmIt->second->setMonitorId(opmIt->second->getMonitorId()+1);
            opmIt->second->setMonitorIncrease(true);

            //cGate* outGate = gateHalf("gate" , cGate::OUTPUT, destAddress[0]);
            //simtime_t endTransmission = outGate->getTransmissionChannel()->getTransmissionFinishTime();
            //if(endTransmission < tnow){
            //    send(monitorFlag, outGate);
            //}
            //else{
             //   sendDelayed(monitorFlag, endTransmission, outGate);
            //}

            //queue.insert(monitorFlag);

           //--------------------算法---------------------
                           //-----greedy multiple operator graph placement---------------
            int nodeIndex = intensiveNodeID;
            sendPlacementPacket(-1,nodeIndex,opm[nodeIndex]);
            clock_t begin = clock();


            vector<int> node_capacity;
            vector<double> input_rate;
            vector<double> response_time;
            if(strategy == 0 && algorithm >= 2){
                opm[nodeIndex]->RL_update_parameter();
                node_capacity = opm[nodeIndex]->getFogNodeCapacity();
                input_rate = getLastInputRate();
                response_time = getLastRecord(response_time_record);
            }


            switch(strategy){
            case 0:
                if(algorithm == 0){
                    placement[nodeIndex] = opm[nodeIndex]->getReMultiOperatorGraphPlacement(replace);
                    //newPlacement = opm->getReMultiOperatorGraphPlacement();
                }
                else if(algorithm == 1){
                    placement[nodeIndex] = opm[nodeIndex]->getReSAOperatorGraphPlacement(0.99, 100);
                }
                else if(algorithm == 2){

                    placement[nodeIndex] = opm[nodeIndex]->Monte_Carlo(node_capacity, input_rate, response_time);
                }
                else if(algorithm == 3){

                    placement[nodeIndex] = opm[nodeIndex]->Sarsa_TD(node_capacity, input_rate, response_time);
                }
                else if(algorithm == 4){

                    placement[nodeIndex] = opm[nodeIndex]->QLearning(node_capacity, input_rate, response_time);
                }
            break;
            case 1:
                if(algorithm == 0){
                    //------simple greedy(nearest) operator graph placement-------------
                    placement[nodeIndex] = opm[nodeIndex]->getSimpleGreedyPlacement(replace);
                    //newPlacement = opm->getSimpleGreedyPlacement();
                }
                else if(algorithm == 1){
                    //-------selfish operator graph placement-----------
                    placement[nodeIndex] = opm[nodeIndex]->getSelfishMultiOperatorPlacement();  //newPlacement = opm->getSelfishMultiOperatorPlacement();
                }
                else if(algorithm == 2){
                    placement[nodeIndex] = opm[nodeIndex]->getSelfishSAOperatorGraphPlacement(0.99,100);
                }
                else if(algorithm == 3){
                    placement[nodeIndex] = opm[nodeIndex]->getSelfResourceAwarePlacement();
                }
                else if(algorithm == 4){
                    placement[nodeIndex] = opm[nodeIndex]->getLoadBalance(1, 0.05, opm[nodeIndex]->getFogNodeNum(), replace);
                }
                else if(algorithm == 5){
                    placement[nodeIndex] = opm[nodeIndex]->getOveralGraphPlacement(replace);
                }
                else if(algorithm == 6){
                    placement[nodeIndex] = opm[nodeIndex]->getResponseTimeGreedyPlacement(replace);
                }
                break;
            case 2:
                break;

            }

            OperatorPlacementManager* opm_i = opm[nodeIndex];

            sendPlacementPacket(nodeIndex,nodeIndex, opm_i);

            clock_t end = clock();
            algorithm_time.push_back((double)(end - begin));

            opm[nodeIndex]->printPlacement(out);


            opmIt ++;
        }
        monitorTime = tnow;

    }

    double msg_processTime = 0.0;

//message handle part
    if(msg == SENDMESSAGE){

        bool process = false;
        for(int i = 0; i < OGNUM; i ++){
              if(timestamp == time[i] && time[i] < timeEnd[i]){
                  app_active[i] = true;
              }
              else{
                  app_active[i] = false;
              }
        }
        for(int i = 0; i < OGNUM; i ++){
            if(app_active[i]){
                process = true;
                break;
            }
        }
        if(process){
            processMessage(msg);
        }

        simtime_t endTransmission;

        int size = queue.getLength();
        if(!queue.isEmpty()){


            EventPacket* pk = (EventPacket*)queue.pop();
            //int ogIndex = pk->getAppNum();
            int nodeIndex = pk->getSrcAddr();
            if(!pk->getMonitorFlag()){
                int ogIndex = pk->getAppNum();

                numSent[ogIndex] = numSent[ogIndex] + 1;
                totalNumSent[ogIndex] = totalNumSent[ogIndex] + 1;
            }



            process_end = clock();
            cGate* outGate = gateHalf("gate" , cGate::OUTPUT, nodeIndex);
            pk->setSendTime(simTime().dbl());
            msg_processTime = (double)(process_end-process_begin);
            processTime += msg_processTime;
            pk->setProcessTime(msg_processTime);
            pk->setTransmissionBeginTime(simTime().dbl());
            send(pk, outGate);

            endTransmission = simTime();
            //endTransmission = outGate->getTransmissionChannel()->getTransmissionFinishTime();
            //if(endTransmission < simTime()){
            //    endTransmission = simTime();
            //}
        }
        else{
            endTransmission = simTime();
        }

        //end send events


        //if(endTransmission.dbl() - simTime().dbl() < sendDelay){
        //    endTransmission = simTime() + sendDelay;
        //}
       bool end = queue.isEmpty();

       for(int i = 0; i < sendTime.size(); ++i){
           if(sendTime[i] < TOTALSENDTIME ||(sendTime[i] == TOTALSENDTIME &&  time[i] < timeEnd[i])){
               end = false;
           }
       }

       if(!end){
           scheduleAt(endTransmission + 1.0 / sendDelay, SENDMESSAGE);
           return;
       }

    }
    else
    {
        //monitor message
        monitorMessageNum ++;

        EventPacket *monitor_message = check_and_cast<EventPacket *>(msg);

        //record network usage
        //int tid = monitor_message->getMonitorID();
        //double datarate =
        /*double datarate = msg->getSenderGate()->getTransmissionChannel()->getNominalDatarate();
        double transTime = simTime().dbl() - monitor_message->getTransmissionBeginTime();
        double netuse = datarate * transTime;


        //totalNetworkUsage += netuse;

        if(networkUsage.count(tid) > 0){
            networkUsage[tid] = networkUsage[tid] + netuse;
        }
        else{
            networkUsage[tid] = netuse;
        }
        if(managerNetworkUsage.count(tid) > 0){
            managerNetworkUsage[tid] = managerNetworkUsage[tid] + netuse;
        }
        else{
            managerNetworkUsage[tid] = netuse;
        }*/
        if(monitor_message->getPlacementMessage()){
            int nodeIndex = monitor_message->getDestAddr();
            opm[monitor_message->getDestAddr()]->updateCapacity(monitor_message->getSrcAddr(),monitor_message->getPlacement().placementNum);
            clock_t begin = clock();

            vector<int> node_capacity;
              vector<double> input_rate;
              vector<double> response_time;
              if(strategy == 0 && algorithm >= 2){
                  opm[nodeIndex]->RL_update_parameter();
                  node_capacity = opm[nodeIndex]->getFogNodeCapacity();
                  input_rate = getLastInputRate();
                  response_time = getLastRecord(response_time_record);
              }


            switch(strategy){
            case 0:
                if(algorithm == 0){
                    placement[nodeIndex] = opm[nodeIndex]->getReMultiOperatorGraphPlacement(replace);
                    //newPlacement = opm->getReMultiOperatorGraphPlacement();
                }
                else if(algorithm == 1){
                    placement[nodeIndex] = opm[nodeIndex]->getReSAOperatorGraphPlacement(0.99, 100);
                }
                else if(algorithm == 2){

                   placement[nodeIndex] = opm[nodeIndex]->Monte_Carlo(node_capacity, input_rate, response_time);
               }
                else if(algorithm == 3){

                     placement[nodeIndex] = opm[nodeIndex]->Sarsa_TD(node_capacity, input_rate, response_time);
                }
                else if(algorithm == 4){

                    placement[nodeIndex] = opm[nodeIndex]->QLearning(node_capacity, input_rate, response_time);
                }
            break;
            case 1:
                if(algorithm == 0){
                    //------simple greedy(nearest) operator graph placement-------------
                    placement[nodeIndex] = opm[nodeIndex]->getSimpleGreedyPlacement(replace);
                    //newPlacement = opm->getSimpleGreedyPlacement();
                }
                else if(algorithm == 1){
                    //-------selfish operator graph placement-----------
                    placement[nodeIndex] = opm[nodeIndex]->getSelfishMultiOperatorPlacement();  //newPlacement = opm->getSelfishMultiOperatorPlacement();
                }
                else if(algorithm == 2){
                    placement[nodeIndex] = opm[nodeIndex]->getSelfishSAOperatorGraphPlacement(0.99,100);
                }
                else if(algorithm == 3){
                    placement[nodeIndex] = opm[nodeIndex]->getSelfResourceAwarePlacement();
                }
                else if(algorithm == 4){
                    placement[nodeIndex] = opm[nodeIndex]->getLoadBalance(1, 0.05, opm[nodeIndex]->getFogNodeNum(), replace);
                }
                else if(algorithm == 5){
                    placement[nodeIndex] = opm[nodeIndex]->getOveralGraphPlacement(replace);
                }
                else if(algorithm == 6){
                    placement[nodeIndex] = opm[nodeIndex]->getResponseTimeGreedyPlacement(replace);
                }
                break;
            case 2:
                break;

            }

            OperatorPlacementManager* opm_i = opm[nodeIndex];
            sendPlacementPacket(nodeIndex,nodeIndex, opm_i);

            map<int, OperatorPlacementManager*>::iterator opmIt = opm.begin();

//            while(opmIt != opm.end()){
//
//                out << "edge node id = " << opmIt->first << endl;
//
//                OperatorPlacementManager* opm_i = opmIt->second;
//
//                vector<int> ogvec = edgeCepMap[opmIt->first];
//                for(int i = 0; i < ogvec.size(); i ++){
//                    OperatorGraphModel* ogm = opm_i->getOperatorGraph(ogvec[i]);
//
//                    double prt = ogm->getPredictedResponseTime();
//                    out << "ogindex = " << ogvec[i] << " -- prt: " << prt  << " -- rt: "<< ogm->getResponseTime() << endl;
//                    //vector<OperatorModel*> ops =  ogm->getOperatorModel();
//                }
//
//                opmIt ++;
//            }

            clock_t end = clock();
            algorithm_time.push_back((double)(end - begin));

            opm[nodeIndex]->printPlacement(out);

        }
        else if(monitor_message->getMonitorMessage()){

            EV << "monitor message from " << monitor_message->getSrcAddr() << endl;


            int nodeIndex = monitor_message->getDestAddr();
            NodeMessage nodeMessage = monitor_message->getFognode();


            //update monitor message
            opm[nodeIndex]->updateFogNode(nodeMessage);
            //placement = opm->getPlacement();

        }
        else if(monitor_message->getReMarkerMessage()){
            int tid = monitor_message->getMarkerID();
            int ogIndex = monitor_message->getAppNum();
            int nodeIndex = destAddress[ogIndex];
            opm[nodeIndex]->updateEventNumber(monitor_message->getAppNum(),
                    monitor_message->getOperatorType(monitor_message->getHopCount()-1),
                    tid, monitor_message->getEventNum());
            //out << "time: " << tid << ", app:" << ogIndex << "event num: " << monitor_message->getEventNum() << endl;
            double response_time = tnow.dbl() - monitor_message->getSendTime();

            //int tid = monitor_message->getTime();
            int appIndex = monitor_message->getAppNum();

            /*out << "tid = " << tid << " -- app " << appIndex << " -- rt = " << response_time
                    << " -- pt = " << monitor_message->getProcessTime()
                    << " -- qt = " << monitor_message->getQueueTime()
                    << " -- tt = " << monitor_message->getTransmissionTime() << endl;
            */


            //int tid = monitor_message->getMarkerID();
            totalNetworkUsage +=  monitor_message->getNetworkUsage();
            if(networkUsage.count(tid) > 0){
                networkUsage[tid] = networkUsage[tid] + monitor_message->getNetworkUsage();
            }
            else{
                networkUsage[tid] =  monitor_message->getNetworkUsage();
            }

            if(managerNetworkUsage.count(tid) > 0){
                managerNetworkUsage[tid] = managerNetworkUsage[tid] + monitor_message->getManagerNetworkUsage();
            }
            else{
                managerNetworkUsage[tid] = monitor_message->getManagerNetworkUsage();
            }
            if(monitor_message->getDestAddr() == -1){


                OperatorGraphModel* og = opm[nodeIndex]->getOperatorGraph(appIndex);

                og->setResponseTime(response_time);
                //record response time
                updateRecord(response_time_record,tid, appIndex, response_time);
//                if(response_time > RT_MAX_CONSTRAINTS[appIndex]){
//                    queue.clear();
//                }

                //update process_time record
                updateRecord(process_time_record, tid, appIndex, monitor_message->getProcessTime());

                //update queue_time_record
                updateRecord(queue_time_record, tid, appIndex, monitor_message->getQueueTime());

                //update transmission time record
                updateRecord(transmission_time_record, tid, appIndex, monitor_message->getTransmissionTime());



                opm[nodeIndex]->calOperatorGraphResponseTime();
                updateRecord(predicted_response_time_record, tid, appIndex, og->getPredictedResponseTime());

                //EV_INFO  << "app_num " << app_num << ":" << "- response time = " << response_time << " in " << time[app_num] << endl;
                //out << "app_num " << app_num << ":" << timestamp << "- response time = " <<  response_time << " , predicted response time = " << og->getPredictedResponseTime() << endl;
                //out << "process time = " << monitor_message->getProcessTime() << ". transmission time = " << monitor_message->getTransmissionTime() << ". QueueTime = " << monitor_message->getQueueTime() << endl;

                endToEndDelayVec.record(response_time);
                eedStats.collect(response_time);


                opm[nodeIndex]->updateResponsetime(monitor_message->getAppNum(), response_time);

            }
        }
        delete msg;


    }
}




//get the destination addresses
std::string EventStorage::getDestAddrs(int app_num, int pathIndex, bool& used){
    string destAddrs;
    std::stringstream sstr2;

    int nodeIndex = destAddress[app_num];

    int appIndex = -1;
    vector<int> ogvec = edgeCepMap[nodeIndex];
    for(int i = 0; i < ogvec.size(); i ++){
        if(ogvec[i] == app_num){
            appIndex = i;
            break;
        }
    }

    if(appIndex != -1 && placement.count(nodeIndex) > 0 && placement[nodeIndex][appIndex].size() > 0 && opm[nodeIndex]->isOperatorGraphPlaced(appIndex)){
        vector<OperatorModel*> ops = placement[nodeIndex][appIndex][pathIndex]->getOperators();
        for(int i = ops.size()-1; i >= 0; i --){
            sstr2 << ops[i]->getFogNode()->getNodeID() << " ";
        }
    }
   //placement = opm->getPlacement();

    else{
        EV_INFO  << "random placement" << endl;
        for(int i = 0; i < 3; i ++){
           sstr2 << destAddress[app_num] << " ";
       }
        used = false;
    }




    destAddrs = sstr2.str() ;
    const char* temp = destAddrs.c_str();
    return destAddrs;
}

bool EventStorage::isAllPlaced(vector<StreamPath*> stream_paths){
    for(int i = 0; i < stream_paths.size(); i ++){
        if(!stream_paths[i]->isAllPlaced()){
            return false;
        }
    }
    return true;
}

//update record
void EventStorage::updateRecord(map<int,map<int, double>>& record, int tid, int appIndex, double time){
    //record response time

    if(record.count(tid) > 0){
        //there is a reocrd in response time
        map<int, double> time_record = record[tid];
        if(time_record.count(appIndex) <= 0 || time_record[appIndex] < time){
            //if(time_record[appIndex] < time){
            time_record[appIndex] = time;
            record[tid] = time_record;
            //}
        }

    }
    else{//create new record
        map<int, double> time_record;
        time_record[appIndex] = time;
        record[tid] = time_record;
    }
}

//calculate average record between last time period
void EventStorage::avgRecordCal(map<int, map<int, double>>& record, vector<double>& avgRecord){
    int roundTimeRecordSize = roundTimeRecord.size(), timeBegin = 0, timeEnd = 0;
    double avg = 0;
    int count = 0;
    if(roundTimeRecordSize < 1){
        return;
    }
    //get the first time and end time
    timeEnd = roundTimeRecord[0];
    for(int time = 0; time < roundTimeRecordSize; ++ time){
        avg = 0;
        count = 0;
        if(time != 0){
            timeBegin = roundTimeRecord[time-1];
        }
        timeEnd = roundTimeRecord[time];
        //plus total record
        for(int i = timeBegin; i < timeEnd; ++ i){
            map<int, double>::iterator mit = record[i].begin();
            while(mit != record[i].end()){
                avg += mit->second;
                ++ count;
                mit ++;
            }

        }
        //calculate average record
       avg /= (double) count;
       avgRecord.push_back(avg);

    }



}

//print record
void EventStorage::printRecord( int begin_time, map<int,map<int, double>>& record, ofstream& out){
    double avg = 0.0;
    vector<double> avg_time;
    vector<int> record_num;
    for(int i = 0; i < OGNUM; i ++){
        avg_time.push_back(0);
        record_num.push_back(0);
    }
    for(int i = begin_time; i < 0+sim_time; i ++){
        if(record.count(i) > 0){
            map<int, double> child_record = record[i];
            if(child_record.size() == 0){
                continue;
            }
            for(int j = 0; j < OGNUM; j ++){
                if(child_record.count(j) > 0){
                     out <<  child_record[j] << "\t";
                     avg += child_record[j];
                     avg_time[j] = avg_time[j] + child_record[j];
                     record_num[j] = record_num[j] + 1;
                }
            }
            out << endl;
        }
    }

    int totalNum = 0;
    for(int i = 0; i < OGNUM; i ++){
        out << "app " << i << " avg time = " << avg_time[i]/record_num[i] << endl;
        totalNum += record_num[i];
    }
    avg = avg/(double)totalNum;

    out << "avg = " << avg << endl;
}


void EventStorage::outputRecord(){
    ofstream out;
    //open result file
    char filename[1024];
    memset(filename, 0 , 1024);
    sprintf(filename, "result_c5_n3_og%d_r%d_%d_%d_dr%d_%d.txt",type[0], opm[intensiveNodeID]->getOperatorGraph(0)->randSeed ,  strategy, algorithm, sendDelayType, poisson_lambda);
    out.open(filename, ios::out);

    //----------------result output---------------------
   //-----------event number record -----------------
    //vector<double> throughputs;
    vector<int> sendTime;
    for(int i = 0; i < OGNUM; i ++){
        //throughputs.push_back(0);
        sendTime.push_back(0);
    }

   out << "-----------------input_rate-------------------" << endl;
   out << eventsRecord.size() << endl;
   for(int i = 0; i < eventsRecord.size(); i ++){
        map<int, int > record = eventsRecord[i];
        int num = 0;
        for(int j = 0; j < OGNUM; j ++){
            num += record[j];
            out <<  record[j] << " "  ;
            if(record[j] > 0){
                sendTime[j] + 1;
            }

        }
        out << num << endl;
   }

  double tnow = simTime().dbl();
  //---------------total algorithm time---------------
//  out << "------------------algorithm time----------------------" << endl;
//  double avg_alg_time = 0;
//  for(int i = 0; i < algorithm_time.size(); i ++){
//      avg_alg_time += algorithm_time[i];
//      out << "algorithm_time " << i << " = " << algorithm_time[i] << endl;
//  }
//  avg_alg_time /= algorithm_time.size();
//  out << "avg algorithm time = " << avg_alg_time << endl;
//  out << "-------------------------------------------------" << endl;

   //---------------total process time --------------
   out << "total time = " << tnow << endl;
   for(int i = 0; i < OGNUM; i ++){
       out  << "app " << i << " total events number " << totalNumSent[i] << ", throughput " << double(totalNumSent[i])/double(sendTime[i]) << endl;
   }
   out << "process time = " << processTime << " -- avg = " << processTime/tnow << endl;

   //-------------network usage output-----------
   out << "total network usage = " << totalNetworkUsage << endl;
   double netUseAvg = 0.0;
   double managerUse = 0.0;
   for(int i = 0; i < 0 + sim_time; i ++){
       if(networkUsage.count(i) > 0){
           out  << networkUsage[i] ;
           netUseAvg += networkUsage[i];
       }
       if(managerNetworkUsage.count(i) > 0){
           managerUse += managerNetworkUsage[i];
           //out << " manager net use -- " << managerNetworkUsage[i] ;
       }
       out << endl;
   }
   netUseAvg /= networkUsage.size();
   out << "network usage average = " << netUseAvg << endl;
   out << "manager usage = " << managerUse << endl;

   int beginTime = 0;
   if(roundTimeRecord.size() > 0){
       beginTime = roundTimeRecord[roundTimeRecord.size()-1];
   }

   out << "------------event number record---------------" << endl;
   printRecord( beginTime , eventNumber_record, out);
   //---------response time record-------------------------
   out << "---------response time---------------" << endl;
   printRecord(beginTime, response_time_record, out);


   //out << "---------predicted response time----------" << endl;
   //printRecord(predicted_response_time_record);

   out << "---------process_time_record---------------" << endl;
   printRecord(beginTime, process_time_record, out);

   out << "---------transmission_time_record---------------" << endl;
   printRecord(beginTime, transmission_time_record, out);

   out << "---------queue_time_record---------------" << endl;
   printRecord(beginTime, queue_time_record, out);

   out << "------------------END-----------------------" <<endl;

   map<int, vector<int>>::iterator it = edgeCepMap.begin();
   while(it != edgeCepMap.end()){
       out << "H = " << opm[it->first]->getH() << endl;
       it ++;
   }

   out << "sim_time = " << sim_time << endl;

   out.flush();

   out.close();
}

void EventStorage::replacement_decision(){
    int nodeIndex = intensiveNodeID;
   vector<int> node_capacity;
   vector<double> input_rate;
   vector<double> response_time;
   if(strategy == 0 && algorithm >= 2){
       opm[nodeIndex]->RL_update_parameter();
       node_capacity = opm[nodeIndex]->getFogNodeCapacity();
       input_rate = getLastInputRate();
       response_time = getLastRecord(response_time_record);
   }
    switch(strategy){
    case 0:
        if(algorithm == 0){
            placement[nodeIndex] = opm[nodeIndex]->getReMultiOperatorGraphPlacement(replace);
            //newPlacement = opm->getReMultiOperatorGraphPlacement();
        }
        else if(algorithm == 1){
            placement[nodeIndex] = opm[nodeIndex]->getReSAOperatorGraphPlacement(0.99, 100);
        }
        else if(algorithm == 2){

           placement[nodeIndex] = opm[nodeIndex]->Monte_Carlo(node_capacity, input_rate, response_time);
       }
        else if(algorithm == 3){

             placement[nodeIndex] = opm[nodeIndex]->Sarsa_TD(node_capacity, input_rate, response_time);
        }
        else if(algorithm == 4){

            placement[nodeIndex] = opm[nodeIndex]->QLearning(node_capacity, input_rate, response_time);
        }
    break;
    case 1:
        if(algorithm == 0){
            //------simple greedy(nearest) operator graph placement-------------
            placement[nodeIndex] = opm[nodeIndex]->getSimpleGreedyPlacement(replace);
            //newPlacement = opm->getSimpleGreedyPlacement();
        }
        else if(algorithm == 1){
            //-------selfish operator graph placement-----------
            placement[nodeIndex] = opm[nodeIndex]->getSelfishMultiOperatorPlacement();  //newPlacement = opm->getSelfishMultiOperatorPlacement();
        }
        else if(algorithm == 2){
            placement[nodeIndex] = opm[nodeIndex]->getSelfishSAOperatorGraphPlacement(0.99,100);
        }
        else if(algorithm == 3){
            placement[nodeIndex] = opm[nodeIndex]->getSelfResourceAwarePlacement();
        }
        else if(algorithm == 4){
            placement[nodeIndex] = opm[nodeIndex]->getLoadBalance(1, 0.05, opm[nodeIndex]->getFogNodeNum(), replace);
        }
        else if(algorithm == 5){
            placement[nodeIndex] = opm[nodeIndex]->getOveralGraphPlacement(replace);
        }
        else if(algorithm == 6){
            placement[nodeIndex] = opm[nodeIndex]->getResponseTimeGreedyPlacement(replace);
        }
        break;
    case 2:
        break;

    }
}
