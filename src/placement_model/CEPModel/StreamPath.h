#pragma once
#include "OperatorModel.h"
#include "../FogModel/FogNode.h"
#include <stack>
#include <fstream>
class StreamPath
{
private:
	vector<OperatorModel* > operators;
	vector<StreamModel* > streams;

	double transmissionDelay;
	double processDelay;
	double responseTime;
	double predicted_response_time;

	double fac(int n);
	//calculate ErlangC
	double ErlangC(int n, double c);

public:
	StreamPath(void);
	StreamPath(StreamPath* );
	~StreamPath(void);

	//void calTransmissionDelay();
	//void calProcessDelay();
	double calResponseTime(double averageW, double averageThroughput,  map<int,map<int,double>> &distable, map<int, int>);
	double predictResponseTime(double averageW, double averageThroughput,  map<int,map<int,double>> &distable, map<int, int>);
	double getPredictedResponseTime();
	double getTransmissionDelay();
	double getProcessDelay();
	void setResponseTime(double);
	double getResponseTime();

	void setPredictedResponseTime(double);

	double totalDelay();

	void addStream(StreamModel*);
	void addOperator(OperatorModel*);

	//print path used for debug
	void printPath(ofstream& out);

	//judge whether the stream path is empty
	bool isEmpty();

	//judge whether the operators in this stream path all placed in edge node.
	bool isAllPlaced();

	//get top of stream path
	OperatorModel* back();

	//delete the top of stream path
	void pop();

	//return operators size
	int size();

	//return the queue of operators
	vector<OperatorModel*> getOperators();

	
};

