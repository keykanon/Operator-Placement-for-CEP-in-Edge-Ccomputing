#include "OperatorPlacementManager.h"

OperatorPlacementManager::OperatorPlacementManager(vector<double> rt_constraints, vector<int> type, vector<int> ogID){
	FogNode* es = new FogNode(0);
	fognetworks = new FogNetworks(es);
	//ogModel = new vector<OperatorGraphModel*>();

	for(int i = 0; i < rt_constraints.size(); i ++){
	    OperatorGraphModel* ogm = new OperatorGraphModel(rt_constraints[i], type[i] , ogID[i]);
	    vector<OperatorModel*> source = ogm->getSource();
        for(int i = 0; i < source.size(); i ++){
            source[i]->setFogNode(es);
        }
	    ogModel.push_back(ogm);
	}

	monitorID = 0;
	monitorIncrease = true;

	int size = 0;
    for(int ogIndex = 0; ogIndex < ogModel.size(); ogIndex ++){
        size += ogModel[ogIndex]->getOperatorModel().size();
    }
    es->setOpNum(size);

	//Wt = 100;
}
OperatorPlacementManager::OperatorPlacementManager(FogNode* es,vector<double> rt_constraints, vector<int> type, vector<int> ogID)
{
    //EV << "es:" << es->getNodeID() << " -c :" << es->getCapacity() << endl;
	fognetworks = new FogNetworks(es);
	//ogModel = new vector<OperatorGraphModel*>();
	for(int i = 0; i < rt_constraints.size(); i ++){
	    OperatorGraphModel* ogm = new OperatorGraphModel(rt_constraints[i], type[i], ogID[i]);
        vector<OperatorModel*> source = ogm->getSource();
        for(int i = 0; i < source.size(); i ++){
            source[i]->setFogNode(es);
        }
        //this->Wt = Wt;
        ogModel.push_back(ogm);
	}
	monitorID = 0;
	monitorIncrease = true;

	int size = 0;
    for(int ogIndex = 0; ogIndex < ogModel.size(); ogIndex ++){
        size += ogModel[ogIndex]->getOperatorModel().size()-1;
    }
    es->setOpNum(size);
}


OperatorPlacementManager::~OperatorPlacementManager(void)
{
	if(fognetworks != NULL){
		delete fognetworks;
	}
	for(int i = 0; i < ogModel.size();i ++){
        delete ogModel[i];
	    ogModel[i] = NULL;
	}
}

//get fog edge's transmission rate
double getTransmissionTime(int EventNum, FogEdge* e){
    return double(EventNum)/e->getTransmission_rate();
}

//get fog edge's end to end delay
double getEndToEndDelay(int EventNum, FogEdge* e){
    //double transTime = EventNum / e->getTransmission_rate();


    return double(EventNum + e->getTotalEventNum())*8.0*1024.0/e->getTransmission_rate();
}


//judge whether every operator has its own edge node
bool OperatorPlacementManager::isAllOperatorPlaced(){
    for(int i = 0; i < ogModel.size(); i ++){
        OperatorGraphModel* ogm = ogModel[i];
        vector<OperatorModel*> ops = ogm->getOperatorModel();
        for(int j = 0; j < ops.size(); j ++){

            //if this operator has not been placed
            if(ops[j]->getFogNode() == NULL){
                return false;
            }
        }
    }
    return true;
}

//judge whether every operators in an operator graph has its own edge node
bool OperatorPlacementManager::isOperatorGraphPlaced(int app_num){
    OperatorGraphModel* ogm = ogModel[app_num];
    vector<OperatorModel*> ops = ogm->getOperatorModel();
    for(int j = 0; j < ops.size(); j ++){

        //if this operator has not been placed
        if(ops[j]->getFogNode() == NULL){
            return false;
        }
    }
    return true;
}



bool isStreamPathsEmpty(vector<StreamPath> stream_path){
	for(int i = 0; i < stream_path.size(); i ++){
		if(!stream_path[i].isEmpty()){
			return false;
		}
	}
	return true;
}



//multiple operator graph placement according to QoS constraints ratio
vector<vector<StreamPath*>> OperatorPlacementManager::getReMultiOperatorGraphPlacement(vector<bool>& replace){

    bool reset = false;
    for(int i = 0; i < ogModel.size(); i ++){
        if(!ogModel[i]->isAllPlaced() || ogModel[i]->isNeedReplaced()){
            reset = true;
            break;
        }
    }

    if(!reset){

        vector<vector<StreamPath*>> ans;
        for(int index = 0; index < ogModel.size(); index ++){
            replace[ogModel[index]->getOperatorGraphId()] = false;
            ans.push_back((ogModel[index]->getStreamPath()));


        }


        return ans;
    }
    resetCapacity();

    double averageW = fognetworks->getAverageW();
    double averageThroughput = fognetworks->getAverageExecutionSpeed();

    map<int, FogNode*> fognodes = fognetworks->getFogNodes();

    map<int, map<int, double>> distable = Floyd();
    map<int,int> eventTable;
    map<int, FogNode*>::iterator fit = fognodes.begin();
    while(fit != fognodes.end()){
        eventTable[fit->first] = 0;
        fit ++;
    }
    /*vector<FogEdge*> edges = fognetworks->getFogedges();
    for(int i = 0; i < edges.size(); i ++){
        int src = edges[i]->getSource()->getNodeID();
        int dest = edges[i]->getDest()->getNodeID();
        map<int,int> et;
        et[src] = 0;
        et[dest] = 0;
        eventTable[src] = et;
        eventTable[dest] = et;
    }*/

    double maxRTR = 0;
    int maxRTR_index = 0;
    vector<double> rt_ratio;
    vector<int> maxPathIndex;

    //initial source operators
    //input operator of every path
    OperatorModel*** in = new OperatorModel**[ogModel.size()];
    for(int i = 0; i < ogModel.size(); i ++){
        in[i] = new OperatorModel*[ogModel[i]->getStreamPath().size()];
        for(int j = 0; j < ogModel[i]->getStreamPathSize(); j ++){
            in[i][j] = ogModel[i]->getSource()[0];
        }
    }


    //initial stream paths
    vector<StreamPath> stream_paths;

    for(int index = 0; index < ogModel.size(); index ++){
        for(int i = 0; i < ogModel[index]->getStreamPathSize(); i ++){
            stream_paths.push_back(*(ogModel[index]->getStreamPath()[i]));
        }
    }

    while(!isAllOperatorPlaced()){

        maxRTR = -1;
        maxRTR_index = 0;
        for(int i = 0; i < ogModel.size(); i ++){
            //calculate max response time
            if(maxPathIndex.size() > i){
                maxPathIndex[i] = ogModel[i]->getFirstServicedStreamPathIndex(averageW, averageThroughput, distable,eventTable);
            }
            else{
                maxPathIndex.push_back(ogModel[i]->getFirstServicedStreamPathIndex(averageW, averageThroughput, distable,eventTable));
            }

            //get the max response time ratio
            if(ogModel[i]->getRTR() > maxRTR && (!ogModel[i]->isAllPlaced()) && maxPathIndex[i] >= 0){
                maxRTR = ogModel[i]->getRTR();
                maxRTR_index = i;
            }
        }

        //calculate the max stream path index
        int maxIndex = 0;
        for(int i = 0; i < maxRTR_index; i ++){
            maxIndex += ogModel[i]->getStreamPath().size();
        }
        maxIndex += maxPathIndex[maxRTR_index];

        //if the operator has been placed
        OperatorModel* op = stream_paths[maxIndex].back();
        if(op->getFogNode() != NULL){

            stream_paths[maxIndex].pop();

        }
        else{//find a fog node to place the operator
            FogNode* inFog = in[maxRTR_index][maxPathIndex[maxRTR_index]]->getFogNode();

            double minTime = 1e12;
            FogNode* destFog = NULL;
            fit = fognodes.begin();
            while(fit != fognodes.end()){
                if(fit->second->getCapacity() <= 0){
                    fit ++;
                    continue;

                }

                if(fit->second ==inFog){
                    destFog = inFog;
                    break;
                }

                double time = 0, transmission_time = 0;
                double eventNum = in[maxRTR_index][maxPathIndex[maxRTR_index]]->getPredictEventNumber();
                //eventNum *= 32 / ogModel.size();
                transmission_time =  distable[inFog->getNodeID()][fit->second->getNodeID()];

               double bandwidth = 1.0 / distable[inFog->getNodeID()][fit->second->getNodeID()];
               double p = ErlangC(fit->second->getOriginCapacity(), (eventTable[fit->second->getNodeID()]+eventNum)/bandwidth);
               if(p > 1 || p < 0 || (eventTable[fit->second->getNodeID()]+eventNum) > bandwidth){
                   p = 1;
                   transmission_time += (((double)(eventTable[fit->second->getNodeID()]+ eventNum) ) * distable[inFog->getNodeID()][fit->second->getNodeID()]);
               }
               else {
                   transmission_time += p * (double)(eventTable[fit->second->getNodeID()]+ eventNum) * distable[inFog->getNodeID()][fit->second->getNodeID()];

               }

               time =transmission_time + 1.0 / fit->second->getThroughput();

               if(time < minTime){
                   minTime = time;
                   destFog = fit->second;
               }

               fit ++;

            }

            //FogNode* destFog = inFog->getSpecialFogNode(maxRTR_index,in[maxRTR_index][maxPathIndex[maxRTR_index]]->getPredictEventNumber(),getEndToEndDelay);
            if(destFog != NULL && destFog->getCapacity() > 0){

                stream_paths[maxIndex].back()->setFogNode(destFog);


                eventTable[destFog->getNodeID()] += in[maxRTR_index][maxPathIndex[maxRTR_index]]->getPredictEventNumber();


                in[maxRTR_index][maxPathIndex[maxRTR_index]] = stream_paths[maxIndex].back();
                destFog->setCapacity(destFog->getCapacity()-1);

            }
            else{
                if(this->fognetworks->getFogNodes().size() > fognetworks->getH() && monitorIncrease){
                    this->fognetworks->increaseHops();
                    monitorIncrease = false;
                    //   return ogModel->getStreamPath();
                }
                break;
            }
        }

    }
    vector<vector<StreamPath*>> ans;
    for(int index = 0; index < ogModel.size(); index ++){

        ans.push_back((ogModel[index]->getStreamPath()));


    }

    for(int index = 0; index < ogModel.size(); index ++){
        ogModel[index]->calResponseTime(averageW, averageThroughput, distable,eventTable);
    }


    return ans;
}

