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

// Created: 2018-0-30 for project MOGPS

package mogp.networks;

import mogp.node.EdgeOperatorNode;
import mogp.operator.EventStorage;
import mogp.networks.Link;

//
// A generated network with ring topology.
//
network Net10_r
{
    parameters:
        int n = default(10);
    submodules:
        node[n]: EdgeOperatorNode {
            Naddress = index;
            capacity = 4;
            @display("p=,,ring,");
            NdestAddress = "1 9";
            gates: g[2];
        }
		eventStorage: EventStorage{
            size = n;
        	address = n;
        	destAddresses = "1 9";
        }
    connections:
        for i=0..n-1 {
            node[i].g[0] <--> Link <--> node[(i+1)%n].g[1];
        }
        for i=0..9 { 
        	eventStorage.gate[i]  <--> node[i].es;
           
        }
        
}
