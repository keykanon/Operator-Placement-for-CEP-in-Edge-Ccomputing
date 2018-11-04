#pragma once

#include "../mapMessage/TrafficMap.h"
#include <fstream>
#include <stdio.h>
class NetInputHandle
{
public:
	NetInputHandle(string filename);
	~NetInputHandle(void);

	TrafficMap* getMap();
private:
	TrafficMap* map;
	ifstream net;
	
};


NetInputHandle::NetInputHandle(string filename)
{
	this->map = new TrafficMap();

	//open the file
	FILE *file;
	if((file = fopen(filename.c_str(), "r")) == NULL){
		printf("Error: %s is not exist.\n", filename.c_str());

		getchar();
		exit(1);
	}
	
	//edge
	string edgeID;
	string from;
	string to;
	Edge *edge = NULL;

	//lane
	string laneID;
	double laneLength = 0;
	string laneShape;

	//junction
	string junctionID;
	//double junctionX = 0;
	//double junctionY = 0;
	string junctionLane1;
	string junctionLane2;
	string junctionShape;

	char line[1024];
	while( fgets(line,1024,file) != NULL){
		string sline(line);

		//printf("%s\n", line);

		if(sline.find("edge id",0) != -1){
			//get the edge
			string ss("\"");
			vector<string>* vec = new vector<string>();
			split( sline, ss, vec);

			edgeID = (*vec)[1];

			edge = new Edge(edgeID);

			//get from to
			if(vec->size() > 5){
				from = (*vec)[3];
				to = (*vec)[5];
				edge->setFrom(from);
				edge->setTo(to);
			}
			delete vec;
		}
		//edge end , add the edge to the map
		else if(sline.find("</edge>", 0) != -1){
			if(edge != NULL){
				map->addEdge(edgeID, edge);
			}
		}
		//lane idl line
		else if(sline.find("lane id",0) != -1){
			//get lane id
			string ss("\"");
			vector<string>* vec = new vector<string>();
			split(sline, ss, vec);

			laneID = (*vec)[1];
			if(vec->size() > 13){
				laneLength = atof((*vec)[11].c_str());
				laneShape = (*vec)[13];
			}
			else if(vec->size() > 11){
				laneLength = atof((*vec)[9].c_str());
				laneShape = (*vec)[11];
			}
			else{
				laneLength = atof((*vec)[7].c_str());
				string s(" ");
				vector<string>* laneShape = new vector<string>();
				split((*vec)[9], s, laneShape);
				
			}
			edge->addLanes(laneID, new Lane(laneID,laneLength, laneShape) );

			delete vec;
		}

		//junction line
		else if(sline.find("junction id",0) != -1){
			string ss("\"");
			vector<string>* vec = new vector<string>();
			split(sline, ss, vec);

			junctionID = (*vec)[1];


			//junction inc lane
			junctionLane1 = (*vec)[9];
			junctionLane2 = (*vec)[11];

			//junction shape
			if(vec->size() > 13){
				junctionShape = (*vec)[13];
			}

			Junction* junction = new Junction(junctionID,(*vec)[5],
				(*vec)[7], junctionLane1, junctionLane2, junctionShape);

			//add junction
			map->addJunction(junctionID, junction);
		}
	}
}


NetInputHandle::~NetInputHandle(void)
{
    delete map;
}

TrafficMap* NetInputHandle::getMap(){
	return this->map;
}
