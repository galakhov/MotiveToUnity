using UnityEngine;
using System.Collections;

[System.Serializable]
public class SkeletonBones : MonoBehaviour {

	public SkeletonBones(){}

	public string[] skeletonBonesNames;
	public Vector3[] skeletonBonesPositions;
	public Quaternion[] skeletonBonesQuaternions;
	public int[] skeletonBonesIDs;

	public int this[int i] {
		get {
			return skeletonBonesIDs[i];
		}
		
		set { 
			skeletonBonesIDs[i] = value; 
		}
	}

	public string getSkeletonBoneName(int i) {
		return skeletonBonesNames[i];
	}
		
	public void	setSkeletonBoneName(int i, string v) { 
		skeletonBonesNames[i] = v; 
	}

	public Vector3 getSkeletonBonePosition(int i) {
			return skeletonBonesPositions[i];
	}
		
	public void setSkeletonBonePosition(int i, Vector3 p) { 
			skeletonBonesPositions[i] = p;
	}
	
	public Quaternion getSkeletonBoneQuaternion(int i) {
		return skeletonBonesQuaternions[i];
	}
		
	public void setSkeletonBoneQuaternion(int i, Quaternion q) { 
		skeletonBonesQuaternions[i] = q;
	}

	public int Length {
		get {
			return skeletonBonesIDs.Length;
		}
	}
	
	public long LongLength {
		get {
			return skeletonBonesIDs.LongLength;
		}
	}
}