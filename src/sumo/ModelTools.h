#ifndef _MODELTOOLS_H
#define _MODELTOOLS_H

#include <string>
#include <vector>
#include <cmath>

using namespace std;

void split(string& s,string& delim,vector<string >* ret)  ;

double calDistance(double x1, double y1, double x2, double y2);

//注意：当字符串为空时，也会返回一个空字符串  
void split(string& s,string& delim,vector< std::string >* ret)  
{  
    size_t last = 0;  
    size_t index=s.find_first_of(delim,last);  
    while (index!=std::string::npos)  
    {  
        ret->push_back(s.substr(last,index-last));  
        last=index+1;  
        index=s.find_first_of(delim,last);  
    }  
    if (index-last>0)  
    {  
        ret->push_back(s.substr(last,index-last));  
    }  
}  

double calDistance(double x1, double y1, double x2, double y2){
	double distance = sqrt( pow(x2-x1,2)+pow(y2-y1,2));
	return distance/10;
}

#endif
