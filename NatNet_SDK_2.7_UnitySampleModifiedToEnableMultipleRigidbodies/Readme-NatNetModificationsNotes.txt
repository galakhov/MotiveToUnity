// Original Code Extended By Author: Bradley Newman - USC Worldbuilding Media Lab - worldbuilding.usc.edu
Modified UnitySample.cpp to enable streaming of multiple rigid bodies to Unity:

Line 345
//ORIGINAL CODE START
/*
sRigidBodyData rbData = data->RigidBodies[0];

bone = new TiXmlElement( "Bone" );  
bones->LinkEndChild( bone );  

bone->SetAttribute      ("ID"  , rbData.ID);
bone->SetAttribute      ("Name", gBoneNames[LOWORD(rbData.ID)].c_str());
bone->SetDoubleAttribute("x"   , rbData.x);
bone->SetDoubleAttribute("y"   , rbData.y);
bone->SetDoubleAttribute("z"   , rbData.z);
bone->SetDoubleAttribute("qx"  , rbData.qx);
bone->SetDoubleAttribute("qy"  , rbData.qy);
bone->SetDoubleAttribute("qz"  , rbData.qz);
bone->SetDoubleAttribute("qw"  , rbData.qw);
*/
//ORIGINAL CODE END

//NEW CODE START: Enables streaming of multiple rigid bodies over UDP to Unity
for (int i = 0; i < data->nRigidBodies; i++) {
	
	sRigidBodyData rbData = data->RigidBodies[i];

	bone = new TiXmlElement("Bone");
	bones->LinkEndChild(bone);

	bone->SetAttribute("ID", rbData.ID);
	bone->SetAttribute("Name", gBoneNames[LOWORD(rbData.ID)].c_str());
	bone->SetDoubleAttribute("x", rbData.x);
	bone->SetDoubleAttribute("y", rbData.y);
	bone->SetDoubleAttribute("z", rbData.z);
	bone->SetDoubleAttribute("qx", rbData.qx);
	bone->SetDoubleAttribute("qy", rbData.qy);
	bone->SetDoubleAttribute("qz", rbData.qz);
	bone->SetDoubleAttribute("qw", rbData.qw);
}
//NEW CODE END
