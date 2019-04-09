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
#include "../placement_model/Reinforcement_Learning.h"

using namespace omnetpp;

class Reinforcement_Learning;
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

    double minimum_input_rate = 100;
    double maximum_input_rate = 2500;

    const int MAX_ITERATION = 100;

	bool init = true;

	const int INIT_BANDWIDTH_COST = 500;

	Reinforcement_Learning* rlearner = NULL;

	const int Wt = 100;
public:
	static const int markerTime = 1;
	OperatorPlacementManager(vector<double> rt_constraints,vector<int> type, vector<int> ogID, int monitor_interval);
	OperatorPlacementManager(FogNode* es, vector<double> rt_constraints, vector<int> type, vector<int> ogID, int monitor_interval);
	~OperatorPlacementManager(void);

	//init Operator Graph and Fog networks to test
	void test_init();

	//used for getOptimalPlacement travel
	void OptimalPlacementTravel(map<OperatorModel*, FogNode*>& M, int ogIndex, int opIndex,
	        double& minResponseTime, map<OperatorModel*, FogNode*>& bestM);

	//get optimal operator placement. it takes a lot of time
	vector<vector<StreamPath*>> getOptimalPlacement(vector<bool>& replace);

	//multi operator graph placement using reschedule
	vector<vector<StreamPath*>> getReMultiOperatorGraphPlacement(vector<bool>& replace);

	//multi operator graph placement using reschedule
    vector<vector<StreamPath*>> getIterationOperatorGraphPlacement(vector<bool>& replace);

    vector<vector<StreamPath*>> getIterationOptimization(vector<bool>& replace);

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

	vector<vector<StreamPath*>> Monte_Carlo(vector<int>& capacity, vector<double>& inputs, vector<double>& response_time);

	vector<vector<StreamPath*>> Sarsa_TD(vector<int>& capacity, vector<double>& inputs, vector<double>& response_time);

	vector<vector<StreamPath*>> QLearning(vector<int>& capacity, vector<double>& inputs, vector<double>& response_time);

	void reinforcement_learning_update_state(vector<int>& capacity, vector<double>& inputs, vector<double>& response_time, int type);
	// ----------Monte Carlo method-----------------
	//输出训练模型
	void RL_output(string type);
	//得到之前运行的训练模型
	void RL_input(string name);

	//设置基本参数
	void RL_update_parameter();

	//增加训练循环计数
	void RL_increase_round_time();

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
	//vector<StreamPath*> getLBResponseTimeGreedyPlacement(int index, vector<bool>& replace)

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

	void updateResponsetime(int app_num, int, double);

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

    vector<int> getFogNodeCapacity(){
        vector<int> capacity;
        map<int, FogNode*> fognodes = fognetworks->getFogNodes();
        map<int, FogNode*>::iterator nit = fognodes.begin();
        while(nit != fognodes.end()){
            if(nit->second == NULL){
                ++ nit;
                continue;
            }
            capacity.push_back(nit->second->getCapacity());
            nit++;
        }

        return capacity;
    }

    void updateCapacity(int fogNodeID, int capacity);

    //重置operator graph
    void resetOG(vector<int> type){
        FogNode* es = this->fognetworks->getES();
        for(int i = 0; i < ogModel.size(); ++ i){
            ogModel[i]->reset(type[i]);
            vector<OperatorModel*> source = ogModel[i]->getSource();
            for(int i = 0; i < source.size(); i ++){
                source[i]->setFogNode(es);
            }
        }
        int size = 0;
        for(int ogIndex = 0; ogIndex < ogModel.size(); ogIndex ++){
            size += ogModel[ogIndex]->getOperatorModel().size()-1;
        }
        es->setOpNum(size);
    }

    void resetES(int fognodeID){
        //update es
        FogNode* es;
        if(fognetworks->getFogNode(fognodeID) == NULL){
            es = new FogNode(fognodeID);
            fognetworks->updateES(es);
            fognetworks->addFogNode(es);
        }
        else{
            es = fognetworks->getFogNode(fognodeID);
            fognetworks->updateES(es);
        }
        //update source to es map
        for(int i = 0; i < this->ogModel.size(); ++ i){
            for(int j = 0; j < this->ogModel[i]->getSource().size(); ++ j){
                this->ogModel[i]->getSource()[j]->setFogNode(es);
            }
        }

    }

};

