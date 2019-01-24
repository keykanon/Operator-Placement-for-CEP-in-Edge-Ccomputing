#pragma once

#include <queue>
#include "StreamModel.h"

using namespace std;

/*----------------------ProcessPredictor-------------------------
predictorNumber: predict event number that this operator should process.
throughput: the throughput of this operator in this time period.
historicalThroughput: the queue of historiccal throughput used for predict throughput
historicalEventNum: the queue of historical event number used for predict predictednumber.
*/
class StreamModel;
class OperatorModel
{
private:
	string operatorID;
	static const int INIT_THROUGHPUT = 1500;
	static const int INIT_PREDICT_NUMBER = 1500;
	int predictedNumber;
	double throughput;

	int resourceRequire;

	vector<double> historicalThroughput;
	vector<int> historicalEventNum;

	double processTime;

	vector<StreamModel*> inputstreams;
	vector<StreamModel*> outputstreams;

	FogNode* fognode;
public:
	OperatorModel(string operatorID,int resourceRequire = 1);
	~OperatorModel(void);

	//inline OperatorModel& operator = ( OperatorModel& op);

	int getPredictEventNumber();
	double getThroughput();
	void predictThroughput();

	//predict event number 
	//method type 0 refers to average predict method
	//method type 1 refers to polynomial interpolation method
	void predictEventNumber(int methodType);
	double getProcessTime();
	
	void addThroughput(double throughput);
	void addEventNum(int time, int eventnum);

	void addInputStreams(StreamModel* inputstream);
	void addOutputStreams(StreamModel* outuputstream);
	vector<StreamModel*> getInputStreams();
	vector<StreamModel*> getOutputStreams();

	string getOperatorID();

	//place operator on this fog node
	void setFogNode(FogNode* fognode);
	
	FogNode* getFogNode();
    vector<int> getHistoricalEventNum();
    int getResourceRequire() const;
    void setResourceRequire(int resourceRequire);
};