void OperatorPlacementManager::calOperatorGraphResponseTime(){
    double averageW = fognetworks->getAverageW();
    double averageThroughput = fognetworks->getAverageExecutionSpeed();

    map<int, FogNode*> fognodes = fognetworks->getFogNodes();

    map<int, map<int, double>> distable = Floyd();
    map<int,int> eventTable;
    map<int, FogNode*>::iterator fit = fognodes.begin();
    while(fit != fognodes.end()){
        eventTable[fit->first] = 0;
        fit ++;
    }
    for(int index = 0; index < ogModel.size(); index ++){
        vector<OperatorModel*> ops = ogModel[index]->getOperatorModel();
        for(int opIndex = 0; opIndex < ops.size(); ++ opIndex){
            FogNode* fognode = ops[opIndex]->getFogNode();
            if(fognode != NULL){
                eventTable[fognode->getNodeID()] += ops[opIndex]->getPredictEventNumber();
            }
        }
    }

    for(int index = 0; index < ogModel.size(); index ++){
        ogModel[index]->calResponseTime(averageW, averageThroughput, distable,eventTable);
    }
}


vector<vector<StreamPath*>> OperatorPlacementManager::getSelfishMultiOperatorPlacement(){


    vector<vector<StreamPath*>> multi_og_placement;


    double averageW = fognetworks->getAverageW();
    double averageThroughput = fognetworks->getAverageExecutionSpeed();

    map<int, FogNode*> fognodes = fognetworks->getFogNodes();

    map<int, map<int, double>> distable =  Floyd();
    //map<int, map<int,int>> eventTable;
    map<int,int> eventTable;
    map<int, FogNode*>::iterator fit = fognodes.begin();
    while(fit != fognodes.end()){
        eventTable[fit->first] = 0;
        fit ++;
    }
    /*vector<FogEdge*> edges = fognetworks->getFogedges();
    for(int i = 0; i < edges.size(); i ++){
        int src = edges[i]->getSource()->getNodeID();
        int dest = edges[i]->getDest()->getNodeID();
        map<int,int> et;
        et[src] = 0;
        et[dest] = 0;
        eventTable[src] = et;
        eventTable[dest] = et;
    }*/

    int maxOgIndex = -1;


    for(int i = 0; i < ogModel.size(); i ++){
        vector<StreamPath*> og_placement ;

        multi_og_placement.push_back(og_placement);
    }

    vector<bool> placed;
    for(int i = 0; i < ogModel.size(); i ++){
        placed.push_back(false);
    }

    for(int i = 0; i < ogModel.size() ;i ++){
        double maxRTR = -1;
        for(int j = 0; j < ogModel.size(); j ++){
            //calculate max response time
            if(!placed[j]){
                //ogModel[j]->getFirstServicedStreamPathIndex(averageW, averageThroughput, distable);

                ogModel[j]->calResponseTime(averageW, averageThroughput, distable,eventTable);
                //get the max response time ratio
                if(ogModel[j]->getRTR() > maxRTR  ){
                    maxRTR = ogModel[j]->getRTR();
                    maxOgIndex = j;
                }
            }
        }

        placed[maxOgIndex] = true;
        vector<StreamPath*> og_placement = getPlacement(maxOgIndex, eventTable);

        multi_og_placement[maxOgIndex] = (og_placement);
    }

    for(int index = 0; index < ogModel.size(); index ++){

        ogModel[index]->calResponseTime(averageW, averageThroughput, distable,eventTable);


    }

    return multi_og_placement;
    //vector<vector<StreamPath*>> ans;

//return answer
/*    for(int index = 0; index < ogModel.size(); index ++){

        ans.push_back((ogModel[index]->getStreamPath()));

    }
    return ans;*/
}

vector<StreamPath*> OperatorPlacementManager::getPlacement(int index, map<int,int>& eventTable)
{
    //if the operator graph is placed and need not to be replaced,just return
    if(ogModel[index]->isAllPlaced() && !ogModel[index]->isNeedReplaced()){

        return ogModel[index]->getStreamPath();

    }

    this->resetOperatorGraph(index);
	//init parameters

    double averageW = fognetworks->getAverageW();
    double averageThroughput = fognetworks->getAverageExecutionSpeed();


    map<int, map<int, double>> distable = fognetworks->getDistanceTable();

    vector<StreamPath> stream_paths;
    for(int i = 0; i < ogModel[index]->getStreamPathSize(); i ++){
        stream_paths.push_back(*(ogModel[index]->getStreamPath()[i]));
    }
	//input operator of every path
	OperatorModel** in = new OperatorModel*[ogModel[index]->getStreamPathSize()];
	FogNode** f = new FogNode*[ogModel[index]->getStreamPathSize()];
	double* t = new double[ogModel[index]->getStreamPathSize()];
	for(int i = 0; i < ogModel[index]->getStreamPathSize(); i ++){
		in[i] = ogModel[index]->getSource()[0];
	}

	//give each operator a placement
	while(!isStreamPathsEmpty(stream_paths)){
		double maxTime = -1;
		int maxIndex = 0;
		for(int i = 0; i < stream_paths.size();i ++){
			//get the best fog node and path with max response time
			if(stream_paths[i].getOperators().size() == 0){
				f[i] = NULL;
				t[i] = -1;
			}
			else{
			    FogNode* inFog = in[i]->getFogNode();

				f[i] = inFog->getSpecialFogNode(index,in[i]->getPredictEventNumber(),getEndToEndDelay);
				t[i] = stream_paths[i].predictResponseTime(averageW, averageThroughput, distable, eventTable);
			}

			//find the path index with max response time
			if(t[i] > maxTime){
				maxTime = t[i];
				maxIndex = i;
			}
		}
		if(stream_paths[maxIndex].back()->getFogNode() != NULL){

		    stream_paths[maxIndex].pop();
		}
		else{
		    if(f[maxIndex] != NULL){

				stream_paths[maxIndex].back()->setFogNode(f[maxIndex]);
				in[maxIndex] = stream_paths[maxIndex].back();
				f[maxIndex]->setCapacity(f[maxIndex]->getCapacity()-1);

			}
            else{
                if(this->fognetworks->getFogNodes().size() > fognetworks->getH() && monitorIncrease){
                    this->fognetworks->increaseHops();
                    monitorIncrease = false;
                    //   return ogModel->getStreamPath();
                }
                break;
            }
		}
	}

	//****************migration aware algorithm*******
	/*vector<Migration> migs = migrationAware();

	if(migs.size() > 0){
        placement = migratePlacement(placement, migs);
        for(int i = 0; i < ogModel->getOperatorModel().size(); i ++){
            ogModel->getOperatorModel()[i]->setFogNode(placement[i].getFogNode());
        }
	}*/

	/*for(int i = 0; i < placement.size(); i ++){
	    FogNode* f = placement[i].getFogNode();
	    if(f != NULL){
	        placement[i].getFogNode()->setCapacity(placement[i].getFogNode()->getCapacity()+1);
	    }
    }*/
	placement.clear();
    for(int i = 0; i < ogModel[index]->getOperatorModel().size(); i ++){

        placement.push_back(*(ogModel[index]->getOperatorModel()[i]));
    }

	delete []in;
	delete []f;
	return ogModel[index]->getStreamPath();
}

//simple operator placement using selfish policy
vector<vector<StreamPath*>> OperatorPlacementManager::getSimpleGreedyPlacement(vector<bool>& replace){
    vector<vector<StreamPath*>> ans;
    for(int i = 0; i < ogModel.size(); i ++){
        vector<StreamPath*> p = getSimpleGreedyPlacement(i, replace);
        ans.push_back(p);
    }

    return ans;
}



/*----------------------getSimpleGreedyPlacement----------------------------
 * get the nearest fog node to es to an operator
 * return the stream path of the cep graph
 */
