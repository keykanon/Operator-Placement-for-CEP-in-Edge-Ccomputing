#pragma once


#include <omnetpp.h>
#include "FogModel/FogNetworks.h"
#include "CEPModel/OperatorModel.h"
#include "CEPModel/StreamModel.h"
#include "CEPModel/OperatorGraphModel.h"
#include "MigModel/Migration.h"
#include "MigModel/MigrationTime.h"
#include "../message/EventPacket_m.h"
#include <fstream>
#include <algorithm>

using namespace omnetpp;
class Migration;
class OperatorPlacementManager
{
private:
    //operator graph numbers

    int monitorID;
    bool monitorIncrease;
    //operator graph response time constraints
    //static vector<int> RT_CONSTRAINTS = {10,10,10,10,10};
	//information of fog networks
	FogNetworks* fognetworks;
	//information of Operator Graph Model
	vector<OperatorGraphModel*> ogModel;

	vector<OperatorModel> placement;

	const int INIT_BANDWIDTH_COST = 500;

	const int Wt = 100;
public:
	static const int markerTime = 1;
	OperatorPlacementManager(vector<double> rt_constraints,vector<int> type, vector<int> ogID);
	OperatorPlacementManager(FogNode* es, vector<double> rt_constraints, vector<int> type, vector<int> ogID);
	~OperatorPlacementManager(void);

	//init Operator Graph and Fog networks to test
	void test_init();

	//multi operator graph placement using reschedule
	vector<vector<StreamPath*>> getReMultiOperatorGraphPlacement(vector<bool>& replace);

	//multi operator graph placement by schedule policy selfish
	vector<vector<StreamPath*>> getSelfishMultiOperatorPlacement();

	//simple operator placement using selfish policy
	vector<vector<StreamPath*>> getSimpleGreedyPlacement(vector<bool>& replace);

    //response time greedy operator placement using selfish policy
    vector<vector<StreamPath*>> getResponseTimeGreedyPlacement(vector<bool>& replace);

	//Simulated Annealing operator graph placement using policy reschedule
	vector<vector<StreamPath*>> getReSAOperatorGraphPlacement(double alpha, double T);

	//Simulated Annealing operator graph placement using policy reschedule
	vector<vector<StreamPath*>> getSelfishSAOperatorGraphPlacement(double alpha, double T);
	//operator graph placement using policy selfless
	//vector<vector<StreamPath*>> getSelflessOperatorPlacement();

	//place the operator graph in an edge node
	vector<vector<StreamPath*>> getOveralGraphPlacement(vector<bool>& replace);

	vector<vector<StreamPath*>> getLoadBalance(double theta, double epsilon, double delta,vector<bool>& replace);
	//vector<vector<StreamPath*>> getGraphLoadBalance(double theta, double epsilon, double delta);

	int** minLatencyFlow(double averageD, vector<int> Vs, vector<int> Vt, vector<int> Vs_phi, vector<int> Vt_phi, double epsilon );

	double AverageResponseTimeOfTasks(FogNode* node);

	double fac(int n);
	double ErlangC(int n, double c);

	//Resource Aware Placement
	vector<vector<StreamPath*>> getSelfResourceAwarePlacement();
	vector<StreamPath*> getResourceAwarePlacement(int index);
	int getLowerBound(vector< FogNode*> edgeNodes, OperatorModel* op, int low, int high);
	bool compare(FogNode* v, OperatorModel* op);

	//single operator graph placement
	vector<StreamPath*> getPlacement(int index, map<int,int>& eventTable);
	vector<StreamPath*> getSimulatedAnealingPlacement(int index, double alpha, double T);
	vector<StreamPath*> getRandomPlacement(int index);
	vector<StreamPath*> getSimpleGreedyPlacement(int index,vector<bool>& replace);
	vector<StreamPath*> getResponseTimeGreedyPlacement(int index,vector<bool>& replace);

	//predict response time of path
	double predictResponseTime(StreamPath* stream_path);

	//update fog networks
	void updateFogNode(NodeMessage nodeMessage);

	//get monitor hops
	int getH();

	//print placement
	void printPlacement(ofstream&);


	//calculate the difference between two placements
	vector<Migration> diffPlacements(vector<OperatorModel>, vector<OperatorModel*>);

	//migrate origin placement to new placement
	vector<OperatorModel> migratePlacement(vector<OperatorModel>, vector<Migration>);

	//predict the response time
	double predictResponseTime(vector<OperatorModel>);
	void calOperatorGraphResponseTime();

	//migration aware algorithm
	vector<Migration> migrationAware(int index);

	//get minimum transmission delay fog node
	int getMinimumDelayFogNodeID(int app_num,int nodeID);

	//use Floyd algorithm to calculate distance table
	map<int,map<int, double>> Floyd();


	//decrease the capacity of a fog node
	void decreaseCapacity(int nodeID);

	//decrease the capacity of a fog node
	void increaseCapacity(int nodeID);
	//reset ES
	void resetES(int index, int fognodeID);

	//judge whether every operator has its own edge node
	bool isAllOperatorPlaced();

	//judge whether every operators in an operator graph has its own edge node
	bool isOperatorGraphPlaced(int app_num);

	//update event number
	void updateEventNumber(int app_num, int operatorType,int time, int eventNum);

	void updateResponsetime(int app_num, double);

	//reset the fog node of one operator graph which placed in
	void resetOperatorGraph(int index);
	//reset every fognode's capacity
	void resetCapacity();
    int getMonitorId() const;
    void setMonitorId(int monitorId);
    bool isMonitorIncrease() const;
    void setMonitorIncrease(bool monitorIncrease);

    OperatorGraphModel* getOperatorGraph(int index);

    int getOperatorGraphNum();

    FogNode* getES();

    int getFogNodeNum();

    void updateCapacity(int fogNodeID, int capacity);
};

