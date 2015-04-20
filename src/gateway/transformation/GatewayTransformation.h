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

#ifndef __SIGNALSANDGATEWAYS_GATEWAYTRANSFORMATION_H_
#define __SIGNALSANDGATEWAYS_GATEWAYTRANSFORMATION_H_

#include <omnetpp.h>

#include <string>
#include <map>
#include <list>
#include <vector>

#include "CanDataFrame_m.h"
#include "AVBFrame_m.h"
#include "CoRE4INET_CTFrame.h"
#include "RCFrame_m.h"
#include "TTFrame_m.h"

#include "PoolMessage_m.h"
#include "UnitMessage_m.h"
#include "GatewayAggregationMessage.h"

namespace SignalsAndGateways {

class GatewayTransformation : public cSimpleModule
{
    private:
        static const int CANCRCBITLENGTH;
    private:
        std::list<unsigned int> canToCan;
        std::map<unsigned int, std::list<std::string> > canToBEEthernet;
        std::list<std::string> beEthernetToCan;
        std::map<unsigned int, std::list<uint16_t> > canToRCEthernet;
        std::map<unsigned int, std::list<uint16_t> > canToTTEthernet;
        std::list<uint16_t> ctEthernetToCan;
        //TODO ...
    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
    private:
        void readConfigXML();
        std::list<cMessage*> transformCanFrame(FiCo4OMNeT::CanDataFrame* canFrame);
        std::list<cMessage*> transformPoolMessage(PoolMessage* poolMessage);
        std::list<cMessage*> transformEthernetFrame(inet::EthernetIIFrame* ethernetFrame);

        inet::EthernetIIFrame* transformCanToBEEthernet(FiCo4OMNeT::CanDataFrame* canFrame);
        inet::EthernetIIFrame* transformCanToBEEthernet(std::list<FiCo4OMNeT::CanDataFrame*> canFrames);
        CoRE4INET::RCFrame* transformCanToRCEthernet(FiCo4OMNeT::CanDataFrame* canFrame);
        CoRE4INET::RCFrame* transformCanToRCEthernet(std::list<FiCo4OMNeT::CanDataFrame*> canFrames);
        CoRE4INET::TTFrame* transformCanToTTEthernet(FiCo4OMNeT::CanDataFrame* canFrame);
        CoRE4INET::TTFrame* transformCanToTTEthernet(std::list<FiCo4OMNeT::CanDataFrame*> canFrames);
        //transformCanToAVBEthernet...

        std::list<FiCo4OMNeT::CanDataFrame*> transformEthernetToCan(inet::EthernetIIFrame* ethernetFrame);
        //transformEthernetToFlexRay...

        void setEthernetFrameSize(inet::EthernetIIFrame* ethernetFrame);
        UnitMessage* generateUnitMessage(FiCo4OMNeT::CanDataFrame* canFrame);
        GatewayAggregationMessage* generateGatewayAggregationMessage(std::list<UnitMessage*> units);

};

} //namespace

#endif