vector<StreamPath*> OperatorPlacementManager::getSimpleGreedyPlacement(int index,vector<bool>& replace){
//error index
    if(ogModel.size() <= index){
        EV << "error index in get Simple Greedy Placement." << endl;
        vector<StreamPath*> ans;
        return ans;
    }

    //if the operator graph is placed and need not to be replaced,just return
    if(ogModel[index]->isAllPlaced()){
        replace[ogModel[index]->getOperatorGraphId()] = false;
        return ogModel[index]->getStreamPath();

    }

    resetOperatorGraph(index);
    map<int, map<int, double>> distable = Floyd();
    map<int, FogNode*> fognodes = fognetworks->getFogNodes();

    FogNode* es = fognetworks->getES();

    vector<OperatorModel*> ops = ogModel[index]->getOperatorModel();
    for(int i = 1; i < ops.size(); i ++){
    //for(int i = ops.size()-1; i > 0; i --){
        //FogNode* neighborNode = fognetworks->getES()->getSpecialFogNode(index,1000,getTransmissionTime);



                //calculate the capacity of operator graph
        int opCapacity = ops[i]->getResourceRequire();

        //if the event source edge node has capacity to host all operator graph
        if(es->getCapacity() >= opCapacity){
            ops[i]->setFogNode(es);

            es->setCapacity(es->getCapacity() - opCapacity);
        }
        else{
            if(ops[i]->getFogNode() != NULL){
                continue;
            }
            bool back = false;
            OperatorModel* tmpOp = ops[i];
            while(tmpOp->getInputStreams()[0]->getSource()->getFogNode() == NULL){
                back = true;
                tmpOp = tmpOp->getInputStreams()[0]->getSource();
            }
            int sourceID = tmpOp->getInputStreams()[0]->getSource()->getFogNode()->getNodeID();
            map<int, double> dest_distance = distable[sourceID];
            double minD = 1e12;
            int minDIndex = -1;

            //travel all node in distance table
            map<int, double>::iterator dit = dest_distance.begin();
            while(dit != dest_distance.end()){
                if(fognodes[dit->first]->getNodeID() == sourceID && fognodes[dit->first]->getCapacity() >= opCapacity){
                    minDIndex = dit->first;
                    break;
                }
                if(dit->second < minD && fognodes[dit->first]->getCapacity() >= opCapacity){
                    minD = dit->second;
                    minDIndex = dit->first;
                }
                dit ++;
            }
            //find the best node
            if(minDIndex != -1){
                fognodes[minDIndex]->setCapacity(fognodes[minDIndex]->getCapacity() - opCapacity);

                tmpOp->setFogNode(fognodes[minDIndex]);

            }
            else{
                if(monitorIncrease){
                    fognetworks->increaseHops();
                    monitorIncrease = false;
                }
                break;
            }
            if(back){
                i --;
            }
        }

    }

    return  ogModel[index]->getStreamPath();

}


//response time greedy operator placement using selfish policy
vector<vector<StreamPath*>> OperatorPlacementManager::getResponseTimeGreedyPlacement(vector<bool>& replace){
    vector<vector<StreamPath*>> ans;
    for(int i = 0; i < ogModel.size(); i ++){
        vector<StreamPath*> p = getResponseTimeGreedyPlacement(i, replace);
        ans.push_back(p);
    }

    return ans;
}

/*----------------------getResponseTimeGreedyPlacement----------------------------
 * get fog node with the minimum delay to es
 * return the stream path of the cep graph
 */
vector<StreamPath*> OperatorPlacementManager::getResponseTimeGreedyPlacement(int index,vector<bool>& replace){
//error index
    if(ogModel.size() <= index){
        EV << "error index in get Simple Greedy Placement." << endl;
        vector<StreamPath*> ans;
        return ans;
    }

    //if the operator graph is placed and need not to be replaced,just return
    if(ogModel[index]->isAllPlaced() && !ogModel[index]->isNeedReplaced()){
        replace[ogModel[index]->getOperatorGraphId()] = false;
        return ogModel[index]->getStreamPath();
    }

    resetOperatorGraph(index);
    map<int, map<int, double>> distable = Floyd();
    map<int, FogNode*> fognodes = fognetworks->getFogNodes();
    map<int, int> eventTable;

    FogNode* es = fognetworks->getES();

    vector<OperatorModel*> ops = ogModel[index]->getOperatorModel();
    for(int i = 1; i < ops.size(); i ++){
    //for(int i = ops.size()-1; i > 0; i --){
        //FogNode* neighborNode = fognetworks->getES()->getSpecialFogNode(index,1000,getTransmissionTime);
        //calculate the capacity of operator graph
        int opCapacity = ops[i]->getResourceRequire();

        //if the event source edge node has capacity to host all operator graph
        if(es->getCapacity() >= opCapacity){
            ops[i]->setFogNode(es);

            es->setCapacity(es->getCapacity() - opCapacity);
        }
        else{
            if(ops[i]->getFogNode() != NULL){
                continue;
            }

            //find the edge node of the frontier operator deployed
            bool back = false;
            OperatorModel* tmpOp = ops[i];
            while(tmpOp->getInputStreams()[0]->getSource()->getFogNode() == NULL){
                back = true;
                tmpOp = tmpOp->getInputStreams()[0]->getSource();
            }

            int sourceID = tmpOp->getInputStreams()[0]->getSource()->getFogNode()->getNodeID();

            map<int, double> dest_distance = distable[sourceID];
            double minD = 1e12, delay = 0.0;
            int minDIndex = -1;

            //travel all node in distance table
            map<int, double>::iterator dit = dest_distance.begin();
            while(dit != dest_distance.end()){

                //the edge node host the frontier operator
                if(fognodes[dit->first]->getNodeID() == sourceID && fognodes[dit->first]->getCapacity() >= opCapacity){
                    minDIndex = dit->first;
                    break;
                }

                //calculate the delay
                delay =  distable[sourceID][dit->first] + 0.01;

                double bandwidth = 1.0 / distable[sourceID][dit->first];
                double p = ErlangC(fognodes[dit->first]->getOriginCapacity(), eventTable[dit->first]/bandwidth);
                if(p > 1 || p < 0 || eventTable[dit->first] > bandwidth){
                    p = 1;
                    delay += (((double)eventTable[dit->first] ) * distable[sourceID][dit->first]);
                }
                else {
                    delay += p * distable[sourceID][dit->first];
                }

                delay += 1.0 / fognodes[dit->first]->getThroughput();

                //find the edge node with the minimum delay
                if(delay < minD && fognodes[dit->first]->getCapacity() >= opCapacity){
                    minD = delay;
                    minDIndex = dit->first;
                }
                dit ++;
            }
            //find the best node
            if(minDIndex != -1){
                fognodes[minDIndex]->setCapacity(fognodes[minDIndex]->getCapacity() - opCapacity);

                tmpOp->setFogNode(fognodes[minDIndex]);

            }
            else{
                if(monitorIncrease){
                    fognetworks->increaseHops();
                    monitorIncrease = false;
                }
                break;
            }
            if(back){
                i --;
            }
        }

    }

    return  ogModel[index]->getStreamPath();

}

//operator graph placement using policy selfless
/*vector<vector<StreamPath*>> OperatorPlacementManager::getSelflessOperatorPlacement(){
    for(int i = 0; i < ogModel.size(); i ++){
        if(ogModel[i]->isAllPlaced() && !ogModel[i]->isNeedReplaced()){

        }
    }
}*/

/* Returns true if there is a path from source 's' to sink 't' in
  residual graph. Also fills parent[] to store the path */
bool bfs(const int V, int* rGraph[], int s, int t, int parent[])
{
    // Create a visited array and mark all vertices as not visited
    bool visited[V];
    memset(visited, 0, sizeof(visited));

    // Create a queue, enqueue source vertex and mark source vertex
    // as visited
    queue <int> q;
    q.push(s);
    visited[s] = true;
    parent[s] = -1;

    // Standard BFS Loop
    while (!q.empty())
    {
        int u = q.front();
        q.pop();

        for (int v=0; v<V; v++)
        {
            if (visited[v]==false && rGraph[u][v] > 0)
            {
                q.push(v);
                parent[v] = u;
                visited[v] = true;
            }
        }
    }

    // If we reached sink in BFS starting from source, then return
    // true, else false
    return (visited[t] == true);
}

// Returns the maximum flow from s to t in the given graph
int fordFulkerson(const int V, int* graph[], int s, int t)
{
    int u, v;

    // Create a residual graph and fill the residual graph with
    // given capacities in the original graph as residual capacities
    // in residual graph
    int* rGraph[V];
    for(int i = 0; i < V; i ++){
        rGraph[i] = new int[V];
    }
                    // Residual graph where rGraph[i][j] indicates
                     // residual capacity of edge from i to j (if there
                     // is an edge. If rGraph[i][j] is 0, then there is not)
    for (u = 0; u < V; u++)
        for (v = 0; v < V; v++)
             rGraph[u][v] = graph[u][v];

    int parent[V];  // This array is filled by BFS and to store path

    int max_flow = 0;  // There is no flow initially

    // Augment the flow while tere is path from source to sink
    while (bfs(V, rGraph, s, t, parent))
    {
        // Find minimum residual capacity of the edges along the
        // path filled by BFS. Or we can say find the maximum flow
        // through the path found.
        int path_flow = INT_MAX;
        for (v=t; v!=s; v=parent[v])
        {
            u = parent[v];
            path_flow = min(path_flow, rGraph[u][v]);
        }

        // update residual capacities of the edges and reverse edges
        // along the path
        for (v=t; v != s; v=parent[v])
        {
            u = parent[v];
            rGraph[u][v] -= path_flow;
            rGraph[v][u] += path_flow;
        }

        // Add path flow to overall flow
        max_flow += path_flow;
    }


    for(int i = 0; i < V; i ++){
        delete []rGraph[i] ;
    }
    // Return the overall flow
    return max_flow;
}


