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

import mogp.networks.Link;

//
// A generated network with full graph topology.
//
network Net10_f
{
    parameters:
        int n = default(10);
    submodules:
        node[n]: EdgeOperatorNode {
            @display("p=,,ring");
            gates: g[n];
        }
    connections:
        for i=0..n-2, for j=i+1..n-1 {
            node[i].g[j] <--> Link <--> node[j].g[i];
        }
}
