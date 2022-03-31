#ifndef FOXPF_H
#define FOXPF_H


#include "foxpfdefines.h"
#include "foxpfstructs.h"

#define PFCLASS_NODE FOXPFCLASS_NODE

#define PFCLASS_GEODE FOXPFCLASS_GEODE
#define PFTYPE_GEODE FOXPFTYPE_GEODE

#define PFCLASS_GROUP FOXPFCLASS_GROUP
#define PFTYPE_GROUP FOXPFTYPE_GROUP
#define PFTYPE_SWITCH FOXPFTYPE_SWITCH
#define PFTYPE_LOD FOXPFTYPE_LOD

#define PFCLASS_SCS FOXPFCLASS_SCS
#define PFTYPE_SCS FOXPFTYPE_SCS
#define PFTYPE_DCS FOXPFTYPE_DCS
#define WOLFPFTYPE_BBOARD WOLFFOXPFTYPE_BBOARD

#define PFGS_POINTS FOXPFGS_POINTS
#define PFGS_LINES FOXPFGS_LINES
#define PFGS_LINESTRIPS FOXPFGS_LINESTRIPS
#define PFGS_TRIS FOXPFGS_TRIS
#define PFGS_QUADS FOXPFGS_QUADS
#define PFGS_TRISTRIPS FOXPFGS_TRISTRIPS
#define PFGS_FLAT_LINESTRIPS FOXPFGS_FLAT_LINESTRIPS
#define PFGS_FLAT_TRISTRIPS FOXPFGS_FLAT_TRISTRIPS

#define PFGS_COORD3 FOXPFGS_COORD3 
#define PFGS_TEXCOORD2 FOXPFGS_TEXCOORD2


#define NODE_FLAG_DCSDIRTY1 FOXNODE_FLAG_DCSDIRTY1
#define NODE_FLAG_SCSDIRTY FOXNODE_FLAG_SCSDIRTY
#define NODE_FLAG_DCSDIRTY2 FOXNODE_FLAG_DCSDIRTY2
#define NODE_FLAG_DCSDIRTY FOXNODE_FLAG_DCSDIRTY

#define NODE_FLAG_NO_INTERSECTION FOXNODE_FLAG_NO_INTERSECTION
#define NODE_FLAG_DCS_NOROT FOXNODE_FLAG_DCS_NOROT
#define NODE_FLAG_DCS_PIVOT FOXNODE_FLAG_DCS_PIVOT
#define NODE_FLAG_INSTANCED FOXNODE_FLAG_INSTANCED




#define PFTEX_LUT_MASK FOXPFTEX_LUT_MASK
#define PFTEX_LUT_NONE FOXPFTEX_LUT_NONE
#define PFTEX_LUT_RGBA16 FOXPFTEX_LUT_RGBA16
#define PFTEX_LUT_IA16 FOXPFTEX_LUT_IA16
#define PFTEX_LUT_INVAL FOXPFTEX_LUT_INVAL

#define PFTEX_DETAIL_MASK FOXPFTEX_DETAIL_MASK
#define PFTEX_DETAIL_CLAMP FOXPFTEX_DETAIL_CLAMP
#define PFTEX_DETAIL_SHARPEN FOXPFTEX_DETAIL_SHARPEN
#define PFTEX_DETAIL_DETAIL FOXPFTEX_DETAIL_DETAIL
#define PFTEX_DETAIL_INVAL FOXPFTEX_DETAIL_INVAL

#define PFTEX_FILTER_MASK FOXPFTEX_FILTER_MASK
#define PFTEX_FILTER_BILERP FOXPFTEX_FILTER_BILERP
#define PFTEX_FILTER_POINT FOXPFTEX_FILTER_POINT
#define PFTEX_FILTER_AVERAGE FOXPFTEX_FILTER_AVERAGE
#define PFTEX_FILTER_INVAL FOXPFTEX_FILTER_INVAL

#define PFTEX_LOD_MASK FOXPFTEX_LOD_MASK
#define PFTEX_LOD_TILE FOXPFTEX_LOD_TILE
#define PFTEX_LOD_LOD FOXPFTEX_LOD_LOD
#define PFTEX_LOD_INVAL FOXPFTEX_LOD_INVAL


#define PFTEX_TILED FOXPFTEX_TILED


