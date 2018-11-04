#include "FogNode.h"
#include <omnetpp.h>
using namespace omnetpp;
FogNode::FogNode(int nodeID, int capacity, double throughput, double bandWidth)
{
	this->nodeID = nodeID;
	this->capacity = capacity;
	this->origin_capacity = capacity;
	this->throughput = throughput;
	this->oriBandwidth = bandWidth;
	this->bandwidth = bandWidth;
	this->opNum = 1;
}


FogNode::~FogNode(void)
{
}


void FogNode::addEdge(FogEdge* edge)
{
    if(edge->getDest( this) == NULL){
        return;
    }
	if(edges.count(edge->getDest(this)->getNodeID()) > 0){
	    FogEdge* e = edges[edge->getDest(this)->getNodeID()];
	    e->setTransmission_rate(edge->getTransmission_rate());
	}
	else{
	    edges[edge->getDest(this)->getNodeID()] = edge;
	}
}


double (*valueFunction)(int Eventnum,FogEdge* e);

FogNode* FogNode::getSpecialFogNode(int app_num,int EventNum, double (*valueFunction)(int Eventnum,FogEdge* e)) {
    if (this->capacity > 0) {
        //EV << "nodeID " << this->nodeID << ":" << capacity << endl;
        // if this node have capacity to host an operator
        return this;
    }
    FogNode* target = NULL;
    double mintime = 1e10;
    double time = 0;

    map<int, FogNode*> travelled_fognode;
    map<int, FogNode*> unTravelled_fognode;
    map<int, double> untravelled_time;
    map<int, int> preNodeId;

    //find the fog with predicted minimum transmission delay and process time
    map<int, FogEdge*>::iterator it;

    it = edges.begin();

    //travel the direct neighbor
    while (it != edges.end()) {
        //it->first;
        FogEdge* e = it->second;
        FogNode* node = e->getDest(this);
        //EV << "node " << e->getDest(this)->getNodeID() << ":" << e->getDest(this)->getCapacity() <<"; " ;
        if(travelled_fognode.count(node->getNodeID()) > 0){
            it ++;
            continue;
        }

        travelled_fognode[node->getNodeID()] = node;
        //if this node in untravel fog node, erase it.
        if(unTravelled_fognode.count(node->getNodeID()) > 0){
            unTravelled_fognode.erase(node->getNodeID());
        }

        time = valueFunction(EventNum,e);
        if(untravelled_time[node->getNodeID()] > 0){

            if(untravelled_time[node->getNodeID()] > time){
                untravelled_time[node->getNodeID()] = time;

                preNodeId[node->getNodeID()] = this->getNodeID();
            }
        }else{
            untravelled_time[node->getNodeID()] = time;

            preNodeId[node->getNodeID()] = this->getNodeID();
        }
        //the fog node has resource for an operator.


        if (e->getDest(this)->getCapacity() <= 0 ) {

            //get the neighbor to get not traversed fog node
            map<int, FogEdge*> neighbor_edges = node->getEdges();
            map<int, FogEdge*>::iterator ngh_it = neighbor_edges.begin();

            //travel this neighbor's neighbor, record in untravelled
            while (ngh_it != neighbor_edges.end()) {
                FogEdge* ne = ngh_it->second;
                FogNode* nnode = ne->getDest(node);

                double temp_time = time;
                //if this neighbor node has not been travelled, record it.
                if(travelled_fognode.count(nnode->getNodeID()) == 0){
                    unTravelled_fognode[nnode->getNodeID()] = nnode;
                    temp_time += valueFunction(EventNum,ne);
                    if(untravelled_time.count(nnode->getNodeID()) > 0){
                        if(untravelled_time[nnode->getNodeID()] > temp_time){
                            untravelled_time[nnode->getNodeID()] = temp_time;

                            preNodeId[nnode->getNodeID()] = node->getNodeID();
                        }


                    }
                    else{
                        untravelled_time[nnode->getNodeID()] = temp_time;
                        preNodeId[nnode->getNodeID()] = node->getNodeID();
                    }
                }
                ngh_it ++;
            }
            it++;


            continue;
        }

        //time += (double) 1.0 / e->getDest(this)->getThroughput();
        if (time < mintime) {
            mintime = time;
            target = e->getDest(this);
            //EV << "mintar:" << target->nodeID << endl;
        }

        it++;
    }

    //if there is some fognode not travelled and has not find target.
    while(target == NULL && !unTravelled_fognode.empty()){
        map<int, FogNode*>::iterator nit = unTravelled_fognode.begin();
        while(nit != unTravelled_fognode.end()){
            FogNode* node = nit->second;

            travelled_fognode[node->getNodeID()] = node;

            time = untravelled_time[node->getNodeID()];


            if (node->getCapacity() <= 0 ) {

                //get the neighbor to get not traversed fog node
                map<int, FogEdge*> neighbor_edges = node->getEdges();
                map<int, FogEdge*>::iterator ngh_it = neighbor_edges.begin();

                //travel this neighbor's neighbor, record in untravelled
                while (ngh_it != neighbor_edges.end()) {
                    FogEdge* ne = ngh_it->second;
                    FogNode* nnode = ne->getDest(node);

                    double temp_time = time;
                    //if this neighbor node has not been travelled, record it.
                    if(travelled_fognode.count(nnode->getNodeID()) == 0){
                        unTravelled_fognode[nnode->getNodeID()] = nnode;
                        temp_time += valueFunction(EventNum,ne);
                        if(untravelled_time.count(nnode->getNodeID()) > 0){
                            if(untravelled_time[nnode->getNodeID()] > temp_time){
                                untravelled_time[nnode->getNodeID()] =temp_time;
                                preNodeId[nnode->getNodeID()] = node->getNodeID();
                            }


                        }
                        else{
                            untravelled_time[nnode->getNodeID()] = temp_time;
                            preNodeId[nnode->getNodeID()] = node->getNodeID();
                        }
                    }
                    ngh_it ++;

                }
                nit++;

                //if(unTravelled_fognode.count(node->getNodeID()) > 0){
                unTravelled_fognode.erase(node->getNodeID());
                //}

                continue;
            }

            //time += (double) 1.0 / node->getThroughput();
            if (time < mintime) {
                mintime = time;
                target = node;
                //EV << "mintar:" << target->nodeID << endl;
            }

            nit++;

            //if(unTravelled_fognode.count(node->getNodeID()) > 0){
            unTravelled_fognode.erase(node->getNodeID());
            //}

        }
    }

    //get all path in this placement
    if(target != NULL){
        int preId = -1;
        FogNode* dest = target;
        while(preId != this->getNodeID()){
            preId = preNodeId[dest->getNodeID()];
            FogEdge* edge = dest->getFogEdge(preId);

            edge->addEventNum(app_num, EventNum);
            dest = edge->getDest(dest);

        }
    }

    return target;

}

