/************************************************************\
	cellgrid.h
	Files for controlling the grid of cells in Ben's project
\************************************************************/

//enum {
//	cellStateEmpty,
//	cellStateHealthy,
//	cellStateCancer,
//	cellStateDead,
//};

#define CELL_FLAG_SHOT				0x01
#define CELL_FLAG_BOUNCE			0x02
#define CELL_FLAG_BOSS				0x04
#define CELL_FLAG_GOOD				0x08

#define CELL_SHIFT_ON_NEXT			0x01
#define CELL_SHIFT_ON				0x02
#define CELL_SHIFT_ON_LAST			0x04

const int kCellGridMaxWalls = 100;

typedef struct OneCell {
	uint8	flags;
	uint8	shift;
	float	goodTimer;
} OneCell;

typedef struct Wall {
	pfVec3	pos[2];
} Wall;

class CellGrid {
public:
	CellGrid(int32 hSize, int32 vSize);
	~CellGrid();

	void Think(void);
	void Draw(void);
	void Clear(void);
	void Mutate(void);
	void SetupWalls(void);
	void DrawWalls(void);
	void DrawHalfPipe(void);
	void DrawCorners(void);
	void LifeUpdate(void);
	OneCell *GetHVCell(int32 h, int32 v, int32 *pIndex);
	OneCell *GetClosestCell(pfVec3 pos, int32 *pCol=NULL, int32 *pRow=NULL, int32 *pIndex=NULL);
	bool KillCellsInRange(pfVec3 center, int32 range, int32 flags);
	void GetBounds(float *pXMin, float *pXMax, float *pYMin, float *pYMax);
	void GetPosFromIndex(int32 index, pfVec3 dest);
	void DrawBackdrop(void);

	float mLifeUpdateTimer;
	float mLifeUpdateTimeInterval;
	int32	mHSize;
	int32	mVSize;
	int32	mNumCells;
	int32	mTotalBadCells;
	float	mCellSize;
	OneCell	*mCells;
	pfVec3	mCenter;
//	Wall	*mWalls;
	pfVec3	mWalls[kCellGridMaxWalls][2];
//	int32	mWallTypes[kCellGridMaxWalls];
	int32	mNumWalls;
};

extern float gHalfPipeRadius;
extern CellGrid	*gCells;

