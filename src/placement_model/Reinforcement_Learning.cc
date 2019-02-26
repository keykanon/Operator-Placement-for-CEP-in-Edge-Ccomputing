#include "Reinforcement_Learning.h"


Reinforcement_Learning::Reinforcement_Learning(
        double lowest, double highest):lowest_input_rate(lowest), highest_input_rate(highest){
}

Reinforcement_Learning::~Reinforcement_Learning(){

}

void Reinforcement_Learning::increase_round_time(){
    roundTime++;
    epsilon -= 1.0/N;
}

void Reinforcement_Learning::setParameter( vector<OperatorGraphModel*>* ogModels, map<int, FogNode*>* fognodes, FogNetworks* fognetworks){
    this->ogModels = ogModels;
    this->fognodes = fognodes;
    this->fognetworks = fognetworks;

    randNumToFogID.clear();
    map<int, FogNode*>::iterator nit = fognodes->begin();
    while(nit != fognodes->end()){
        randNumToFogID.push_back(nit->first);
        ++ nit;
    }
    if(first_init){
        first_init = false;
        initial_policy();
        action = policy.begin()->second;
        apply_action();

        //init state
        map<int, FogNode*>::iterator nit = fognodes->begin();
        while(nit != fognodes->end()){
            if(nit->second == NULL){
                ++ nit;
                continue;
            }
            state.capacity.push_back(nit->second->getCapacity());
            nit++;
        }

        for(int i = 0; i < ogModels->size(); ++ i){
            state.input_rate.push_back( transformInputRate((*ogModels)[i]->getEventNumber()));
        }
    }
}

Action Reinforcement_Learning::getRandomAction(){
    Action action;
    int modNum = fognodes->size();

    int totalOperatorNum = 0;
    for(int i = 0; i < ogModels->size(); ++ i){
        totalOperatorNum += (*ogModels)[i]->getOperatorModel().size();
    }

    vector<int> randToID = randNumToFogID;

    for(int i = 0; i < modNum; ++ i){
        FogNode* fn = (*fognodes)[randToID[i]];
        fn->setCapacity(fn->getOriginCapacity());
    }


    for(int i = 0, ogIndex = 0, opIndex = 0; i < totalOperatorNum; ++ i, ++ opIndex){
        if(opIndex >= (*ogModels)[ogIndex]->getOperatorModel().size()){
            opIndex %= (*ogModels)[ogIndex]->getOperatorModel().size();
            ++ ogIndex;
        }
        if((*ogModels)[ogIndex]->getOperatorModel()[opIndex]->getOperatorID() == "es"){
            continue;
        }
        int randnum = rand() % (modNum);

        //calculate the

        //OperatorGraphModel* og = (*ogModels)[ogIndex];
        OperatorModel* op =  (*ogModels)[ogIndex]->getOperatorModel()[opIndex];
        FogNode* fn = (*fognodes)[randToID[randnum]];
        action.act[op] = fn;
        op->setFogNode(fn);
        fn->setCapacity(fn->getCapacity() - 1);
        if(fn->getCapacity() == 0){
            modNum --;
            randToID.erase(randToID.begin() + randnum);
        }

    }

    this->action = action;

    return action;
}

void Reinforcement_Learning::initial_policy(){
    if(type > 0){

       char mc_filename[1024];
       sprintf(mc_filename, "t%d", N);
       RL_input(mc_filename);
       if(type == 2){
           epsilon = 0;
       }
       return;
    }
    State s;

    map<int, FogNode*>::iterator nit = fognodes->begin();
    while(nit != fognodes->end()){
        s.capacity.push_back(nit->second->getOriginCapacity());
        ++ nit;
    }

    for(int i = 0; i < ogModels->size(); ++ i){
        s.input_rate.push_back(low);
    }

    travel_state(s, 0);
}


void Reinforcement_Learning::apply_action(){
    map<OperatorModel*, FogNode*>::iterator mit = action.act.begin();
    while(mit != action.act.end()){
        mit->first->setFogNode(mit->second);
        mit ++;
    }
}

