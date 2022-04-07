
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/gl.h>
#include <stdarg.h>
#include <time.h>
#include "genincludes.h"
#include "camera.h"
#include "timer.h"
#include "guideway.h"
#include "texture.h"
#include "draw.h"
//#include "displacement.h"
#include "cellgrid.h"
#include "cellport.h"
#include "glider.h"
#include "projectile.h"
#include "hud.h"
#include "boss.h"
#include "glowSpark.h"
#include "bubble.h"
#include "slides.h"
#include "random.h"
#include "font.h"
#include "tsufile.h"
#include "translation.h"

FontSystem	*gFontSys = NULL;

FontRecord::FontRecord(void)
{
	mName[0] = 0;
	mTexWidth = 128;
	mTexHeight = 128;
}

FontRecord::~FontRecord(void)
{
}

FontSystem::FontSystem(void)
{
	pfSetVec2(mSetPos, 0, 0);
	pfSetVec2(mPos, 0, 0);
	pfSetVec2(mScale, 1, 1);
	pfSetVec4(mColor[0], 1, 1, 1, 1);
	pfSetVec4(mColor[1], 1, 1, 1, 1);
	mCurrentFont = FONT_ID_MAIN;
	mAlignment = FONT_ALIGNMENT_LEFT;
	mEnabled = false;
	mBoundsBox[PF_X][0] = 0;
	mBoundsBox[PF_X][1] = 640;
	mBoundsBox[PF_Y][0] = 0;
	mBoundsBox[PF_Y][1] = 480;
	mMinimumSize = 0.0f;
	mAlwaysSquare = false;
}

FontSystem::~FontSystem(void)
{
}

void FontSystem::LoadAll(void)
{
	LoadFont(FONT_ID_MAIN, TRANSLATE(TXT_fontAr16), TEXID_FONT_MAIN);
}

int FontSystem::GetSlotFromUnicode(int unicode)
{
	int	i;
	int	fontID = 0;	/**** we ended up with only one font ****/
	FontRecord	*f = &(mFonts[fontID]);

	for (i = 0; i < NUM_CHARACTER_SLOTS; i++) {
		if (f->mFontChars[i].mCode == unicode) return(i);
	}
	return(0);
}

bool FontSystem::LoadFont(int fontID, char *name, int texID)
{
	char		pathName[256], str[256], *line = NULL, *fbuf = NULL;
	int			i, version, code, size, slot;
	FILE		*fp;
	FontRecord	*f = &(mFonts[fontID]);
	void		*tsuptr;
	uint32		tsusize;

	strcpy(f->mName, name);
	f->mTexID = texID;
	sprintf(pathName, "%s/%s.txt", TexturePath, name);

	tsuMarkFile(pathName);
	tsuptr = (char*)tsuGetMemFile(pathName, &tsusize);
	if (tsuptr) {
		line = (char*)tsuptr;
	} else {
		fp = fopen(pathName, "rb");
		if (fp) {
			fseek(fp, 0, SEEK_END);
			size = ftell(fp);
			fseek(fp, 0, SEEK_SET);
			fbuf = new char[size+1];
			fbuf[size] = 0;
			fread(fbuf, 1, size, fp);
			fclose(fp);
			line = fbuf;
		}
	}
	if (line) {
		sscanf(line, " %s %d %d %d %d %d %d ", str, &version, &f->mNumChars, &f->mNumPages, &f->mMinCode, &f->mMaxCode, &f->mCellHeight);
		if (strcmp(str, "FONTRAST")) return(false);
		if (version != 1) return(false);
		line = strchr(line, '\n');
		line++;
		line = strchr(line, '\n');
		line++;
		line = strchr(line, '\n');
		line++;
		for (i = 0; i < NUM_CHARACTER_SLOTS; i++) {
			f->mFontChars[i].mCode = 0;
			f->mFontChars[i].mPageNumber = 0;
			f->mFontChars[i].mPositionX = f->mFontChars[i].mPositionY = 0;
			f->mFontChars[i].mWidth = f->mFontChars[i].mHeight = 0;
			f->mFontChars[i].mLeadingXOffset = f->mFontChars[i].mTrailingXOffset = 0;
		}
		for (i = 0; i < f->mNumChars; i++) {
			sscanf(line, " %d ", &code);
			if (code < 256) {
				slot = code;
			} else {
				slot = 256;
				while ((f->mFontChars[slot].mCode != 0) && (slot < NUM_CHARACTER_SLOTS-1)) {
					slot++;
				}
			}
			f->mFontChars[slot].mCode = code;
			sscanf(line, " %d %d %f %f %f %f %f %f ", &code,
				&f->mFontChars[slot].mPageNumber,
				&f->mFontChars[slot].mPositionX, &f->mFontChars[slot].mPositionY,
				&f->mFontChars[slot].mWidth, &f->mFontChars[slot].mHeight,
				&f->mFontChars[slot].mLeadingXOffset, &f->mFontChars[slot].mTrailingXOffset);
			line = strchr(line, '\n');
			line++;

			/**** This is kind of a hack, but it makes the asian languages work much better! ****/
			if (f->mFontChars[slot].mTrailingXOffset < f->mFontChars[slot].mWidth) {
				f->mFontChars[slot].mTrailingXOffset = f->mFontChars[slot].mWidth;
			}
		}
		if (fbuf) delete[] fbuf;
		return(true);
	}
	return(false);
}

