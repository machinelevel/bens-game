
#define CHIA_FLAG_DIAGONALS			0x00000001
#define CHIA_FLAG_TAPER_ALPHA		0x00000002
#define CHIA_FLAG_FIRE_EFFECT		0x00000004
#define CHIA_FLAG_TORNADO_EFFECT	0x00000008
#define CHIA_FLAG_REVERSE_ORDER		0x00000010

#define QUICKMODEL_FLAG_TRIS			0x00000001
#define QUICKMODEL_FLAG_QUADS			0x00000002
#define QUICKMODEL_FLAG_COLOR_PER_VERT	0x00000004
#define QUICKMODEL_FLAG_COLOR_PER_FACE	0x00000008
#define QUICKMODEL_FLAG_COLOR_PER_MODEL	0x00000010
#define QUICKMODEL_FLAG_NORMAL_PER_VERT	0x00000020
#define QUICKMODEL_FLAG_NORMAL_PER_FACE	0x00000040

typedef struct quickModel {
	pfMatrix	matrix;
	pfVec3		scale;
	pfVec3		*vList;
	pfVec3		*nList;
	pfVec4		*cList;
	pfVec2		*tList;
	uint16		*iList;
	uint32		indexNum;
	int32		textureID;
	uint32		flags;
} quickModel;

void drawFloorSquare(float x, float y, float z, float size);
void drawFloor(void);
void AppDrawText(GLuint x, GLuint y, GLuint scale, char* format, ...);
void PostDraw(void);
void DrawMainWindow(int tile_h, int tile_v);
void DrawQuickModel(quickModel *qm);
void DrawChiaQuad(pfVec3 *verts, pfVec3 extrusion, int layers, int numFins, float alphaScale, uint32 flags);
