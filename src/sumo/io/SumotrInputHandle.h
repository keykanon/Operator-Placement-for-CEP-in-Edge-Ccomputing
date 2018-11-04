

#pragma once

#include "../mapMessage/TrafficMap.h"
#include "../vehicleMessage/TrafficLogs.h"
#include <string>
#include <fstream>
#include <stdio.h>
class SumotrInputHandle
{
public:
	SumotrInputHandle(string filename, TrafficMap* map);
	~SumotrInputHandle(void);

	TrafficLogs* getTrafficLogs();
private:
	TrafficMap* map;
	TrafficLogs trafficLogs;
};

SumotrInputHandle::SumotrInputHandle(string filename, TrafficMap* map){

	//get map
	this->map = map;

	//
	FILE* file;

	//read the file sumotr and handle
	if((file = fopen(filename.c_str(), "r")) == NULL){
		printf("Error: %s is not exist.\n", filename.c_str());

		getchar();
		exit(1);
	}

	char line[1024];

	double time = 0;
	string str_time;
	bool timestepFlag = true;

	//edge
	string edgeID ;

	//lane
	string laneID;

	//vehicle id
	string vehicleID;
	//pos
	double pos;
	//speed
	double speed;

	while( fgets(line,1024,file) != NULL){
		string sline(line);
		
		//the line of <time step>
		if(sline.find("timestep") != -1 && timestepFlag){
			string ss("\"");
			vector<string>* vec = new vector<string>();
			split( sline, ss, vec);

			str_time = (*vec)[1];
			time = atof(str_time.c_str());

			//set the time step flag
			timestepFlag = false;

			delete vec;
		}
		//the line of "</timestep>"
		else if(sline.find("timestep") != -1){
			timestepFlag = true;
		}

		//the line <edge id = "">
		else if(sline.find("edge id") != -1){
			string ss("\"");
			vector<string>* vec = new vector<string>();
			split( sline, ss, vec);

			edgeID = (*vec)[1];

			delete vec;
		}

		//the line <lane id = "">
		else if(sline.find("lane id") != -1){
			string ss("\"");
			vector<string>* vec = new vector<string>();
			split( sline, ss, vec);

			laneID = (*vec)[1];

			delete vec;
		}

		//the line <vehicle id = "" pos = "" speed = "">
		else if(sline.find("vehicle id") != -1){
			string ss("\"");
			vector<string>* vec = new vector<string>();
			split( sline, ss, vec);

			vehicleID = (*vec)[1];
			pos = atof((*vec)[3].c_str());
			speed = atof((*vec)[5].c_str());

			//create vehicle status
			vehicleStatus* status = new vehicleStatus(vehicleID, edgeID, laneID,
				pos, speed, map);

			//add traffic log
			trafficLogs.addLog(vehicleID, time, status);

			delete vec;
		}

	}
	fclose(file);
}

SumotrInputHandle::~SumotrInputHandle(){
	delete map;
}

TrafficLogs* SumotrInputHandle::getTrafficLogs(){
	return &this->trafficLogs;
}
