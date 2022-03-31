
typedef float	foxpfVec2[2];
typedef float	foxpfVec3[3];
typedef float	foxpfVec4[4];
typedef float	foxpfMatrix[4][4];
typedef float	foxpfSmallMatrix[4][3];	/* don't need the last column for many things */



typedef struct {
    foxpfVec3	min;
    foxpfVec3	max;
} foxpfBox;


typedef struct foxpfTilingElement {
	short   TMem;
	uchar	Line;
	uchar    ClampMirrorFlags; /* 0xF0 is S, 0x0F is T */

	uchar    WrapMaskS,WrapMaskT;
	uchar    ShiftS,ShiftT;

	short	uls,ult,lrs,lrt;

} foxpfTilingElement;


typedef struct foxpfTextureDef {
	long	Flags;
	short	ShiftedUSize,ShiftedVSize;  /* Shifted by TEXTURE_IMAGE_FRAC */
	short	tileSize[2]; /* Size of tiles that this texture didn't have to be chopped into. */

	uchar	ImageFormat;
	uchar    ImageSize;
	uchar	BaseTile;
	uchar	NumTiles;
	
	short   hsize,vsize;
	short   SScale,TScale;
	short	LoadDxt;
	short	LoadSize;

	foxpfTilingElement *TilingInfo[7]; 
	void        *ImagePointer;   /* Stores the library ID num of the texture when first loaded */
	void        *TablePointer;
} foxpfTexture;

typedef struct foxpfMaterialDef {
	long	 Flags;
	short    CycleType;
	uchar 	CombineMode1[8];
	uchar 	CombineMode2[8];
	long     RenderMode1;
	long     RenderMode2;
	uchar    PrimColor[6];
	uchar    EnvColor[4];
	uchar    FogColor[4];
	uchar    BlendColor[4];

	/*  Things not yet implemented */
	/*
	PipelineMode
	CombineKey
	AlphaCompare
	DepthSource
	BlendMask
	SetColorDither
   */
} foxpfMaterial;



typedef struct foxpfGeoStateDef {
	long	Flags;
	short	UScroll,VScroll;  /* Used by texscroll.c to scroll textures */
	foxpfTexture *TheTexture;
	foxpfMaterial *TheMaterial;
} foxpfGeoState;






typedef struct foxpfGeoSetDef {
	foxpfGeoState *TheGState;
	void	*Attributes;
	foxpfBox	BBox;
	short	NumPrims;
	short	PrimType;
	long	*PrimLengths;
	void	*IData;          /* For normal collision geometry, this is the index lists. If we are stitched it's a (pfGeode *)  */
	void	*VData;  		  
	long	GeomDisplayPtr;  /* Pointer to Display list for geometry */
	long	VtxData;  		 /* Pointer to Vtxs used in Display List */
	short	VDataLength;     /* in elements (pfVec3s) */
	short	VtxDataLength;	 /* in elements (Vtxs) */
	short	Dummy;			 /*   */
	short	VtxStitchLoad;	 /* We're doing a stitching thing. Load the first X verts under the IData's Geode Matrix */
} foxpfGeoSet;


typedef struct NodeStructDef {
	long	Type;
	long   	DrawTravMask;
	long    ISectTravMask;
	ushort	BitFields;  /* Please see foxpfdefines.h */
#if 0
	ushort	 CleanFrameNum;     /* The last frame in which we constructed a display list for this node */
	void    *CleanDisplayList;  /* Pointer to display list in DPHeap that we constructed earlier this frame */
#else
	ushort   SpecialFunction1;  /* Used in place of node callbacks. eventually implemented differently? */
	long     SpecialFunction2;
#endif	
} foxpfNode;


typedef foxpfNode 	  *NodePtr;

typedef struct GroupDataStructDef {
	long	NumChildren;
	struct	NodeStructDef	**Children;
} GroupData;

typedef struct foxpfDCSDef {
	foxpfNode NodeInfo;
	GroupData GroupInfo;
	foxpfSmallMatrix   Matrix;
	foxpfVec3	  Pivot;
} foxpfDCS;

typedef struct foxpfSCSDef {
	foxpfNode NodeInfo;
	GroupData GroupInfo;
	foxpfSmallMatrix   Matrix;
} foxpfSCS;


typedef struct wolffoxpfBboardDef {
	foxpfNode NodeInfo;
	GroupData GroupInfo;
	short TransMode;
	short RotMode;
	foxpfVec3 Axis;
} wolffoxpfBboard;


typedef struct foxpfSwitchDef {
	foxpfNode NodeInfo;
	GroupData GroupInfo;
	long SwitchVal;
} foxpfSwitch;

#define MAX_LOD_RANGES 8
typedef struct foxpfLODDef {
	foxpfNode NodeInfo;
	GroupData GroupInfo;
	float LODRanges[MAX_LOD_RANGES];
	foxpfVec3 Center;
} foxpfLOD;



typedef struct foxpfGroupDef {
	foxpfNode NodeInfo;
	GroupData GroupInfo;
} foxpfGroup;

typedef struct foxpfGeodeDef {
	foxpfNode NodeInfo;
	long	NumGSets;
	foxpfGeoSet	**GeoSets;
	foxpfBox	BBox;
	void		*UsedMatrix1;
	void		*UsedMatrix2;
} foxpfGeode;




