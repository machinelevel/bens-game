
/************************************************************\
	camera.h
\************************************************************/

void GliderCam(void);
void MoveCamera(void);
void CameraLookAtTarget(void);
void CameraSwingAroundDegrees(float x, float y, float z);
void SetCameraOnAxis(long axis, long sign);
void LevelCamera(void);
void InitCamera(void);
void SetGLViewToCamera(void);

extern pfMatrix	GlobalCameraMatrix;
