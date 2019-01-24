#include "OperatorModel.h"


OperatorModel::OperatorModel(string operatorID, int resourceRequire)
{
	throughput = INIT_THROUGHPUT;
	this->operatorID = operatorID;
	fognode = NULL;
	this->resourceRequire = resourceRequire;
	
}

OperatorModel::~OperatorModel(void)
{
}
/*
inline OperatorModel& OperatorModel::operator = (OperatorModel& op){
    if(&op == this){
        return *this;
    }
    //this->operatorID() = op.getOperatorID();
    this->predictedNumber = op.getPredictEventNumber();
    this->throughput = op.getThroughput();
    this->processTime = op.getProcessTime();
    this->fognode = op.getFogNode();
}*/

double OperatorModel::getThroughput()
{
	return this->throughput;
}

int OperatorModel::getPredictEventNumber(){
    this->predictEventNumber(0);
	return this->predictedNumber;
}

//predict throughput
void OperatorModel::predictThroughput(){
	if(historicalThroughput.size() == 0){
		throughput = INIT_THROUGHPUT;
	}
	double average = 0;
	//acumulate historical throughput
	for(int i = 0; i < historicalThroughput.size(); i ++){
		average += historicalThroughput[i];
	}
	throughput = average / historicalThroughput.size();
}


//predict event number 
//method type 0 refers to average predict method
//method type 1 refers to polynomial interpolation method
void OperatorModel::predictEventNumber(int methodType){
	//if there is no historical event number in record
	if(historicalEventNum.size() == 0){
		predictedNumber = INIT_PREDICT_NUMBER;
		return;
	}
	//select predict method type
	predictedNumber = 0;
	int begin = 0, size = 0;

	switch(methodType){
	case 0://historical average method
	    begin = historicalEventNum.size()-10;
	    size = (begin == 0)?historicalEventNum.size():10;
	    begin = (begin >= 0)?begin:0;

		for(int i = begin; i < historicalEventNum.size(); i ++){
			predictedNumber += historicalEventNum[i];
		}
		predictedNumber /= size;
		break;
	case 1://polynomial interpolation method
		double ans = 0;
		
		size = historicalEventNum.size();
		//polynomial interpolation method
		int end = (size%2 == 0)?1:0;
		for(int j = size-1; j >= end; j --){
			double base = 1;
			for(int k = size-1; k >= end; k --){
				if(j != k){
					base *= (double)(size - k)/(double)(j-k);
			
				}
			}
			base *= historicalEventNum[j];
			ans += base;
		}
		predictedNumber = ans;
		if(predictedNumber < 0){
		    predictedNumber = 0;
		}
		break;
	}
}

double OperatorModel::getProcessTime(){
	return this->processTime;
}

void OperatorModel::addThroughput(double throughput){
	historicalThroughput.push_back(throughput);	
}

void OperatorModel::addEventNum(int time, int eventnum){
    if(historicalEventNum.size() > time){
        int orinum = historicalEventNum[time];
        historicalEventNum[time] = orinum + eventnum;

    }
    else{
        historicalEventNum.push_back(eventnum);
    }
}

void OperatorModel::addInputStreams(StreamModel* inputstream){
	inputstreams.push_back(inputstream);
}

void OperatorModel::addOutputStreams(StreamModel* outputstream){
	this->outputstreams.push_back(outputstream);
}

vector<StreamModel*> OperatorModel::getInputStreams(){
	return this->inputstreams;
}

vector<StreamModel*> OperatorModel::getOutputStreams(){
	return this->outputstreams;
}

string OperatorModel::getOperatorID(){
	return this->operatorID;
}	

//place operator on this fog node
void OperatorModel::setFogNode(FogNode* fognode){

	this->fognode = fognode;
}

FogNode* OperatorModel::getFogNode(){
	return this->fognode;
}

vector<int> OperatorModel::getHistoricalEventNum() {
    return historicalEventNum;
}

int OperatorModel::getResourceRequire() const {
    return resourceRequire;
}

void OperatorModel::setResourceRequire(int resourceRequire) {
    this->resourceRequire = resourceRequire;
}
