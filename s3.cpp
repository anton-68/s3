/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
/**
* @file s3.cpp
* @brief Simple test driver for S3
* @author Anton Bondarenko
* @version 1
* @date 2012-08-24
*/

#include "s3.h"
#include "s3_sip_endpoint.h"
#include "s3_h323_endpoint.h"
    
#define DEFAULT_TRACE_LEVEL 0
#define DEFAULT_SIP_PORT 5060
     
PCREATE_PROCESS(S3);

S3::S3() : PProcess(MANUFACTURER_TEXT, 
                    PRODUCT_NAME_TEXT,
                    MAJOR_VERSION, 
                    MINOR_VERSION, 
                    BUILD_TYPE, 
                    BUILD_NUMBER), manager(NULL) 
{
}
 
void S3::Main() {
    PArgList & args = GetArguments();
    args.Parse("v-verbose."
               "r-route:"
               "p-port:"
               , FALSE);    

    if(args.HasOption('v')) {
        PTrace::SetLevel(args.GetOptionCount('v'));
        PTRACE(0, "S3\tTrace level set to " << args.GetOptionCount('v'));
    }
    else {
        PTrace::SetLevel(DEFAULT_TRACE_LEVEL);
        PTRACE(0, "S3\tTrace level set to " << DEFAULT_TRACE_LEVEL);
    }

    if(args.HasOption('r'))     
        PTRACE(5, "S3\tSetting route to " << args.GetOptionString('r'));
    else
        PTRACE(5, "S3\tSetting route to localhost");

    PTrace::SetStream(&cout);
    PTrace::ClearOptions(PTrace::DateAndTime);
    PTrace::SetOptions(PTrace::Timestamp);
    PTrace::SetOptions(PTrace::Thread);
    
    manager = new S3Manager();
    manager->SetUDPPorts(6000, 7999);
    manager->SetTCPPorts(8000, 9999);

    PTRACE(5, "S3\tManager created");
    
    string command;
    
    OpalTransportAddress * bindingSIPAddress;
    if(!args.HasOption('p'))
        bindingSIPAddress = new OpalTransportAddress("udp$*", DEFAULT_SIP_PORT);
    else
        bindingSIPAddress = new OpalTransportAddress("udp$*",  args.GetOptionString('p').AsUnsigned());
    OpalTransportAddress * bindingH323Address = new OpalTransportAddress("tcp$*", 1720);

    S3SIPEndpoint * sipEP = new S3SIPEndpoint(* manager);
    //sipEP->SetRetryTimeouts(6000, 36000);
    sipEP->StartListeners(* bindingSIPAddress);
    PTRACE(5, "S3\tSIP listeners started");
    S3H323Endpoint * h323EP = new S3H323Endpoint(* manager);
    PTRACE(5, "S3\tH.323 endpoint created");

    h323EP->StartListeners(* bindingH323Address);
    PTRACE(5, "S3\tH.323 listeners started");
    h323EP->DisableFastStart(PFalse);
    h323EP->DisableH245Tunneling(PFalse);
    h323EP->DisableH245inSetup(PFalse);
    PTRACE(5, "S3\tH.245 mode configured");

    if(args.HasOption('r'))
        manager->AddRouteEntry(args.GetOptionString('r'));
    else
        manager->AddRouteEntry("h323.*=sip:<da>@127.0.0.1");

    PTRACE(5, "S3\tRoutes loaded");
    
    cout << "Enter 'quit' for exit" << endl;
    while (command.compare("quit") != 0) 
        cin >> command;
    return;
}
