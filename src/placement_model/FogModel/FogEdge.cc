#include "FogEdge.h"
#include "FogNode.h"

FogEdge::FogEdge(string edgeID, FogNode* s, FogNode* d, double tran_rate)
{
	this->edgeID = edgeID;
	this->source = s;
	this->dest = d;
	transmission_rate = tran_rate;
	totalEventNum = 0;
}


FogEdge::~FogEdge(void)
{
}

void FogEdge::setTransmission_rate(double transmission_rate)
{
	this->transmission_rate = transmission_rate;
}

double FogEdge::getTransmission_rate()
{
	return this->transmission_rate;
}

string FogEdge::getEdgeID(){
	return edgeID;
}

FogNode* FogEdge::getSource(){
    return this->source;
}

FogNode* FogEdge::getDest(){
    return this->dest;
}

/*a node linked wants to get another one
input s means the node linked to this fog edge
return the another node linked to this fog edge
*/
FogNode* FogEdge::getDest(FogNode* s){
	if(s == NULL){
		return NULL;
	}
	if(s->getNodeID() == source->getNodeID()){
		return dest;
	}
	else if(s->getNodeID() == dest->getNodeID()){
		return source;
	}
	else{
		return NULL;
	}

}

//compare fog edge is the edge from s to d
bool FogEdge::compareEdge(FogNode* s, FogNode* d){
	if(s == NULL || d == NULL){
		printf("error:compareEdge s or d == NULL \r\n");
	}
	if(source->getNodeID() == s->getNodeID() && dest->getNodeID() == d->getNodeID()){
		return true;
	}else if(source->getNodeID() == d->getNodeID() && dest->getNodeID() == s->getNodeID()){
		return true;
	}
	else{
		return false;
	}
}

//clear every in edge
void FogEdge::clear(FogNode* s){
    if(s->getNodeID() == source->getNodeID()){
        delete dest;
        dest = NULL;
    }
    else if(dest->getNodeID() == s->getNodeID()){
        delete source;
        source = NULL;
    }
    else{
        printf("error: s!= source and s!= dest \r\n");
    }

}

int FogEdge::getTotalEventNum() const {
    return totalEventNum;
}

void FogEdge::setTotalEventNum(int totalEventNum) {
    this->totalEventNum = totalEventNum;
}

//add event num
void FogEdge::addEventNum(int app_num, int num){
    if(eventNum.count(app_num) > 0){
        eventNum[app_num] = eventNum[app_num] + num;
    }
    else{
        eventNum[app_num] = num;
    }
    totalEventNum += num;
}

//clear event num
void FogEdge::clearEventNum(){
    eventNum.clear();
    totalEventNum = 0;
}

//clear event number of an operator graph whose index is app_num
void FogEdge::clearEventNum(int app_num){
    if(eventNum.count(app_num) > 0){
        totalEventNum -= eventNum[app_num];
        eventNum[app_num] = 0;
    }
}
