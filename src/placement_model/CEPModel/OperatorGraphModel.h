#pragma once

#include "OperatorModel.h"
#include "StreamModel.h"
#include "StreamPath.h"

#include <vector>
#include <stack>

class OperatorGraphModel
{
private:
    int operatorGraphID;

	vector<OperatorModel*> source;
	vector<OperatorModel*> operators;
	queue<StreamModel*> streams;

	vector<StreamPath*> stream_paths;
	double response_time;
	double predicted_response_time;

	double response_time_constraints;




	void initialOG3();

	void randomOG(int ogNum);
public:
	int randSeed = 1;
	OperatorGraphModel(double rt_constraint,int type, int ogID);
	~OperatorGraphModel(void);

	//get StreamPath through operators and streams
	void calStreamPath(OperatorModel* comsumer);

	int getEventNumber();

	//calculate response time of the operator graph
	int calResponseTime(double averageW, double averageThroughput, map<int, map<int, double>>& distable, map<int,int>& eventTable);

	//calculate response time of the operator graph
	int getFirstServicedStreamPathIndex(double averageW, double averageThroughput, map<int, map<int, double>>& distable,map<int,int>& eventTable);
	void setResponseTime(double);
	double getResponseTime();
	double getPredictedResponseTime();
	//get response time constraint
	//ratio = predicted_response_time / response_time_constraints
	double getRTR();
	vector<OperatorModel*> getOperatorModel();

	//print used for debug
	void printPaths(ofstream& out);

	//get the number of stream paths
	int getStreamPathSize();

	//judge whether the operators in operator graph placed
	bool isAllPlaced();

	//judge whether the operators need to be replaced
	bool isNeedReplaced();

	//get sources
	vector<OperatorModel*> getSource();

	//get stream paths
	vector<StreamPath*> getStreamPath();
    int getOperatorGraphId() const;
    void setOperatorGraphId(int operatorGraphId);
    //int getRandSeed(){return this->randSeed;}
};

