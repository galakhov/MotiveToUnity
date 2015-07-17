//=============================================================================
// Original Code Extended By Author: Bradley Newman - USC Worldbuilding Media Lab - worldbuilding.usc.edu
//
// Copyright © NaturalPoint, Inc. All Rights Reserved.
// 
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall NaturalPoint, Inc. or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//=============================================================================


/*

UnitySample.cpp

This program connects to a NatNet server, receives a data stream, encodes a skeleton to XML, and
outputs XML locally over UDP to Unity.  The purpose is to illustrate how to get data into Unity3D.

Usage [optional]:

	UnitySample [ServerIP] [LocalIP]

	[ServerIP]			IP address of the server (e.g. 192.168.0.107) ( defaults to local machine)
*/

#include <stdio.h>
#include <tchar.h>
#include <conio.h>
#include <winsock2.h>
#include <string>
#include <sstream>
#include <map>

#include "NatNetTypes.h"
#include "NatNetClient.h"

#include "tinyxml/tinyxml.h"  //== for xml encoding of Unity3D payload
#include "NatNetRepeater.h"   //== for transport of data over UDP to Unity3D

//== Slip Stream globals ==--

cSlipStream gSlipStream("127.0.0.1",16000);
std::map<std::string, std::string> gBoneNames;
std::map<std::string, int> gBoneParentIDs;

#pragma warning( disable : 4996 )

void __cdecl DataHandler(sFrameOfMocapData* data, void* pUserData);		// receives data from the server
void __cdecl MessageHandler(int msgType, char* msg);		            // receives NatNet error mesages
void resetClient();
int CreateClient(int iConnectionType);

unsigned int MyServersDataPort = 3130;
unsigned int MyServersCommandPort = 3131;

NatNetClient* theClient;
FILE* fp;

char szMyIPAddress[128] = "";
char szServerIPAddress[128] = "";

void SendXMLToUnity(sFrameOfMocapData *data, void* pUserData);

