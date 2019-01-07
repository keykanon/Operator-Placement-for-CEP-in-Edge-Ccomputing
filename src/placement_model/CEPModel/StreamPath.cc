#include "StreamPath.h"
#include <omnetpp.h>

using namespace omnetpp;
StreamPath::StreamPath(void)
{
}

StreamPath::StreamPath(StreamPath* sp){
	this->operators = sp->operators;
	this->streams = sp->streams;
	this->transmissionDelay = 0;
	this->processDelay = 0;
	this->responseTime = 0;
}

StreamPath::~StreamPath(void)
{
}

double StreamPath::getPredictedResponseTime(){
    return this->predicted_response_time;
}

double StreamPath::fac(int n){
    double ans = 1;
    for(int i = 1; i <= n; i ++){
        ans = ans* i;
    }
    return ans;
}
//calculate ErlangC
double StreamPath::ErlangC(int n, double c){
    double lambda = c;
    double ans = 1;

    ans = pow(lambda, n) / fac(n)  ;

    //calculate denominator
    double denominator = 0;
    for(int k = 0; k < n; k ++){
        denominator += pow(lambda,k)/fac(k);
    }
    denominator *= 1- (lambda/(double)n);
    denominator += ans;

    ans /= denominator;

    return ans;

}

double StreamPath::calResponseTime(double averageW, double averageThroughput,  map<int,map<int, double>> &distable,map<int, int> eventTable){
    double tp = 0;
        //source node
        int size = operators.size();
        double rate = 8.0 * 1024.0;
        FogNode* fs = operators[size-1]->getFogNode();

        for(int i = operators.size()-1; i >= 0; i --){
            //predict event number for every operator
            operators[i]->predictEventNumber(0);

            FogNode* fd = operators[i]->getFogNode();
            double transmission_time = 0;
            if(fs != NULL && fd != NULL && distable.count(fs->getNodeID()) > 0 && distable[fs->getNodeID()].count(fd->getNodeID())){
                if(fs->getNodeID() == fd->getNodeID()){
                    tp += 1.0 / fd->getThroughput();
                    fs = fd;
                    continue;
                }



                //calculate response time

                transmission_time =  distable[fs->getNodeID()][fd->getNodeID()];
                transmission_time += 0.01;
                //EV << "dis[" << fs->getNodeID() << "][" << fd->getNodeID() << "] = " <<  distable[fs->getNodeID()][fd->getNodeID()] << endl;
                double bandwidth = 1.0 / distable[fs->getNodeID()][fd->getNodeID()];
                double p = ErlangC(fd->getOriginCapacity(), eventTable[fd->getNodeID()]/bandwidth);
                if(p > 1 || p < 0 || eventTable[fd->getNodeID()] > bandwidth){
                    p = 1;
                    transmission_time += (((double)eventTable[fd->getNodeID()] ) * distable[fs->getNodeID()][fd->getNodeID()]);
                }
                else {
                    transmission_time += p * distable[fs->getNodeID()][fd->getNodeID()];

                }

                tp += transmission_time  + 1.0 / fd->getThroughput();


            }
            else{

                transmission_time = rate / averageW;
                transmission_time += 0.01;
                //EV << "averageW : " << averageW << endl;
                //double bandwidth = 1.0 / transmission_time;
                double p = ErlangC(this->getOperators()[this->getOperators().size()-1]->getFogNode()->getOriginCapacity(), rate*operators[i]->getPredictEventNumber()/averageW);
                if(p > 1 || p < 0 ){
                    p = 1;
                    transmission_time = rate * (operators[i]->getPredictEventNumber() )/averageW;
                }
                else {
                    transmission_time += p*rate / averageW;
                }



                if(fs != NULL && fd != NULL && fs->getNodeID() == fd->getNodeID()){
                    transmission_time = 0;
                }

                tp += transmission_time +
                        1.0 / averageThroughput;
            }




            fs = fd;

        }

        this->predicted_response_time = tp;
        return tp;
}

