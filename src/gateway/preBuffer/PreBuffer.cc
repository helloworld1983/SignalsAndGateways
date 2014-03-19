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

#include "PreBuffer.h"
#include "InterConnectMsg_m.h"
#include "FieldSequenceMessage_m.h"
#include "candataframe_m.h"

Define_Module(PreBuffer);

void PreBuffer::initialize()
{
    // TODO - Generated method body
}

void PreBuffer::handleMessage(cMessage *msg)
{
    if(msg->arrivedOn("in")){
        InterConnectMsg *interDataStructure = dynamic_cast<InterConnectMsg*>(msg);
        if(strcmp(interDataStructure->getFrameFormat(), "canDataFrame") == 0){
            FieldSequenceMessage *fieldSequence = new FieldSequenceMessage;
            fieldSequence->setTransportFrame(interDataStructure->getTransportFrame());
            interDataStructure->encapsulate(fieldSequence);
            send(interDataStructure, "out");
        }else if (strcmp(interDataStructure->getFrameFormat(), "transportFrame") == 0){
            CanDataFrame *canDataFrame = dynamic_cast<CanDataFrame*>(interDataStructure->decapsulate());
            send(canDataFrame, "out");
        }
    }
}