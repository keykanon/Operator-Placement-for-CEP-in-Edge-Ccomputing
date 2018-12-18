#include "OperatorGraphModel.h"


OperatorGraphModel::OperatorGraphModel(double rt_constraint, int type, int ogID)
{

    this->operatorGraphID = ogID;

    randomOG(type);


    /*switch(type){
    case 0:{
        // accident detect cep

        //init OperatorModel
        OperatorModel* es = new OperatorModel("eventstorage");
        OperatorModel* speed = new OperatorModel("speed");
        OperatorModel* lane = new OperatorModel("lane");
        OperatorModel* accident = new OperatorModel("accident");

        //set source
        source.push_back(es);

        operators.push_back(es);
        operators.push_back(speed);
        operators.push_back(lane);
        operators.push_back(accident);

        //init StreamModel
        StreamModel* es_speed = new StreamModel("es-speed",es, speed);
        StreamModel* es_lane = new StreamModel("es-lane", es, lane);
        StreamModel* speed_acc = new StreamModel("speed-accident", speed, accident);
        StreamModel* lane_acc = new StreamModel("lane-accident", lane, accident);

        streams.push(es_speed);
        streams.push(es_lane);
        streams.push(speed_acc);
        streams.push(lane_acc);

        //add streams to operators
        es->addOutputStreams(es_speed);
        es->addOutputStreams(es_lane);

        speed->addInputStreams(es_speed);
        speed->addOutputStreams(speed_acc);

        lane->addInputStreams(es_lane);
        lane->addOutputStreams(lane_acc);

        accident->addInputStreams(speed_acc);
        accident->addInputStreams(lane_acc);

        //calculate StreamPath
        calStreamPath(accident);
        break;
    }
    case 1:{
        initialOG1();
        break;
    }
    case 3:{
        initialOG3();
        break;
    }
    case 5:{
        initialOG5();
        break;
    }
    case 8:{
        initialOG8();
        break;
    }
    default:
        break;
    }*/
	//printPaths();
	this->response_time_constraints = rt_constraint;
	response_time = 0;
}



//initial operator graph 3
void OperatorGraphModel::initialOG3(){
    /* accident detect cep
    es --> speed --> accident
        \->lane -/>
    */
    //init OperatorModel
    OperatorModel* es = new OperatorModel("eventstorage");
    OperatorModel* speed = new OperatorModel("op1");
    OperatorModel* lane = new OperatorModel("op2");
    OperatorModel* accident = new OperatorModel("op3");

    //set source
    source.push_back(es);

    operators.push_back(es);
    operators.push_back(speed);
    operators.push_back(lane);
    operators.push_back(accident);

    //init StreamModel
    StreamModel* es_speed = new StreamModel("es-op1",es, speed);
    StreamModel* es_lane = new StreamModel("es-op2", es, lane);
    StreamModel* speed_acc = new StreamModel("op1-op3", speed, accident);
    StreamModel* lane_acc = new StreamModel("op2-op3", lane, accident);

    streams.push(es_speed);
    streams.push(es_lane);
    streams.push(speed_acc);
    streams.push(lane_acc);

    //add streams to operators
    es->addOutputStreams(es_speed);
    es->addOutputStreams(es_lane);

    speed->addInputStreams(es_speed);
    speed->addOutputStreams(speed_acc);

    lane->addInputStreams(es_lane);
    lane->addOutputStreams(lane_acc);

    accident->addInputStreams(speed_acc);
    accident->addInputStreams(lane_acc);

    //calculate StreamPath
    calStreamPath(accident);

}


//define compare function
struct operator_cmp{
    bool operator ()(OperatorModel* a,OperatorModel* b){
        return( atoi(a->getOperatorID().c_str()) < atoi(b->getOperatorID().c_str()));//��Сֵ����
    }
};


