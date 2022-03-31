
/************************************************************\
	guideway.h
\************************************************************/

typedef struct guideArc {
	float	length;		/**** arc length in world units ****/
	float	curvature;	/**** 1.0 / radius of curvature.   0 == straight  ****/
	pfVec3	curveAxis;	/**** the axis around which the curve takes place ****/
	pfVec4	color;
	pfMatrix	endMat[2];	/**** matrices that define the endpoints ****/
} guideArc;

#define gwARC_LENGTH(_degrees, _radius) (2.0f * ((float)PF_PI) * (_radius) * ((_degrees) / 360.0f))
#define gwARC_DEGREES(_length, _curvature) (360.0f * _length * _curvature / (2.0f * ((float)PF_PI)))
#define gwRADIUS_TO_CURVATURE(_radius)	(1.0f / (_radius))
#define gwCURVATURE_TO_RADIUS(_curvature)	((_curvature == 0.0f) ? 0.0f : (1.0f / (_curvature)))


void InitGuideways(void);
void DrawGLGuideway(long flags);
void DrawGLGuidewayNearBird(long flags, long sections, float nearDist, float farDist);
void DrawGLBird(long flags);
void MoveBird(void);

extern void AdjustBirdSpeed(long amount);
extern void BirdJump(long type);
