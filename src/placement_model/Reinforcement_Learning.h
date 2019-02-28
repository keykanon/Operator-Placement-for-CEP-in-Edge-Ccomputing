#pragma once

#include <omnetpp.h>
#include <vector>
#include <map>
#include "FogModel/FogNetworks.h"
#include "CEPModel/OperatorModel.h"
#include "CEPModel/StreamModel.h"
#include "CEPModel/OperatorGraphModel.h"
#include <float.h>
#include <limits.h>
#include <fstream>


using namespace std;

enum InputRate{lowest=1, low, slow, middle, shigh, high, highest};

struct State_detail{
    //capacity
    vector<int> capacity;

    //input rate
    vector<vector<int>> input_rate;
    vector<double> response_time;
};

class State{
public:
    //capacity
    vector<int> capacity;

    //input rate
    // divide to 3 level: low,middle, high
    vector<int> input_rate;

    bool operator <(const State& s) const{
        int size = 0;
        if(this->input_rate.size() > s.input_rate.size()){
            size = s.input_rate.size();
        }
        else{
            size = this->input_rate.size();
        }

        int i = 0;
        for(; i < size; ++ i){
            if(this->input_rate[i] < s.input_rate[i]){
                return true;
            }
            else if(this->input_rate[i] > s.input_rate[i]){
                return false;
            }
        }
        if(i == size){
            if(this->input_rate.size() > s.input_rate.size()){
                return true;
            }
            else{
                return false;
            }
        }
        return false;
    }

    bool operator ==(const State& s) const{
        if(this->input_rate.size() != s.input_rate.size()){
            return false;
        }

        int size = this->input_rate.size();
        for(int i = 0; i < size; ++ i){
            if(this->input_rate[i] != s.input_rate[i]){
                return false;
            }
        }
        return true;
    }
};

struct Action{
    map<OperatorModel*, FogNode*> act;
    bool operator <(const Action& a) const{
        return act < a.act;
    }
};

struct Q_parameter{
    State s;
    Action a;
};

struct Reward{
    map<State, map<Action, double>> r;
    map<State, map<Action, int>> count;
};

struct Probability{
    map<State*, map< Action*, map<State*, int>>> p;
};

#define N 3
#define EPSILON_TYPE 1

class Reinforcement_Learning
{
protected:
    //edge node and operator graph information
    vector<OperatorGraphModel*>* ogModels;
    map<int, FogNode*>*  fognodes;
    FogNetworks* fognetworks;
    vector<int> randNumToFogID;

    //RL
    map<State, map<Action, double>> Q;
    map<State, map<Action, int>> Qcount;
    map<State, double> state_epsilon;
    State state;
    Action action;
    Reward reward;
    Probability probability;
    int roundTime = 0;
    double gamma = 0.8;        //折算因子
    double step_size = 0.5;     //步长

    int type = 2; // 0 for train, 1 for test, 2 apply policy once
    double epsilon = 1.0;
    bool first_init = true;
    //result policy
    map<State, Action> policy;

    const double lowest_input_rate;
    const double highest_input_rate;

    //random policy as initial policy
    void initial_policy();
    //travel state
    void travel_state(State& s, int ogIndex);
    //place operator on the edge node
    void apply_action();
    //get a random action
    Action getRandomAction();
    //transform double input rate to InputRate
    int transformInputRate(double input_rate);
    //transform action to placement form
    vector<vector<StreamPath*>> transformAction(Action& a);
public:
    Reinforcement_Learning( double lowest, double highest);
    ~Reinforcement_Learning();

    void setParameter(vector<OperatorGraphModel*>* ogModels, map<int, FogNode*>* fognodes, FogNetworks* fognetworks);

    void increase_round_time();


    void RL(vector<int>& capacity, vector<int>& inputs, vector<double>& response_time);
    void RL_input(string name);
    void RL_output(string type);


    //update policy
    vector<vector<StreamPath*>> reinforcement_learning_update(vector<int>& capacity, vector<double>& inputs, vector<double>& response_time, int type);



    //monte carlo method
    void Monte_Carlo_value_update(Q_parameter& qp, vector<double>& response_time);



    //Temporal-Difference Learning
    void Sarsa_Temporal_Difference_update(Q_parameter& qp, vector<double>& response_time);

};