void Reinforcement_Learning::travel_state(State& s, int ogIndex){
    if(ogIndex ==  ogModels->size()){
        policy[s] = getRandomAction();
        return;
    }
    for(int sInput = 1; sInput <= 3; ++ sInput){
        s.input_rate[ogIndex] = sInput;
        travel_state(s, ogIndex+1);
    }
}


int Reinforcement_Learning::transformInputRate(double i){
    double step_length = (highest_input_rate - lowest_input_rate)/3;
    for(int i = low; i <= high; ++i){
        if(i < lowest_input_rate + step_length * i){
            return i;
        }
    }
    return high;
}

void Reinforcement_Learning::RL(vector<int>& capacity, vector<int>& inputs, vector<double>& response_time){

}

vector<vector<StreamPath*>> Reinforcement_Learning::transformAction(Action& a){
    //set operator on fognode
    map<OperatorModel*, FogNode*>::iterator it = a.act.begin();
    while(it != a.act.end()){
        it->first->setFogNode(it->second);
        ++ it;
    }

    //transform action to placement form
    vector<vector<StreamPath*>> ret;
    for(int i = 0; i < ogModels->size(); ++ i){
        ret.push_back((*ogModels)[i]->getStreamPath());
    }

    return ret;
}

//reinforcement learning update policy
vector<vector<StreamPath*>> Reinforcement_Learning::reinforcement_learning_update(
        vector<int>& capacity, vector<double>& inputs, vector<double>& response_time, int type){
    //transform state and action
        Q_parameter qp;
        qp.s = state;

        state.capacity = capacity;
        if(inputs.size() < ogModels->size() ){
            for(int i = 0; i < ogModels->size(); ++i){
                state.input_rate[i] = middle;
            }
        }
        else{
            for(int i = 0; i < ogModels->size(); ++i){
                state.input_rate[i] = transformInputRate(inputs[i]);
            }
        }

        qp.a = action;

        //travel and reward random: if randNum > epsilon, argmax Q; else random action
        int randNum = rand() % N;

        //choose the argmax a' Q(x, a')
        if(randNum > epsilon * N){
            double maxValue = DBL_MIN;
            map<Action, double>::iterator it = Q[qp.s].begin();
            while(it != Q[qp.s].end()){
                if(it->second > maxValue){
                    maxValue = it->second;
                    action = it->first;
                }
                ++ it;
            }
        }
        //choose a random action
        else{
            getRandomAction();
    //        apply_action();
    //        double averageW = fognetworks->getAverageW();
    //        double averageThroughput = fognetworks->getAverageExecutionSpeed();
    //        map<int, map<int, double>> distable = fognetworks->Floyd();
    //        map<int,int> eventTable;
    //        map<int, FogNode*>::iterator fit = fognodes->begin();
    //        while(fit != fognodes->end()){
    //            eventTable[fit->first] = 0;
    //            fit ++;
    //        }
    //
    //        double avg_predicted_response_time = 0;
    //        for(int i = 0; i < ogModels->size(); ++ i){
    //            avg_predicted_response_time -= (*ogModels)[i]->calResponseTime(averageW, averageThroughput, distable, eventTable);
    //        }
    //        avg_predicted_response_time /= ogModels->size();
    //
    //        reward.r[qp.s][action] = (reward.r[qp.s][action] * reward.count[qp.s][action] + avg_predicted_response_time)
    //                /(reward.count[qp.s][action] +1);
    //        reward.r[qp.s][action] ++;

        }

        //calculate the reward
        switch(type){
        case 0:
            Monte_Carlo_value_update(qp, response_time);
            break;
        case 1:
            Sarsa_Temporal_Difference_update(qp, response_time);
        default:
            break;
        }

        state = qp.s;

        return transformAction(action);
}

/*--------------------------policy input and output-----------------------------
 *
 */