void FogNode::setCapacity(int capacity)
{
	this->capacity = capacity;
	//EV << "node " << this->nodeID << ":" << capacity;
}

const int FogNode::getCapacity() const
{
	return this->capacity;
}

void FogNode::resetCapacity(){
    this->capacity = this->origin_capacity;
    map<int,FogEdge*>::iterator it = edges.begin();
    while(it != edges.end()){
        it->second->clearEventNum();
        it ++;
    }
}

int FogNode::getNodeID(){
	return nodeID;
}

double FogNode::getThroughput(){
	return this->throughput;
}

void FogNode::setThroughput(double throughput){
	this->throughput = throughput;
}

FogEdge* FogNode::getFogEdge(int destNodeID){
    if(edges.count(destNodeID) > 0){
        return edges[destNodeID];
    }
    return NULL;
}

//host a new operator op
void FogNode::addOperator(OperatorModel* op){
	operators.push_back(op);
}

//the operator move or down
void FogNode::deleteOperator(OperatorModel* op)
{
	for (vector<OperatorModel*>::iterator iter = operators.begin(); iter != operators.end(); iter++)
    {
		//find the FogNode which has the same id. delete it.
		if((*iter)->getOperatorID() == op->getOperatorID()){
			operators.erase(iter);
		}
    }
}

//clear every edge neighbor this node
void FogNode::clearEdge(){
    map<int, FogEdge*>::iterator it;

    it = edges.begin();

    while(it != edges.end())
    {
        //it->first;
        //delete it->second;
        it ++;
    }

    edges.clear();

}

//update edge information
bool FogNode::updateEdge(int destnodeID, double transmission_rate,int dest_capacity, map<int, FogNode*>* fognodes){
    bool newEdge = false;
    if(edges.count(destnodeID) > 0){//update transmission rate
        edges[destnodeID]->setTransmission_rate(transmission_rate);
    }
    else{//add new node
        char edgeID[30];
        sprintf(edgeID, "%d-%d", nodeID, destnodeID);
        newEdge = true;

        //get dest fog node
        FogNode* dest = NULL;
        if(fognodes->count(destnodeID) > 0){
            dest = (*fognodes)[destnodeID];
        }
        else{
            dest = new FogNode(destnodeID, dest_capacity);
            (*fognodes)[destnodeID] = dest;
        }
        FogEdge* edge = new FogEdge(edgeID, this, dest, transmission_rate);
        edges[destnodeID] = edge;
    }
    return newEdge;
}

//get fog edge linked to this fog node
map<int, FogEdge*> FogNode::getEdges(){
    return edges;
}

double FogNode::getBandwidth() const {
    return bandwidth;
}

void FogNode::setBandwidth(double bandwidth) {
    if(bandwidth > this->oriBandwidth){
        bandwidth = oriBandwidth;
    }
    this->bandwidth = bandwidth;
}

int FogNode::getOriginCapacity() const {
    return origin_capacity;
}

void FogNode::setOriginCapacity(int originCapacity) {
    origin_capacity = originCapacity;
}

double FogNode::getOriBandwidth() const {
    return oriBandwidth;
}

int FogNode::getOpNum() const {
    return opNum;
}

void FogNode::setOpNum(int opNum) {
    this->opNum = opNum;
}
