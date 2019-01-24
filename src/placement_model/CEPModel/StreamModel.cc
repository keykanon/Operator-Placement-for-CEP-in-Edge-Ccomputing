#include "StreamModel.h"


StreamModel::StreamModel(string streamID,OperatorModel* source, OperatorModel* dest)
{
	this->streamID = streamID;
	this->predictedNumber = INIT_EVENT_NUM;
	this->source = source;
	this->dest = dest;
}

StreamModel::~StreamModel(void)
{
}



void StreamModel::setFogEdge(vector<FogEdge*> fogedges)
{
	this->fogedges = fogedges;
}

void StreamModel::addHistrocalRecord(int eventNumber)
{
	this->historicalRecord.push(eventNumber);
}

OperatorModel* StreamModel::getSource(){
	return this->source;
}

OperatorModel* StreamModel::getDest(){
	return dest;
}

string StreamModel::getStreamID(){
	return this->streamID;
}