#define PFML_R FOXPFML_R
#define PFML_G FOXPFML_G
#define PFML_B FOXPFML_B
#define PFML_A FOXPFML_A
#define PFML_M FOXPFML_M
#define PFML_L FOXPFML_L


#define PFML_PRIMCOLOR FOXPFML_PRIMCOLOR
#define PFML_ENVCOLOR FOXPFML_ENVCOLOR
#define PFML_FOGCOLOR FOXPFML_FOGCOLOR
#define PFML_BLENDCOLOR FOXPFML_BLENDCOLOR



#define PFGSTATE_LIGHTED FOXPFGSTATE_LIGHTED
#define PFGSTATE_FOGGED FOXPFGSTATE_FOGGED
#define PFGSTATE_SMOOTH_SHADED FOXPFGSTATE_SMOOTH_SHADED
#define PFGSTATE_BACKFACE_ELIM FOXPFGSTATE_BACKFACE_ELIM
#define PFGSTATE_TEXGEN_REFLECT FOXPFGSTATE_TEXGEN_REFLECT
#define PFGSTATE_TEXGEN_LINEAR FOXPFGSTATE_TEXGEN_LINEAR
#define PFGSTATE_FRONTFACE_ELIM FOXPFGSTATE_FRONTFACE_ELIM

#define PFGSTATE_TILED_TEXTURE FOXPFGSTATE_TILED_TEXTURE
#define PFGSTATE_TEXTURESCROLL FOXPFGSTATE_TEXTURESCROLL

#define PFSTATE_CULLFACE FOXPFSTATE_CULLFACE
#define PFSTATE_TEXTURE FOXPFSTATE_TEXTURE

#define PFCF_OFF FOXPFCF_OFF
#define PFCF_BACK FOXPFCF_BACK
#define PFCF_FRONT FOXPFCF_FRONT
#define PFCF_BOTH FOXPFCF_BOTH

#define WOLFPFBB_ROT WOLFFOXPFBB_ROT
#define WOLFPFBB_TRANS WOLFFOXPFBB_TRANS

#define WOLFPFBB_ROT_FREE WOLFFOXPFBB_ROT_FREE
#define WOLFPFBB_ROT_AXIAL WOLFFOXPFBB_ROT_AXIAL
#define WOLFPFBB_ROT_POINT_EYE WOLFFOXPFBB_ROT_POINT_EYE
#define WOLFPFBB_ROT_POINT_WORLD WOLFFOXPFBB_ROT_POINT_WORLD

#define WOLFPFBB_TRANS_FREE WOLFFOXPFBB_TRANS_FREE
#define WOLFPFBB_TRANS_FIXED WOLFFOXPFBB_TRANS_FIXED






#define pfVec2 foxpfVec2
#define pfVec3 foxpfVec3
#define pfVec4 foxpfVec4
#define pfMatrix foxpfMatrix
#define pfSmallMatrix foxpfSmallMatrix


#define pfBox foxpfBox
#define pfTilingElement foxpfTilingElement
#define pfTexture foxpfTexture
#define pfMaterial foxpfMaterial
#define pfGeoState foxpfGeoState
#define pfGeoSet foxpfGeoSet
#define pfNode foxpfNode
#define pfDCS foxpfDCS
#define pfSCS foxpfSCS
#define wolfpfBboard wolffoxpfBboard
#define pfSwitch foxpfSwitch
#define pfLOD foxpfLOD
#define pfGroup foxpfGroup
#define pfGeode foxpfGeode




#define PF_OFF	    0
#define PF_ON	    1

#define PF_X	    0
#define PF_Y	    1
#define PF_Z	    2
#define PF_W	    3
#define PF_T	    3	/* Translation row in matrices */

#define PF_H	    0	/* Heading */
#define PF_P	    1	/* Pitch */
#define PF_R	    2	/* Roll */

#define PF_PI 3.14159265358979332846

#if 0
typedef float	pfVec2[2];
typedef float	pfVec3[3];
typedef float	pfVec4[4];
typedef float	pfMatrix[4][4];
typedef float	pfSmallMatrix[4][3];	/* don't need the last column for many things */
#endif

typedef struct {
    pfVec3	xyz;
    pfVec3	hpr;
} pfCoord;

typedef struct {
    pfVec3	pos;
    pfVec3	dir;
    float	length;
} pfSeg;

