
enum {
	FONT_ID_MAIN,
	FONT_ID_NUMBERS,

	FONT_ID_HOWMANY	// Just to count and terminate
};

#define FONT_ALIGNMENT_LEFT		0x0001
#define FONT_ALIGNMENT_CENTER	0x0002
#define FONT_ALIGNMENT_RIGHT	0x0004

#define NUM_CHARACTER_SLOTS 1000

class FontChar {
public:
	int			mCode;					// The Unicode representation of the character. If you're dealing only with ASCII characters, you can just treat this as an ASCII code.  
	int			mPageNumber;			// Zero-based index of the texture page containing this character. If your entire font fits on a single page, this value will always be zero.  
	float		mPositionX, mPositionY;	// The location of the top-left corner of the character within the texture page. (0,0) represents the top-left corner of the texture page.  
	float		mWidth, mHeight;		// The width and height of the character cell on the texture page  
	float		mLeadingXOffset;		// LeadingXOffset refers to the number of pixels by which the character should overlap the preceeding character (eg. In the word "My" using an italicised font, the tail of the letter "y" may extend leftwards a few pixels into the space occupied by the "M").  
	float		mTrailingXOffset;		// TrailingXOffset refers to the number of pixels between LeadingXOffset and the left edge of the next character to be rendered.  
};

class FontRecord {
public:
	char		mName[256];
	int32		mTexID, mTexWidth, mTexHeight;

	int			mNumChars;		// The number of characters stored in the font.  
	int			mNumPages;		// The number of texture pages used to store the font. This will be at least 1.  
	int			mMinCode;		// The lowest-numbered character code represented by the font  
	int			mMaxCode;		// The highest-numbered character code represented by the font. Note that NumChars is not necessarily the same as ((MaxCode - MinCode) + 1), as the characters within the range may not be contiguous.  
	int			mCellHeight;	// The value of Height (pixels) when the font was generated. 
	FontChar	mFontChars[NUM_CHARACTER_SLOTS];
	FontRecord(void);
	~FontRecord(void);
};

class FontSystem {
public:
	FontRecord	mFonts[FONT_ID_HOWMANY];
	pfVec2	mSetPos;
	pfVec2	mPos;
	pfVec2	mScale;
	pfVec2	mBoundsBox[2];
	pfVec4	mColor[2];
	int		mCurrentFont;
	int		mAlignment;
	bool	mEnabled;

	float	mMinimumSize;	// The smallest this font should ever be written
	bool	mAlwaysSquare;	// Should it always always be square?

	FontSystem(void);
	~FontSystem(void);
	void	LoadAll(void);
	bool	LoadFont(int fontID, char *name, int texID);

	void	SetPos(float x, float y);
	void	SetScale(float scaleX, float scaleY, bool ignoreRules = false);
	void	SetAlign(int alignment);
	void	SetColor(float *color1, float *color2 = NULL);
	void	SetBox(float x1, float y1, float x2, float y2);

	int		GetSpecialCharacter(char *cp);
	char	*NextCharacter(char *cp);
	int		GetSlotFromUnicode(int unicode);
	float	GetWordWidth(char *str);
	float	GetStringWidth(char *fmt, ...);
	void	BeginDraw(int fontID);
	void	EndDraw(void);
	void	DrawString(char *fmt, ...);
};

extern FontSystem	*gFontSys;


