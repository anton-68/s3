/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
/**
* @file s3_call.cpp
* @brief 
* @author Anton Bondarenko
* @version 1
* @date 2012-08-28
*/

#include "s3_call.h"




//      S3Call

S3Call::S3Call(OpalManager & manager) : OpalCall(manager) 
{
    PTRACE(5, "S3\t::S3Call");
}




//               addOriginatorCapability

PBoolean S3Call::addOriginatorCapability(S3MediaDescriptor & descriptor)
{
    PTRACE(5, "S3\t->S3Call::addOriginatorCapability"); 
    if(descriptor.isEmpty()) 
        return false;

    PBoolean capExists = false;
    S3MediaDescriptorList::iterator cap = capabilities[S3MediaDescriptor::originator].begin(); \
    while (cap != capabilities[S3MediaDescriptor::originator].end() && *cap != descriptor) cap++;

    if (cap != capabilities[S3MediaDescriptor::originator].end())
        capExists = true;

    if (!capExists)
    {
        capabilities[S3MediaDescriptor::originator].Append(&descriptor);
        return true;
    } 
    else 
    {
        PTRACE(4, "S3\tCapability is already added: " << descriptor);
        if (cap->merge(descriptor)) 
        {
            PTRACE(4, "S3\tCapabilities merged: " << *cap);
            return true;
        }
        else 
        {
            PTRACE(4, "S3\tCapabily dropped");
            return false;
        }
    }
}




//               addDescriptor

PBoolean S3Call::addDescriptor(S3MediaDescriptor & descriptor, const S3MediaDescriptor::Role role)
{
    PTRACE(5, "S3\t->S3Call::addDescriptor" << endl);

    if(descriptor.isEmpty()) 
    {
        PTRACE(2, "S3\tS3Call::addDescriptor failed, descriptor is empty");
        return false;
    }

    PBoolean capExists = false;
    S3MediaDescriptorList::iterator cap = capabilities[role].begin(); 
    while (cap != capabilities[role].end() && *cap != descriptor) cap++;
    if (cap != capabilities[role].end())
        capExists = true;

    if (!capExists)
    {
        capabilities[role].Append(&descriptor);
        return true;
    } 
    else 
    {
        PTRACE(4, "S3\tCapability is already added: " << descriptor);
        if (cap->mergeH323OLC(descriptor)) 
        {
            PTRACE(4, "S3\tCapabilities merged: " << *cap);
            return true;
        }
        else 
        {
            PTRACE(4, "S3\tCapabily dropped");
            return false;
        }
    }
}




//               checkSelect

PBoolean S3Call::checkSelect()
{
    PTRACE(5, "S3\t->S3Call::checkSelect");

    S3MediaDescriptorList::iterator oDescriptor, tDescriptor;
    S3MediaDescriptor * descriptor;

    for(oDescriptor = capabilities[S3MediaDescriptor::originator].begin(); oDescriptor != 
                      capabilities[S3MediaDescriptor::originator].end(); ++oDescriptor) 
        for(tDescriptor = capabilities[S3MediaDescriptor::terminator].begin(); tDescriptor != 
                          capabilities[S3MediaDescriptor::terminator].end(); ++tDescriptor) 
            if(*oDescriptor == *tDescriptor) 
            {
                descriptor = descriptorFactory.createDescriptor(*oDescriptor);
                descriptor->merge(*tDescriptor);
                if(descriptor->isComplete())
                    sharedCapabilities.Append(descriptor); 
            }

    PTRACE(5, "S3\tcheckSelect sharedCapabilities size = " 
                << sharedCapabilities.GetSize() << ", capabiliites :" << endl);  
    for(oDescriptor = sharedCapabilities.begin(); oDescriptor != sharedCapabilities.end(); ++oDescriptor) 
    {
        oDescriptor->s3print(cout);
        cout << endl;
    }
    
    PBoolean audio, video = false;
    oDescriptor = sharedCapabilities.begin();
    while((!audio || !video) && oDescriptor != sharedCapabilities.end()) 
    {
        if(oDescriptor->isAudio()) 
        {
            selectedDescriptor[S3MediaDescriptor::audio] = &(*oDescriptor);
            audio = true;
        }
        else 
        {
            if(oDescriptor->isVideo()) 
            {
                selectedDescriptor[S3MediaDescriptor::video] = &(*oDescriptor);
                video = true;
            }
        }
        ++oDescriptor;
    }

    if(selectedDescriptor[S3MediaDescriptor::audio]==NULL) 
    {
        selectedDescriptor[S3MediaDescriptor::audio]= new S3MediaDescriptor();
        PTRACE(2, "S3\tcheckSelect failed, common supported audio format not found");   
        return false;
    }

    if(selectedDescriptor[S3MediaDescriptor::video]==NULL) 
    {
        selectedDescriptor[S3MediaDescriptor::video] = new S3MediaDescriptor();
        PTRACE(3, "S3\tcheckSelect: common supported video format not found");  
    }

    return true;
}



//                     getSelectedDescriptor

S3MediaDescriptor * S3Call::getSelectedDescriptor(S3MediaDescriptor::Type media) 
{
    PTRACE(5, "S3\t->S3Call::getSelectedDescriptor");
    
    if (media != S3MediaDescriptor::unsupported)
        return selectedDescriptor[media];
    else
        return NULL;
}



//               OnConnected

PBoolean S3Call::OnConnected(OpalConnection & connection) 
{
    PTRACE(5, "S3\t->S3Call::OnConnected");
    if(checkSelect())
        if(OpalCall::OnConnected(connection)) 
            return true;
        else 
        {
            PTRACE(2, "S3\tOpalCall::OnConnected failed");
            return false;
        }
    else 
    {
        PTRACE(2, "S3\tS3Call::checkSelect failed");
        return false;
    }
}