int _tmain(int argc, _TCHAR* argv[])
{
    int iResult;
    int iConnectionType = ConnectionType_Multicast;
    //int iConnectionType = ConnectionType_Unicast;
    
    // parse command line args
    if(argc>1)
    {
        strcpy(szServerIPAddress, argv[1]);	// specified on command line
        printf("Connecting to server at %s...\n", szServerIPAddress);
    }
    else
    {
        strcpy(szServerIPAddress, "");		// not specified - assume server is local machine
        printf("Connecting to server at LocalMachine\n");
    }
    if(argc>2)
    {
        strcpy(szMyIPAddress, argv[2]);	    // specified on command line
        printf("Connecting from %s...\n", szMyIPAddress);
    }
    else
    {
        strcpy(szMyIPAddress, "");          // not specified - assume server is local machine
        printf("Connecting from LocalMachine...\n");
    }

    // Create NatNet Client
    iResult = CreateClient(iConnectionType);
    if(iResult != ErrorCode_OK)
    {
        printf("Error initializing client.  See log for details.  Exiting");
        return 1;
    }
    else
    {
        printf("Client initialized and ready.\n");
    }


	// send/receive test request
	printf("[SampleClient] Sending Test Request\n");
	void* response;
	int nBytes;
	char str_temp [10];
	iResult = theClient->SendMessageAndWait("TestRequest", &response, &nBytes);
	if (iResult == ErrorCode_OK)
	{
		printf("[SampleClient] Received: %s", (char*)response);
	}

	// Retrieve Data Descriptions from server
	printf("\n\n[SampleClient] Requesting Data Descriptions...");
	sDataDescriptions* pDataDefs = NULL;
	int nBodies = theClient->GetDataDescriptions(&pDataDefs);

	if(!pDataDefs)
	{
		printf("[SampleClient] Unable to retrieve Data Descriptions.");
	}
	else
	{
        printf("[SampleClient] Received %d Data Descriptions:\n", pDataDefs->nDataDescriptions );
        for(int i=0; i < pDataDefs->nDataDescriptions; i++)
        {
            printf("Data Description # %d (type=%d)\n", i, pDataDefs->arrDataDescriptions[i].type);
            if(pDataDefs->arrDataDescriptions[i].type == Descriptor_MarkerSet)
            {
                // MarkerSet
                sMarkerSetDescription* pMS = pDataDefs->arrDataDescriptions[i].Data.MarkerSetDescription;
                printf("MarkerSet Name : %s\n", pMS->szName);
                for(int i=0; i < pMS->nMarkers; i++)
                    printf("%s\n", pMS->szMarkerNames[i]);

            }
            else if(pDataDefs->arrDataDescriptions[i].type == Descriptor_RigidBody)
            {
				//ORIGINAL CODE START 
				
                // RigidBody
                sRigidBodyDescription* pRB = pDataDefs->arrDataDescriptions[i].Data.RigidBodyDescription;
                printf("RigidBody Name : %s\n", pRB->szName);
                printf("RigidBody ID : %d\n", pRB->ID);
                printf("RigidBody Parent ID : %d\n", pRB->parentID);
                printf("Parent Offset : %3.2f,%3.2f,%3.2f\n", pRB->offsetx, pRB->offsety, pRB->offsetz);
				//ORIGINAL CODE END
				sprintf (str_temp, "%d", LOWORD(pRB->ID));
				gBoneNames[str_temp] = pRB->szName;

				//NEW CODE START
				/*
				sRigidBodyDescription* pRB = pDataDefs->arrDataDescriptions[i].Data.RigidBodyDescription;
				printf("RigidBody Name : %s\n", pRB->szName);
				printf("RigidBody ID : %d\n", pRB->ID);
				printf("RigidBody Parent ID : %d\n", pRB->parentID);
				printf("Parent Offset : %3.2f,%3.2f,%3.2f\n", pRB->offsetx, pRB->offsety, pRB->offsetz);
				for (int j = 0; j < pRB->nRigidBodies; j++)
				{
					sRigidBodyDescription* pRB = &pRB->RigidBodies[j];
					printf("  RigidBody Name : %s\n", pRB->szName);
					printf("  RigidBody ID : %d\n", pRB->ID);
					printf("  RigidBody Parent ID : %d\n", pRB->parentID);
					printf("  Parent Offset : %3.2f,%3.2f,%3.2f\n", pRB->offsetx, pRB->offsety, pRB->offsetz);

					// populate bone name dictionary for use in xml ==--
					gBoneNames[pRB->ID] = pRB->szName;
				}*/
				//NEW CODE END
            }
            else if(pDataDefs->arrDataDescriptions[i].type == Descriptor_Skeleton)
            {
                // Skeleton
                sSkeletonDescription* pSK = pDataDefs->arrDataDescriptions[i].Data.SkeletonDescription;
                printf("Skeleton Name : %s\n", pSK->szName);
                printf("Skeleton ID : %d\n", pSK->skeletonID);
                printf("RigidBody (Bone) Count : %d\n", pSK->nRigidBodies);
                for(int j=0; j < pSK->nRigidBodies; j++)
                {
                    sRigidBodyDescription* pRB = &pSK->RigidBodies[j];
					sprintf (str_temp, "%d%d", pSK->skeletonID, LOWORD(pRB->ID));
                    printf("  Skeleton RigidBody Name : %s\n", pRB->szName);
                    printf("  Skeleton RigidBody ID : %d\n", pRB->ID);
                    printf("  Skeleton RigidBody Parent ID : %d\n", pRB->parentID);
                    printf("  Parent Offset : %3.2f,%3.2f,%3.2f\n", pRB->offsetx, pRB->offsety, pRB->offsetz);

                    // populate bone name dictionary for use in xml ==--
                    //gBoneNames[pRB->ID] = pRB->szName;
					gBoneNames[str_temp] = pRB->szName;
					gBoneParentIDs[str_temp] = pRB->parentID;
                }
            }
            else
            {
                printf("Unknown data type.");
                // Unknown
            }
        }      
	}

	// Ready to receive marker stream!
	printf("\nClient is connected to server and listening for data...\n");
	int c;
	bool bExit = false;
	while(c =_getch())
	{
		switch(c)
		{
			case 'q':
				bExit = true;		
				break;	
			case 'r':
				resetClient();
				break;	
            case 'p':
                sServerDescription ServerDescription;
                memset(&ServerDescription, 0, sizeof(ServerDescription));
                theClient->GetServerDescription(&ServerDescription);
                if(!ServerDescription.HostPresent)
                {
                    printf("Unable to connect to server. Host not present. Exiting.");
                    return 1;
                }
                break;	
            case 'f':
                {
                    sFrameOfMocapData* pData = theClient->GetLastFrameOfData();
                    printf("Most Recent Frame: %d", pData->iFrame);
                }
                break;	
            case 'm':	                        // change to multicast
                iResult = CreateClient(ConnectionType_Multicast);
                if(iResult == ErrorCode_OK)
                    printf("Client connection type changed to Multicast.\n\n");
                else
                    printf("Error changing client connection type to Multicast.\n\n");
                break;
            case 'u':	                        // change to unicast
                iResult = CreateClient(ConnectionType_Unicast);
                if(iResult == ErrorCode_OK)
                    printf("Client connection type changed to Unicast.\n\n");
                else
                    printf("Error changing client connection type to Unicast.\n\n");
                break;


			default:
				break;
		}
		if(bExit)
			break;
	}

	// Done - clean up.
	theClient->Uninitialize();

	return ErrorCode_OK;
}

