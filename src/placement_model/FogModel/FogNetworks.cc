#include "FogNetworks.h"
//#include <omnetpp.h>

//using namespace omnetpp;

FogNetworks::FogNetworks(FogNode* es)
{
	this->es = es;
	fognodes[es->getNodeID()] = es;
	hops = INIT_HOP;
}


FogNetworks::~FogNetworks(void)
{
}

void FogNetworks::updateES(FogNode* es)
{
	this->es = es;
}

FogNode* FogNetworks::getES()
{
	return this->es;
}

//add a fog node to fog networks
void FogNetworks::addFogNode(FogNode* node)
{
    if(node->getNodeID() > maxFogNodeID){
        maxFogNodeID = node->getNodeID();
    }
    if(fognodes.count(node->getNodeID()) > 0){
        FogNode* fognode = fognodes[node->getNodeID()];
        fognode->setCapacity(node->getCapacity());

        //update edge
        //fognode->clearEdge();
        //EV << "node " << fognode->getNodeID() << ":" << fognode->getCapacity() << endl;
        //fognode->updateEdge(destnodeID, transmission_rate, dest_capacity, fognodes)
    }
    else{
        fognodes[node->getNodeID()] = node;
    }
}

void FogNetworks::updateFogNode(NodeMessage nodeMessage){
    //if fog networks has the fog node message
    if(fognodes.count(nodeMessage.nodeID) > 0){
        FogNode* fognode = fognodes[nodeMessage.nodeID];
        //fognode->setCapacity(nodeMessage.capacity);

        //fognode->clearEdge();

        for(int i = 0; i < nodeMessage.neighborNum; i ++){
            FogNode* dest = NULL;
            //find the neighbor
            if(fognodes.count(nodeMessage.neighborNodeID[i]) > 0){
                dest = fognodes[nodeMessage.neighborNodeID[i]];
                bool newEdge = fognode->updateEdge(nodeMessage.neighborNodeID[i], nodeMessage.transmission_rate[i], nodeMessage.neighborCapacity[i], &fognodes);
                dest->updateEdge(fognode->getNodeID(),nodeMessage.transmission_rate[i], fognode->getCapacity(), &fognodes);
                if(newEdge){
                    FogEdge* edge = fognode->getFogEdge(nodeMessage.neighborNodeID[i]);
                    addFogEdge(edge);
                }
            }
            else{//there is no this neighbor in record
                FogNode* dest = new FogNode(nodeMessage.neighborNodeID[i], nodeMessage.neighborCapacity[i]);
                //EV << "node " << dest->getNodeID() << ":" << dest->getCapacity() << endl;
                addFogNode(dest);
                char edgeID[30];
                sprintf(edgeID, "%d-%d", nodeMessage.nodeID, dest->getNodeID());
                FogEdge* edge = new FogEdge(edgeID, fognode, dest, nodeMessage.transmission_rate[i]);

                addFogEdge(edge);
                fognode->addEdge(edge);
                dest->addEdge(edge);
            }

        }

        //fognode->updateEdge(destnodeID, transmission_rate, dest_capacity, fognodes)
    }
    else{// fog networks don't have the fog node message
        //create fog node
        FogNode* node = new FogNode(nodeMessage.nodeID, nodeMessage.capacity);
        //create fog node edge
        for(int i = 0; i < nodeMessage.neighborNum; i ++){
            FogNode* dest = NULL;
            //find the neighbor
            if(fognodes.count(nodeMessage.neighborNodeID[i]) > 0){
                dest = fognodes[nodeMessage.neighborNodeID[i]];
                node->updateEdge(nodeMessage.neighborNodeID[i], nodeMessage.transmission_rate[i], nodeMessage.neighborCapacity[i], &fognodes);
                dest->updateEdge(node->getNodeID(), nodeMessage.transmission_rate[i], node->getCapacity(), &fognodes);
            }
            else{//there is no this neighbor in record
                FogNode* dest = new FogNode(nodeMessage.neighborNodeID[i], nodeMessage.neighborCapacity[i]);
                //EV << "node " << dest->getNodeID() << ":" << dest->getCapacity() << endl;
                addFogNode(dest);
                char edgeID[30];
                sprintf(edgeID, "%d-%d", nodeMessage.nodeID, dest->getNodeID());
                FogEdge* edge = new FogEdge(edgeID, node, dest, nodeMessage.transmission_rate[i]);

                node->addEdge(edge);
                addFogEdge(edge);
                dest->addEdge(edge);
            }

        }
        addFogNode(node);
    }
}

