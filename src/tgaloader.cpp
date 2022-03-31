/*******************************************************************************\
	loadtga.c
	
	This is targa-reading code, converted it from C++
	to straight C, so it can be used from within a C library.
\*******************************************************************************/

#include "genincludes.h"

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <memory.h>
#include "tgaloader.h"
#pragma warning (disable: 4101) // unreferenced local variable---on PS2, it's disabled in the .mcp


/* Defines for flags in TGA header */

#define UPPER_LEFT 0x20
#define LOWER_LEFT 0x00

#define TWO_INTERLEAVE 0x40
#define FOUR_INTERLEAVE 0x80

#define BASE            0
#define RUN             1
#define LITERAL         2

// .TGA file header

#pragma pack(1)     // Gotta pack these structures!

typedef struct
{
     uchar idlen;
     uchar cmtype;
     uchar imgtype;

     uint16 cmorg;
     uint16 cmlen;
     uchar cmes;

     int16 xorg;
     int16 yorg;
     int16 width;
     int16 height;
     uchar pixsize;
     uchar desc;

#ifdef PLAYSTATION
	char pad[14];
#endif

} TGAHeaderPacked;

typedef struct
{
	int16 extsize;
	char authorname[41];
	char comments[324];
	int16 td_month;
	int16 td_day;
	int16 td_year;
	int16 td_hour;
	int16 td_minute;
	int16 td_second;
	char jobname[41];
	int16 jt_hours;
	int16 jt_minutes;
	int16 jt_seconds;
	char softwareID[41];
	int16 sw_version;
	char version_letter;
	char key_a,key_r,key_g,key_b;
	int16 aspect_w;
	int16 aspect_h;
	int16 gamma_numerator;
	int16 gamma_denominator;
	int32 color_corr_table;
	int32 postage_stamp;
	int32 scan_line;
	char alpha_attributes;

#ifdef PLAYSTATION
	char pad[1];
#endif

} TGAExtraPacked;

typedef struct
{
#ifdef PLAYSTATION
	char pad[6];	/// pad the beginning!
#endif
	int32 ext_area;
	int32 dev_area;
	char signature[18];

} TGAFooterPacked;

#ifdef _PSX2
 #pragma pack(0)     // Gotta pack these structures!
#else
 #pragma pack()     // Gotta pack these structures!
#endif

typedef struct
{
     unsigned char idlen;
     unsigned char cmtype;
     unsigned char imgtype;

     unsigned short cmorg;
     unsigned short cmlen;
     unsigned char cmes;

     short xorg;
     short yorg;
     short width;
     short height;
     unsigned char pixsize;
     unsigned char desc;
} TGAHeader;

typedef struct
{
     short extsize;
     char authorname[41];
     char comments[324];
     short td_month;
     short td_day;
     short td_year;
     short td_hour;
     short td_minute;
     short td_second;
     char jobname[41];
     short jt_hours;
     short jt_minutes;
     short jt_seconds;
     char softwareID[41];
     short sw_version;
     char version_letter;
     char key_a,key_r,key_g,key_b;
     short aspect_w;
     short aspect_h;
     short gamma_numerator;
     short gamma_denominator;
     int32 color_corr_table;
     int32 postage_stamp;
     int32 scan_line;
     char alpha_attributes;
} TGAExtra;

typedef struct {
     int32 ext_area;
     int32 dev_area;
     char signature[18];
} TGAFooter;


static TGAExtra	extra;
static TGAHeader	hdr;
static TGAFooter	foot;
static Boolean		hflip, vflip;
static Boolean		gotGamma;
//static float		gamma;    // Gamma
static float		aspect;   // Pixel aspect ratio




#define	READ_VAR(x)	fread(&(x), 1, sizeof(x), pFile)

typedef	uchar uint24[3];




//-----------------------------------------------------------------------------
// *> BitmapIO_TGA::ReadFooter()
//
//    Read a .TGA file footer and the extra data in a Targa 2.0 file, if present

#define TGA_ALPHA_NONE 0
#define TGA_ALPHA_IGNORE 1
#define TGA_ALPHA_RETAIN 2
#define TGA_ALPHA_HASALPHA 3
#define TGA_ALPHA_PREMULT 4

