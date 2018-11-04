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

package mogp.operator;
import mogp.operator.Node;

import mogp.operator.Link;

//
// Generated network with random topology (10 nodes, 9 edges, seed=100).
//
network OG10 {
    submodules:
        node0 : Node;
        node1 : Node;
        node2 : Node;
        node3 : Node;
        node4 : Node;
        node5 : Node;
        node6 : Node;
        node7 : Node;
        node8 : Node;
        node9 : Node;
    connections:
        node0.gate++ <--> Link <--> node1.gate++;
        node0.gate++ <--> Link <--> node6.gate++;
        node1.gate++ <--> Link <--> node2.gate++;
        node1.gate++ <--> Link <--> node3.gate++;
        node1.gate++ <--> Link <--> node5.gate++;
        node2.gate++ <--> Link <--> node4.gate++;
        node2.gate++ <--> Link <--> node8.gate++;
        node3.gate++ <--> Link <--> node7.gate++;
        node7.gate++ <--> Link <--> node9.gate++;
}
