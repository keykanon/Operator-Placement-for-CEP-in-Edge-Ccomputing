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
import mogp.operator.EventStorage;
//
// Generated network with random topology (30 nodes, 50 edges, seed=100).
//

channel Link extends ned.DatarateChannel {
    parameters:
        int cost = default(0);
        delay = 10ms;//uniform(10ms,25ms);
        datarate = uniform(10Mbps,20Mbps);
}

channel PoorLink extends ned.DatarateChannel {
    parameters:
        int cost = default(0);
        delay = uniform(0.010ms,0.025ms);
        datarate = uniform(4Mbps,10Mbps);
}

channel ESLink extends ned.DatarateChannel {
 	parameters:
 	    int cost = default(0);
 	    delay = default(0ms);
 	    datarate = default(4Mbps);   
}

network Net30 {
    submodules:
        node[30] : EdgeNode{
        	Naddress = index;
        	capacity = 1;
        	NdestAddress = "1 29";
        }
        //node1 : EdgeNode;
        //node2 : EdgeNode;
//        node3 : EdgeNode;
//        node4 : EdgeNode;
//        node5 : EdgeNode;
//        node6 : EdgeNode;
//        node7 : EdgeNode;
//        node8 : EdgeNode;
//        node9 : EdgeNode;
//        node10 : EdgeNode;
//        node11 : EdgeNode;
//        node12 : EdgeNode;
//        node13 : EdgeNode;
//        node14 : EdgeNode;
//        node15 : EdgeNode;
//        node16 : EdgeNode;
//        node17 : EdgeNode;
//        node18 : EdgeNode;
//        node19 : EdgeNode;
//        node20 : EdgeNode;
//        node21 : EdgeNode;
//        node22 : EdgeNode;
//        node23 : EdgeNode;
//        node24 : EdgeNode;
//        node25 : EdgeNode;
//        node26 : EdgeNode;
//        node27 : EdgeNode;
//        node28 : EdgeNode;
//        node29 : EdgeNode;
        eventStorage: EventStorage{
            size = 30;
            address = 30;
            destAddresses = "1 29";
        }
    connections:
        node[0].g++ <--> Link <--> node[1].g++;
        node[0].g++ <--> Link <--> node[21].g++;
        node[0].g++ <--> Link <--> node[6].g++;
        node[0].g++ <--> Link <--> node[22].g++;
        node[0].g++ <--> Link <--> node[11].g++;
        node[0].g++ <--> Link <--> node[28].g++;
        node[1].g++ <--> Link <--> node[16].g++;
        node[1].g++ <--> Link <--> node[2].g++;
        node[1].g++ <--> Link <--> node[3].g++;
        node[1].g++ <--> Link <--> node[19].g++;
        node[1].g++ <--> Link <--> node[5].g++;
        node[1].g++ <--> Link <--> node[25].g++;
        node[1].g++ <--> Link <--> node[11].g++;
        node[2].g++ <--> Link <--> node[4].g++;
        node[2].g++ <--> Link <--> node[8].g++;
        node[3].g++ <--> Link <--> node[18].g++;
        node[3].g++ <--> Link <--> node[7].g++;
        node[3].g++ <--> Link <--> node[10].g++;
        node[3].g++ <--> Link <--> node[12].g++;
        node[4].g++ <--> Link <--> node[24].g++;
        node[4].g++ <--> Link <--> node[27].g++;
        node[5].g++ <--> Link <--> node[6].g++;
        node[5].g++ <--> Link <--> node[12].g++;
        node[5].g++ <--> Link <--> node[14].g++;
        node[6].g++ <--> Link <--> node[18].g++;
        node[6].g++ <--> Link <--> node[9].g++;
        node[6].g++ <--> Link <--> node[28].g++;
        node[6].g++ <--> Link <--> node[13].g++;
        node[7].g++ <--> Link <--> node[17].g++;
        node[7].g++ <--> Link <--> node[9].g++;
        node[7].g++ <--> Link <--> node[26].g++;
        node[7].g++ <--> Link <--> node[11].g++;
        node[8].g++ <--> Link <--> node[21].g++;
        node[8].g++ <--> Link <--> node[28].g++;
        node[8].g++ <--> Link <--> node[29].g++;
        node[9].g++ <--> Link <--> node[23].g++;
        node[10].g++ <--> Link <--> node[14].g++;
        node[11].g++ <--> Link <--> node[22].g++;
        node[12].g++ <--> Link <--> node[14].g++;
        node[12].g++ <--> Link <--> node[15].g++;
        node[14].g++ <--> Link <--> node[16].g++;
        node[17].g++ <--> Link <--> node[25].g++;
        node[17].g++ <--> Link <--> node[26].g++;
        node[18].g++ <--> Link <--> node[27].g++;
        node[19].g++ <--> Link <--> node[20].g++;
        node[19].g++ <--> Link <--> node[24].g++;
        node[21].g++ <--> Link <--> node[29].g++;
        node[22].g++ <--> Link <--> node[23].g++;
        node[24].g++ <--> Link <--> node[28].g++;
        node[27].g++ <--> Link <--> node[29].g++;
        for i=0..29 { 
        	eventStorage.gate[i] <--> ESLink <--> node[i].es;
           
        }
}
