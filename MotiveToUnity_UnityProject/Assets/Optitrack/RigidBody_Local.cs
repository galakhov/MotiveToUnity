using UnityEngine;
using System.Collections;

public class RigidBody_Local : MonoBehaviour 
{
    public string rigidBodyName = "Rigid Body 1";

    private Vector3    oldPosition    = new Vector3();
    private Quaternion oldOrientation = new Quaternion();
	private Quaternion oldAngularV    = new Quaternion();
	private Vector3 thisPosition;
	private Quaternion thisOrientation;
	private Vector3 posVelocity;
	private Quaternion thisAngularV;
	private Quaternion AngularAcc;
	private int ind;

	public void Start()
	{

	}

	public void Update() 
    {
		ind = OptitrackRigidBodyManager.instance.getRigidBodyIndByName(rigidBodyName);
		if (ind == -1)
			return;

		thisPosition = OptitrackRigidBodyManager.instance.getRigidBodyPosByInd(ind);
		thisOrientation = OptitrackRigidBodyManager.instance.getRigidBodyRotByInd (ind);

		posVelocity		= thisPosition - oldPosition;
		thisAngularV	= thisOrientation * Quaternion.Inverse (oldOrientation);
		AngularAcc		= thisAngularV * Quaternion.Inverse (oldAngularV);

		transform.localPosition = thisPosition;
		transform.localRotation = thisOrientation;

		oldPosition		= new Vector3(thisPosition.x, thisPosition.y, thisPosition.z);
		oldOrientation	= new Quaternion(thisOrientation.x, thisOrientation.y, thisOrientation.z, thisOrientation.w);
		oldAngularV		= new Quaternion (thisAngularV.x, thisAngularV.y, thisAngularV.z, thisAngularV.w);
	}
}