void OperatorGraphModel::randomOG(int opNum){
    //initial OperatorModel
    OperatorModel* es = new OperatorModel("es");
    //set source
    source.push_back(es);
    operators.push_back(es);
    vector<OperatorModel*> opLeft;
    priority_queue<OperatorModel*, vector<OperatorModel*>, operator_cmp> opToDeal;

    //initial operators
    OperatorModel* op[opNum];
    for(int i = 0; i < opNum; i ++){
        char name[3] = "\0\0";
        sprintf(name, "%d", i);
        op[i] = new OperatorModel(name);
        operators.push_back(op[i]);
        opLeft.push_back(op[i]);
    }

    //the last operator
    opToDeal.push(opLeft.back());
    opLeft.pop_back();


    //initial StreamModel
    while(!opLeft.empty()){
        OperatorModel* opTar = opToDeal.top();

        opToDeal.pop();


        int opIndex = opLeft.size()-1;

        //add link
        char name[64]="\0\0\0";
        sprintf(name, "%s-%s",opLeft[opIndex]->getOperatorID().c_str(), opTar->getOperatorID().c_str());
        StreamModel* link = new StreamModel(name, opLeft[opIndex],opTar);
        streams.push(link);
        opTar->addInputStreams(link);
        opLeft[opIndex]->addOutputStreams(link);

        //change vector of left and to deal
        opToDeal.push(opLeft[opIndex]);
        opLeft.pop_back();

        srand(randSeed);
        for(int i = opLeft.size()-1; i >= 0 ; i --){
            //if(rand() % (opLeft.size()) == 0){
            if(atoi(opLeft[i]->getOperatorID().c_str()) < atoi(opTar->getOperatorID().c_str()) && rand() % (opLeft.size()+1) == 0){
                //add link
                char name[64];
                sprintf(name, "%s-%s",opLeft[i]->getOperatorID().c_str(), opTar->getOperatorID().c_str());
                StreamModel* link = new StreamModel(name, opLeft[i],opTar);
                streams.push(link);
                opTar->addInputStreams(link);

                opLeft[i]->addOutputStreams(link);

                //change vector of left and to deal

                opToDeal.push(opLeft[i]);
                opLeft.erase(opLeft.begin()+i);



            }
        }
        if(rand() % (2) == 0){
            char name[64];
            sprintf(name, "es-%s", opTar->getOperatorID().c_str());
            StreamModel* link = new StreamModel(name, es,opTar);
            streams.push(link);
            opTar->addInputStreams(link);

            es->addOutputStreams(link);
        }

    }

    while(!opToDeal.empty()){
    //for(int j = 0; j < opToDeal.size(); j ++){
        char name[64];
        sprintf(name, "es-%s",opToDeal.top()->getOperatorID().c_str());
        StreamModel* link = new StreamModel(name, es,opToDeal.top());
        streams.push(link);
        opToDeal.top()->addInputStreams(link);
        opToDeal.pop();
        //opToDeal[j]->addInputStreams(link);
        es->addOutputStreams(link);
    }

    //calculate StreamPath
    calStreamPath(op[opNum-1]);
}


OperatorGraphModel::~OperatorGraphModel(void)
{
}

//get StreamPath through operators and streams
void OperatorGraphModel::calStreamPath(OperatorModel* consumer){
	
	stack<StreamModel*> stream_stack;
	stack<StreamPath*> stream_paths_stack;
	//stack<OperatorModel*> operator_stack;
	//operator_stack.push(consumer);

	vector<StreamModel*> input_streams = consumer->getInputStreams();
	//only one operator
	if(input_streams.size() == 0){

	}



	for(int i = 0; i < input_streams.size(); i ++){
	    StreamPath* spath = new StreamPath();
        spath->addOperator(consumer);
        stream_paths_stack.push(new StreamPath(spath));
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
		spath->addOperator(op);


		vector<StreamModel* > input_streams = op->getInputStreams();

		delete(stream_paths_stack.top());
		stream_paths_stack.pop();

		//this path find end.
		if(input_streams.size() == 0){
			//delete stream_paths.pop


			this->stream_paths.push_back(spath);
		}
		else{

			//push this stream_path
		    for(int i = 0; i < input_streams.size(); i ++){
		        stream_paths_stack.push(new StreamPath(spath));
		    }
		}

		//push stream model into stream_stack
		for(int i = 0; i < input_streams.size(); i ++){
			StreamModel* stream = input_streams[i];
			//spath->addStream(stream);
			//OperatorModel* op = stream->getSource();
			stream_stack.push(stream);

		}
	}


	for(int i = 0; i < stream_paths.size(); i ++){
	    vector<OperatorModel*> ops = stream_paths[i]->getOperators();
	    for(int opIndex = ops.size()-2; opIndex >= 0; opIndex --){
	       ops[opIndex]->addEventNum(0, ops[opIndex+1]->getPredictEventNumber()/4);

	    }
	}

	//return this->stream_paths;
}