double StreamPath::predictResponseTime(double averageW, double averageThroughput,  map<int,map<int, double>> &distable,map<int, int> eventTable){

    double tp = 0;
    //source node
    int size = operators.size();
    double rate = 8.0 * 1024.0;
    FogNode* fs = operators[size-1]->getFogNode();

    for(int i = operators.size()-1; i >= 0; i --){
        //predict event number for every operator
        operators[i]->predictEventNumber(0);

        FogNode* fd = operators[i]->getFogNode();
        double transmission_time = 0;
        if(fs != NULL && fd != NULL && distable.count(fs->getNodeID()) > 0 && distable[fs->getNodeID()].count(fd->getNodeID())){
            if(fs->getNodeID() == fd->getNodeID()){
                tp += 1.0 / fd->getThroughput();
                fs = fd;
                continue;
            }

            eventTable[fd->getNodeID()] = eventTable[fd->getNodeID()] + operators[i]->getPredictEventNumber();

            //record event table
           /* if(eventTable.count(fs->getNodeID()) > 0){
                map<int, int> et = eventTable[fs->getNodeID()];
                if(et.count(fd->getNodeID()) > 0){
                    et[fd->getNodeID()] = et[fd->getNodeID()] + operators[i]->getPredictEventNumber();

                }
                else{
                    map<int,FogEdge*> src_edges = fs->getEdges();
                    map<int,FogEdge*>::iterator sit = src_edges.begin();

                    map<int,FogNode*> travelled;
                    travelled[fs->getNodeID()] = fs;
                    vector<FogNode*> untravel;

                    //travel fs's edges
                    while(sit != src_edges.end()){
                    //for(int i = 0; i < src_edges.size(); i ++){
                        FogNode* node = sit->second->getDest(fs);
                        travelled[node->getNodeID()] = node;
                        map<int,FogEdge*> mid_edges = node->getEdges();
                        if(mid_edges.count(fd->getNodeID())){

                            et[node->getNodeID()] = et[node->getNodeID()]+ operators[i]->getPredictEventNumber();
                            eventTable[node->getNodeID()][fd->getNodeID()] = eventTable[node->getNodeID()][fd->getNodeID()] + operators[i]->getPredictEventNumber();
                            sit = src_edges.end();
                            break;
                        }

                        sit ++;
                    }

                    //et[fd->getNodeID()] =  operators[i]->getPredictEventNumber();
                }
                eventTable[fs->getNodeID()] = et;
            }
            else{
                map<int, int> et;
                et[fd->getNodeID()] = operators[i]->getPredictEventNumber();
                eventTable[fs->getNodeID()] = et;
            }*/

            //calculate response time

            transmission_time =  distable[fs->getNodeID()][fd->getNodeID()];
            transmission_time += 0.01;
            //EV << "dis[" << fs->getNodeID() << "][" << fd->getNodeID() << "] = " <<  distable[fs->getNodeID()][fd->getNodeID()] << endl;
            double bandwidth = 1.0 / distable[fs->getNodeID()][fd->getNodeID()];
            double p = ErlangC(fd->getOriginCapacity(), eventTable[fd->getNodeID()]/bandwidth);
            if(p > 1 || p < 0 || eventTable[fd->getNodeID()] > bandwidth){
                p = 1;
                transmission_time += (((double)eventTable[fd->getNodeID()] ) * distable[fs->getNodeID()][fd->getNodeID()]);
            }
            else {
                transmission_time += p * distable[fs->getNodeID()][fd->getNodeID()];

            }
            //if(rate* (double) eventTable[fs->getNodeID()][fd->getNodeID()] > distable[fs->getNodeID()][fd->getNodeID()]){
            //transmission_time +=  ((double)eventTable[fs->getNodeID()][fd->getNodeID()] - rate / distable[fs->getNodeID()][fd->getNodeID()]) / distable[fs->getNodeID()][fd->getNodeID()];
                //transmission_time /= (double) eventTable[fs->getNodeID()][fd->getNodeID()];
            //}

            tp += transmission_time  + 1.0 / fd->getThroughput();


        }
        else{

            transmission_time = rate / averageW;
            transmission_time += 0.01;
            //EV << "averageW : " << averageW << endl;
            //double bandwidth = 1.0 / transmission_time;
            double p = ErlangC(this->getOperators()[this->getOperators().size()-1]->getFogNode()->getOriginCapacity(), rate*operators[i]->getPredictEventNumber()/averageW);
            if(p > 1 || p < 0 ){
                p = 1;
                transmission_time = rate * (operators[i]->getPredictEventNumber() )/averageW;
            }
            else {
                transmission_time += p*rate / averageW;
            }
            //if(rate* operators[i]->getPredictEventNumber() > averageW){
            //    transmission_time += (rate* operators[i]->getPredictEventNumber() - averageW)/averageW;
                //transmission_time /= operators[i]->getPredictEventNumber();
            //}


            if(fs != NULL && fd != NULL && fs->getNodeID() == fd->getNodeID()){
                transmission_time = 0;
            }

            tp += transmission_time +
                    1.0 / averageThroughput;
        }




        fs = fd;

    }

    this->predicted_response_time = tp;
    return tp;
}