typedef struct {
    pfVec3	normal;
    float	offset;		/* pt dot normal = offset */
} pfPlane;




//#if COMPILE_TYPE == CT_PC
//#define pfSqrt	ejasm_sqrt
//#else
//#pragma intrinsic (sqrtf)
#define pfSqrt sqrtf
//#endif

#define PF_SQUARE(_x) ((_x)*(_x))
#define PF_MIN2(_x1,_x2) (((_x1) < (_x2)) ? (_x1) : (_x2))
#define PF_MAX2(_x1,_x2) (((_x1) > (_x2)) ? (_x1) : (_x2))


#define PF_ABS(_x1) ((_x1 < 0) ? -(_x1) : (_x1))	
#define PF_ABSLT(_x1,_x2) ((_x1) < (_x2) && -(_x1) < (_x2))
#define PF_ABSGT(_x1,_x2) ((_x1) > (_x2) || -(_x1) > (_x2))
#define PF_ABSLE(_x1,_x2) ((_x1) <= (_x2) && -(_x1) <= (_x2))
#define PF_ABSGE(_x1,_x2) ((_x1) >= (_x2) || -(_x1) >= (_x2))

#define PFCOPY_VEC2(_dst, _v) \
    (((_dst)[0] = (_v)[0]),     \
     ((_dst)[1] = (_v)[1]))

#define PFNEGATE_VEC2(_dst, _v) \
    (((_dst)[0] = -(_v)[0]),     \
     ((_dst)[1] = -(_v)[1]))

#define PFADD_VEC2(_dst, _v1,_v2) \
    (((_dst)[0] = (_v1)[0]+(_v2)[0]),     \
     ((_dst)[1] = (_v1)[1]+(_v2)[1]))

#define PFSUB_VEC2(_dst, _v1, _v2)     \
    (((_dst)[0] = (_v1)[0] - (_v2)[0]), \
     ((_dst)[1] = (_v1)[1] - (_v2)[1]))

#define PFDOT_VEC2(_v1,_v2) ((_v1)[0]*(_v2)[0] + (_v1)[1]*(_v2)[1])

#define PFLENGTH_VEC2 pfLengthVec2
/* Use the function so we can catch underflows */
/*  #define PFLENGTH_VEC2(_v) \
    (pfSqrt(PFDOT_VEC2((_v), (_v))))  */

#define PFSCALE_VEC2 pfScaleVec2
/* Use the function so we can catch underflows */

#define PFADD_SCALED_VEC2(_dst, _v1, _s2, _v2)   \
    (((_dst)[0] = (_v1)[0] + (_s2)*(_v2)[0]), \
     ((_dst)[1] = (_v1)[1] + (_s2)*(_v2)[1]))

#define PFSQR_DISTANCE_PT2(_v1, _v2) \
    (PF_SQUARE((_v1)[0]-(_v2)[0]) + \
     PF_SQUARE((_v1)[1]-(_v2)[1]))

#define PFSET_VEC2(_dst, _x, _y) \
    (((_dst)[0] = (_x)), \
     ((_dst)[1] = (_y)))

#define PFSET_VEC3(_dst, _x, _y, _z) \
    (((_dst)[0] = (_x)), \
     ((_dst)[1] = (_y)), \
     ((_dst)[2] = (_z)))

#define PFEQUAL_VEC3(_v1, _v2) \
    (((_v1)[0] == (_v2)[0])&&     \
     ((_v1)[1] == (_v2)[1])&&     \
     ((_v1)[2] == (_v2)[2]))

#define PFCOPY_VEC3(_dst, _v) \
    (((_dst)[0] = (_v)[0]),     \
     ((_dst)[1] = (_v)[1]),     \
     ((_dst)[2] = (_v)[2]))

#define PFNEGATE_VEC3(_dst, _v) \
    (((_dst)[0] = -(_v)[0]),     \
     ((_dst)[1] = -(_v)[1]),     \
     ((_dst)[2] = -(_v)[2]))

#define PFADD_VEC3(_dst, _v1,_v2) \
    (((_dst)[0] = (_v1)[0]+(_v2)[0]),     \
     ((_dst)[1] = (_v1)[1]+(_v2)[1]),     \
     ((_dst)[2] = (_v1)[2]+(_v2)[2]))

