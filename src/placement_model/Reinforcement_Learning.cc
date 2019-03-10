#include "Reinforcement_Learning.h"


Reinforcement_Learning::Reinforcement_Learning(
        double lowest, double highest):lowest_input_rate(lowest), highest_input_rate(highest){
    this->lowest_node_rate = lowest_input_rate;
    this->highest_node_rate = highest_input_rate * APP_NUM;
    this->highest_node_rate *= 0.5;
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
void Reinforcement_Learning::setParameter( vector<OperatorGraphModel*>* ogModels, map<int, FogNode*>* fognodes, FogNetworks* fognetworks, OperatorPlacementManager* opm){
    this->ogModels = ogModels;
    this->fognodes = fognodes;
    this->fognetworks = fognetworks;
    this->opm = opm;

    randNumToFogID.clear();
    map<int, FogNode*>::iterator nit = fognodes->begin();
    while(nit != fognodes->end()){
        randNumToFogID.push_back(nit->first);
        ++ nit;
    }
    if(first_init){
        first_init = false;
        srand(clock());
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

#if STATE_ACTION_MODEL
        for(int i = 0; i < ogModels->size(); ++ i){
            state.input_rate.push_back( transformInputRate((*ogModels)[i]->getEventNumber()));
        }
#else
        for(int i = 0; i < fognodes->size(); ++i){
            state.node_input.push_back( transformInputRate((*fognodes)[randNumToFogID[i]]->getNodeInput()));
        }
#endif
    }
}

//得到一个随机的放置
Action Reinforcement_Learning::getRandomAction(){

    int modNum = fognodes->size();


#if STATE_ACTION_MODEL
    //计算总共的operator数量
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
#else

    //计算总共的operator数量
    int totalOperatorNum = 0;
    for(int i = 0; i < ogModels->size(); ++ i){
        totalOperatorNum += (*ogModels)[i]->getOperatorModel().size()-1;
    }
    OperatorModel* op = NULL;
    FogNode* edgenode = NULL;

    //获得随机opeartor 以及 随机的edge node
    int randOperatorNo = rand() % totalOperatorNum;
    int randNodeID = rand() % (modNum);

    //计算OperatorModel
    for(int ogIndex = 0; ogIndex < ogModels->size(); ++ ogIndex){
        if(randOperatorNo >= (*ogModels)[ogIndex]->getOperatorModel().size()-1){
            randOperatorNo -= (*ogModels)[ogIndex]->getOperatorModel().size()-1;
        }
        else{
            op = (*ogModels)[ogIndex]->getOperatorModel()[randOperatorNo+1];
            break;
        }
    }

    edgenode = (*fognodes)[randNumToFogID[randNodeID]];
    while(edgenode->getCapacity() == 0){
        edgenode = (*fognodes)[randNumToFogID[(randNodeID+1)%modNum]];
    }

    op->setFogNode(edgenode);

    action.act[op] = edgenode;
#endif

    return action;
}

//初始化策略
void Reinforcement_Learning::initial_policy(){
    if(type > 0){

       char mc_filename[1024];
       sprintf(mc_filename, "%d_t%d",ALG_TYPE, input_N);
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

#if STATE_ACTION_MODEL
    for(int i = 0; i < ogModels->size(); ++ i){
        s.input_rate.push_back(lowest);
    }
#else
    for(int i = 0; i < fognodes->size(); ++ i){
        s.node_input.push_back(lowest);
    }
#endif
    travel_state(s, 0);
}

//在模型中设置放置指针
void Reinforcement_Learning::apply_action(){
#if STATE_ACTION_MODEL
    //reset capacity
    map<int, FogNode*>::iterator fit = fognodes->begin();
    while(fit != fognodes->end()){
        fit->second->setCapacity(fit->second->getOriginCapacity());
        fit ++;
    }
#endif
    //apply action
    map<OperatorModel*, FogNode*>::iterator mit = action.act.begin();
    while(mit != action.act.end()){
        mit->first->setFogNode(mit->second);
        mit ++;
    }
}

//递归遍历所有的状态
#if STATE_ACTION_MODEL

void Reinforcement_Learning::travel_state(State& s, int ogIndex){
    if(ogIndex ==  ogModels->size()){
#else

void Reinforcement_Learning::travel_state(State& s, int nodeIndex){
    if(nodeIndex == fognodes->size()){
#endif

        //set action
#if INITIAL_ACTION == 0
        Action a = getRandomAction();
#else
        Action a = getResponseTimeAwareAction();
#endif
        policy[s] = a;

        //update reward threshold
        double avg_predicted_response_time = predict_response_time();
        reward_threshold[s] = {1,avg_predicted_response_time};

        state_epsilon[s] = epsilon;

        //update Q
        Q[s][a] = 1.0/avg_predicted_response_time;

        return;
    }
    for(int sInput = lowest; sInput <= highest; ++ sInput){
#if STATE_ACTION_MODEL
        s.input_rate[ogIndex] = sInput;
        travel_state(s, ogIndex+1);
#else
        s.node_input[nodeIndex] = sInput;
        travel_state(s, nodeIndex+1);
#endif

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

//得到响应时间感知放置
Action Reinforcement_Learning::getResponseTimeAwareAction(){
    //返回值
    Action reta;

    //进行响应时间感知放置方法
    vector<bool> replace;
    for(int appIndex = 0; appIndex < ogModels->size(); ++ appIndex){
        replace.push_back(true);
    }
    opm->getReMultiOperatorGraphPlacement(replace);

    //转换为Action
    for(int appIndex = 0; appIndex < ogModels->size(); ++ appIndex){
        vector<OperatorModel*> ogm = (*ogModels)[appIndex]->getOperatorModel();
        for(int opIndex = 0; opIndex < ogm.size(); ++ opIndex){
            OperatorModel* om = ogm[opIndex];
            reta.act[om] = om->getFogNode();
        }
    }

    return reta;
}

void Reinforcement_Learning::RL(vector<int>& capacity, vector<int>& inputs, vector<double>& response_time){

}

//返回给operator placement model对应的放置方案
vector<vector<StreamPath*>> Reinforcement_Learning::transformAction(Action& a){
    //reset capacity
     map<int, FogNode*>::iterator fit = fognodes->begin();
     while(fit != fognodes->end()){
         fit->second->setCapacity(fit->second->getOriginCapacity());
         fit ++;
     }

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

#if STATE_ACTION_MODEL
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
#else
    for(int i = 0; i < fognodes->size(); ++ i){
        state.node_input[i] = transformInputRate((*fognodes)[randNumToFogID[i]]->getNodeInput());
    }
#endif
    qp.a = action;

    //calculate the reward
    switch(type){
    case 2:
        Monte_Carlo_value_update(qp_vec[0],response_time);
        break;
    case 3:
        Sarsa_Temporal_Difference_update(qp_vec[0], qp,response_time);
        break;
    case 4:
        QLearning_update(qp_vec[0], qp, response_time);
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

#if STATE_ACTION_MODEL
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
#else
        for(int i = 0; i < fognodes->size(); ++ i){
            state.node_input[i] = transformInputRate((*fognodes)[randNumToFogID[i]]->getNodeInput());
        }
#endif
        qp.a = action;

        if(qp_vec.size() == 2){
            qp_vec[0] = qp_vec[1];
            qp_vec[1] = qp;
        }
        else if(qp_vec.size() == 1){
            qp_vec.push_back(qp);
        }
        else{
            qp_vec.push_back(qp);
        }

        if(response_time.size() == 0){
            action = getResponseTimeAwareAction();
        }
        else{
            //travel and reward random: if randNum > epsilon, argmax Q; else random action
            int randNum = rand() % N;

            //choose the argmax a' Q(x, a')
    #if EPSILON_TYPE == 0
            if(randNum > epsilon * N){
    #else //EPSILON TYPE == 1
            if(randNum > state_epsilon[state] * N){
    #endif
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

                }while(avg_predicted_response_time >= rt_threshold );
                //更新reward_threshold

               reward_threshold[state].second = (reward_threshold[state].first * reward_threshold[state].second + avg_predicted_response_time)
                       / (double)(reward_threshold[state].first+1.0);
               reward_threshold[state].first ++;

            }

        }




        //calculate the reward
        switch(type){
        case 0:
            Monte_Carlo_value_update(qp_vec[0],   response_time);
            break;
        case 1:
            Sarsa_Temporal_Difference_update(qp_vec[0], qp, response_time);
            break;
        case 2:
            QLearning_update(qp_vec[0], qp, response_time);
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

#if STATE_ACTION_MODEL
        //input input_rate
        int input_rate_size = 0;
        in >> input_rate_size;
        for(int iindex = 0; iindex < input_rate_size; ++ iindex){
            int input_rate = 0;
            in >> input_rate;
            s.input_rate.push_back(input_rate);
        }
#else
        //input node_input
        int node_input_size = 0;
        in >> node_input_size;
        for(int nindex = 0; nindex < node_input_size; ++ nindex){
            int node_input = 0;
            in >> node_input;
            s.node_input.push_back(node_input);
        }
#endif
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

        if(this->type == 2){
            state_epsilon[s] = 0;
        }
        else{
            state_epsilon[s] = 0.1;
        }


        Q[s][a] = 1/avg_predicted_response_time;
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

#if STATE_ACTION_MODEL
        out << pit->first.input_rate.size() << endl;
        for(int i = 0; i < pit->first.input_rate.size(); ++ i){
            out << pit->first.input_rate[i] << " ";
        }
#else
        out << pit->first.node_input.size() << endl;
        for(int i = 0; i < pit->first.node_input.size(); ++ i){
            out << pit->first.node_input[i] << " ";
        }
#endif
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
            r += 1/response_time[i];
            if(response_time[i] >= monitor_reward_threshold){
                r = DBL_MIN;
            }
        }
        r /= response_time.size();


    //update Q
    Q[qp.s][qp.a] = (Q[qp.s][qp.a] * Qcount[qp.s][qp.a] + r) / (Qcount[qp.s][qp.a]+1);
    Qcount[qp.s][qp.a] ++;
}





/*-------------------------Temporal Difference Learning Part------------------------
 *
 */
void Reinforcement_Learning::Sarsa_Temporal_Difference_update(Q_parameter& qp, Q_parameter& qp2, vector<double>& response_time){
    //calculate the reward
    double r = 0;
    for(int i = 0; i < response_time.size(); ++ i){
        r += 1/response_time[i];
        if(response_time[i] >= monitor_reward_threshold){
            r = DBL_MIN;
        }
    }
    r /= response_time.size();


    //update Q
    Q[qp.s][qp.a] = Q[qp.s][qp.a] + step_size * (r + gamma * Q[qp2.s][qp2.a] - Q[qp.s][qp.a]);

}


/*----------------------------------Q Learning Part---------------------------------
 *
 */
void Reinforcement_Learning::QLearning_update(Q_parameter& qp, Q_parameter& qp2, vector<double>& response_time){
    //calculate the reward
    double r = 0;
    for(int i = 0; i < response_time.size(); ++ i){
        r += 1/response_time[i];
        if(response_time[i] >= monitor_reward_threshold){
            r = DBL_MIN;
        }
    }
    r /= response_time.size();

    //update Q
    Q[qp.s][qp.a] = (1-step_size) * Q[qp.s][qp.a] + step_size * (r + gamma * Q[qp2.s][policy[qp2.s]]);
}