void FontSystem::SetPos(float x, float y)
{
	pfSetVec2(mSetPos, x, y);
	pfSetVec2(mPos, x, y);
}

void FontSystem::SetScale(float scaleX, float scaleY, bool ignoreRules)
{
	if (!ignoreRules) {
		if (scaleX < mMinimumSize) scaleX = mMinimumSize;
		if (scaleY < mMinimumSize) scaleY = mMinimumSize;
		if (mAlwaysSquare) scaleX = scaleY;
	}
	pfSetVec2(mScale, scaleX, scaleY);
}

void FontSystem::SetBox(float x1, float y1, float x2, float y2)
{
	mBoundsBox[PF_X][0] = x1;
	mBoundsBox[PF_X][1] = x2;
	mBoundsBox[PF_Y][0] = y1;
	mBoundsBox[PF_Y][1] = y2;
}

void FontSystem::SetAlign(int alignment)
{
	mAlignment = alignment;
}

void FontSystem::SetColor(float *color1, float *color2)
{
	if (color1) {
		pfCopyVec4(mColor[0], color1);
		if (color2) {
			pfCopyVec4(mColor[1], color2);
		} else {
			pfCopyVec4(mColor[1], color1);
		}
	}
}

void FontSystem::BeginDraw(int fontID)
{
	FontRecord	*f = &(mFonts[fontID]);
	int		win_width, win_height, i;
	pfVec4	hue = {1,1,1,1};
	float	x = 0, y = 0, w = 100, h = 100;
	int		texWidth, texHeight;
	float	win_aspect, aspect640x480, sval, cval;
	float	baseScale = 640.0f / 1024.0f;
	static float	bx = 0.0f, by = 0.0f, scale = 0.0f;

	if (mEnabled) EndDraw();

	getTextureInfo(f->mTexID, NULL, &f->mTexWidth, &f->mTexHeight);
	if (f->mTexWidth <= 0 || f->mTexHeight <= 0) return;

	mCurrentFont = fontID;

//	win_width  = glutGet(GLUT_WINDOW_WIDTH);
//	win_height = glutGet(GLUT_WINDOW_HEIGHT);
//	win_aspect = (float)win_width/(float)win_height;
//	aspect640x480 = 640.0f/480.0f;

///	glMatrixMode(GL_PROJECTION);
///	glPushMatrix();
///	glLoadIdentity();
///	glOrtho(0, 640, 480, 0, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.0f);
	glDepthMask(false);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);

	UseLibTexture(f->mTexID);
	mEnabled = true;
}

void FontSystem::EndDraw(void)
{
	if (!mEnabled) return;

	mEnabled = false;
///	glMatrixMode(GL_PROJECTION);
///	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glDepthMask(true);
	glDisable(GL_ALPHA_TEST);
}

float FontSystem::GetStringWidth(char *fmt, ...)
{
	va_list args;
	FontRecord	*f = &(mFonts[mCurrentFont]);
	float		width = 0.0f;
	char			str[2048], *cp;
	int			ch;

	//	strcpy(str, fmt);
	va_start(args, fmt);
	vsprintf(str, fmt, args);
	va_end(args);

	for (cp = str; (ch=*cp)!=0 && (*cp != '\n'); cp = NextCharacter(cp)) {
		ch = GetSpecialCharacter(cp);
		width += f->mFontChars[ch].mTrailingXOffset;
	}
	width *= mScale[PF_X];
	return(width);
}