#define PFSUB_VEC3(_dst, _v1,_v2) \
    (((_dst)[0] = (_v1)[0]-(_v2)[0]),     \
     ((_dst)[1] = (_v1)[1]-(_v2)[1]),     \
     ((_dst)[2] = (_v1)[2]-(_v2)[2]))

#define PFSCALE_VEC3(_dst, _s, _v) \
    (((_dst)[0] = (_s) * (_v)[0]),     \
     ((_dst)[1] = (_s) * (_v)[1]),     \
     ((_dst)[2] = (_s) * (_v)[2]))

#define PFCOMBINE_VEC3(_dst, _s1, _v1, _s2, _v2)   \
    (((_dst)[0] = (_s1)*(_v1)[0] + (_s2)*(_v2)[0]), \
     ((_dst)[1] = (_s1)*(_v1)[1] + (_s2)*(_v2)[1]), \
     ((_dst)[2] = (_s1)*(_v1)[2] + (_s2)*(_v2)[2]))

#define PFADD_SCALED_VEC3(_dst, _v1, _s2, _v2)   \
    (((_dst)[0] = (_v1)[0] + (_s2)*(_v2)[0]), \
     ((_dst)[1] = (_v1)[1] + (_s2)*(_v2)[1]), \
     ((_dst)[2] = (_v1)[2] + (_s2)*(_v2)[2]))

#define PFDOT_VEC3(_v1,_v2) ((_v1)[0]*(_v2)[0] + (_v1)[1]*(_v2)[1] + (_v1)[2]*(_v2)[2])

#define PFSQR_DISTANCE_PT3(_v1, _v2) \
    (PF_SQUARE((_v1)[0]-(_v2)[0]) + \
     PF_SQUARE((_v1)[1]-(_v2)[1]) + \
     PF_SQUARE((_v1)[2]-(_v2)[2]))

#define PFLENGTH_VEC3 pfLengthVec3
/* Use the function so we can catch underflows */
/*#define PFLENGTH_VEC3(_v) \
    (pfSqrt(PFDOT_VEC3((_v), (_v)))) */

#define PFCOPY_VEC4(_dst, _v) \
    (((_dst)[0] = (_v)[0]),     \
     ((_dst)[1] = (_v)[1]),     \
     ((_dst)[2] = (_v)[2]),     \
     ((_dst)[3] = (_v)[3]))

#define PFSET_VEC4(_dst, _x, _y, _z, _w) \
    (((_dst)[0] = (_x)), \
     ((_dst)[1] = (_y)), \
     ((_dst)[2] = (_z)), \
     ((_dst)[3] = (_w)))

#define PFADD_VEC4(_dst, _v1,_v2) \
    (((_dst)[0] = (_v1)[0]+(_v2)[0]),     \
     ((_dst)[1] = (_v1)[1]+(_v2)[1]),     \
     ((_dst)[2] = (_v1)[2]+(_v2)[2]),     \
     ((_dst)[3] = (_v1)[3]+(_v2)[3]))

#define PFSUB_VEC4(_dst, _v1,_v2) \
    (((_dst)[0] = (_v1)[0]-(_v2)[0]),     \
     ((_dst)[1] = (_v1)[1]-(_v2)[1]),     \
     ((_dst)[2] = (_v1)[2]-(_v2)[2]),     \
     ((_dst)[3] = (_v1)[3]-(_v2)[3]))

#define PFSCALE_VEC4(_dst, _s, _v) \
    (((_dst)[0] = (_s) * (_v)[0]),     \
     ((_dst)[1] = (_s) * (_v)[1]),     \
     ((_dst)[2] = (_s) * (_v)[2]),     \
     ((_dst)[3] = (_s) * (_v)[3]))




#define FXIDENTMAT {{1.0f, 0.0f, 0.0f, 0.0f},  \
					{0.0f, 1.0f, 0.0f, 0.0f},  \
					{0.0f, 0.0f, 1.0f, 0.0f},  \
					{0.0f, 0.0f, 0.0f, 1.0f}}

