/*
 * Migration.h
 *
 *  Created on: 2017Äê11ÔÂ28ÈÕ
 *      Author: airsola
 */

#ifndef PLACEMENT_MODEL_MIGMODEL_MIGRATION_H_
#define PLACEMENT_MODEL_MIGMODEL_MIGRATION_H_

#include "../OperatorPlacementManager.h"

class Migration {
private:
    OperatorModel* op;
    FogNode* source;
    FogNode* dest;
    double time;
    double cost;
public:
    Migration();
    Migration(OperatorModel* op, FogNode* s, FogNode* d, double time, double delay = 0.1);

    virtual ~Migration();
    double getCost() const;
    void setCost(double cost);
    FogNode* getDest() const;
    void setDest(FogNode* dest);
    FogNode* getSource() const;
    void setSource(FogNode* source);
    OperatorModel* getOp();
    void setOp(OperatorModel* op);
    double getTime() const;
    void setTime(double time);
};

#endif /* PLACEMENT_MODEL_MIGMODEL_MIGRATION_H_ */
