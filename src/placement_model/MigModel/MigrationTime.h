/*
 * MigrationTime.h
 *
 *  Created on: 2017Äê11ÔÂ28ÈÕ
 *      Author: airsola
 */

#ifndef PLACEMENT_MODEL_MIGMODEL_MIGRATIONTIME_H_
#define PLACEMENT_MODEL_MIGMODEL_MIGRATIONTIME_H_

#include "Migration.h"

class Migration;
class Migration_Time {
private:
    vector<OperatorModel> placement;
    vector<Migration> mig;
    double time;
public:
    Migration_Time();
    Migration_Time(vector<OperatorModel>,vector<Migration> m, double time);
    virtual ~Migration_Time();

    vector<Migration> getMig() ;
    void setMig(const vector<Migration>& mig);
    double getTime();
    void setTime(double time);
    vector<OperatorModel> getPlacement();
    void setPlacement(const vector<OperatorModel>& placement);

    //add a migration
    void addMigration(Migration);
};

#endif /* PLACEMENT_MODEL_MIGMODEL_MIGRATIONTIME_H_ */
