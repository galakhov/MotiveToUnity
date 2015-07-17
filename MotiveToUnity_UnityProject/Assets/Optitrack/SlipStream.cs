
//=============================================================================----
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
//=============================================================================----

// This script is intended to be attached to a Game Object.  It will receive
// XML data from the NatNet UnitySample application and notify any listening
// objects via the PacketNotification delegate.

using UnityEngine;
using System.Collections;
using System;
using System.Text;
using System.Net;
using System.Net.Sockets;

public delegate void PacketReceivedHandler(object sender, string PacketData);

public class SlipStream : MonoBehaviour
{
	public string IP = "127.0.0.1";
	public int Port  = 16000;
	public event PacketReceivedHandler PacketNotification;
	[HideInInspector] public int bytesReceived = 0;
	private int maxSubPacketProcess;
	
	private IPEndPoint mRemoteIpEndPoint;
	private Socket     mListener;
	private byte[]     mReceiveBuffer;
	private string     mPacket;
	private int        mPreviousSubPacketIndex = 0;
	private const int  kMaxSubPacketSize       = 1400;
	private XmlDocument xmlDoc;
	
	[HideInInspector] public string[] skeletonNames;
	[HideInInspector] public static SlipStream Instance;
	
	void Awake()
	{
		Instance = this;
		mReceiveBuffer = new byte[kMaxSubPacketSize];
		mPacket        = System.String.Empty;
		mRemoteIpEndPoint = new IPEndPoint(IPAddress.Parse(IP), Port);
		mListener = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
		mListener.Bind(mRemoteIpEndPoint);
        mListener.Blocking          = false;
		mListener.ReceiveBufferSize = 128*1024;

		xmlDoc = new XmlDocument();
	}
	
	void Start()
	{
		// Receives first packet to initialize skeletonBones directly on Start
		try
		{
			bytesReceived = mListener.Receive(mReceiveBuffer);

			maxSubPacketProcess = 200;
			
			while(bytesReceived>0 && maxSubPacketProcess>0)
			{
				//== ensure header is present ==--
				if(bytesReceived>=2)
				{
					int  subPacketIndex = mReceiveBuffer[0];
					bool lastPacket     = mReceiveBuffer[1]==1;
					
					if(subPacketIndex==0)
					{
						mPacket = System.String.Empty;
					}
					
					if(subPacketIndex==0 || subPacketIndex==mPreviousSubPacketIndex+1)
					{
						mPacket += Encoding.ASCII.GetString(mReceiveBuffer, 2, bytesReceived-2);
						
						mPreviousSubPacketIndex = subPacketIndex;

						if(lastPacket)
						{
							//== ok packet has been created from sub packets and is complete ==--
							//== notify listeners ==--
							//if(PacketNotification!=null) {
							//	PacketNotification(this, mPacket);
							//}

							//Debug.Log ("Starting stream: mPacket: "+mPacket);
							
							xmlDoc.LoadXml(mPacket);

							// the amount of skeletons defined in motive
							XmlNodeList sk_list = xmlDoc.SelectNodes("/NodeList/Skeleton");
							
							// Collect skeleton data
							if(sk_list.Count > 0) {
								// select all bones of all skeletons and put it in one list
								XmlNodeList sk_b_list = xmlDoc.SelectNodes("/NodeList/Skeleton/Bone");

								// get only root bones with those specific attributes and specific names
								XmlNodeList sk_names = xmlDoc.SelectNodes("/NodeList/Skeleton/Bone[@parentID='0']");
								int i = 0;
								skeletonNames = new string[sk_names.Count];
								foreach (XmlNode xn in sk_names)
								{
									skeletonNames[i] = System.Convert.ToString(xn.Attributes["Name"].Value);
									// extract Motive's skeleton names from these nodes
									skeletonNames[i] = skeletonNames[i].Substring(0, skeletonNames[i].Length - 4);
									i++;
								}
								OptitrackRigidBodyManager.instance.skeletonBonesList = sk_b_list;
								OptitrackRigidBodyManager.instance.skeletonList = sk_list;
							}
							break; // exit from while loop after receiving of this first packet
						}
					}			
				}
				
				bytesReceived = mListener.Receive(mReceiveBuffer);
				
				//== time this out of packets are coming in faster than we can process ==--
				maxSubPacketProcess--;
			}
		}
		catch(System.Exception ex)
		{}
	}
 
	public void UDPRead()
	{
		try
		{
			bytesReceived = mListener.Receive(mReceiveBuffer);
			
			maxSubPacketProcess = 200;
			
			while(bytesReceived>0 && maxSubPacketProcess>0)
			{
				//== ensure header is present ==--
				if(bytesReceived>=2)
				{
					int  subPacketIndex = mReceiveBuffer[0];
					bool lastPacket     = mReceiveBuffer[1]==1;
					
					if(subPacketIndex==0)
					{
						mPacket = System.String.Empty;
					}
					
					if(subPacketIndex==0 || subPacketIndex==mPreviousSubPacketIndex+1)
					{
						mPacket += Encoding.ASCII.GetString(mReceiveBuffer, 2, bytesReceived-2);
						
						mPreviousSubPacketIndex = subPacketIndex;
						
						if(lastPacket)
						{
							//== ok packet has been created from sub packets and is complete ==--
							//== notify listeners ==--
							if(PacketNotification!=null) {
								PacketNotification(this, mPacket);
							}
						}
					}			
				}
				
				bytesReceived = mListener.Receive(mReceiveBuffer);
				
				//== time this out of packets are coming in faster than we can process ==--
				maxSubPacketProcess--;
			}
		}
		catch(System.Exception ex)
		{}
	}
 
	void Update()
	{
		UDPRead();
	}

    void OnApplicationQuit() {
        mListener.Close();
    }
}
