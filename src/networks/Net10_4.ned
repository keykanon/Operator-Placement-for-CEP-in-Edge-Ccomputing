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
import mogp.node.EdgeOperatorNode;
import mogp.operator.EventStorage;
import mogp.networks.Link;

//
// Generated network with random topology (10 nodes, 20 edges, seed=95).
//
network Net10_4
{
    parameters:
        int nodeNum = 10;
        @display("bgb=396,217");


    submodules:

        node[10]: EdgeOperatorNode {
            Naddress = index;
            capacity = 4;
            NdestAddress = "1 9";
        }
        eventStorage: EventStorage {
            size = nodeNum;
            address = nodeNum;
            destAddresses = "1 9";
        }
    connections:
        node[0].g++ <--> Link <--> node[1].g++;
        node[0].g++ <--> Link <--> node[4].g++;
        node[0].g++ <--> Link <--> node[6].g++;
        node[0].g++ <--> Link <--> node[7].g++;
        node[0].g++ <--> Link <--> node[8].g++;
        node[1].g++ <--> Link <--> node[2].g++;
        node[1].g++ <--> Link <--> node[5].g++;
        node[1].g++ <--> Link <--> node[6].g++;
        node[2].g++ <--> Link <--> node[3].g++;
        node[2].g++ <--> Link <--> node[7].g++;
        node[2].g++ <--> Link <--> node[9].g++;
        node[3].g++ <--> Link <--> node[4].g++;
        node[3].g++ <--> Link <--> node[5].g++;
        node[3].g++ <--> Link <--> node[8].g++;
        node[3].g++ <--> Link <--> node[9].g++;
        node[4].g++ <--> Link <--> node[8].g++;
        node[4].g++ <--> Link <--> node[9].g++;
        node[6].g++ <--> Link <--> node[8].g++;
        node[6].g++ <--> Link <--> node[9].g++;
        node[7].g++ <--> Link <--> node[8].g++;

        for i=0..9 {
            eventStorage.gate[i] <--> node[i].es;

        }
}