float FontSystem::GetWordWidth(char *str)
{
	FontRecord	*f = &(mFonts[mCurrentFont]);
	float		width = 0.0f;
	char		*cp;
	int			ch;

	for (cp = str; (ch=*cp)!=0 && (*cp != '\n') && (*cp != ' '); cp = NextCharacter(cp)) {
		ch = GetSpecialCharacter(cp);
		width += f->mFontChars[ch].mTrailingXOffset;
	}
	width *= mScale[PF_X];
	return(width);
}

int FontSystem::GetSpecialCharacter(char *cp)
{
	int		ch;
	ch = *(unsigned char *)cp;
	if (ch == '^') {
		ch = ((cp[1] - '0') * 100) + ((cp[2] - '0') * 10) + ((cp[3] - '0') * 1);
	}
	return(ch);
}

char *FontSystem::NextCharacter(char *cp)
{
	if (*cp == '^') {
		return(cp+4);
	}
	return(cp+1);
}

void FontSystem::DrawString(char *fmt, ...)
{
	va_list args;
	FontRecord	*f = &(mFonts[mCurrentFont]);
	FontChar	*fc;
	char		str[2048], *cp;
	int			ch;
	int			slot;
	float		u1, u2, v1, v2, x1, x2, y1, y2, z;

	if (!mEnabled) return;

	//	strcpy(str, fmt);
	va_start(args, fmt);
	vsprintf(str, fmt, args);
	va_end(args);



	if (mAlignment == FONT_ALIGNMENT_CENTER) {
		mPos[PF_X] -= 0.5f * GetStringWidth(str);
		if (mPos[PF_X] < 0) mPos[PF_X] = 0;
	} else if (mAlignment == FONT_ALIGNMENT_RIGHT) {
		mPos[PF_X] -= 1.0f * GetStringWidth(str);
		if (mPos[PF_X] < 0) mPos[PF_X] = 0;
	}

	glBegin(GL_QUADS);
	for (cp = str; (ch=*cp)!=0; cp = NextCharacter(cp)) {
		ch = GetSpecialCharacter(cp);
		switch (ch) {
			case '\n':
				mPos[PF_X] = mSetPos[PF_X];
				mPos[PF_Y] += mScale[PF_Y] * f->mCellHeight;
				if (mAlignment == FONT_ALIGNMENT_CENTER) {
					mPos[PF_X] -= 0.5f * GetStringWidth(cp+1);
					if (mPos[PF_X] < 0) mPos[PF_X] = 0;
				} else if (mAlignment == FONT_ALIGNMENT_RIGHT) {
					mPos[PF_X] -= 1.0f * GetStringWidth(cp+1);
					if (mPos[PF_X] < 0) mPos[PF_X] = 0;
				}
				break;
			case ' ':
				fc = &(f->mFontChars[ch]);
				mPos[PF_X] += mScale[PF_X] * fc->mTrailingXOffset;
				if (mPos[PF_X] + GetWordWidth(cp+1) > mBoundsBox[PF_X][1]) {
					mPos[PF_X] = mSetPos[PF_X];
					mPos[PF_Y] += mScale[PF_Y] * f->mCellHeight;
				}
				break;
			default:
				fc = &(f->mFontChars[ch]);
				u1 = fc->mPositionX / f->mTexWidth;
				u2 = (fc->mPositionX + fc->mWidth) / f->mTexWidth;
				v1 = 1.0f - (fc->mPositionY / f->mTexHeight);
				v2 = 1.0f - ((fc->mPositionY + fc->mHeight) / f->mTexHeight);
				x1 = mPos[PF_X] - (mScale[PF_X] * fc->mLeadingXOffset);
				x2 = x1 + (mScale[PF_X] * fc->mWidth);
				y1 = mPos[PF_Y];
				y2 = y1 + (mScale[PF_Y] * fc->mHeight);
				z = 0.0f;

//printf("x %f %f, y %f %f\n", x1, x2, y1, y2);
				glColor4fv(mColor[0]);
				glTexCoord2f(u1, v1);
				glVertex3f(x1, y1, z);
				glTexCoord2f(u2, v1);
				glVertex3f(x2, y1, z);

				glColor4fv(mColor[1]);
				glTexCoord2f(u2, v2);
				glVertex3f(x2, y2, z);
				glTexCoord2f(u1, v2);
				glVertex3f(x1, y2, z);

				mPos[PF_X] += mScale[PF_X] * fc->mTrailingXOffset;
				break;
		}
	}
	glEnd();
}