static int  ReadFooter(FILE* pFile)
{
	TGAFooterPacked packedfoot;
	TGAExtraPacked packedextra;
	
	// Initialize the extra area fields we're interested in
	memset(extra.authorname, 0, 41);
	memset(extra.comments, 0, 324);
	extra.td_month = 1;
	extra.td_day = 1;
	extra.td_year = 1980;
	extra.td_hour = 0;
	extra.td_minute = 0;
	extra.td_second = 0;
	memset(extra.jobname, 0, 41);
	extra.jt_hours = 0;
	extra.jt_minutes = 0;
	extra.jt_seconds = 0;
	memset(extra.softwareID, 0, 41);
	extra.sw_version = 0;
	extra.version_letter = ' ';
	extra.key_a = 0;
	extra.key_r = 0;
	extra.key_g = 0;
	extra.key_b = 0;
	extra.aspect_w = 1;
	extra.aspect_h = 1;
	extra.gamma_numerator = 1;
	extra.gamma_denominator = 1;
	extra.color_corr_table = 0;
	extra.postage_stamp = 0;
	extra.scan_line = 0;
	extra.alpha_attributes = TGA_ALPHA_PREMULT;

	// Now grab it!
	fseek(pFile, 0 - sizeof(TGAFooterPacked), SEEK_END);

	
	if (fread(&packedfoot, 1, sizeof(TGAFooterPacked),pFile) != sizeof(TGAFooterPacked))
		return 0;

	/// copy each member of the struct into the aligned footer
	memcpy( &foot.ext_area, &packedfoot.ext_area, sizeof(packedfoot.ext_area) );
	memcpy( &foot.ext_area, &packedfoot.dev_area, sizeof(packedfoot.dev_area) );
	memcpy( &foot.signature, &packedfoot.signature, sizeof(packedfoot.signature) );

	// Check the signature
	if(strncmp(foot.signature,"TRUEVISION-XFILE",16)!=0)
		return 1;

	// Got the signature, let's grab the footer (if any)
	if(!foot.ext_area)
		return 1;		// No extension area, let's blow!

	fseek(pFile, foot.ext_area, SEEK_SET);

	if (fread(&packedextra, 1, sizeof(TGAExtraPacked),pFile) != sizeof(TGAExtraPacked))
		return 0;

#ifdef PLAYSTATION
	fseek(pFile,  -sizeof(packedextra.pad), SEEK_CUR );
#endif

	/// copy each member of the struct into the aligned extra
	memcpy( &extra.extsize				, &packedextra.extsize, sizeof(packedextra.extsize) );
	memcpy( extra.authorname			, packedextra.authorname				, sizeof(packedextra.authorname			   ) );
	memcpy( extra.comments			, packedextra.comments			   		, sizeof(packedextra.comments			   ) );
	memcpy( &extra.td_month			, &packedextra.td_month			   		, sizeof(packedextra.td_month			   ) );
	memcpy( &extra.td_day				, &packedextra.td_day				   	, sizeof(packedextra.td_day				   ) );
	memcpy( &extra.td_year				, &packedextra.td_year				   	, sizeof(packedextra.td_year				   ) );
	memcpy( &extra.td_hour				, &packedextra.td_hour				   	, sizeof(packedextra.td_hour				   ) );
	memcpy( &extra.td_minute			, &packedextra.td_minute			   	, sizeof(packedextra.td_minute			   ) );
	memcpy( &extra.td_second			, &packedextra.td_second			   	, sizeof(packedextra.td_second			   ) );
	memcpy( extra.jobname				, packedextra.jobname				   	, sizeof(packedextra.jobname				   ) );
	memcpy( &extra.jt_hours			, &packedextra.jt_hours			   		, sizeof(packedextra.jt_hours			   ) );
	memcpy( &extra.jt_minutes			, &packedextra.jt_minutes			   	, sizeof(packedextra.jt_minutes			   ) );
	memcpy( &extra.jt_seconds			, &packedextra.jt_seconds			   	, sizeof(packedextra.jt_seconds			   ) );
	memcpy( extra.softwareID			, packedextra.softwareID			   	, sizeof(packedextra.softwareID			   ) );
	memcpy( &extra.sw_version			, &packedextra.sw_version			   	, sizeof(packedextra.sw_version			   ) );
	memcpy( &extra.version_letter		, &packedextra.version_letter		   	, sizeof(packedextra.version_letter		   ) );
	memcpy( &extra.key_a				, &packedextra.key_a				   	, sizeof(packedextra.key_a				   ) );
	memcpy( &extra.key_r				, &packedextra.key_r				   	, sizeof(packedextra.key_r				   ) );
	memcpy( &extra.key_g				, &packedextra.key_g				   	, sizeof(packedextra.key_g				   ) );
	memcpy( &extra.key_b				, &packedextra.key_b				   	, sizeof(packedextra.key_b				   ) );
	memcpy( &extra.aspect_w			, &packedextra.aspect_w			   		, sizeof(packedextra.aspect_w			   ) );
	memcpy( &extra.aspect_h			, &packedextra.aspect_h			   		, sizeof(packedextra.aspect_h			   ) );
	memcpy( &extra.gamma_numerator		, &packedextra.gamma_numerator		   	, sizeof(packedextra.gamma_numerator		   ) );
	memcpy( &extra.gamma_denominator	, &packedextra.gamma_denominator	  	, sizeof(packedextra.gamma_denominator	  ) );
	memcpy( &extra.color_corr_table	, &packedextra.color_corr_table	   		, sizeof(packedextra.color_corr_table	   ) );
	memcpy( &extra.postage_stamp		, &packedextra.postage_stamp		   	, sizeof(packedextra.postage_stamp		   ) );
	memcpy( &extra.scan_line			, &packedextra.scan_line			   	, sizeof(packedextra.scan_line			   ) );
	memcpy( &extra.alpha_attributes	, &packedextra.alpha_attributes	   		, sizeof(packedextra.alpha_attributes	   ) );


	if(extra.gamma_denominator)
	{
		gotGamma = TRUE;
//		gamma = (float)extra.gamma_numerator / (float)extra.gamma_denominator;
	}
	if(extra.aspect_h)
		aspect = (float)extra.aspect_w / (float)extra.aspect_h;
	return 1;
}