#define PFCOPY_MAT(_dst,_src) \
		(((_dst)[0][0]= (_src)[0][0]), \
 		 ((_dst)[0][1]= (_src)[0][1]), \
 		 ((_dst)[0][2]= (_src)[0][2]), \
 		 ((_dst)[0][3]= (_src)[0][3]), \
 		 ((_dst)[1][0]= (_src)[1][0]), \
 		 ((_dst)[1][1]= (_src)[1][1]), \
 		 ((_dst)[1][2]= (_src)[1][2]), \
 		 ((_dst)[1][3]= (_src)[1][3]), \
 		 ((_dst)[2][0]= (_src)[2][0]), \
 		 ((_dst)[2][1]= (_src)[2][1]), \
 		 ((_dst)[2][2]= (_src)[2][2]), \
 		 ((_dst)[2][3]= (_src)[2][3]), \
 		 ((_dst)[3][0]= (_src)[3][0]), \
 		 ((_dst)[3][1]= (_src)[3][1]), \
 		 ((_dst)[3][2]= (_src)[3][2]), \
 		 ((_dst)[3][3]= (_src)[3][3]))

#define PFCOPY_FROM_SMALL_MAT(_dst,_src) \
		(((_dst)[0][0]= (_src)[0][0]), \
 		 ((_dst)[0][1]= (_src)[0][1]), \
 		 ((_dst)[0][2]= (_src)[0][2]), \
 		 ((_dst)[0][3]= 0), \
 		 ((_dst)[1][0]= (_src)[1][0]), \
 		 ((_dst)[1][1]= (_src)[1][1]), \
 		 ((_dst)[1][2]= (_src)[1][2]), \
 		 ((_dst)[1][3]= 0), \
 		 ((_dst)[2][0]= (_src)[2][0]), \
 		 ((_dst)[2][1]= (_src)[2][1]), \
 		 ((_dst)[2][2]= (_src)[2][2]), \
 		 ((_dst)[2][3]= 0), \
 		 ((_dst)[3][0]= (_src)[3][0]), \
 		 ((_dst)[3][1]= (_src)[3][1]), \
 		 ((_dst)[3][2]= (_src)[3][2]), \
 		 ((_dst)[3][3]= 1))

#define PFCOPY_TO_SMALL_MAT(_dst,_src) \
		(((_dst)[0][0]= (_src)[0][0]), \
 		 ((_dst)[0][1]= (_src)[0][1]), \
 		 ((_dst)[0][2]= (_src)[0][2]), \
 		 ((_dst)[1][0]= (_src)[1][0]), \
 		 ((_dst)[1][1]= (_src)[1][1]), \
 		 ((_dst)[1][2]= (_src)[1][2]), \
 		 ((_dst)[2][0]= (_src)[2][0]), \
 		 ((_dst)[2][1]= (_src)[2][1]), \
 		 ((_dst)[2][2]= (_src)[2][2]), \
 		 ((_dst)[3][0]= (_src)[3][0]), \
 		 ((_dst)[3][1]= (_src)[3][1]), \
 		 ((_dst)[3][2]= (_src)[3][2]))

#define PFMAKE_IDENT_MAT(_dst) \
   (((_dst)[0][0] = 1), \
    ((_dst)[0][1] = 0), \
    ((_dst)[0][2] = 0), \
    ((_dst)[0][3] = 0), \
    ((_dst)[1][0] = 0), \
    ((_dst)[1][1] = 1), \
    ((_dst)[1][2] = 0), \
    ((_dst)[1][3] = 0), \
    ((_dst)[2][0] = 0), \
    ((_dst)[2][1] = 0), \
    ((_dst)[2][2] = 1), \
    ((_dst)[2][3] = 0), \
    ((_dst)[3][0] = 0), \
    ((_dst)[3][1] = 0), \
    ((_dst)[3][2] = 0), \
    ((_dst)[3][3] = 1))

#define PFMAKE_SCALE_MAT(_dst,_sx,_sy,_sz) \
   (((_dst)[0][0] = (_sx)), \
    ((_dst)[0][1] = 0), \
    ((_dst)[0][2] = 0), \
    ((_dst)[0][3] = 0), \
    ((_dst)[1][0] = 0), \
    ((_dst)[1][1] = (_sy)), \
    ((_dst)[1][2] = 0), \
    ((_dst)[1][3] = 0), \
    ((_dst)[2][0] = 0), \
    ((_dst)[2][1] = 0), \
    ((_dst)[2][2] = (_sz)), \
    ((_dst)[2][3] = 0), \
    ((_dst)[3][0] = 0), \
    ((_dst)[3][1] = 0), \
    ((_dst)[3][2] = 0), \
    ((_dst)[3][3] = 1))