double StreamPath::getTransmissionDelay(){
	return this->transmissionDelay;
}

double StreamPath::getProcessDelay(){
	return this->processDelay;
}

void StreamPath::setResponseTime(double responseTime){
	this->responseTime = responseTime;
}

double StreamPath::getResponseTime(){
	return this->responseTime;
}

void StreamPath::setPredictedResponseTime(double predicted_rt){
    this->predicted_response_time = predicted_rt;
}

double StreamPath::totalDelay(){
	return this->getTransmissionDelay() + this->getProcessDelay();
}

void StreamPath::addStream(StreamModel* stream){
	streams.push_back(stream);
}

void StreamPath::addOperator(OperatorModel* op){
	operators.push_back(op);
}

//print path used for debug
void StreamPath::printPath(ofstream& out){
    EV << operators[0]->getOperatorID().c_str() << " ";
    out << operators[0]->getOperatorID().c_str() << " ";
	//printf("%s ", operators[0]->getOperatorID().c_str());
	if(operators[0]->getFogNode() != NULL){
	    EV << "on " << operators[0]->getFogNode()->getNodeID() ;
	    out << "on " << operators[0]->getFogNode()->getNodeID() ;
	}
	EV << "--";
	out << "--";
	for(int i = 0; i < streams.size(); i ++){
	    EV << streams[i]->getStreamID().c_str() << " -- ";
	    out << streams[i]->getStreamID().c_str() << " -- ";
	    EV << operators[i+1]->getOperatorID().c_str() << " ";
	    out << operators[i+1]->getOperatorID().c_str() << " ";
	    if(operators[i+1]->getFogNode() != NULL){
	        EV << "on " << operators[i+1]->getFogNode()->getNodeID() ;
	        out << "on " << operators[i+1]->getFogNode()->getNodeID() ;
	    }
	    EV << "--";
	    out << "--";
		//printf("%s ", streams[i]->getStreamID().c_str());
		//printf("%s ", operators[i+1]->getOperatorID().c_str());

	}
	//printf("\r\n");
	EV << endl;
	out << endl;
}

//judge whether the stream path is empty
bool StreamPath::isEmpty(){
	return operators.empty();
}

//judge whether the operators in this stream path all placed in edge node.
bool StreamPath::isAllPlaced(){

    for(int i = 0; i < operators.size(); i ++){
        if(operators[i]->getFogNode() == NULL){
            return false;
        }
    }
    return true;
}

//get top of stream path
OperatorModel* StreamPath::back(){
	return operators.back();
}

//delete the top of stream path
void StreamPath::pop(){
	this->operators.pop_back();
}

//return operators size
int StreamPath::size(){
	return this->operators.size();
}

//return the queue of operators
vector<OperatorModel*> StreamPath::getOperators(){
	return this->operators;
}