//-----------------------------------------------------------------------------
// *> BitmapIO_TGA::ReadHeader()
//
//    Read a .TGA file header

static int  ReadHeader(FILE *pFile)
{
	TGAHeaderPacked packedhdr;
	if (fread(&packedhdr,1, sizeof(TGAHeaderPacked),pFile) != sizeof(TGAHeaderPacked))
		return 0;

#ifdef PLAYSTATION
	fseek(pFile,  -sizeof(packedhdr.pad), SEEK_CUR );
#endif

	/// copy each member of the struct into the aligned header
	memcpy( &hdr.idlen   , &packedhdr.idlen   , sizeof(packedhdr.idlen   ) );
	memcpy( &hdr.cmtype  , &packedhdr.cmtype  , sizeof(packedhdr.cmtype  ) );
	memcpy( &hdr.imgtype , &packedhdr.imgtype , sizeof(packedhdr.imgtype ) );
	memcpy( &hdr.cmorg   , &packedhdr.cmorg   , sizeof(packedhdr.cmorg   ) );
	memcpy( &hdr.cmlen   , &packedhdr.cmlen   , sizeof(packedhdr.cmlen   ) );
	memcpy( &hdr.cmes    , &packedhdr.cmes    , sizeof(packedhdr.cmes    ) );
	memcpy( &hdr.xorg    , &packedhdr.xorg    , sizeof(packedhdr.xorg    ) );
	memcpy( &hdr.yorg    , &packedhdr.yorg    , sizeof(packedhdr.yorg    ) );
	memcpy( &hdr.width   , &packedhdr.width   , sizeof(packedhdr.width   ) );
	memcpy( &hdr.height  , &packedhdr.height  , sizeof(packedhdr.height  ) );
	memcpy( &hdr.pixsize , &packedhdr.pixsize , sizeof(packedhdr.pixsize ) );
	memcpy( &hdr.desc    , &packedhdr.desc    , sizeof(packedhdr.desc    ) );

	return 1;
}





static Boolean Load16BitTGA (FILE *pFile, uint16 *usData)
{
	uint16 *pData = usData;
	int iTotalPixels = hdr.height * hdr.width;

	//-- Bypass the image identification field, if present -------------------

	if (hdr.idlen)
		fseek(pFile, (int32)hdr.idlen,SEEK_CUR);

	//-- Bypass the color map, if present ------------------------------------

	if (hdr.cmlen)
		fseek(pFile, (int32)hdr.cmlen*(int32)((hdr.cmes+7)/8),SEEK_CUR);

	switch (hdr.imgtype)
	{
		case 2:
				// Uncompressed
			fread(usData, 1, hdr.height * hdr.width * (hdr.pixsize / 8), pFile);
			break;

		case 10:
				// Compressed
			{
				int x = 0;
				int y = hdr.height - 1;
				uchar rle;
				uint16 pixel;

				while (usData - pData < iTotalPixels)
				{
					READ_VAR(rle);

					if (rle>127)
					{
						int ix;
						
						rle-=127;

						READ_VAR(pixel);

						for(ix=0; ix<rle; ++ix)
							*usData++ = pixel;
					}
					else
					{
						rle++;

						fread(usData, 1, sizeof(*usData)*rle,pFile);

						usData += rle;
					}
				}
			}
			break;
	}

	return(TRUE);
}


