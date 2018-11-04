//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

package mogp.node;

import mogp.operator.Operator;
import mogp.operator.AccidentOperator;
import mogp.operator.LaneOperator;
import mogp.operator.SpeedOperator;
import mogp.operator.EventStorage;

module EdgeNode
{
    parameters:
        int Naddress; //address in network
        int capacity;
        string NdestAddress;
        double executionTime @unit(s) = uniform(1ms,100ms);
        @display("i=abstract/router_vs");
    gates:
        inout g[];
        inout es;

    submodules:
        
        accident[capacity]: AccidentOperator {
            address = Naddress;
            destAddresses = NdestAddress;
            @display("p=140,60");
        }
        lane[capacity]: LaneOperator {
            address = Naddress;
            destAddresses = NdestAddress;
            @display("p=223,60");
        }
        speed[capacity]: SpeedOperator {
            address = Naddress;
			destAddresses = NdestAddress;
            @display("p=48,60");
        }
        //eventStorage: EventStorage {
        //    address = EdgeNode.address;
        //    @display("p=48,130");
        //}
        //operator: <operatorType> like Operator {
        //    parameters:
		//		address = address;
		//		@display("p=140,60");                

        //}
        routing: Router {
            parameters:
                @display("p=140,130");
            gates:
                gate[sizeof(g)];
                localIn[3*capacity];
                localOut[4*capacity];
            

        }
        queue[sizeof(g)]: Queue {
           parameters:
                @display("p=80,200,row");
        }
    connections allowunconnected:

    	for i = 0..capacity-1{
            routing.localIn[0+i*3] <-- speed[i].accident;
            routing.localOut[0+i*4] --> speed[i].eventstorage;
            routing.localIn[1+i*3] <-- lane[i].accident;
            routing.localOut[1+i*4] --> lane[i].eventstorage;
            routing.localOut[2+i*4] --> accident[i].speedIn;
            routing.localOut[3+i*4] --> accident[i].laneIn;
            routing.localIn[2+i*3] <-- accident[i].user;
    	} 
        //routing.localIn[2] <-- eventStorage.speed;
        //routing.localIn[3] <-- eventStorage.lane;

        for i=0..sizeof(g)-1 {
            routing.gate[i] <--> queue[i].router;
            queue[i].line <--> g[i];
			//routing.gate[i] <--> g[i];
        }
        
        routing.eventStorage <--> es;

}


