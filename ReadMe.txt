Originally created at the USC World Building Media Lab
worldbuilding.usc.edu

IMPROVED BY DMITRI G.

Release Notes:

Version 2.3
Fixes: 
- Minor fixes to get the right names in the xml

Version 2.25

Improvements: 
- Support for multiple skeletons added to UnitySample.cpp
- XML Nodes changed to allow the usage of multiple skeletons and rigid bodies in UnitySample.cpp
- Some attributes added to these new XML nodes in UnitySample.cpp
- Another version of rigid body script added to UnityProject
- New class with arrays for multiple skeletons added to Unity Project

Version 2.1
Fixes: 
- Fixed linker error in Visual Studio project and rebuilt UnitySample.exe for release.

Version 2. 

Improvements:
Added ability to override a specific RB origin instead of using the origin assigned to OptitrackRigidBodyManager.

OptitrackRigidBody.cs
- Added ability to choose whether you want to use positional and/or rotational tracking.

Fixes:
- There was an issue where if you rotated the origin away from the World alignment it didn't correctly rotate all the rigid bodies. Now you can rotate the origin to some orientation other than the World and all RBs will get rotated correctly.
- SlipStream port was being used after running the app once. Fix kills the SlipStream when the application quits so the port is freed up again.