#define PFGET_MAT_ROWVEC3(_m, _r, _v) \
    (((_v)[0] = (_m)[(_r)][0]), \
     ((_v)[1] = (_m)[(_r)][1]), \
     ((_v)[2] = (_m)[(_r)][2]))

#define PFSET_MAT_ROWVEC3(_m, _r, _v) \
    (((_m)[(_r)][0] = ((_v)[0])), \
     ((_m)[(_r)][1] = ((_v)[1])), \
     ((_m)[(_r)][2] = ((_v)[2])))

#define PFGET_MAT_COLVEC3(_m, _c, _v) \
    (((_v)[0] = (_m)[0][(_c)]), \
     ((_v)[1] = (_m)[1][(_c)]), \
     ((_v)[2] = (_m)[2][(_c)]))

#define PFSET_MAT_COLVEC3(_m, _c, _v) \
    (((_m)[0][(_c)] = ((_v)[0])), \
     ((_m)[1][(_c)] = ((_v)[1])), \
     ((_m)[2][(_c)] = ((_v)[2])))



#define PFSWITCH_ON		-1
#define PFSWITCH_OFF		-2




#define NUMPARENTS 200
#define NUMCHILDREN 500

#define NODE_NOROTATION(_node)				((_node)->BitFields & NODE_FLAG_DCS_NOROT)

#define PFTRAV_ISECT	0
#define PFTRAV_CULL		1
#define PFTRAV_DRAW		2

#define WOLFPFCHANTRAV_ISECTANY	3
#define WOLFPFCHANTRAV_DRAWANY	4
#define WOLFPFCHANTRAV_ISECTALL	5
#define WOLFPFCHANTRAV_DRAWALL	6




typedef struct {
	long	Flags;
	long	CurCameraNum;
	
	long	RenderSkipMask;
	long	MonitorSprite;
	
	long	ScreenRect[4];

	pfMatrix ViewOffset;
	pfMatrix CamMatrix;
	pfMatrix CamMatrixNotOffset;
	pfMatrix Projection;
	ushort	perspNorm;
	float horizFOV;
	float aspect;
	float perspectiveDist;	/* usually (near+far)/2, but can be moved in or out to improve precision */
	float nearClip;
	float farClip;
	float LODScale;
	float LODFade;
	float LODFrustScale;
	float CompositeLODScale;

	long	DrawTravMaskAll;  /* All 1 bits must be set for node to pass */
	long	DrawTravMaskAny;  /* Any 1 bits must be set for node to pass */
	long	ISectTravMaskAll;
	long	ISectTravMaskAny;
	
	pfGroup *SceneRoot;
} pfChannel;

/**** Multichannel stuff ****/
#define MAX_NUM_CHANNELS	4
#define CHANNEL_FLAG_ACTIVE	0x0001
extern pfChannel	ChannelArray[MAX_NUM_CHANNELS];
extern pfChannel	*GlobalChannel;






void pfSinCos(float arg, float* s, float* c);
float pfArcSin(float arg);
float pfArcCos(float arg);
float pfArcTan2(float y, float x);
float 	pfTan(float _arg);


void	pfSetVec2(pfVec3 v, float xx, float yy);
void	pfAddVec2(pfVec2 _dst, const pfVec2 _v1, const pfVec2 _v2);
void	pfScaleVec2(pfVec2 _dst, float _s, pfVec2 _v);
float	pfLengthVec2(pfVec2 _v);
float	pfNormalizeVec2(pfVec2 _v);
float	pfSqrDistancePt2(pfVec2 _v1, pfVec2 _v2);
void     pfAddScaledVec2(pfVec2 _dst, const pfVec2 _v1, float _s,
					pfVec2 _v2);

void pfCopyVec4(pfVec4 dst, pfVec4 src);