static Boolean Load24BitTGA (FILE *pFile, uint24 *p24Data)
{
	int i;
	uint24 *pData = p24Data;
	uchar bSwap;
	int iTotalPixels = hdr.height * hdr.width;

	//-- Bypass the image identification field, if present -------------------

	if (hdr.idlen)
		fseek(pFile, (int32)hdr.idlen,SEEK_CUR);

	//-- Bypass the color map, if present ------------------------------------

	if (hdr.cmlen)
		fseek(pFile, (int32)hdr.cmlen*(int32)((hdr.cmes+7)/8),SEEK_CUR);

	switch (hdr.imgtype)
	{
		case 2:
				// Uncompressed
			fread(p24Data, 1, hdr.height * hdr.width * (hdr.pixsize / 8), pFile);
			break;

		case 10:
				// Compressed
			{
				int x = 0;
				int y = hdr.height - 1;
				uchar rle;
				uchar pixel[3];
				int iEnd = iTotalPixels;

				while (iEnd)
				{
					READ_VAR(rle);

					if (rle>127)
					{
						int ix;
						
						rle-=127;

						READ_VAR(pixel);

						for(ix=0; ix<rle; ++ix)
						{
							(*p24Data)[0] = pixel[0];
							(*p24Data)[1] = pixel[1];
							(*p24Data)[2] = pixel[2];
							p24Data++;
							iEnd--;
						}
					}
					else
					{
						rle++;

						fread(p24Data, 1, sizeof(*p24Data)*rle, pFile);

						p24Data += rle;
						iEnd -= rle;
					}
				}
			}
			break;
	}

	// Targa files are in BGR format.  Flip the pixels;

	i = iTotalPixels;
	while (i--) {
		bSwap=pData[0][2];
		pData[0][2]=pData[0][0];
		pData[0][0]=bSwap;
		pData++;
	}

	return(TRUE);
}


static Boolean Load32BitTGA (FILE *pFile, uint32 *ulData)
{
	uint32 *pData = ulData;
	int iTotalPixels = hdr.height * hdr.width;

	//-- Bypass the image identification field, if present -------------------

	if (hdr.idlen)
		fseek(pFile, (uint32)hdr.idlen,SEEK_CUR);

	//-- Bypass the color map, if present ------------------------------------

	if (hdr.cmlen)
		fseek(pFile, (int32)hdr.cmlen*(int32)((hdr.cmes+7)/8),SEEK_CUR);

	switch (hdr.imgtype)
	{
		case 2:
				// Uncompressed
			fread(ulData, 1, hdr.height * hdr.width * (hdr.pixsize / 8), pFile);
			break;

		case 10:
				// Compressed
			{
				int x = 0;
				int y = hdr.height - 1;
				uchar rle;
				uint32 pixel;

				while (ulData - pData < iTotalPixels)
				{
					READ_VAR(rle);

					if (rle>127)
					{
						int ix;
						
						rle-=127;

						READ_VAR(pixel);

						for(ix=0; ix<rle; ++ix)
							*ulData++ = pixel;
					}
					else
					{
						rle++;

						fread(ulData, 1, sizeof(*ulData)*rle,pFile);

						ulData += rle;
					}
				}
			}
			break;
	}
#if 0
		// Targa files are in BGR format.  Flip the pixels;
	{
		int i = hdr.height * hdr.width;
		PIXEL_32 *p32Data = (PIXEL_32 *) pData;

		while (i--)
		{
			p32Data->FlipData();
	/// !!! RWS: Inverting the alpha channel? Why?
///	void FlipData() { uchar bHold = bStuff[0]; bStuff[0] = bStuff[2]; bStuff[2] = bHold; bStuff[3] ^= 0xFF; }
	void FlipData() { uchar bHold = bStuff[0]; bStuff[0] = bStuff[2]; bStuff[2] = bHold; }

//			uchar bSwap;
//
//			bSwap = p32Data->bStuff[0];
//			p32Data->bStuff[0] = p32Data->bStuff[2];
//			p32Data->bStuff[2] = bSwap;
//
//       // and "invert" the alpha
//			p32Data->bStuff[3] = 255 - p32Data->bStuff[3];

			p32Data++;
		}
	}
#endif	

	return(TRUE);
}





#define COPY16(_d,_s) (_d)=(_s)
#define COPY24(_d,_s) {(_d)[0]=(_s)[0];(_d)[1]=(_s)[1];(_d)[2]=(_s)[2];}
#define COPY32(_d,_s) (_d)=(_s)
						

