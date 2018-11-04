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

package mogp.networks;
import mogp.node.EdgeNode;

import mogp.networks.Link;
import mogp.operator.EventStorage;

//
// Generated network with random topology (3 nodes, 3 edges, seed=100).
//
network Net3 {
    parameters:
        int nodeNum = 3;
    submodules:
        node[nodeNum] : EdgeNode{
            Naddress = index;
            capacity = 4;
            NdestAddress = "0 2";
        }
		eventStorage: EventStorage{
            size = nodeNum;
            address = nodeNum;
            destAddresses = "0 2";
        }
    connections:
        node[0].g++ <-->  node[1].g++;
        //node[0].g++ <--> Link <--> node[2].g++;
        node[1].g++ <-->  node[2].g++;
        for i=0..nodeNum-1 { 
        	eventStorage.gate[i] <-->  node[i].es;
           
        }
}
