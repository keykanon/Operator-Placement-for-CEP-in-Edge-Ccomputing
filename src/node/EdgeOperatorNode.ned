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

network EdgeOperatorNode
{
    parameters:
        int Naddress; //address in network
        int capacity;
        double executionTime @unit(s) = uniform(1ms,100ms);
        string NdestAddress;
        @display("i=abstract/router_vs");
    gates:
        inout g[];
        inout es;

    submodules:
        operator[capacity]: Operator {
            address = Naddress;
            destAddresses = NdestAddress;
            lambda = executionTime;
            @display("p=140,48");
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
                localIn[capacity];
                localOut[capacity];


        }
        queue[sizeof(g)]: Queue {
            parameters:
                @display("p=80,200,row");
        }
    connections allowunconnected:

        for i=0..capacity-1 {
            routing.localIn[i] <-- operator[i].out;
            routing.localOut[i] --> operator[i].in;
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
