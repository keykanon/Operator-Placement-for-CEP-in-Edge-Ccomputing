#pragma once

#include "../FogModel/FogEdge.h"
#include "OperatorModel.h"
//#include "FogNode.h"
#include <queue>

using namespace std;

/*---------------------Operator Predictor----------------------

fognode is the fog node host this operator.
transmission_rate

*/
class FogEdge;
class OperatorModel;
class StreamModel
{
private:
	string streamID;
	static const int INIT_EVENT_NUM = 100000;
	int predictedNumber;
	queue<int> historicalRecord;
	double transmission_rate;
	vector<FogEdge*> fogedges;
	//StreamModel* inputstream;
	//queue<OperatorPredictor*> outputstream;
	OperatorModel* source;
	OperatorModel* dest;
public:
	StreamModel(string streamID,OperatorModel* sourcee, OperatorModel* dest);
	~StreamModel(void);



	void setFogEdge(vector<FogEdge*> fogedges);

	void addHistrocalRecord(int eventNumber);

	OperatorModel* getSource();
	OperatorModel* getDest();

	string getStreamID();
	


};