int** OperatorPlacementManager::minLatencyFlow(double averageD, vector<int> Vs, vector<int> Vt, vector<int> Vs_phi, vector<int> Vt_phi, double epsilon ){
    //Construct a flor network with latency weighted edges
    vector<FogNode*> fognodes = fognetworks->getResources();
    const int vN = fognodes.size();

    int* G[vN];
    for(int i = 0; i < vN; i ++){
        G[i] = new int[vN];
    }

    for(int i = 0; i < vN; i ++){
        for(int j = 0; j < vN; j ++){
            G[i][j] = 0;
        }
    }

    for(int i = 0; i < Vs_phi.size(); i ++){
        for(int j = 0; j < Vt_phi.size(); j ++){
            int u = (Vs_phi[i] > Vt_phi[j])?Vt_phi[i]:Vs_phi[j];
            G[Vs[i]][Vt[j]] = u;
        }
    }

    int** flow = new int*[vN];
    for(int i = 0; i < vN; i ++){
        flow[i] = new int[vN];
    }

    for(int i = 0; i < vN; i ++){
        for(int j = 0; j < vN; j ++){
            flow[i][j] = 0;
        }
    }

    //vector<vector<int>> flow;
    for(int s = 0; s < Vs.size(); s ++){

        for(int t = 0; t < Vt.size(); t ++){
            //if(s == t){
            //    flow_t.push_back(0);
            //    continue;
            //}
            int maxFlow = fordFulkerson(vN, G, Vs[s], Vt[t]);
            flow[Vs[s]][Vt[t]] = maxFlow;
        }

    }


    for(int i = 0; i < vN; i ++){
        delete []G[i];
    }
    return flow;


}


//Cloudlet Load Balance Algorithm
vector<vector<StreamPath*>> OperatorPlacementManager::getLoadBalance(double theta, double epsilon, double delta, vector<bool>& replace){
    //resetCapacity();
    bool reset = false;
    for(int i = 0; i < ogModel.size(); i ++){
        if(!ogModel[i]->isAllPlaced() ){
            reset = true;
            break;
        }
    }

    if(!reset){

        vector<vector<StreamPath*>> ans;
        for(int index = 0; index < ogModel.size(); index ++){
            replace[ogModel[index]->getOperatorGraphId()] = false;
            ans.push_back((ogModel[index]->getStreamPath()));
        }
        return ans;
    }




    map<int, map<int, double>> distable = Floyd();

    FogNode* es = fognetworks->getES();
    int esIndex = -1;
    vector<FogNode*> fognodes = fognetworks->getResources();

    for(int i = 0; i < fognodes.size();  i++){
        if(fognodes[i]->getNodeID() == es->getNodeID()){
            esIndex = i;
            break;
        }
    }

    vector<double> Ti;

    int maxTIndex = 0, minTIndex = 0;

    int size = 0;
    for(int ogIndex = 0; ogIndex < ogModel.size(); ogIndex ++){
        size += ogModel[ogIndex]->getOperatorModel().size();
    }
    //es->setOpNum(size);
    //es->setBandwidth(es->getOriBandwidth() - INIT_BANDWIDTH_COST * size);

    double maxT = -1, minT = 1e12;
    for(int i = 0; i < fognodes.size(); i ++){

        double t = AverageResponseTimeOfTasks(fognodes[i]);
        Ti.push_back(t);
        if(t > maxT){
            maxT = t;
            maxTIndex = i;
        }
        if(t < minT){
            minT = t;
            minTIndex= i;
        }
    }

    double Delta = 1e12, averageD;

    //find the inital value of averageD using binary search
    while(abs(Delta) > delta){
        vector<int> Vs,Vt, Vs_phi, Vt_phi;
        averageD = (maxT + minT)/2.0;
        int total_phi = 0;
        for(int i = 0; i < fognodes.size(); i ++){
            if(Ti[i] > averageD){
                Vs.push_back(i);
                //double lambda = (fognodes[i]->getOriBandwidth()-fognodes[i]->getBandwidth());
                //Vs_phi.push_back(lambda/INIT_BANDWIDTH_COST);

                int opN = fognodes[i]->getOpNum();
                int phi = opN - fognodes[i]->getOriginCapacity();
                for(; phi < opN; phi ++){
                    fognodes[i]->setOpNum(opN - phi);
                    if(abs(averageD - AverageResponseTimeOfTasks(fognodes[i]))/averageD <= epsilon){
                        Vs_phi.push_back(phi);
                        total_phi += phi;
                        break;
                    }
                }
                if(phi >= opN){
                    Vs_phi.push_back(0);
                }
                fognodes[i]->setOpNum(opN);

            }
        }

        for(int i = 0; i < fognodes.size(); i ++){
            if(Ti[i] <= averageD){
                Vt.push_back(i);
                //double lambda = (fognodes[i]->getBandwidth());
                //Vt_phi.push_back(lambda/INIT_BANDWIDTH_COST);
                int opN = fognodes[i]->getOpNum();
                int phi_down = fognodes.size()- Vs.size();
                int phi = (total_phi/ phi_down) +1 ;
                for(; phi < size; phi ++){
                    fognodes[i]->setOpNum(opN + phi);
                    if(abs(averageD - AverageResponseTimeOfTasks(fognodes[i]))/averageD <= epsilon){
                        Vt_phi.push_back(phi);
                        //total_phi -= phi;
                        break;
                    }
                }
                if(phi >= size){
                    Vt_phi.push_back(1);
                }
                fognodes[i]->setOpNum(opN);
            }
        }

        Delta = 0;
        for(int i = 0; i < Vs_phi.size(); i ++){
            Delta += Vs_phi[i];
        }
        for(int i = 0; i < Vt_phi.size(); i ++){
            Delta -= Vt_phi[i];
        }

        if(Delta > 0){
            minT = averageD;
        }
        else{
            maxT = averageD;
        }
    }


    double averageD_ = 1e12;
    //adjust the value of averageD for the network delay between cloudlets
    /*for(int i = 0; i < fognodes.size(); i ++){
        if(Ti[i] > averageD){
            Vs.push_back(i);
        }
        else{
            Vt.push_back(i);
        }
    }*/


    int** flow = NULL;
    vector<double> totalDj;
    double minDj = 1e12;
    int minDjIndex = -1;
    for(int i = 0; i < fognodes.size(); i ++){
        totalDj.push_back(1e12);
    }

    vector<int> lastVs, lastVt;

    while(abs(averageD - averageD_) > theta){
        int total_phi = 0;
        vector<int> Vs,Vt,Vs_phi, Vt_phi;
        for(int i = 0; i < fognodes.size(); i ++){
            if(Ti[i] > averageD){
                //double lambda = (fognodes[i]->getOriBandwidth()-fognodes[i]->getBandwidth());
                Vs.push_back(i);
                int opN = fognodes[i]->getOpNum();
                int phi = opN - fognodes[i]->getOriginCapacity();
                //int phi = 1;
                for(; phi < opN; phi ++){
                    fognodes[i]->setOpNum(opN - phi);
                    if(abs(averageD - AverageResponseTimeOfTasks(fognodes[i]))/averageD <= epsilon){
                        Vs_phi.push_back(phi);
                        total_phi += phi;
                        break;
                    }
                }
                if(phi >= opN){
                    Vs_phi.push_back(0);
                }
                fognodes[i]->setOpNum(opN);
                //Vs_phi.push_back(lambda/INIT_BANDWIDTH_COST );
            }
        }
        for(int i = 0; i < fognodes.size(); i ++){
            if(Ti[i] <= averageD){
                //double lambda = (fognodes[i]->getOriBandwidth()-fognodes[i]->getBandwidth());
                //Vt_phi.push_back(lambda/INIT_BANDWIDTH_COST);
                Vt.push_back(i);
                int opN = fognodes[i]->getOpNum();
                int phi_down = fognodes.size()-Vs.size();
                int phi = (total_phi/ phi_down) +1;

                //int phi = 1;
                for(; phi < size; phi ++){
                    fognodes[i]->setOpNum(opN + phi);
                    if(abs(averageD - AverageResponseTimeOfTasks(fognodes[i]))/averageD <= epsilon){
                        Vt_phi.push_back(phi);
                        //total_phi -= phi;
                        break;
                    }
                }
                if(phi >= size){
                    Vt_phi.push_back(1);
                }
                fognodes[i]->setOpNum(opN);

            }
        }
        if(flow != NULL){
            for(int i = 0; i < fognodes.size(); i ++){
                delete []flow[i];
            }
            delete []flow;
        }
        flow = minLatencyFlow(averageD, Vs, Vt, Vs_phi, Vt_phi, epsilon);

        EV << endl << "flow : "  << endl;
        for(int i = 0; i < fognodes.size(); i ++){
            for(int j = 0; j < fognodes.size(); j ++){
                EV << flow[i][j] << " ";
            }
            EV << endl;
        }
        EV << endl;

        double maxDj = -1;
        minDj = 1e12;

        for(int j = 0; j < Vt.size(); j ++){
            double tnet = 0;
            //for(int i = 0; i < Vs.size(); i ++){

            //double up = double(flow[esIndex][Vt[j]]) * 1024.0*8;
            int tarIndex = fognodes[Vt[j]]->getNodeID();
            //double down = distable[es->getNodeID()][tarIndex];

            tnet =  double(flow[esIndex][Vt[j]]) * distable[es->getNodeID()][tarIndex];


            //}
            double Dj = Ti[Vt[j]] + tnet;
            totalDj[Vt[j]] = Dj;

            if(Dj > maxDj){
                maxDj = Dj;
            }

            if(Dj < minDj){
                minDj = Dj;
                minDjIndex = Vt[j];
            }
        }

        averageD_ = maxDj;
        averageD = (averageD + averageD_)/2.0;

        lastVs = Vs;
        lastVt = Vt;
    }


    double minDelay = 1e12;
    int tarIndex = -1;
    es->setOpNum(0);
    for(int i = 0; i < ogModel.size(); i ++){
        vector<OperatorModel*> ops = ogModel[i]->getOperatorModel();
        vector<int> fogUse;
        for(int fogIndex = 0; fogIndex < fognodes.size(); fogIndex ++){
            fogUse.push_back(0);
        }
        for(int opIndex = 1; opIndex < ops.size(); opIndex ++){

            if(ops[opIndex]->getFogNode() == NULL){

                int esSize = es->getCapacity()/(ogModel.size()-i) ;
               if(es->getCapacity() > 0 && (fogUse[esIndex] < esSize)){
                   ops[opIndex]->setFogNode(es);
                   es->setCapacity(es->getCapacity() - ops[opIndex]->getResourceRequire());
                   es->setOpNum(es->getOpNum()+1);
                   fogUse[esIndex] = fogUse[esIndex] + 1;
               }
               else{

                   int sourceID = ops[opIndex]->getInputStreams()[0]->getSource()->getFogNode()->getNodeID();
                   int sourceIndex = -1;
                   for(int i = 0; i < fognodes.size();  i++){
                       if(fognodes[i]->getNodeID() == sourceID){
                           sourceIndex = i;
                           break;
                       }
                   }

                   for(int j = 0; j < lastVt.size(); j ++){
                       totalDj[lastVt[j]] = Ti[lastVt[j]] + distable[sourceID][fognodes[lastVt[j]]->getNodeID()];
                   }
                   minDelay = 1e12;
                   tarIndex = -1;
                   for(int j = 0; j < fognodes.size(); j ++){
                       if(flow[esIndex][j] > 0 && fognodes[j]->getCapacity() > 0 && fogUse[j] < (flow[esIndex][j]/(ogModel.size()-i))+1){
                           if(sourceID == fognodes[j]->getNodeID()){
                               tarIndex = j;
                               break;
                           }
                           if(totalDj[j] < minDelay){
                               tarIndex = j;
                               minDelay = totalDj[j];

                           }
                       }
                   }

                   if(tarIndex == -1){
                       if(this->fognetworks->getFogNodes().size() > fognetworks->getH() && monitorIncrease){
                           this->fognetworks->increaseHops();
                           monitorIncrease = false;

                       }
                       break;
                   }

                   ops[opIndex]->setFogNode(fognodes[tarIndex]);
                   fognodes[tarIndex]->setCapacity(fognodes[tarIndex]->getCapacity() - ops[opIndex]->getResourceRequire());
                   flow[esIndex][tarIndex] --;
                   fognodes[tarIndex]->setOpNum(fognodes[tarIndex]->getCapacity()+1);
                   fogUse[tarIndex] = fogUse[tarIndex] + 1;
               }
            }
        }
    }

    /*double minDelay = 1e12;
    int tarIndex = -1;
    //FogNode* es = fognetworks->getES();
    for(int i = 0; i < ogModel.size(); i ++){
        vector<OperatorModel*> ops = ogModel[i]->getOperatorModel();
        for(int opIndex = 1; opIndex < ops.size(); opIndex ++){
            if(ops[opIndex]->getFogNode() == NULL){
                if(es->getCapacity() > 0){
                    ops[opIndex]->setFogNode(es);
                    es->setCapacity(es->getCapacity() - ops[opIndex]->getResourceRequire());
                }
                else{
                    minDelay = 1e12;
                    tarIndex = -1;
                    for(int j = 0; j < fognodes.size(); j ++){
                        if(flow[es->getNodeID()][j] > 0 && fognodes[j]->getCapacity() > 0){
                            if(totalDj[j] < minDelay){
                                tarIndex = j;
                                minDelay = totalDj[j];

                            }
                        }
                    }

                    ops[opIndex]->setFogNode(fognodes[tarIndex]);
                    fognodes[tarIndex]->setCapacity(fognodes[tarIndex]->getCapacity() - ops[opIndex]->getResourceRequire());
                    flow[es->getNodeID()][tarIndex] --;

                }
            }

        }
    }*/


    vector<vector<StreamPath*>> ans;
    for(int i = 0; i < ogModel.size(); i ++){
        ans.push_back(ogModel[i]->getStreamPath());
    }

    return ans;
}