void Reinforcement_Learning::RL_input(string name){
    ifstream in;

    char filename[1024];
    memset(filename, 0 , 1024);
    sprintf(filename, "RL_%s.txt", name.c_str());
    in.open(filename, ios::in);

//------------policy--------------------
    int size = 0;

    in >> size ;

    //travel all state
    for(int i = 0; i < size; ++ i){
        // --------------state---------------------
        State s;

        //input capacity
        int capacity_size  = 0;
        in >> capacity_size;
        for(int cindex = 0; cindex < capacity_size; ++ cindex){
            int capacity = 0;
            in >> capacity;
            s.capacity.push_back(capacity);
        }

        //input input_rate
        int input_rate_size = 0;
        in >> input_rate_size;
        for(int iindex = 0; iindex < input_rate_size; ++ iindex){
            int input_rate = 0;
            in >> input_rate;
            s.input_rate.push_back(input_rate);
        }

        //----------------action------------------
        Action a;
        int action_size = 0;
        in >> action_size;
        for(int aindex = 0; aindex < action_size; ++ aindex){
            int ogIndex = 0, opIndex = 0, fogID = 0;
            in >> ogIndex >> opIndex >> fogID;
            OperatorModel* op = (*ogModels)[ogIndex]->getOperatorModel()[opIndex+1];
            FogNode* node = (*fognodes)[fogID];
            a.act[op] = node;
        }

        //set policy
        policy[s] = a;
    }

    in.close();

}

void Reinforcement_Learning::RL_output(string type){
    if(this->type == 2){
        return;
    }
    ofstream out;
    char filename[1024];
    memset(filename, 0 , 1024);
    sprintf(filename, "RL_%s.txt", type.c_str());
    out.open(filename, ios::out);

//------------policy--------------------
    map<State, Action>::iterator pit = policy.begin();
    out << policy.size() << endl;

    while(pit != policy.end()){
        //-----------State----------------
        out <<  pit->first.capacity.size() << endl;
        for(int i = 0; i < pit->first.capacity.size(); ++ i){
            out << pit->first.capacity[i] << " ";
        }
        out << endl;
        out << pit->first.input_rate.size() << endl;
        for(int i = 0; i < pit->first.input_rate.size(); ++ i){
            out << pit->first.input_rate[i] << " ";
        }
        out << endl;

        //----------Action---------------
        out << pit->second.act.size() << endl;
        map<OperatorModel*, FogNode*>::iterator ait = pit->second.act.begin();
        while(ait != pit->second.act.end()){
            out << ait->first->getOperatorGraphID() << " " << ait->first->getOperatorID().c_str() << " ";
            out << ait->second->getNodeID() << endl;
            ++ ait;
        }
        out << endl;
        ++ pit;
    }

    out.flush();
    out.close();

}

/*-------------------------Monte Carlo Learning Part-----------------------
 *
 */

void Reinforcement_Learning::Monte_Carlo_value_update(Q_parameter& qp, vector<double>& response_time){

    //calculate the reward
    double r = 0;
    for(int i = 0; i < response_time.size(); ++ i){
        r -= response_time[i];
    }
    r /= response_time.size();


    //update Q
    Q[qp.s][qp.a] = (Q[qp.s][qp.a] * Qcount[qp.s][qp.a] + r) / (Qcount[qp.s][qp.a]+1);
    Qcount[qp.s][qp.a] ++;
}





/*-------------------------Temporal Difference Learning Part------------------------
 *
 */
void Reinforcement_Learning::Sarsa_Temporal_Difference_update(Q_parameter& qp, vector<double>& response_time){
    //calculate the reward
    double r = 0;
    for(int i = 0; i < response_time.size(); ++ i){
        r -= response_time[i];
    }
    r /= response_time.size();


    //update Q
    Q[qp.s][qp.a] = Q[qp.s][qp.a] + step_size * (r + gamma * Q[state][action] - Q[qp.s][qp.a]);

}



