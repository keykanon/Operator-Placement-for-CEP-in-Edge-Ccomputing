#pragma once

#include "FogNode.h"
#include "FogEdge.h"
#include <map>

#include "../../message/EventPacket_m.h"

using namespace std;

/* ----------fog networks---------------
es is the event storage, we should know which fog node the 
data source in
fognodes are all fog node in fog networks local fog node can monitoring
fogedges are conenction between fog nodes in fog networks

*/
class FogNetworks
{
private:
    const static int INIT_W = 2048;
	const static int INIT_HOP = 2;
	FogNode* es;
	map<int,FogNode*> fognodes;
	vector<FogEdge*> fogedges;
	map<int,map<int,double>> distance_table;
	int hops;
	int maxFogNodeID;
public:
	FogNetworks(FogNode* es);
	~FogNetworks(void);

	map<int, FogNode*> getFogNodes();

	//get the left resource
	vector<FogNode*> getResources();

	void updateES(FogNode* es);
	FogNode* getES();

	//add a fog node to fog networks
	void addFogNode(FogNode* node);

	void updateFogNode(NodeMessage nodeMessage);

	//add a fog edge to fog networks
	void addFogEdge(FogEdge* edge);

	//delete fog node in fog networks
	void deleteFogNode(FogNode* node);

	//delete fog edge in fog networks
	void deleteFogEdge(FogEdge* edge);

	//get FogEdge from two fog node s and d
	FogEdge* getFogEdge(FogNode* s, FogNode* d);


	FogNode* getFogNode(int NodeID);

	//get the transmission rate between fog node s and fog node d
	double getFogEdgeW(FogNode* s, FogNode* d);

	//have no fog node to place. increase fog networks managed
	void increaseHops();

	//too much fog nodes to manage. decrease number of fog nodes managed
	void decreaseHops();

	//get monitor hops H
	int getH();

	//get average communication latency in edges
	double getAverageCommunicationLatency();

	//get average w
	double getAverageW();

	//judge whether the resource in fog node is empty
	bool isEmpty();
	//get average execution speed
	double getAverageExecutionSpeed();
    int getMaxFogNodeId() const;
    void setMaxFogNodeId(int maxFogNodeId);
    map<int, map<int, double> >& getDistanceTable();
    void setDistanceTable(const map<int, map<int, double> >& distanceTable);
    vector<FogEdge*>& getFogedges();
};

