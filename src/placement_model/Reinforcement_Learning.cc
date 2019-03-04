#include "Reinforcement_Learning.h"


Reinforcement_Learning::Reinforcement_Learning(
        double lowest, double highest):lowest_input_rate(lowest), highest_input_rate(highest){

}

Reinforcement_Learning::~Reinforcement_Learning(){

}

//更新策略
void Reinforcement_Learning::update_policy(){
    double maxValue = DBL_MIN;
    map<State, map<Action, double>>::iterator qit = Q.begin();
    while(qit != Q.end()){
        maxValue = DBL_MIN;
        map<Action, double>::iterator it = qit->second.begin();
        while(it != qit->second.end()){
            if(it->second > maxValue){
                maxValue = it->second;
                policy[qit->first] = it->first;
            }
            ++ it;
        }
        qit ++;
    }
}

//一次训练完成
//在1/N之前，每次减少1/N，之后为 1/训练次数。保障每次都有一定概率随机，搜索新的空间
//对每一个状态都设置了epsilon，避免数据不均衡导致部分状态没有探索过程，部分探索过度。
void Reinforcement_Learning::increase_round_time(){
    roundTime++;
    epsilon -= 1.0/N;


    update_policy();
}

//初始化设置参数
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

//得到一个随机的放置
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

//初始化策略
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
        s.input_rate.push_back(lowest);
    }

    travel_state(s, 0);
}

//在模型中设置放置指针
void Reinforcement_Learning::apply_action(){
    map<OperatorModel*, FogNode*>::iterator mit = action.act.begin();
    while(mit != action.act.end()){
        mit->first->setFogNode(mit->second);
        mit ++;
    }
}

//递归遍历所有的状态
void Reinforcement_Learning::travel_state(State& s, int ogIndex){
    if(ogIndex ==  ogModels->size()){
        //set action
        Action a = getRandomAction();
        policy[s] = a;

        //update reward threshold
        double avg_predicted_response_time = predict_response_time();
        reward_threshold[s] = {1,avg_predicted_response_time};

        state_epsilon[s] = 0.9;

        //update Q
        Q[s][a] = -avg_predicted_response_time;

        return;
    }
    for(int sInput = lowest; sInput <= highest; ++ sInput){
        s.input_rate[ogIndex] = sInput;
        travel_state(s, ogIndex+1);
    }
}

//把输入速率从double值转换为对应的输入速率区间
int Reinforcement_Learning::transformInputRate(double i){
    double step_length = (highest_input_rate - lowest_input_rate)/(highest - lowest);
    for(int i = lowest; i <= highest; ++i){
        if(i < lowest_input_rate + step_length * i){
            return i;
        }
    }
    return highest;
}

void Reinforcement_Learning::RL(vector<int>& capacity, vector<int>& inputs, vector<double>& response_time){

}

//返回给operator placement model对应的放置方案
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

//update state
void Reinforcement_Learning::update_state(vector<int>& capacity, vector<double>& inputs, vector<double>& response_time, int type){
    if(ogModels == NULL){
        return;
    }

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

    //calculate the reward
    switch(type){
    case 2:
        Monte_Carlo_value_update(qp, response_time);
        break;
    case 3:
        Sarsa_Temporal_Difference_update(qp, response_time);
    default:
        break;
    }

    if(state_epsilon[state] < (1.0 / (double)N)){
       state_epsilon[state] = 1.0 / (1.0 / state_epsilon[state] + 1);
    }
    else{
        state_epsilon[state] -= 1.0 / N;
    }
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
#if EPSILON_TYPE == 0
        if(randNum > epsilon * N){
            double maxValue = DBL_MIN;
            map<Action, double>::iterator it = Q[state].begin();
            while(it != Q[state].end()){
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

#else //EPSILON TYPE == 1
        if(randNum > state_epsilon[state] * N){
            double maxValue = DBL_MIN;
            map<Action, double>::iterator it = Q[state].begin();
            while(it != Q[state].end()){
                if(it->second > maxValue){
                    maxValue = it->second;
                    action = it->first;
                }
                ++ it;
            }
        }
        //choose a random action
        else{
            double avg_predicted_response_time = 0, rt_threshold = 0;
            do{
                getRandomAction();
                avg_predicted_response_time = predict_response_time();



                rt_threshold = reward_threshold[state].second;

            }while(avg_predicted_response_time >= rt_threshold);
            //更新reward_threshold
           reward_threshold[state].second = (reward_threshold[state].first * reward_threshold[state].second + avg_predicted_response_time)
                   / (double)(reward_threshold[state].first+1.0);
           reward_threshold[state].first ++;

        }
#endif





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
        action = a;

        double avg_predicted_response_time = predict_response_time();
        reward_threshold[s] = {1,avg_predicted_response_time};

        state_epsilon[s] = 0.1;

        Q[s][a] = -avg_predicted_response_time;
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