void pfSetVec3(pfVec3 v, float xx, float yy, float zz);
void pfCopyVec3(pfVec3 dst, pfVec3 src);
void pfNegateVec3(pfVec3 dst, pfVec3 src);
Boolean pfEqualVec3(pfVec3 v1, pfVec3 v2);
void	pfAddVec3(pfVec3 _dst, const pfVec3 _v1, const pfVec3 _v2);
void	pfSubVec3(pfVec3 _dst, const pfVec3 _v1, const pfVec3 _v2);
float pfDotVec3(pfVec3 _v1, pfVec3 _v2);
float pfLengthVec3(pfVec3 _v);
float pfSqrDistancePt3(pfVec3 _v1, pfVec3 _v2);
float pfDistancePt2(pfVec2 _v1, pfVec2 _v2);
float pfDistancePt3(pfVec3 _v1, pfVec3 _v2);
float	pfNormalizeVec3(pfVec3 _v);
void	pfCrossVec3(pfVec3 _dst, const pfVec3 _v1, const pfVec3 _v2);
void	pfScaleVec3(pfVec3 _dst, float _s, pfVec3 _v);
extern void	pfCombineVec3(pfVec3 _dst, float _s1, const pfVec3 _v1,
			      float _s2, const pfVec3 _v2);
void     pfAddScaledVec3(pfVec3 _dst, const pfVec3 _v1, float _s,
					pfVec3 _v2);

void	pfSetMatRowVec3(pfMatrix _dst, long _r, const pfVec3 _v);
void	pfGetMatRowVec3(pfMatrix _m, long _r, pfVec3 _dst);

void	pfXformPt3(pfVec3 _dst, const pfVec3 _v, const pfMatrix _m);
void	pfXformVec3(pfVec3 _dst, const pfVec3 _v, const pfMatrix _m);

void 	pfSetVec4(pfVec4 v, float xx, float yy, float zz, float ww);
void 	pfScaleVec4(pfVec4 _dst, float _s, pfVec4 _v);
void	pfXformVec4(pfVec4 _dst, const pfVec4 _v, const pfMatrix _m);

void pfCopyMat(pfMatrix _dst, const pfMatrix a);
void pfMakeEulerMat(pfMatrix _dst, float h, float p, float r);
void pfMakeCoordMat(pfMatrix _dst, const pfCoord* _c);
void pfMakeScaleMat(pfMatrix _dst, float xx, float yy, float zz);
void pfMakeTransMat(pfMatrix _dst, float xx, float yy, float zz);
void pfMakeRotMat(pfMatrix _dst, float degrees, float xx, float yy, float zz);
void pfMakeIdentMat(pfMatrix _dst);
void pfGetOrthoMatCoord(pfMatrix _src, pfCoord* _dst);
void pfMultMat(pfMatrix _dst, const pfMatrix a, const pfMatrix b);
void pfPreMultMat(pfMatrix _dst, const pfMatrix b);
void pfPostMultMat(pfMatrix _dst, const pfMatrix a);
void pfPreRotMat(pfMatrix _dst, float degrees, float xx, float yy, float zz, pfMatrix _m);
long pfInvertMat(pfMatrix _dst, const pfMatrix _m);
void pfInvertAffineMat(pfMatrix _dst, const pfMatrix _m);
void pfInvertOrthoMat(pfMatrix _dst, const pfMatrix _m);
void pfInvertOrthoNMat(pfMatrix _dst, const pfMatrix _m);
void pfPostRotMat(pfMatrix _dst, pfMatrix _m, float degrees, float x, float y, float z);
void pfPreScaleMat(pfMatrix _dst, float xx, float yy, float zz, pfMatrix _m);
void pfPostScaleMat(pfMatrix _dst, pfMatrix _m, float xx, float yy, float zz);
void pfPreTransMat(pfMatrix _dst, float xx, float yy, float zz, pfMatrix _m);



void pfMakePtsPlane(pfPlane *ThePlane, pfVec3 p1, pfVec3 p2, pfVec3 p3);
void pfMakeNormPtPlane(pfPlane *ThePlane, pfVec3 norm, pfVec3 pos);

void pfDCSRot(pfDCS *ThisDCS, float hh, float pp, float rr);
void pfDCSTrans(pfDCS *ThisDCS, float xx, float yy, float zz);
void pfDCSMat(pfDCS *ThisDCS, pfMatrix ThisMatrix);
void pfDCSCoord(pfDCS *ThisDCS, pfCoord *c);
void pfGetDCSMat(pfDCS *ThisDCS, pfMatrix ThisMatrix);
void pfGetSCSMat(pfSCS *ThisSCS, pfMatrix ThisMatrix);

void pfSwitchVal(pfSwitch *ThisSwitch, long Val);
long pfGetSwitchVal(pfSwitch *ThisSwitch);
	