#define IMAGEREAD(_n)  \
  \
				w##_n##Buf = (uint##_n## *)(malloc(pixelsize * sizeof(uint##_n##)));  \
				  \
				if (!Load##_n##BitTGA (pFile, w##_n##Buf)) {  \
					free(w##_n##Buf);  \
					goto ReadImageERROR;  \
				} else {  \
  \
					if ( hflip && vflip ) {  \
  \
						int i, iTop;  \
						uint##_n wPixel;  \
  \
						iTop = pixelsize - 1;  \
  \
						for ( i = 0; i < iTop; i++ )  \
						{  \
							COPY##_n##(wPixel,w##_n##Buf[i]);  \
							COPY##_n##(w##_n##Buf[i],w##_n##Buf[iTop]);  \
							COPY##_n##(w##_n##Buf[iTop],wPixel);  \
  \
							iTop--;  \
						}  \
					}  \
					else if (hflip)  \
					{  \
						int x, xt, h, w;  \
						uint##_n *wbuf;  \
						uint##_n wPixel;  \
						  \
						h=hdr.height;  \
						w=hdr.width;  \
						wbuf=w##_n##Buf;  \
  \
						while( h-- )  \
						{  \
							xt = w-1;  \
							x = 0;  \
  \
							while (x < xt)  \
							{  \
								COPY##_n##(wPixel,wbuf[x]);  \
								COPY##_n##(wbuf[x],wbuf[xt]);  \
								COPY##_n##(wbuf[xt],wPixel);  \
  \
								x++;  \
								xt--;  \
							}  \
  \
							wbuf += w;  \
						}  \
					}  \
					else if (vflip)  \
					{  \
						int x, xt, h, w;  \
						uint##_n *wBuf;  \
						uint##_n *wBuft;  \
						uint##_n wBuffer[2048];  \
						  \
						h=hdr.height;  \
						w=hdr.width;  \
						wBuf=w##_n##Buf;  \
						wBuft = wBuf + (w * (h - 1));  \
  \
						while ( wBuf < wBuft )  \
						{  \
							memcpy( wBuffer,	wBuf,		w * sizeof( uint##_n## ) );  \
							memcpy( wBuf,		wBuft,	w * sizeof( uint##_n## ) );  \
							memcpy( wBuft,		wBuffer,	w * sizeof( uint##_n## ) );  \
  \
							wBuf += w;  \
							wBuft -= w;  \
						}  \
					}  \
				}  



static void ReorderBytes32(uint32 *w32Buf, int32 pixelsize)
{
	uchar *CharPtr;
	int32 i;
	uchar TChar;

	CharPtr = (uchar *)(w32Buf);
	for (i=0;i<pixelsize;i++) {
		TChar=CharPtr[2];
		CharPtr[2]=CharPtr[0];
		CharPtr[0]=TChar;
		CharPtr+=4;
	}
}


bool ReadTGAImage (FILE* pFile, void **IData, uint32 *w, uint32 *h, uint32 *depth)
{
	int result = 0;
	uint32 pixelsize;
	uint16 *w16Buf;
	uint24 *w24Buf;
	uint32 *w32Buf;

	result = ReadFooter(pFile);
	if (!result)
	{
		return(FALSE);
	}

	// Return to the beginning of the file
	fseek(pFile, 0, SEEK_SET);

	result = ReadHeader(pFile);
	if(result)
	{
		hflip     = (hdr.desc & 0x10) ? TRUE : FALSE;   // Need hflip
		vflip     = (hdr.desc & 0x20) ? TRUE : FALSE;   // Need vflip
		hdr.desc &= 0xcf;                               // Mask off flip bits

			// REVISIT - Figure out why we need to do this - rf
	//	hflip = !hflip;
//		vflip = !vflip;	// ej removed this 11/16/00. It means we have to flip V in some models...


		pixelsize=hdr.width * hdr.height;

//	mpBmp->Init (hdr.width, hdr.height, gFormat8888	);
		switch(hdr.pixsize)
		{
			case 8:
				if (hdr.desc!=0 && hdr.desc!=1 && hdr.desc!=8)
					break;

				// Check for grayscale
				switch(hdr.imgtype)
				{
					case 1:
					case 9:
						break;

					case 3:
					case 11:
						break;
					}
				break;

			case 16:
				IMAGEREAD(16);
				*IData=w16Buf;
				break;
			case 24:
				IMAGEREAD(24);
				*IData=w24Buf;
				break;
			case 32:
				IMAGEREAD(32);
				ReorderBytes32(w32Buf,hdr.height * hdr.width);
				*IData=w32Buf;
				break;
			default:  ;
		}
	}

	*w=hdr.width;
	*h=hdr.height;
	*depth=hdr.pixsize;
	
	return(TRUE);
	
ReadImageERROR:
	
	return (FALSE);
}