double OperatorPlacementManager::AverageResponseTimeOfTasks(FogNode* node){
    int n = node->getOriginCapacity();
    double lambda = node->getOpNum()+0.328;//(node->getOriBandwidth()-node->getBandwidth());

    double throughput = node->getOriBandwidth() / INIT_BANDWIDTH_COST;
    double ans = ErlangC(n, lambda/throughput);
    ans = ans/(n* node->getThroughput() - lambda);
    ans += 1 / throughput;
    return ans;
}

double OperatorPlacementManager::fac(int n){
    double ans = 1;
    for(int i = 1; i <= n; i ++){
        ans = ans* i;
    }
    return ans;
}

//calculate ErlangC
double OperatorPlacementManager::ErlangC(int n, double c){
    double A = c* n;
    double ans = 1;

    ans = pow(A, n) / fac(n) /(1.0-c) ;

    //calculate denominator
    double denominator = 0;
    for(int k = 0; k < n; k ++){
        denominator += pow(A,k)/fac(k);
    }
    denominator += ans;

    ans /= denominator;

    return ans;

}


//compare fognode
bool compFogNode(const FogNode* a, const FogNode* b){
    if(a->getCapacity() == b->getCapacity()){
        return a->getBandwidth() < b->getBandwidth();
    }
    return (a->getCapacity() < b->getCapacity());
}

void QuickSort(vector<FogNode*>& fognodes, int begin, int end){
    if (begin < end){
        FogNode* key = fognodes[begin];
        int i = begin, j = end;
        while (i < j){
            while (i < j && !compFogNode(fognodes[j],key)) j--;
            if (i < j) fognodes[i++] = fognodes[j];
            while (i < j && !compFogNode(key,fognodes[i])) i++;
            if (i < j) fognodes[j--] = fognodes[i];
        }
        fognodes[i] = key;
        QuickSort(fognodes, begin, i - 1);
        QuickSort(fognodes, i + 1, end);
    }

}

//get multiple resource aware placement
vector<vector<StreamPath*>> OperatorPlacementManager::getSelfResourceAwarePlacement(){
    vector<vector<StreamPath*>> placement;
    for(int i = 0; i < ogModel.size(); i ++){
        placement.push_back(getResourceAwarePlacement(i));
    }
    return placement;
}

//get resource aware placement
vector<StreamPath*> OperatorPlacementManager::getResourceAwarePlacement(int index){
    vector<FogNode*> fognodes = fognetworks->getResources();
    vector<OperatorModel*> ops = ogModel[index]->getOperatorModel();

    //if the operator graph is placed and need not to be replaced,just return
    if(ogModel[index]->isAllPlaced() && !ogModel[index]->isNeedReplaced()){

        return ogModel[index]->getStreamPath();

    }
    resetOperatorGraph(index);

    if(fognodes.size() == 0){
        return ogModel[index]->getStreamPath();
    }

    int low = 0, high = fognodes.size()-1;
    QuickSort(fognodes,low, high);

    for(int start = 1; start < ops.size(); start ++){

        int index = getLowerBound(fognodes,ops[start],low, high);
        if(index != -1){
            ops[start]->setFogNode(fognodes[index]);
            fognodes[index]->setBandwidth(fognodes[index]->getBandwidth() - ops[start]->getPredictEventNumber());
            fognodes[index]->setCapacity(fognodes[index]->getCapacity() - ops[start]->getResourceRequire());
            QuickSort(fognodes,low, high);
            low = index+1;
        }
        else{
            ops[start]->setFogNode(fognodes[high]);
        }
    }
    return ogModel[index]->getStreamPath();
}


int OperatorPlacementManager::getLowerBound(vector<FogNode*> edgeNodes, OperatorModel* op, int low, int high){
    int length = edgeNodes.size();
    int mid = (low + high)/2;

    //find the lower bound of edge node satisfy the resource require.
    while(true){
        FogNode* node = edgeNodes[mid];
        if(compare(node,op)){
            high = mid-1;
            if(high < low){
                return mid;
            }
        }
        else{
            low = mid + 1;
            if(low > high){
                return (mid < length-1)?mid+1:-1;
            }
        }
        mid = (low+high)/2;
    }
}


//compare the fog node resource and the operator require resource
bool OperatorPlacementManager::compare(FogNode* v, OperatorModel* op){
    if(v->getCapacity() >= op->getResourceRequire() && v->getBandwidth() >= op->getPredictEventNumber()){
        return true;
    }
    return false;
}





