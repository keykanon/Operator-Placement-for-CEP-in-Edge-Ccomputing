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

//
// TODO generated message class
//


struct NodeMessage{
 
  int nodeID;
  int capacity;
  
  int neighborNodeID[1024]; 
  int neighborCapacity[1024];
  
  int neighborNum;
  
  double transmission_rate[1024];
} ;

struct Placement{
    int placementNum;
  	int OperatorGraphIndex[128];
  	int operatorID[128];
  	  
};

packet EventPacket {
    bool placementMessage;
    Placement placement;
    
    NodeMessage fognode;
    int monitorID;
    bool monitorMessage;
    bool monitorFlag;
    int monitorH;
    
    bool used;
    string destAddrs;
    int srcAddr;
    int destAddr;
    int port;
    int appNum;
    
    
    int hopCount;
    
    bool markerMessage;
    double sendTime;
    double queueTime;
    double queueBeginTime;
    double transmissionTime;
    double transmissionBeginTime;
    double processTime;
    int markerID;

    double networkUsage;
    double managerNetworkUsage;
    
    int eventNum;
    int operatorType[100];
    bool reMarkerMessage;
    
	int time;
	int vehicleID;
	double speed;
	string laneID;
}
