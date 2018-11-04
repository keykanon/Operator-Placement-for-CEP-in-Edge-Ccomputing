#pragma once
#include "FogEdge.h"

#include <vector>
#include "../CEPModel/OperatorModel.h"
#include <map>



class FogNode
{
private:
	int nodeID;
	int capacity;
	int origin_capacity;

	double oriBandwidth;
	double bandwidth;

	double throughput;
	std::map<int, FogEdge*> edges;

	vector<OperatorModel*> operators;
	int opNum;
public:
	FogNode(int nodeID, int capacity = 10,double throughput = 4000.0,double bandwidth = 2e4/8.0);
	~FogNode(void);



	void addEdge(FogEdge* edge);
	FogEdge* getFogEdge(int destNodeID);
	void setCapacity(int capacity);
	const int getCapacity() const;

	void resetCapacity();
	int getNodeID();
	double getThroughput();
	void setThroughput(double);

	//host a new operator op
	void addOperator(OperatorModel* op);

	//FogNode* getMinRateFogNode();

	FogNode* getSpecialFogNode(int app_num,int EventNum, double (*valueFunction)(int Eventnum,FogEdge* e));
	//the operator move or down
	void deleteOperator(OperatorModel* op);

	//get minimum w fognode
    //get minimum w fognode


	//clear the edge of fognode
	void clearEdge();

	//update an edge node
	bool updateEdge(int destnodeID, double transmission_rate, int dest_capacity, map<int, FogNode*>* fognodes);

	//get fog edge linked to this fog node
	map<int, FogEdge*> getEdges();
    double getBandwidth() const;
    void setBandwidth(double bandwidth);
    int getOriginCapacity() const;
    void setOriginCapacity(int originCapacity);
    double getOriBandwidth() const;
    int getOpNum() const;
    void setOpNum(int opNum);
};

