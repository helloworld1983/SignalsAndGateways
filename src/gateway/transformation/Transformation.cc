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

#include "Transformation.h"
#include "IdentifierFieldElement.h"
#include "DataFieldElement.h"
#include "TimestampFieldElement.h"
#include "TransportHeaderFieldElement.h"

using namespace dataStruct;

Define_Module(Transformation);

void Transformation::initialize()
{
    this->transformMap = new StaticTransformationIDList();
}

void Transformation::handleMessage(cMessage *msg)
{
    if(msg->arrivedOn("in")){
        send(transform(msg), "out");
    }
}

cPacket *Transformation::transform(cMessage *msg){
    InterConnectMsg *interDataStructure = dynamic_cast<InterConnectMsg*>(msg);
    cPacket *returnMessage = NULL;
    EV << "transform entrance" << endl;
    if(transformMap->getTransformationID(interDataStructure->getTransformationID()) > 0){
        EV << "transform: " << "transformationID > 0" << endl;
        if(strcmp(interDataStructure->getTransformationID(),"canTocan") == 0){
            EV << "transform: " << "canTocan" << endl;
            if(strcmp(interDataStructure->getFrameFormat(), "canDataFrame") == 0){
                EV << "transform: " << "canDataFrame" << endl;
                CanDataFrame *canDataFrame = dynamic_cast<CanDataFrame*>(interDataStructure->decapsulate());
                FieldSequence transportFrame = transformCanToTransport(canDataFrame);
                interDataStructure->setTransportFrame(transportFrame);
                returnMessage = interDataStructure;
            }else if(strcmp(interDataStructure->getFrameFormat(), "transportFrame") == 0){
                EV << "transform: " << "transportFrame" << endl;
                FieldSequence transportFrame = interDataStructure->getTransportFrame();
                CanDataFrame *canDataFrame = transformTransportToCan(transportFrame);
                interDataStructure->encapsulate(canDataFrame);
                returnMessage = interDataStructure;
            }
        }
    }else{
        //invalid transformation ID
    }
    return returnMessage;
}

FieldSequence Transformation::transformCanToTransport(CanDataFrame *msg){
    /*
     * Uebersetzungsprotokoll
     */
    FieldSequence protocolFieldSequence;
    std::shared_ptr<dataStruct::IdentifierFieldElement> identifier (new IdentifierFieldElement());
    EV << "Transformation: getCanID(): " << msg->getCanID();
    identifier->setIdentifier(msg->getCanID());
    std::shared_ptr<dataStruct::DataFieldElement> data (new DataFieldElement(msg->getDataArraySize()));
    data->setDataLength(msg->getDataArraySize());
    for (unsigned int i=0; i<msg->getDataArraySize(); i++){
        data->setData(msg->getData(i), i);
    }
    std::shared_ptr<dataStruct::TimestampFieldElement>  timestamp (new TimestampFieldElement());
    timestamp->setTimestamp(clock());
    /*
     * Transportprotokollheader
     */
    std::shared_ptr<dataStruct::TransportHeaderFieldElement>  transportHeader (new TransportHeaderFieldElement());
    transportHeader->setStaticTranslationId(1);
    transportHeader->setStaticBusId(0);
    transportHeader->setActualityFlag(true);
    /*
     * Create sequence
     */
    protocolFieldSequence.push_back(transportHeader);
    protocolFieldSequence.push_back(identifier);
    protocolFieldSequence.push_back(data);
    protocolFieldSequence.push_back(timestamp);

    return protocolFieldSequence;
}


CanDataFrame *Transformation::transformTransportToCan(FieldSequence transportFrame){
    CanDataFrame *canDataFrame = new CanDataFrame;
    for (int i = 0; not transportFrame.empty(); i++) {
        std::shared_ptr<FieldElement> element = transportFrame.at(transportFrame.size());
        if(std::dynamic_pointer_cast<IdentifierFieldElement>(element) != NULL){
            std::shared_ptr<IdentifierFieldElement> specificElement  = std::dynamic_pointer_cast<IdentifierFieldElement>(element);
            canDataFrame->setCanID(specificElement->getIdentifier());
        }else if (std::dynamic_pointer_cast<DataFieldElement>(element) != NULL){
            std::shared_ptr<DataFieldElement> specificElement = std::dynamic_pointer_cast<DataFieldElement>(element);
            for (int i = 0; specificElement->getDataLength() < i ; i++){
                canDataFrame->setData(specificElement->getData(i), i);
            }
        }else if (std::dynamic_pointer_cast<TimestampFieldElement>(element) != NULL){
            std::shared_ptr<TimestampFieldElement> specificElement = std::dynamic_pointer_cast<TimestampFieldElement>(element);
            canDataFrame->setTimestamp(specificElement->getTimestamp());
        }else if (std::dynamic_pointer_cast<TransportHeaderFieldElement>(element) != NULL){

        }
        transportFrame.pop_back();
    }
    return canDataFrame;
}