//Simulated Annealing operator graph placement using policy selfish
vector<vector<StreamPath*>> OperatorPlacementManager::getSelfishSAOperatorGraphPlacement(double alpha, double T){
    vector<vector<StreamPath*>> placement;

    Floyd();

    //get placement in the order of ogModel vector
    for(int i = 0; i < ogModel.size(); i ++){
        vector<StreamPath*> p;

        //if ogModel[i] need not to be replaced
        if(ogModel[i]->isAllPlaced() && !ogModel[i]->isNeedReplaced()){
            p = ogModel[i]->getStreamPath();
        }
        else{//get simulated anealing placement
            p = this->getSimulatedAnealingPlacement(i, alpha, T);
        }

        placement.push_back(p);
    }

    return placement;
}





//------------------------------------------------------------------
//Simulated Annealing operator graph placement using policy reschedule
vector<vector<StreamPath*>> OperatorPlacementManager::getReSAOperatorGraphPlacement(double alpha, double T){


    vector<vector<StreamPath*>> ans;
    vector<OperatorModel*> ops;
    vector<int> ops_ogIndex;

    this->getSelfishMultiOperatorPlacement();

    map<int, FogNode*> fognodes = fognetworks->getFogNodes();
    vector<FogNode*> res = fognetworks->getResources();
    //map<int, FogNode*> fognodes = fognetworks->getFogNodes();
    map<int, map<int, double>> distable = Floyd();
    map<int,int> eventTable;
        map<int, FogNode*>::iterator fit = fognodes.begin();
        while(fit != fognodes.end()){
            eventTable[fit->first] = 0;
            fit ++;
        }
    /*map<int, map<int, int>> eventTable;
    vector<FogEdge*> edges = fognetworks->getFogedges();
    for(int i = 0; i < edges.size(); i ++){
        int src = edges[i]->getSource()->getNodeID();
        int dest = edges[i]->getDest()->getNodeID();
        map<int,int> et;
        et[src] = 0;
        et[dest] = 0;
        eventTable[src] = et;
        eventTable[dest] = et;
    }*/

    //------get random placement as initial placement
    /*for(int i = 0; i < ogModel.size(); i ++){
        for(int j = 1; j < ogModel[i]->getOperatorModel().size(); j ++){
            ops.push_back(ogModel[i]->getOperatorModel()[j]);
            ops_ogIndex.push_back(i);
        }
    }*/

    //while T is larger than end condition
    while(T > 1){
        for(int i = 1; i < ops.size(); i ++){
            srand((unsigned int)clock());

            if(res.size() > 0){
                int ranId = rand() % res.size();//fognetworks->getMaxFogNodeId();

            //if(fognodes.count(ranId) > 0 && fognodes[ranId]->getCapacity() > 0){
                FogNode* fognode = res[ranId];
                if(ops[i]->getFogNode() == NULL ){
                    ops[i]->setFogNode(res[ranId]);
                    res[ranId]->setCapacity(res[ranId]->getCapacity()-1);
                    if(res[ranId]->getCapacity() == 0){
                        res.erase(res.begin() + ranId);
                    }
                }
                else{
                    //calculate the origin placement response time
                    FogNode* oriNode = ops[i]->getFogNode();
                    ogModel[ops_ogIndex[i]]->calResponseTime(fognetworks->getAverageW(), fognetworks->getAverageExecutionSpeed(), distable, eventTable);
                    double oriRt = ogModel[ops_ogIndex[i]]->getPredictedResponseTime();

                    //calculate the new placement response time
                    ops[i]->setFogNode(fognode);
                    ogModel[ops_ogIndex[i]]->calResponseTime(fognetworks->getAverageW(), fognetworks->getAverageExecutionSpeed(), distable, eventTable);
                    double newRt = ogModel[ops_ogIndex[i]]->getPredictedResponseTime();

                    srand((unsigned int)clock());
                    if(newRt < oriRt && exp( (oriRt-newRt) / T) < (double)rand()/(double)RAND_MAX){
                        ops[i]->setFogNode(oriNode);

                    }
                    else{
                        if(oriNode->getCapacity() == 0){
                            res.push_back(oriNode);
                        }
                        oriNode->setCapacity(oriNode->getCapacity()+1);
                        fognode->setCapacity(fognode->getCapacity()-1);
                        if(fognode->getCapacity() == 0){
                            res.erase(res.begin()+ranId);
                        }
                    }
                }
            }
            else{
                //the fognetworks have no resources
                //if(fognetworks->isEmpty()){
                    if(monitorIncrease){
                        fognetworks->increaseHops();
                        monitorIncrease = false;
                    }
                    for(int ogIndex = 0; ogIndex < ogModel.size(); ogIndex ++){
                        ans.push_back(ogModel[ogIndex]->getStreamPath());
                    }
                    return ans;
                //}

            }
        }


        T = T* alpha;
    }

    for(int ogIndex = 0; ogIndex < ogModel.size(); ogIndex ++){
        ans.push_back(ogModel[ogIndex]->getStreamPath());
    }

    return ans;
}






//------------------------------------------------------------------


//------------------------Simulated Anealing Placement---------------
vector<StreamPath*> OperatorPlacementManager::getSimulatedAnealingPlacement(int index, double alpha, double T){

    OperatorGraphModel* og = ogModel[index];
    vector<OperatorModel*> ops= og->getOperatorModel();
    map<int, FogNode*> fognodes = fognetworks->getFogNodes();
    map<int, int> eventTable;

    map<int, FogNode*>::iterator fit = fognodes.begin();
    while(fit != fognodes.end()){
        eventTable[fit->first] = 0;
        fit ++;
    }
    /*vector<FogEdge*> edges = fognetworks->getFogedges();
    for(int i = 0; i < edges.size(); i ++){
        int src = edges[i]->getSource()->getNodeID();
        int dest = edges[i]->getDest()->getNodeID();
        map<int,int> et;
        et[src] = 0;
        et[dest] = 0;
        eventTable[src] = et;
        eventTable[dest] = et;
    }*/

    getPlacement(index,eventTable);

    map<int, map<int, double>> distable = fognetworks->getDistanceTable();
    //map<int, FogNode*> fognodes = fognetworks->getFogNodes();
    vector<FogNode*> res = fognetworks->getResources();



    //while T is larger than end condition
    while(T > 1){
        for(int i = 1; i < ops.size(); i ++){
            srand((unsigned int)clock());
            if(res.size() > 0){
                int ranId = rand() % res.size();

            //if(fognodes.count(ranId) > 0 && fognodes[ranId]->getCapacity() > 0){
                FogNode* fognode = res[ranId];
                if(ops[i]->getFogNode() == NULL ){
                    ops[i]->setFogNode(res[ranId]);
                    res[ranId]->setCapacity(res[ranId]->getCapacity()-1);
                    if(res[ranId]->getCapacity() == 0){
                        res.erase(res.begin() + ranId);
                    }
                }
                else{
                    //calculate the origin placement response time
                    FogNode* oriNode = ops[i]->getFogNode();
                    og->calResponseTime(fognetworks->getAverageW(), fognetworks->getAverageExecutionSpeed(), distable,eventTable);
                    double oriRt = og->getPredictedResponseTime();

                    //calculate the new placement response time
                    ops[i]->setFogNode(fognode);
                    og->calResponseTime(fognetworks->getAverageW(), fognetworks->getAverageExecutionSpeed(), distable,eventTable);
                    double newRt = og->getPredictedResponseTime();

                    srand((unsigned int)clock());
                    if(newRt < oriRt && exp( (oriRt-newRt) / T) < (double)rand()/(double)RAND_MAX){
                        ops[i]->setFogNode(oriNode);

                    }
                    else{
                        if(oriNode->getCapacity() == 0){
                            res.push_back(oriNode);
                        }
                        oriNode->setCapacity(oriNode->getCapacity()+1);
                        fognode->setCapacity(fognode->getCapacity()-1);
                        if(fognode->getCapacity() == 0){
                            res.erase(res.begin()+ranId);
                        }
                    }
                }
            }
            else{
                //the fognetworks have no resources
                //if(fognetworks->isEmpty()){
                if(monitorIncrease){
                    fognetworks->increaseHops();
                    monitorIncrease = false;
                }
                return og->getStreamPath();
                //}

            }
        }


        T = T* alpha;
    }
    return og->getStreamPath();
}




//place the operator graph in an edge node
vector<vector<StreamPath*>> OperatorPlacementManager::getOveralGraphPlacement(vector<bool>& replace){

    bool migrate = false;
    for(int i = 0; i < ogModel.size(); i ++){
        if(!ogModel[i]->isAllPlaced()){
            migrate = true;

            break;
        }
    }

    if(migrate){
        resetCapacity();
    }
    else{
        vector<vector<StreamPath*>> placement;
        for(int i = 0; i < ogModel.size(); i ++){
            replace[ogModel[i]->getOperatorGraphId()] = false;
            placement.push_back(ogModel[i]->getStreamPath());
        }
        return placement;
    }

    map<int, map<int, double>> distable = Floyd();
    map<int,FogNode*> fognodes = fognetworks->getFogNodes();

    for(int i = 0; i < ogModel.size(); i ++){
        vector<OperatorModel*> ops = ogModel[i]->getOperatorModel();
        FogNode* es = ogModel[i]->getSource()[0]->getFogNode();

        //calculate the capacity of operator graph
        int opCapacity = 0;
        for(int opIndex = 1; opIndex < ops.size(); opIndex ++){
            opCapacity += ops[opIndex]->getResourceRequire();
        }
        //if the event source edge node has capacity to host all operator graph
        if(es->getCapacity() >= opCapacity){
            for(int opIndex = 1; opIndex < ops.size(); opIndex ++){
                ops[opIndex]->setFogNode(es);
            }
            es->setCapacity(es->getCapacity() - opCapacity);
        }
        else{
            map<int, double> dest_distance = distable[es->getNodeID()];
            double minD = 1e12;
            int minDIndex = -1;

            //travel all node in distance table
            map<int, double>::iterator dit = dest_distance.begin();
            while(dit != dest_distance.end()){
                if(dit->second < minD && fognodes[dit->first]->getCapacity() >= opCapacity){
                    minD = dit->second;
                    minDIndex = dit->first;
                }
                dit ++;
            }
            //find the best node
            if(minDIndex != -1){
                fognodes[minDIndex]->setCapacity(fognodes[minDIndex]->getCapacity() - opCapacity);
                for(int opIndex = 1; opIndex < ops.size(); opIndex ++){
                    ops[opIndex]->setFogNode(fognodes[minDIndex]);
                }
            }
            else{
                break;
            }
        }

    }

    vector<vector<StreamPath*>> placement;
    for(int i = 0; i < ogModel.size(); i ++){
        placement.push_back(ogModel[i]->getStreamPath());
    }
    return placement;
}













