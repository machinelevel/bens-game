
#define DIS_FLAG_FLAT			0x0001
#define DIS_FLAG_HOLLOW			0x0002
#define DIS_FLAG_THICK			0x0004
#define DIS_FLAG_AVERAGE_NORM	0x0008
#define DIS_FLAG_SHOW_BACKFACE	0x0010
#define DIS_FLAG_BLEND			0x0020
#define DIS_FLAG_NO_ZBUFFER		0x0040
#define DIS_FLAG_WIREFRAME		0x0080
#define DIS_FLAG_SHAKE			0x0100

extern Boolean	DCubeMouseDisturb;
extern pfVec3	DCubeDisturbMouseLoc;

void TestDisplacementCube(void);
void SwitchDCubeMaterial(long jump);
void DCubeToggleFlat(void);
void DCubeToggleBackface(void);
void DCubeToggleWireframe(void);
void DCubeToggleMouseDisturb(void);
void DCubeToggleShake(void);
void DCubeToggleClock(void);
void DCubeChangeDepth(float delta);
void DCubeChangeLayers(long jump);
void DCubeSubdivide(long jump);

