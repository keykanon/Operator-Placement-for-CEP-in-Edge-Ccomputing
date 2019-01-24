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

module Node {
    parameters:
        @display("i=abstract/router_vs");
    gates:
        inout gate[];
    connections allowunconnected:
}

channel Link extends ned.DatarateChannel {
    parameters:
        int cost = default(0);
}

//
// Generated network with random topology (5 nodes, 4 edges, seed=100).
//
network OG5 {
    submodules:
        node0 : Node;
        node1 : Node;
        node2 : Node;
        node3 : Node;
        node4 : Node;
    connections:
        node0.gate++ <--> Link <--> node1.gate++;
        node1.gate++ <--> Link <--> node2.gate++;
        node1.gate++ <--> Link <--> node3.gate++;
        node2.gate++ <--> Link <--> node4.gate++;
}