//------------------------random placement--------------------
vector<StreamPath*> OperatorPlacementManager::getRandomPlacement(int index){
    vector<OperatorModel*> ops = ogModel[index]->getOperatorModel();
    map<int,FogNode*> fognodes = fognetworks->getFogNodes();

    //place operators in random fog node
    for(int i = 1; i < ops.size(); i ++){
        int ranId = rand() % fognetworks->getMaxFogNodeId();
        if(fognodes.count(ranId) > 0 && fognodes[ranId]->getCapacity() > 0){
            FogNode* fognode = fognodes[ranId];
            ops[i]->setFogNode(fognode);
        }
    }
    return ogModel[index]->getStreamPath();
}

//-------------------------------------------------------------------------


double OperatorPlacementManager::predictResponseTime(StreamPath* stream_path){
	double averageW = fognetworks->getAverageW();
	double averageThroughput = fognetworks->getAverageExecutionSpeed();

	double tp = 0;

	vector<OperatorModel*> operators = stream_path->getOperators();
	//source node
	FogNode* fs = fognetworks->getES();
	
	for(int i = operators.size()-1; i >= 0; i --){
		//predict event number for every operator
		operators[i]->predictEventNumber(1);

		FogNode* fd = operators[i]->getFogNode();
		if(fs != NULL && fd != NULL){
			tp += operators[i]->getPredictEventNumber() / fognetworks->getFogEdgeW(fs,fd) +
			        operators[i]->getPredictEventNumber() / fd->getThroughput();
		}
		else{
			tp += operators[i]->getPredictEventNumber() / averageW +
			        operators[i]->getPredictEventNumber()/ averageThroughput;
		}
		fs = fd;

	}

	stream_path->setPredictedResponseTime(tp);
	
	
	return tp;
}

//init Operator Graph and Fog networks to test
void OperatorPlacementManager::test_init(){
	FogNode* pre = NULL;
	FogNode* newn = NULL;
	for(int i = 1; i < 4; i ++){
		pre = newn;
		newn = new FogNode(i, rand()%3+1);
		newn->setThroughput(1000+rand()%1000);
		fognetworks->addFogNode(newn);

		char edgeID[30];
		if(pre != NULL){
			sprintf(edgeID, "%d-%d", pre->getNodeID(), newn->getNodeID());

			FogEdge* edge = new FogEdge(edgeID, pre, newn, rand()%3+0.8);
			pre->addEdge(edge);
			newn->addEdge(edge);
			fognetworks->addFogEdge(edge);
		}
	
		sprintf(edgeID, "%d-%d", fognetworks->getES()->getNodeID(), newn->getNodeID());
		FogEdge* edge = new FogEdge(edgeID, fognetworks->getES(), newn , rand()%4 + 1);
		fognetworks->getES()->addEdge(edge);
		newn->addEdge(edge);
		fognetworks->addFogEdge(edge);
	}
}

//update fog networks
void OperatorPlacementManager::updateFogNode(NodeMessage nodeMessage){
    fognetworks->updateFogNode(nodeMessage);

}

void OperatorPlacementManager::updateCapacity(int fognodeID, int capacity){
    for(int ogIndex = 0; ogIndex < ogModel.size(); ogIndex ++){
        vector<OperatorModel*> ops = ogModel[ogIndex]->getOperatorModel();
        for(int opIndex = 1; opIndex < ops.size(); opIndex ++){
            if(ops[opIndex]->getFogNode() != NULL && ops[opIndex]->getFogNode()->getNodeID() == fognodeID){
                resetOperatorGraph(ogIndex);
                break;
            }
        }
    }

    //int decreaseCapacity = fognetworks->getFogNode(fognodeID)->getOriginCapacity() - capacity;
    //fognetworks->getFogNode(fognodeID)->setOriginCapacity(capacity);

    fognetworks->getFogNode(fognodeID)->setCapacity(capacity);

}

//get monitor hops
int OperatorPlacementManager::getH(){
    //if(fognetworks != NULL){
    return    fognetworks->getH();
    //}
    //else{
     //   printf( "error: fog networks == NULL\r\n" );
    //}
}

OperatorGraphModel* OperatorPlacementManager::getOperatorGraph(int index){
    for(int i = 0; i < ogModel.size(); i ++){
        if(ogModel[i]->getOperatorGraphId() == index){
            return ogModel[i];
        }
    }

    return NULL;
}

int OperatorPlacementManager::getOperatorGraphNum(){
    return ogModel.size();
}

FogNode* OperatorPlacementManager::getES(){
    return fognetworks->getES();
}

int OperatorPlacementManager::getFogNodeNum(){
    return fognetworks->getFogNodes().size();
}

void OperatorPlacementManager::printPlacement(ofstream& out){
    for(int index = 0; index < ogModel.size(); index ++){
        vector<StreamPath*> sp = ogModel[index]->getStreamPath();
        for(int i = 0; i < sp.size(); i ++){
            sp[i]->printPath(out);
        }
        out << "--------------------------------------------" << endl;
    }
}

//calculate the difference between two placements
vector<Migration> OperatorPlacementManager::diffPlacements(vector<OperatorModel> ori_place, vector<OperatorModel*> new_place){
    vector<Migration> mig;
    for(int i = 0; i < ori_place.size(); i ++){
        if(strcmp(ori_place[i].getOperatorID().c_str(), new_place[i]->getOperatorID().c_str()) == 0){
            if(ori_place[i].getFogNode()->getNodeID() != new_place[i]->getFogNode()->getNodeID()){
                Migration m(new_place[i], ori_place[i].getFogNode(), new_place[i]->getFogNode(), 10);
                mig.push_back(m);
            }
        }
    }

    return mig;
}

//migrate origin placement to new placement
//input origin placement and migration
//return the new placement
vector<OperatorModel> OperatorPlacementManager::migratePlacement(vector<OperatorModel> placement, vector<Migration> mig){
    for(int i = 0; i < placement.size(); i++){
        for(int j = 0; j < mig.size(); j ++){
            if(strcmp(placement[i].getOperatorID().c_str(),
                    mig[j].getOp()->getOperatorID().c_str()) == 0 ){
                if(placement[i].getFogNode()->getNodeID() == mig[j].getSource()->getNodeID()){
                    placement[i].setFogNode(mig[j].getDest());
                    break;
                }
            }
        }
    }
    return placement;
}

//predict the response time
double OperatorPlacementManager::predictResponseTime(vector<OperatorModel> placement){
    vector<StreamPath*> stream_paths;

    stack<StreamModel*> stream_stack;
    stack<StreamPath*> stream_paths_stack;
    //stack<OperatorModel*> operator_stack;
    //operator_stack.push(consumer);

    vector<StreamModel*> input_streams = placement[0].getInputStreams();
    //only one operator

    StreamPath* spath = new StreamPath();
    spath->addOperator(&placement[0]);
    stream_paths_stack.push(spath);
    for(int i = 0; i < input_streams.size(); i ++){
        StreamModel* stream = input_streams[i];
        //spath->addStream(stream);
        stream_stack.push(stream);
    }
    while(!stream_stack.empty()){
        //OperatorModel* tar_op = operator_stack.top();
        //top of stream_paths and stream_stack
        StreamPath* spath = new StreamPath(stream_paths_stack.top());
        StreamModel* stream = stream_stack.top();

        //stream_stack.pop
        stream_stack.pop();

        //add streamModel and operatorModel
        spath->addStream(stream);
        OperatorModel* op = stream->getSource();
        for(int i = 0; i < placement.size(); i ++){
            if(strcmp(op->getOperatorID().c_str(),placement[i].getOperatorID().c_str()) == 0){
                spath->addOperator(&placement[i]);
                break;
            }
        }

        vector<StreamModel* > input_streams = op->getInputStreams();

        //this path find end.
        if(input_streams.size() == 0){
            //delete stream_paths.pop
            delete(stream_paths_stack.top());
            stream_paths_stack.pop();

            stream_paths.push_back(spath);
        }
        else{
            //push this stream_path
            stream_paths_stack.push(spath);
        }
        //push stream model into stream_stack
        for(int i = 0; i < input_streams.size(); i ++){
            StreamModel* stream = input_streams[i];
            //spath->addStream(stream);
            //OperatorModel* op = stream->getSource();
            stream_stack.push(stream);

        }
    }

    double maxTp = -1;
    for(int i = 0; i < stream_paths.size(); i ++){
        double tp = stream_paths[i]->getPredictedResponseTime();
        if(tp > maxTp){
            maxTp = tp;
        }
    }

    return maxTp;
}