//add a fog edge to fog networks
void FogNetworks::addFogEdge(FogEdge* edge)
{
    FogNode* s = edge->getSource();
    FogNode* d = edge->getDest();
    if(s != NULL && d != NULL){
        distance_table[s->getNodeID()][d->getNodeID()] = 1024.0*8.0 / edge->getTransmission_rate();
        distance_table[d->getNodeID()][s->getNodeID()] = 1024.0*8.0 / edge->getTransmission_rate();
    }
    if(getFogEdge(edge->getSource(), edge->getDest()) == NULL){
        fogedges.push_back(edge);
    }
    else{
        FogEdge* ori_edge = getFogEdge(edge->getSource(), edge->getDest());
        ori_edge->setTransmission_rate(edge->getTransmission_rate());
        //deleteFogEdge(ori_edge);
        //fogedges.push_back(edge);
    }
}

//get FogEdge from two fog node s and d
FogEdge* FogNetworks::getFogEdge(FogNode* s, FogNode* d){

	for(int i = 0; i < fogedges.size(); i ++){
		if(fogedges[i]->compareEdge(s,d)){
			return fogedges[i];
		}
	}
	return NULL;
}

//get the transmission rate between fog node s and fog node d
double FogNetworks::getFogEdgeW(FogNode* s, FogNode* d){
	FogEdge* edge = getFogEdge(s,d);
	if(edge != NULL){
		return edge->getTransmission_rate();
	}
	else{
		return 1e10;
	}
}


//delete fog node in fog networks
void FogNetworks::deleteFogNode(FogNode* node)
{
	if(node == NULL){
		return;
	}
	fognodes.erase(node->getNodeID());
}

FogNode* FogNetworks::getFogNode(int nodeID){
    if(fognodes.count(nodeID) > 0){
        return fognodes[nodeID];
    }
    else{
        return NULL;
    }
}

//delete fog edge in fog networks
void FogNetworks::deleteFogEdge(FogEdge* edge)
{
	for (vector<FogEdge*>::iterator iter = fogedges.begin(); iter != fogedges.end(); iter++)
    {
		//find the FogEdge which has the same id. delete it.
		if((*iter)->getEdgeID() == edge->getEdgeID()){
			fogedges.erase(iter);//.erase(iter);
		}
    }
}

//have no fog node to place. increase fog networks managed
void FogNetworks::increaseHops(){
	hops ++;
}

//too much fog nodes to manage. decrease number of fog nodes managed
void FogNetworks::decreaseHops(){
	hops --;
}

//get monitor hops H
int FogNetworks::getH(){

    return hops;
}

//get average w in edges
double FogNetworks::getAverageCommunicationLatency(){
	return 0;
}

//get average execution speed
double FogNetworks::getAverageExecutionSpeed(){
	//if there is none fog node in fog networks
	if(fognodes.size() == 0){
		return 0;
	}

	double ans = 0;
	int totalSize = 0;
	//accumulation
	map<int,FogNode*>::iterator it;

	it = fognodes.begin();

	while(it != fognodes.end())
	{
	    //it->first;
	    ans += it->second->getThroughput();
	    it ++;
	    totalSize ++;
	}


	ans = ans/totalSize;
	return ans;
}

//get average w
double FogNetworks::getAverageW(){
	// if there is no fog edge in fog networks
	if(fogedges.size() == 0){
		return INIT_W;
	}

	double averageW = 0;
	//accumulation
	for(int i = 0; i < fogedges.size(); i ++){
		averageW += fogedges[i]->getTransmission_rate();
	}

	averageW = averageW / fogedges.size();
	//averageW = 1.0 / (averageW) * 1024.0 * 8;

	return averageW;
}

//judge whether the resource in fog node is empty
bool FogNetworks::isEmpty(){
    map<int, FogNode*>::iterator it;

    it = fognodes.begin();

    //travel to find a fog node has resources
    while(it != fognodes.end())
    {
        //it->first;

        if(it->second->getCapacity() > 0){
            return false;
        }
        it ++;
    }
    return true;
}

map<int, FogNode*> FogNetworks::getFogNodes(){
    return fognodes;
}

//get the left resource
vector<FogNode*> FogNetworks::getResources(){
    vector<FogNode*> res;
    map<int,FogNode*>::iterator it = fognodes.begin();
    while(it != fognodes.end()){
        res.push_back(it->second);
        it ++;
    }
    /*for(int i = 0; i < getMaxFogNodeId(); i ++){
        if(fognodes.count(i) > 0 && fognodes[i]->getCapacity() > 0){
            res.push_back(fognodes[i]);
        }
    }*/
    return res;
}

int FogNetworks::getMaxFogNodeId() const {
    return maxFogNodeID;
}

void FogNetworks::setMaxFogNodeId(int maxFogNodeId) {
    maxFogNodeID = maxFogNodeId;
}

map<int, map<int, double> >& FogNetworks::getDistanceTable() {
    return distance_table;
}

void FogNetworks::setDistanceTable(
        const map<int, map<int, double> >& distanceTable) {
    distance_table = distanceTable;
}

vector<FogEdge*>& FogNetworks::getFogedges(){
    return fogedges;
}