void pfLODRange(pfLOD* _lod, long _index, float _range);
float pfGetLODRange(pfLOD* _lod, long _index);




long pfGetType(pfNode* _node);
long pfGetNumChildren(pfGroup* _node);
pfNode* pfGetChild(pfGroup* _group, long _childnum);
/*long pfGetNumParents(pfNode* _node);
pfNode* pfGetParent(pfNode* _group, long _parentnum); */



long		pfGetGSetNumPrims(pfGeoSet* _gset);
long		pfGetGSetPrimType(pfGeoSet* _gset);
long*		pfGetGSetPrimLengths(pfGeoSet* _gset);
void		pfGetGSetAttrLists(pfGeoSet* _gset, long _attr, void** _alist, ushort** _ilist);
void		pfGetGSetBBox(pfGeoSet* _gset, pfBox* _box);

long		pfGetNumGSets(pfGeode* _geode);
pfGeoSet*	pfGetGSet(pfGeode* _geode, long _index);



void		pfGStateAttr(pfGeoState* _gs, long _attr, void* _a);

long		pfGetGStateMode(pfGeoState* _gs, long _mode);
pfGeoState *pfGetGSetGState(pfGeoSet* _gs);



void pfChanViewOffsets(pfChannel *chan, pfVec3 xyz, pfVec3 hpr);
void pfChanView(pfChannel *chan, pfVec3 xyz, pfVec3 hpr);
void pfGetChanView(pfChannel *chan, pfVec3 xyz, pfVec3 hpr);
void pfChanViewMat(pfChannel *chan, pfMatrix ChanMat);
void pfChanScene(pfChannel *chan, pfGroup *SceneRoot);
void	pfChanTravMask(pfChannel* _chan, long _trav, long _mask);
long	pfGetChanTravMask(pfChannel* _chan, long _trav);


/* pfChanLODAttr() */
#define PFLOD_SCALE		2
#define PFLOD_FADE		3
#define PFLOD_FRUST_SCALE	5

void	pfChanLODAttr(pfChannel* _chan, long _attr, float _val);
float pfGetChanLODAttr(pfChannel* _chan, long _attr);


#define PFTRAV_SELF		0x010
#define PFTRAV_DESCEND		0x020
#define PF_SET			1
#define PF_OR			2
#define PF_AND			3


long	pfNodeTravMask(pfNode* _node, long _which, ulong _mask, long _setMode, long _bitOp);
ulong	pfGetNodeTravMask(pfNode* _node, long _which);


#ifndef INFOXPF

/*  #include "foxpf.p"  */

#define pfGetType(_node) pfGetType((pfNode*)(_node))
#define pfGetNumChildren(_node) pfGetNumChildren((pfGroup*)(_node))
/* #define pfGetNumParents(_node) pfGetNumParents((pfNode*)(_node)) */

/*#define pfGetChild(_node,_childnum) pfGetChild((pfGroup*)(_node),_childnum) */
/* Used to speed up child fetch */
#define pfGetChild(_node,_childnum) ((pfNode *)(((pfGroup*)_node)->GroupInfo.Children[_childnum])) /* Note no NULL Check on parent*/

/* #define pfGetParent(_node,_parentnum) pfGetParent((pfNode*)(_node),_parentnum)   */
#define pfNodeTravMask(_node, _which, _mask, _setMode, _bitOp) pfNodeTravMask((pfNode *)(_node), _which, _mask, _setMode, _bitOp)
#define pfGetNodeTravMask(_node, _which) pfGetNodeTravMask((pfNode *)(_node), _which)

/**** multichannel stuff ****/
long GetChannelCameraNum(pfChannel *chan);
void SetChannelCameraNum(pfChannel *chan, long cam);
long GetChannelFlags(pfChannel *chan);
void SetChannelFlags(pfChannel *chan, long flags);
void ClearChannelFlags(pfChannel *chan, long flags);
long GetNumChannels(void);
pfChannel *GetChannel(long id);






/***********************************************\
   Fox pf Extensions
\***********************************************/




void foxpfInitNode(pfNode *_node, long Type);
void *foxpfGetGSetAttrs(pfGeoSet *_gset);
void foxpfGSetAttrs(pfGeoSet *_gset, void *_data);








#endif /* INFOXPF */
#endif	/* FOXPF_H */