//migration aware algorithm
vector<Migration> OperatorPlacementManager::migrationAware(int index){
// ********************Migration Aware Part********************

    vector<Migration> mig = diffPlacements(placement, ogModel[index]->getOperatorModel());
    if(mig.size() == 0){
        return mig;
    }
    Migration_Time **mtArray = new Migration_Time*[mig.size()];
    for(int i = 0; i < mig.size(); i ++){
        mtArray[i] = new Migration_Time[Wt];
    }
    for(int j = 0; j <= Wt; j ++){
        if(j < mig[0].getTime()){
            Migration_Time mt;
            mtArray[0][j] = mt;
        }
        else{
            vector<Migration> tempM;
            tempM.push_back(mig[0]);
            double tempT = predictResponseTime(migratePlacement(placement,tempM));
            Migration_Time mt(placement,tempM, tempT);
            mtArray[0][j] = mt;
        }
    }
    for(int i = 1; i < mig.size(); i ++){
        for(int j = 1; j < Wt; j ++){
            if(j < mig[i].getCost()){
                mtArray[i][j] = mtArray[i-1][j];
            }
            else{
                vector<Migration> tempM = mtArray[i-1][j].getMig();
                tempM.push_back(mig[i]);
                double tempT = predictResponseTime(migratePlacement(placement,tempM));
                if(mtArray[i-1][j].getTime() > tempT){
                    mtArray[i][j] = mtArray[i-1][j];
                }
                else{
                    Migration_Time mt(placement,tempM, tempT);
                    mtArray[i][j] = mt;
                }
            }
        }
    }

    delete mtArray[(int)Wt];
    return mig;
    //******************************************************
}

//get minimum transmission delay fog node
int OperatorPlacementManager::getMinimumDelayFogNodeID(int app_num,int nodeID){

    FogNode* fognode =  fognetworks->getFogNode(nodeID);
    if(fognode == NULL){
        return -1;
    }
    FogNode* neighborNode = fognode->getSpecialFogNode(app_num,1000,getTransmissionTime);
    if(neighborNode == NULL){
        return -1;
    }
    //EV << neighborNode->getNodeID() << endl;

    return neighborNode->getNodeID();
}

//decrease the capacity of a fog node
void OperatorPlacementManager::decreaseCapacity(int nodeID){
    FogNode* fognode = this->fognetworks->getFogNode(nodeID);
    if(fognode != NULL){
        fognode->setCapacity(fognode->getCapacity() -1);
        //EV << "dec fognode " << nodeID << ": " << fognode->getCapacity() << endl;
    }
}

//decrease the capacity of a fog node
void OperatorPlacementManager::increaseCapacity(int nodeID){
    FogNode* fognode = this->fognetworks->getFogNode(nodeID);
    if(fognode != NULL){
        fognode->setCapacity(fognode->getCapacity() + 1);
        //EV << "inc fognode " << nodeID << ": " << fognode->getCapacity() << endl;
    }
}

//reset ES
void OperatorPlacementManager::resetES(int index,int fognodeID){
    //update es
    FogNode* es;
    if(fognetworks->getFogNode(fognodeID) == NULL){
        es = new FogNode(fognodeID);
        fognetworks->updateES(es);
        fognetworks->addFogNode(es);
    }
    else{
        es = fognetworks->getFogNode(fognodeID);
       fognetworks->updateES(es);
    }
    //update source to es map
    for(int i = 0; i < this->ogModel[index]->getSource().size(); i ++){
        this->ogModel[index]->getSource()[i]->setFogNode(es);
    }

}

//update event number
void OperatorPlacementManager::updateEventNumber(int app_num, int operatorType,int time, int eventNum){
    getOperatorGraph(app_num)->getOperatorModel()[operatorType]->addEventNum(time,eventNum);
}

//update response time of app_num
void OperatorPlacementManager::updateResponsetime(int app_num, double response_time){
    getOperatorGraph(app_num)->setResponseTime(response_time);
}

//reset the fog node of one operator graph which placed in
void OperatorPlacementManager::resetOperatorGraph(int index){
    //if ogModel[index] need not to be replaced
    /*if(ogModel[index]->isAllPlaced() && !ogModel[index]->isNeedReplaced()){
        return;
    }*/

    OperatorGraphModel* og = ogModel[index];
    vector<OperatorModel*> ops = og->getOperatorModel();

    //delete placement of every operators in ogMode[index] except event storage
    for(int j = 1; j < ops.size(); j ++){
        FogNode* fognode = ops[j]->getFogNode();
        if(fognode != NULL){
            //increase the capacity of the fog node
            fognode->setCapacity(fognode->getCapacity()+ops[j]->getResourceRequire());
            fognode->setBandwidth(fognode->getBandwidth() + ops[j]->getPredictEventNumber());
        }
        //delete placement;
        ops[j]->setFogNode(NULL);
    }

    vector<FogEdge*> edges = fognetworks->getFogedges();
    for(int i = 0; i < edges.size(); i ++){
        edges[i]->clearEventNum(index);
    }

}

//reset every fognode's capacity
void OperatorPlacementManager::resetCapacity(){
    map<int,FogNode*>::iterator it;
    map<int,FogNode*> fognodes = fognetworks->getFogNodes();
    vector<FogEdge*> fogedges = fognetworks->getFogedges();

    for(int i = 0; i < fogedges.size(); ++ i){
        fogedges[i]->setTotalEventNum(0);
    }

    it = fognodes.begin();

    //reset node's capacity and its edges
    while(it != fognodes.end())
    {
        //it->first;
        FogNode* node = it->second;
        node->resetCapacity();
        it ++;
    }

    //reset cep operators' placement
    for(int i = 0; i < ogModel.size(); i ++){
        vector<OperatorModel*> ops = ogModel[i]->getOperatorModel();
        for(int j = 1; j < ops.size(); j ++){
            ops[j]->setFogNode(NULL);
        }
    }


}

//use Floyd algorithm to calculate distance table
map<int, map<int, double>> OperatorPlacementManager::Floyd(){
    //Floyd algorithm
    map<int, FogNode*>::iterator kit, iit, jit;
    map<int, FogNode*> fognodes = fognetworks->getFogNodes();
    map<int, map<int, double>> distable = fognetworks->getDistanceTable();
    kit = fognodes.begin();
    iit = fognodes.begin();
    jit = fognodes.begin();

    while(iit != fognodes.end()){
        jit = fognodes.begin();
        while(jit != fognodes.end()){
            if(!(distable.count(iit->first) > 0 &&
                    distable[iit->first].count(jit->first) > 0)){
                map<int, double> coli = distable[iit->first];
                coli[jit->first] = 1e12;
                distable[iit->first] = coli;

                map<int, double> colj = distable[jit->first];
                colj[iit->first] = 1e12;
                distable[jit->first] = colj;

            }

            jit ++;
        }

        iit ++;
    }

    iit = fognodes.begin();
    jit = fognodes.begin();

    while(kit != fognodes.end())
    {
        iit = fognodes.begin();
        while(iit != fognodes.end()){
            jit = fognodes.begin();
            while(jit != fognodes.end()){
                //if(distable.count(iit->first) > 0 &&
                 //       distable[iit->first].count(kit->first) > 0 &&
                 //       distable.count(kit->first) > 0 &&
                 //       distable[kit->first].count(jit->first) > 0){
                    if(distable.count(iit->first) > 0 &&
                            distable[iit->first].count(jit->first)>0){
                        if(distable[iit->first][jit->first] > distable[iit->first][kit->first] + distable[kit->first][jit->first]){
                            map<int, double> coli = distable[iit->first];
                            coli[jit->first] = distable[iit->first][kit->first] + distable[kit->first][jit->first];
                            distable[iit->first] = coli;

                            map<int, double> colj = distable[jit->first];
                            colj[iit->first] = distable[iit->first][kit->first] + distable[kit->first][jit->first];;
                            distable[jit->first] = colj;
                            //distable[iit->first][jit->first] = distable[iit->first][kit->first] + distable[kit->first][jit->first];
                        }
                    }
                    else{
                        map<int, double> coli = distable[iit->first];
                        coli[jit->first] = distable[iit->first][kit->first] + distable[kit->first][jit->first];
                        distable[iit->first] = coli;

                        map<int, double> colj = distable[jit->first];
                        colj[iit->first] = distable[iit->first][kit->first] + distable[kit->first][jit->first];;
                        distable[jit->first] = colj;
                        //distable[iit->first][jit->first] = distable[iit->first][kit->first] + distable[kit->first][jit->first];
                        //distable[jit->first][iit->first]= distable[iit->first][kit->first] + distable[kit->first][jit->first];
                    }
                //}

                jit ++;
            }
            iit ++;
        }
        //it->first;
        //it->second;
        kit ++;
    }
    return distable;
}

int OperatorPlacementManager::getMonitorId() const {
    return monitorID;
}

void OperatorPlacementManager::setMonitorId(int monitorId) {
    monitorID = monitorId;
}

bool OperatorPlacementManager::isMonitorIncrease() const {
    return monitorIncrease;
}

void OperatorPlacementManager::setMonitorIncrease(bool monitorIncrease) {
    this->monitorIncrease = monitorIncrease;
}