/*------------------return the total event number of this operator graph-------------
 *
 */
int OperatorGraphModel::getEventNumber(){
    int eventNumber = 0;
    for(int i = 1; i < this->operators.size(); ++ i){
        if(operators[i]->getPredictEventNumber() > eventNumber){
            eventNumber = operators[i]->getPredictEventNumber();
        }
    }
    return eventNumber;
}

//calculate response time of the operator graph
int OperatorGraphModel::calResponseTime(double averageW, double averageThroughput, map<int, map<int, double>>& distable, map<int,int>& eventTable){
    double maxResponseTime = -1;
    int maxPathIndex = 0;
    for(int i = 0; i < stream_paths.size(); i ++){
        double predicted_rt = stream_paths[i]->calResponseTime(averageW, averageThroughput, distable, eventTable );

        //find the max response time stream path index
        if(predicted_rt > maxResponseTime){
            maxResponseTime = predicted_rt;
            maxPathIndex = i;
        }
    }

    predicted_response_time = maxResponseTime;
    return maxPathIndex;
}

//calculate response time of the operator graph
int OperatorGraphModel::getFirstServicedStreamPathIndex(double averageW, double averageThroughput, map<int, map<int, double>>& distable, map<int,int>& eventTable){
    double maxResponseTime = -1;
    this->predicted_response_time = 0;
    int maxPathIndex = -1;
    for(int i = 0; i < stream_paths.size(); i ++){
        double predicted_rt = stream_paths[i]->predictResponseTime(averageW, averageThroughput, distable,eventTable);

        //find the max response time stream path index
        if(predicted_rt > maxResponseTime && !stream_paths[i]->isAllPlaced()){
            maxResponseTime = predicted_rt;
            maxPathIndex = i;
        }
        if(predicted_rt > this->predicted_response_time){
            this->predicted_response_time = predicted_rt;
        }
    }

    return maxPathIndex;
}

double OperatorGraphModel::getPredictedResponseTime(){
    return this->predicted_response_time;
}

//get response time constraint
//ratio = predicted_response_time / response_time_constraints
double OperatorGraphModel::getRTR(){
    //predicted_response_time = 0;
    for(int i = 0; i < this->stream_paths.size(); i ++){
        double prt = stream_paths[i]->getPredictedResponseTime();
        if(prt > predicted_response_time){
            predicted_response_time = prt;
        }
    }
    return predicted_response_time / response_time_constraints;
}

void OperatorGraphModel::setResponseTime(double response_time){
    this->response_time = response_time;
}

double OperatorGraphModel::getResponseTime(){
	return this->response_time;
}

vector<OperatorModel*> OperatorGraphModel::getOperatorModel(){
	return this->operators;
}




void OperatorGraphModel::printPaths(ofstream& out){

	for(int i = 0; i < stream_paths.size(); i ++){
		stream_paths[i]->printPath(out);
	}
	out << "---------------------------------------------------" << endl;
}

//get the number of stream paths
int OperatorGraphModel::getStreamPathSize(){
	return this->stream_paths.size();
}

//judge whether the operators in operator graph placed
bool OperatorGraphModel::isAllPlaced(){
    for(int i = 0; i < operators.size();i ++){
        if(operators[i]->getFogNode() == NULL){
            return false;
        }
    }
    return true;
}

//judge whether the operators need to be replaced
bool OperatorGraphModel::isNeedReplaced(){
    return response_time >= this->response_time_constraints;

}

vector<OperatorModel*> OperatorGraphModel::getSource(){
	return this->source;
}

vector<StreamPath*> OperatorGraphModel::getStreamPath(){
	return this->stream_paths;
}

int OperatorGraphModel::getOperatorGraphId() const {
    return operatorGraphID;
}

void OperatorGraphModel::setOperatorGraphId(int operatorGraphId) {
    operatorGraphID = operatorGraphId;
}
