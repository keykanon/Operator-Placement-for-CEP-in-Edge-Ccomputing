#pragma once


#include <string>
#include <map>
using namespace std;

class FogNode;
class FogEdge
{
private:
	//static int edgeIDCount;
	string edgeID;
	double transmission_rate;
	map<int,int> eventNum;
	int totalEventNum;
	FogNode* source;
	FogNode* dest;
public:
	FogEdge(string edgeID, FogNode* s, FogNode* d, double tran_rate = 10000000/8);
	~FogEdge(void);

	void setTransmission_rate(double transmission_rate);
	double getTransmission_rate();
	string getEdgeID();
	//a node linked wants to get another one
	FogNode* getDest(FogNode*);

	FogNode* getSource();
	FogNode* getDest();
	//compare fog edge is the edge from s to d
	bool compareEdge(FogNode* s, FogNode* d);

	//clear every in edge
	void clear(FogNode* s);

	//add event num
	void addEventNum(int app_num, int num);

	//clear event num
	void clearEventNum();

	//clear event number of an operator graph whose index is app_num
	void clearEventNum(int app_num);

    int getTotalEventNum() const;
    void setTotalEventNum(int totalEventNum);
};