// Establish a NatNet Client connection
int CreateClient(int iConnectionType)
{
    // release previous server
    if(theClient)
    {
        theClient->Uninitialize();
        delete theClient;
    }

    // create NatNet client
    theClient = new NatNetClient(iConnectionType);

    // [optional] use old multicast group
    //theClient->SetMulticastAddress("224.0.0.1");

    // print version info
    unsigned char ver[4];
    theClient->NatNetVersion(ver);
    printf("NatNet Sample Client (NatNet ver. %d.%d.%d.%d)\n", ver[0], ver[1], ver[2], ver[3]);

    // Set callback handlers
    theClient->SetMessageCallback(MessageHandler);
    theClient->SetVerbosityLevel(Verbosity_Debug);
    theClient->SetDataCallback( DataHandler, theClient );	// this function will receive data from the server

    // Init Client and connect to NatNet server
    // to use NatNet default port assigments
    int retCode = theClient->Initialize(szMyIPAddress, szServerIPAddress);
    // to use a different port for commands and/or data:
    //int retCode = theClient->Initialize(szMyIPAddress, szServerIPAddress, MyServersCommandPort, MyServersDataPort);
    if (retCode != ErrorCode_OK)
    {
        printf("Unable to connect to server.  Error code: %d. Exiting", retCode);
        return ErrorCode_Internal;
    }
    else
    {
        // print server info
        sServerDescription ServerDescription;
        memset(&ServerDescription, 0, sizeof(ServerDescription));
        theClient->GetServerDescription(&ServerDescription);
        if(!ServerDescription.HostPresent)
        {
            printf("Unable to connect to server. Host not present. Exiting.");
            return 1;
        }
        printf("[SampleClient] Server application info:\n");
        printf("Application: %s (ver. %d.%d.%d.%d)\n", ServerDescription.szHostApp, ServerDescription.HostAppVersion[0],
            ServerDescription.HostAppVersion[1],ServerDescription.HostAppVersion[2],ServerDescription.HostAppVersion[3]);
        printf("NatNet Version: %d.%d.%d.%d\n", ServerDescription.NatNetVersion[0], ServerDescription.NatNetVersion[1],
            ServerDescription.NatNetVersion[2], ServerDescription.NatNetVersion[3]);
        printf("Client IP:%s\n", szMyIPAddress);
        printf("Server IP:%s\n", szServerIPAddress);
        printf("Server Name:%s\n\n", ServerDescription.szHostComputerName);
    }

    return ErrorCode_OK;

}

