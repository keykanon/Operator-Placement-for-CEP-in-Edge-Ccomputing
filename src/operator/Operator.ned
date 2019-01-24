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
simple Operator
{
    int address;
    string destAddresses;  // destination addresses
    double lambda @unit(ms) = default(10ms);// the execution rate 
    volatile double sleepTime @unit(s) = default(30s); // sleep time between bursts
    volatile double burstTime @unit(s) = default(10s); // duration of a burst
    volatile double sendIaTime @unit(s) = default(exponential(1s)); // time between generating packets during a burst
    volatile int packetLength @unit(byte); // length of a message
    @display("i=block/source");
    @signal[endToEndDelay](type="simtime_t");
    @signal[hopCount](type="long");
    @signal[sourceAddress](type="long");
    @statistic[endToEndDelay](title="end-to-end delay of arrived packets";unit=s;interpolationmode=none);
    @statistic[hopCount](title="hop count of arrived packets";interpolationmode=none);
    @statistic[sourceAddress](title="source address of arrived packets";interpolationmode=none);

	gates:
	    input in;
	    output out;
}
