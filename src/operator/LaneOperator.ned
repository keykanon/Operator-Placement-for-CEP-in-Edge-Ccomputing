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

//
// TODO auto-generated module
//
simple LaneOperator
{
    parameters:
        int address;
  		string destAddresses;  // destination addresses
  		volatile int packetLength @unit(byte); // length of a message
       	@signal[endToEndDelay](type="simtime_t");
    	@signal[hopCount](type="long");
   	 	@signal[sourceAddress](type="long");
    	@statistic[endToEndDelay](title="end-to-end delay of arrived packets";unit=s;interpolationmode=none);
    	@statistic[hopCount](title="hop count of arrived packets";interpolationmode=none);
    	@statistic[sourceAddress](title="source address of arrived packets";interpolationmode=none);
    gates:
        input eventstorage;
        output accident;
        
}