// Create XML from frame data and output to Unity
void SendFrameToUnity(sFrameOfMocapData *data, void *pUserData)
{
	/*
		// Code from: \include\NatNetTypes.h
		// Rigid Body Data (single frame of one rigid body)
		typedef struct sRigidBodyData
		{
			int ID;                                 // RigidBody identifier
			float x, y, z;                          // Position
			float qx, qy, qz, qw;                   // Orientation
			int nMarkers;                           // Number of markers associated with this rigid body
			MarkerData* Markers;                    // Array of marker data ( [nMarkers][3] )
			int* MarkerIDs;                         // Array of marker IDs
			float* MarkerSizes;                     // Array of marker sizes
			float MeanError;                        // Mean measure-to-solve deviation
			short params;                           // Host defined tracking flags
			sRigidBodyData()
			{
				Markers = 0; MarkerIDs = 0; MarkerSizes = 0; params=0;
			}
		} sRigidBodyData;
		
		// Skeleton Data
		typedef struct
		{
			int skeletonID;                                          // Skeleton identifier
			int nRigidBodies;                                        // # of rigid bodies
			sRigidBodyData* RigidBodyData;                           // Array of RigidBody data
		} sSkeletonData;
		
		// Single frame of data (for all tracked objects)
		typedef struct
		{
			int iFrame;                                 // host defined frame number
			int nMarkerSets;                            // # of marker sets in this frame of data
			sMarkerSetData MocapData[MAX_MODELS];       // MarkerSet data
			int nOtherMarkers;                          // # of undefined markers
			MarkerData* OtherMarkers;                   // undefined marker data
			int nRigidBodies;                           // # of rigid bodies
			sRigidBodyData RigidBodies[MAX_RIGIDBODIES];// Rigid body data (see: sRigidBodyData structure)
			int nSkeletons;                             // # of Skeletons
			sSkeletonData Skeletons[MAX_SKELETONS];     // Skeleton data (see: sSkeletonData structure)
			int nLabeledMarkers;                        // # of Labeled Markers
			sMarker LabeledMarkers[MAX_LABELED_MARKERS];// Labeled Marker data (labeled markers not associated with a "MarkerSet")
			float fLatency;                             // host defined time delta between capture and send
			unsigned int Timecode;                      // SMPTE timecode (if available)
			unsigned int TimecodeSubframe;              // timecode sub-frame data
			double fTimestamp;                          // FrameGroup timestamp
			short params;                               // host defined parameters

		} sFrameOfMocapData;
	*/
	// form XML document

    TiXmlDocument doc;  
    TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );
	char buff [10];

	TiXmlElement * root = new TiXmlElement( "NodeList" );

	//TiXmlElement * root0 = new TiXmlElement( "RigidBodies" );  
	//root->SetAttribute("skeletonID", skData.skeletonID);

	//data->RigidBodies

	if(data->nRigidBodies > 0 ) //data->RigidBodies>0
	{
		root->SetAttribute("nRigidBodies", data->nRigidBodies);
		TiXmlElement * bones0 = new TiXmlElement( "RigidBodies" );
		TiXmlElement * bone0;
		//NEW CODE START: Enables streaming of multiple rigid bodies over UDP to Unity
		for (int i = 0; i < data->nRigidBodies; i++) {
			sRigidBodyData rbData = data->RigidBodies[i];

			// separate RB's markers from skeleton's markers
			//if (rbData.nMarkers < 20) // if number of markers associated with this rigid body is less than 20
			
			bone0 = new TiXmlElement("RigidBody");
			
			sprintf (buff, "%d", LOWORD(rbData.ID));

			bone0->SetAttribute("ID", rbData.ID);
			bone0->SetAttribute("Name", gBoneNames[buff]); // gBoneNames[LOWORD(rbData.ID)].c_str() //gBoneNames[LOWORD(rbData.ID)].c_str()
			bone0->SetDoubleAttribute("x", rbData.x);
			bone0->SetDoubleAttribute("y", rbData.y);
			bone0->SetDoubleAttribute("z", rbData.z);
			bone0->SetDoubleAttribute("qx", rbData.qx);
			bone0->SetDoubleAttribute("qy", rbData.qy);
			bone0->SetDoubleAttribute("qz", rbData.qz);
			bone0->SetDoubleAttribute("qw", rbData.qw);

			bones0->LinkEndChild(bone0);
		}
		//NEW CODE END

		//root0->LinkEndChild( bones0 );
		root->LinkEndChild( bones0 );
	}

	if(data->nSkeletons > 0) //data->Skeletons>0
    {
		root->SetAttribute("nSkeletons", data->nSkeletons);
		//NEW CODE START: rigid bodies attached to their skeletons (in case of skeleton streaming as rigid bodies)
		for (int n = 0; n < data->nSkeletons; n++) {

			sSkeletonData skData = data->Skeletons[n];

			TiXmlElement * bones = new TiXmlElement( "Skeleton" );
			bones->SetAttribute("skeletonID", skData.skeletonID);

			for (int j = 0; j < skData.nRigidBodies; j++)
			{
				TiXmlElement * bone = new TiXmlElement("Bone");

				// gBoneNames[identifier]: to select the correct name of a RB-bone.
				// It is saved into 'buff' variable and consists of a string with 'skeletonID+RigidBodyID'
				sprintf (buff, "%d%d", LOWORD(skData.skeletonID), LOWORD(skData.RigidBodyData[j].ID));

				bone->SetAttribute("ID", LOWORD(skData.RigidBodyData[j].ID)-1);
				bone->SetAttribute("RBID", skData.RigidBodyData[j].ID);
				bone->SetAttribute("SKRBID", buff); // gBoneNames identifier is used here (see above)
				bone->SetAttribute("Name", gBoneNames[buff]); // gBoneNames[LOWORD(skData.RigidBodyData[j].ID)].c_str()// gBoneNames[LOWORD(rbData.ID)].c_str()
				bone->SetAttribute("parentID", gBoneParentIDs[buff]);
				bone->SetDoubleAttribute("x", skData.RigidBodyData[j].x);
				bone->SetDoubleAttribute("y", skData.RigidBodyData[j].y);
				bone->SetDoubleAttribute("z", skData.RigidBodyData[j].z);
				bone->SetDoubleAttribute("qx", skData.RigidBodyData[j].qx);
				bone->SetDoubleAttribute("qy", skData.RigidBodyData[j].qy);
				bone->SetDoubleAttribute("qz", skData.RigidBodyData[j].qz);
				bone->SetDoubleAttribute("qw", skData.RigidBodyData[j].qw);

				bones->LinkEndChild(bone);
			}
			root->LinkEndChild( bones );
		}
		//NEW CODE END
    }
	
	// convert xml document into a buffer filled with data ==--

	//TiXmlElement * root = new TiXmlElement( "Latency" );
	
	// host defined time delta between capture and send
	root->SetDoubleAttribute("LatencyTimeDelta", data->fLatency);

	doc.LinkEndChild( decl );
	doc.LinkEndChild( root );
	//doc.LinkEndChild( root0 );
	//doc.LinkEndChild( root1 );

    std::ostringstream stream;
    stream << doc;
    std::string str =  stream.str();
    const char* buffer = str.c_str();

    // stream xml data over UDP via SlipStream ==--

    gSlipStream.Stream((unsigned char *) buffer,strlen(buffer));
}

// DataHandler receives data from the server
void __cdecl DataHandler(sFrameOfMocapData* data, void* pUserData)
{
	NatNetClient* pClient = (NatNetClient*) pUserData;

	printf("Received frame %d\n", data->iFrame);

    SendFrameToUnity(data, pUserData);
}

// MessageHandler receives NatNet error/debug messages
void __cdecl MessageHandler(int msgType, char* msg)
{
	printf("\n%s\n", msg);
}

void resetClient()
{
	int iSuccess;

	printf("\n\nre-setting Client\n\n.");

	iSuccess = theClient->Uninitialize();
	if(iSuccess != 0)
		printf("error un-initting Client\n");

	iSuccess = theClient->Initialize(szMyIPAddress, szServerIPAddress);
	if(iSuccess != 0)
		printf("error re-initting Client\n");


}

