#if 0
/* 
 * 	MacUtils.cpp
 *  Copyright © 2004 Make-a-Wish Foundation.  All Rights Reserved.
 *
 *
 *	Macintosh utility files for Ben's game.
 */	
 
#ifndef __ALIASES__
#include <Aliases.h>
#endif

#include <HIServices/InternetConfig.h>
#include <Quicktime/Quicktime.h>
#include "FSp_fopen.h"
#include "types.h"
#include "foxpf.h"
#include "slides.h"
#include "macutils.h"

// External functions not in a header
extern void ShutdownApplication(int exitError);

/*
 *	Function prototypes
 */
Boolean 	IsMacOSXRuntime(void);
StringPtr	StringToP(char *str);
OSErr		FSpGetDirInfo(const FSSpec * inFileSpec, CInfoPBPtr outCInfoPB);
OSErr		FSGetDirectoryID(const FSSpec * inFileSpec, long * outDirID);
OSErr		MakeFSSpec(SInt16 inVRefNum, SInt32 inDirID, ConstStr255Param inName, FSSpec * outFileSpec);
OSErr		MakeFSPath(SInt16 inVRefNum, SInt32 inDirID, ConstStr255Param inPartialPath, ScriptCode inScriptCode, FSSpec * outFolderSpec, Boolean inAllowAliasInPath = false, Boolean inAllowCreateFolders = true);
OSErr		FSpGetCatInfo(const FSSpec * inSpec, CInfoPBPtr outCatInfo);
OSErr		FSMakeFSRef(short inVRefNum, long inDirID, ConstStr255Param inFileName, FSRef & outFSRef);

// Functions stolen from MoreFilesExtras
OSErr		GetParentID(SInt16 inVRefNum, SInt32 inDirID, ConstStrFileNameParam inFileName, SInt32 * outParID);

// other funcs
OSErr		CountFolderContents(FSSpec *inFolderSpec, uint16 *outCount);
OSErr 		GetFileByIndex(const FSSpec * inFolderSpec, SInt16 inIndex, byte inResolveAlias, FSSpec * outFileSpec, byte * outIsFolder, UInt32 * outLogFileSize);
OSErr		ResolveFolderFSSpec(FSSpec *ioFolderSpec);

/*
 *	Globals
 */
Boolean	gAppInited = false;
FSSpec	gAppFSSpec;
FSSpec	gExecutableFSSpec;
FSSpec	gResourcesFSSpec;

enum
{
	kPascalStringMaxLength	= 255
};

#pragma mark ---------- App Utils ---------------

Boolean mac_init()
{
	OSErr						tempErr;
	ProcessSerialNumber			curPSN;
	ProcessInfoRec				processInfo;
	Boolean						success = false;
	
	if ((success = IsMacOSXRuntime()) == true)
	{
		memset(&gAppFSSpec, 0, sizeof(FSSpec));
		memset(&gExecutableFSSpec, 0, sizeof(FSSpec));
		memset(&gResourcesFSSpec, 0, sizeof(FSSpec));
		
		// Now record some information about our application
		tempErr = GetCurrentProcess(&curPSN);
		if (tempErr == noErr)
		{
			processInfo.processInfoLength = sizeof(processInfo);
			processInfo.processName = NULL;
			processInfo.processAppSpec = &gExecutableFSSpec;
			tempErr = GetProcessInformation(&curPSN, &processInfo);
		
			if (tempErr == noErr)
			{
				// now check to see if the app is packaged...
				// determine the location of the Application...
				CFBundleRef bundleRef;
				bundleRef = CFBundleGetMainBundle();
				if ( bundleRef != NULL )
				{
					CFURLRef mainBundleURL;
					
					mainBundleURL = CFBundleCopyBundleURL( bundleRef );
					if ( mainBundleURL != NULL )
					{
						FSRef mainBundleFSRef;
						if ( CFURLGetFSRef( mainBundleURL, &mainBundleFSRef ) )
						{
							tempErr = FSGetCatalogInfo( &mainBundleFSRef, kFSCatInfoNone, NULL, NULL, &gAppFSSpec, NULL );
							if (tempErr == noErr)
							{
								//
								// Calling CFBundleGetMainBundle from an application that is not bundled will return
								// a reference to the folder containing the application. So we check to see if
								// that is what we got here and, if so, we copy the executable's spec on top of the
								// application spec.
								//
								SInt32 parentDirID;
								tempErr = GetParentID( gExecutableFSSpec.vRefNum, gExecutableFSSpec.parID, "\p", &parentDirID );
								if ( tempErr == noErr )
								{
									if ( parentDirID == gAppFSSpec.parID )
									{
										// the app is not bundled, so we should copy the location of the executable...
										gResourcesFSSpec 	= gAppFSSpec;
										gAppFSSpec 		 	= gExecutableFSSpec;
									}
									else
									{
										// Get the directory ID of the bundle folder
										SInt32	bundleDirNum	= 0;
										 (void) ::FSGetDirectoryID(&gAppFSSpec, &bundleDirNum);
										tempErr = ::FSMakeFSSpec(	gAppFSSpec.vRefNum, 
																	bundleDirNum, 
																	"\p:Contents:Resources",
																	&gResourcesFSSpec);
									}
									
									// Okay, everything is done, we're inited								
									if (tempErr == noErr)
										gAppInited = true;
								}
							}
						}
						
						CFRelease( mainBundleURL );
					}
				}
			}
		}

		EnterMovies();
	}
	
	return (success);
}

void
mac_teardown()
{
	// call the function in Main
	ExitMovies();
	
	// Now free up the movie data
	mac_sound_free();
}

/*------------------------------------------------------------------------------
	mac_fopen
	
	Opens a file with the given permissions.
------------------------------------------------------------------------------*/

FILE *
mac_fopen( 
	const char				*inFileName,
	const char				*inPermissions)
{
	OSErr			err = noErr;
	FSSpec			fileSpec;
	FSRef			fileRef;
	FILE 			*outFilePtr = NULL;
	CFBundleRef 	bundleRef;
	FSRef 			parentFSRef;
	
	
	if (gAppInited == false)
	{
		// For some reason we weren't inited so return NULL
		return (NULL);
	}
	
	bundleRef = CFBundleGetMainBundle();
	if ( bundleRef != NULL )
	{
		CFURLRef mainBundleURL;
		
		mainBundleURL = CFBundleCopyBundleURL( bundleRef );
		if ( mainBundleURL != NULL )
		{
			FSRef mainBundleFSRef;
			if ( CFURLGetFSRef( mainBundleURL, &mainBundleFSRef ) )
			{
				err = FSGetCatalogInfo( &mainBundleFSRef, kFSCatInfoNone, NULL, NULL, NULL, &parentFSRef );
				if (err == noErr)
				{
					CFURLRef 		cfRef, cfBaseRef;
					CFURLRef		baseURL;
					CFStringRef		cfString = CFStringCreateWithCString( kCFAllocatorDefault, inFileName, kCFStringEncodingASCII );
					
					baseURL =  CFURLCreateFromFSRef(kCFAllocatorDefault, &parentFSRef);
					
					if (cfString && baseURL)
					{
						cfRef = CFURLCreateWithString(kCFAllocatorDefault, cfString, baseURL);
						
						if (cfRef)
						{
							if (!CFURLGetFSRef( cfRef, &fileRef ))
							{
								if ((inPermissions[0] == 'w') || (inPermissions[0] == 'a'))
								{
   	 								FSSpec		spec;
							        CFIndex 	nameStringSize = CFStringGetLength(cfString);
							        Ptr 		nameStringPtr = NewPtr(nameStringSize*2);

							        CFStringGetCharacters(cfString, (CFRange){0, nameStringSize}, (UniChar *)nameStringPtr);
						            err = FSCreateFileUnicode(&parentFSRef, nameStringSize, (UniChar *)nameStringPtr, 
						                NULL, NULL, &fileRef, &spec);
								
									DisposePtr(nameStringPtr);
								}
							}

							outFilePtr = FSRef_fopen(&fileRef, inPermissions);

							if (outFilePtr != NULL)
							{	
								return (outFilePtr);
							}
						
							CFRelease(cfRef);
						}
					
						CFRelease(cfString);
						CFRelease(baseURL);
					}
				}
			}
		
			CFRelease(mainBundleURL);
		}
	}	
	
	return (outFilePtr);
}

/*
 *	Opens a file in the resource folder of the bundle.
 *	Note:
 *		Any files that are going to be in the resource folder
 *		are considered CASE sensitive for names, so be warned.
 */
FILE *mac_open_resource_file(
	const char				*inFileName,
	const char				*inPermissions)
{
	CFBundleRef 	bundleRef;
	FSRef 			parentFSRef;
	CFURLRef		resourceURL;
	CFStringRef		cfString;
	FSRef			fileRef;
	FILE 			*outFilePtr = NULL;
	char			*cString;
		
	bundleRef = CFBundleGetMainBundle();
	if ( bundleRef != NULL )
	{
		cfString = CFStringCreateWithCString( kCFAllocatorDefault, inFileName, kCFStringEncodingASCII );
		if (cfString)
		{
			resourceURL = CFBundleCopyResourceURL(bundleRef, cfString, NULL, NULL);
			
			if(resourceURL)
			{
				if (CFURLGetFSRef( resourceURL, &fileRef ))
				{
					outFilePtr = FSRef_fopen(&fileRef, inPermissions);
				}
			}
		}
	}

	return (outFilePtr);
}


uint16 mac_count_folder_contents(
	char *inFolderName)
{
	OSErr		err = noErr;
	FSSpec		folderSpec;
	FILE 		*outFilePtr = NULL;
	StrFileName	fileSpecFolderName;
	uint16		folderItemCount = 0;
	
	if (gAppInited == false)
	{
		// For some reason we weren't inited so return NULL
		return (NULL);
	}
	
	strcpy((char *)fileSpecFolderName, inFolderName);
	
	::StringToP((char *) fileSpecFolderName);

	// 	Use the application vRefNum and parID, hopefully anything passed in here has the
	//	right relative paths
	err = ::MakeFSSpec(gAppFSSpec.vRefNum, gAppFSSpec.parID, fileSpecFolderName, &folderSpec);
	if (err == fnfErr)
		err = noErr;

	err = CountFolderContents( &folderSpec, &folderItemCount);
	
	return (folderItemCount);

}

Boolean 
mac_get_filename_by_index(
	char 	*inFolderName, 
	char 	*inFileName, 
	uint16 inFileIndex)
{
	OSErr			err = noErr;
	FSSpec			folderSpec;
	FSSpec			fileSpec;
	FILE 			*outFilePtr = NULL;
	StrFileName		fileSpecFolderName;
	uint16			folderItemCount = 0;
	byte			outIsFolder;
	unsigned long	outLogFileSize;
	Boolean			foundResult = false;
	
	if (gAppInited == false)
	{
		// For some reason we weren't inited so return NULL
		return (NULL);
	}
	
	strcpy((char *)fileSpecFolderName, inFolderName);
	
	::StringToP((char *) fileSpecFolderName);

	// 	Use the application vRefNum and parID, hopefully anything passed in here has the
	//	right relative paths
	err = ::MakeFSSpec(gAppFSSpec.vRefNum, gAppFSSpec.parID, fileSpecFolderName, &folderSpec);
	if (err == fnfErr)
		err = noErr;

	err= ResolveFolderFSSpec(&folderSpec);
	if (err == noErr)
	{
		err = GetFileByIndex( &folderSpec, inFileIndex, false, &fileSpec, &outIsFolder, &outLogFileSize);
		if (err == noErr)
		{
			::CopyPascalStringToC(fileSpec.name, inFileName);
			foundResult = true;
		}
	}
		
	return (foundResult);
}

void mac_LaunchURL(char *urlStr)
{
    OSStatus err;

    ICInstance inst;
    long startSel;
    long endSel;
    
	// Use your creator code if you have one!
	err = ICStart(&inst, 'Bens');
	if (err == noErr) 
	{
	    startSel = 0;
	    endSel = 0;
	    err = ICLaunchURL(inst, "\p", urlStr, strlen(urlStr), &startSel, &endSel);
	}
	(void) ICStop(inst);
	  
	if (!gOptionLaunchInWindow) 
	{
     	mac_teardown();
    }
} 


#pragma mark ---------- OS Utils ---------------

// IsMacOSXRuntime

// Runtime check to see if we are running on Mac OS X

Boolean IsMacOSXRuntime (void)
{
    UInt32 response;
    return (Gestalt(gestaltSystemVersion, (SInt32 *) &response) == noErr) && (response >= 0x01000);
} 

#define USE_CONSTANT_TIME		0

uint32 mac_get_time()
{
#if MAC_BUILD
	static unsigned long long startTimeMS = 0;
	unsigned long long microseconds;

	//	Mac version
	// 		*** Have to return current milliseconds, making sure we are couting up and can wrap.
	// 			brad
#if USE_CONSTANT_TIME
	if (startTimeMS == 0)
	{
		startTimeMS = microseconds;
	}
	
	microseconds = startTimeMS - microseconds;

	startTimeMS += 30;
#else
	Microseconds((UnsignedWide*)&microseconds); // since boot
	microseconds /= 1000;
	
	if (startTimeMS == 0)
	{
		startTimeMS = microseconds;
	}
	
	microseconds -= startTimeMS;
#endif
	
	return (microseconds);
	
#endif
	return (0);
}

#pragma mark ---------- String Utils ---------------

/*------------------------------------------------------------------
	CopyMemory

	This function performs a BlockMove operation, first checking
	for very small moves and doing them in-place.

	Parameters:
		In:		inSource		- source of copy
				inDest			- destination of copy
				inByteCount		- number of bytes to compare
------------------------------------------------------------------*/

void
CopyMemory(const void* inSource, void* inDest, UInt32 inByteCount)
{
	if (inByteCount == 4)
		*(UInt32*)inDest = *(UInt32*)inSource;
	else if (inByteCount == 2)
		*(UInt16*)inDest = *(UInt16*)inSource;
	else if (inByteCount == 1)
		*(UInt8*)inDest = *(UInt8*)inSource;
	else
		BlockMoveData((Ptr)inSource, (Ptr)inDest, inByteCount);
}

/*------------------------------------------------------------------
	UMin32

	This function returns the minimum of two 32-bit unsigned ints.
------------------------------------------------------------------*/

UInt32 UMin32(UInt32 x, UInt32 y)
{
	return (x < y ? x : y);
}


/*
 *	StringToP
 */
StringPtr
StringToP(char *str)
{
	::CopyCStringToPascal(str, (StringPtr)str);
	return (StringPtr)str;
}

/*------------------------------------------------------------------
	CopyPString

	This function performs a strcpy on a Pascal string.

	Parameters:
		In:		inSource		- source of copy
				inDest			- destination of copy
				inMaxLength		- maximum length of result string
------------------------------------------------------------------*/

void
CopyPString(ConstStr255Param inSource, StringPtr inDest, UInt32 inMaxLength)
{
	UInt32 length = UMin32(StrLength(inSource), inMaxLength);
	
	if (length)
		CopyMemory(&inSource[1], &inDest[1], length);

	inDest[0] = length;
}

#pragma mark ---------- Folder/File Utils ---------------

/*------------------------------------------------------------------
	MakeFSSpec

	This routine creates a file specification record from the given
	parameters. It replaces the API call FSMakeFSSpec() because
	FSMakeFSSpec() does not properly handle creating an fsspec for
	the root directory of a	PC Exchange volume.

	Parameters:
		In:		inVRefNum		- the volume
				inDirID			- the directory ID
				inName			- the item name or partial pathname
		Out:	outFileSpec		- file spec to folder
				(return)		- true if found
------------------------------------------------------------------*/

OSErr
MakeFSSpec(	SInt16				inVRefNum,
			SInt32				inDirID,
			ConstStr255Param	inName,
			FSSpec*				outFileSpec)
{
	OSErr status;
	HParamBlockRec params;
	Str255 name;

	if (inDirID != fsRtDirID || StrLength(inName) != 0)
	{
		status = ::FSMakeFSSpec(inVRefNum, inDirID, inName, outFileSpec);
	}
	else
	{
		// Get the root directory name
		params.volumeParam.ioCompletion = NULL;
		BlockMoveData(inName, name, 256);
		params.volumeParam.ioNamePtr 	= name;
		params.volumeParam.ioVRefNum 	= inVRefNum;
		params.volumeParam.ioVolIndex	= 0;	// find using vRefNum only

		status = ::PBHGetVInfoSync(&params);
		if (status != noErr)
			return(status);

		status = ::FSMakeFSSpec(inVRefNum,
								fsRtParID, 
								name, 
								outFileSpec);
	}

	return status;
}

/*------------------------------------------------------------------
	FSpGetDirInfo

	This routine calls PBGetCatInfoSync with the FSSpec.

	Parameters:
		In:		inFileSpec		- file spec
		Out:	outCInfoPB		- ptr to a CInfoPBRec
				(return)		- noErr or file system error
------------------------------------------------------------------*/

OSErr
FSpGetDirInfo(	const FSSpec *	inFileSpec,
				CInfoPBPtr 		outCInfoPB)
{
	OSErr			err = noErr;

	memset(outCInfoPB, 0, sizeof(CInfoPBRec));

	outCInfoPB->dirInfo.ioVRefNum = inFileSpec->vRefNum;
	outCInfoPB->dirInfo.ioDrDirID = inFileSpec->parID;
	outCInfoPB->dirInfo.ioNamePtr = (StringPtr) inFileSpec->name;

	err = PBGetCatInfoSync(outCInfoPB);

	if (!err && !(outCInfoPB->hFileInfo.ioFlAttrib & ioDirMask))
		err = dirNFErr;

	return err;
}

/*------------------------------------------------------------------
	FSGetDirectoryID

	This routine gets the ioDrDirID from an FSSpec.

	Parameters:
		In:		inFileSpec		- file spec
		Out:	dirID			- ioDrDirID
				(return)		- noErr or file system error
------------------------------------------------------------------*/

OSErr
FSGetDirectoryID(	const FSSpec *	inFileSpec,
					long *			outDirID)
{
	OSErr 			err = noErr;
	CInfoPBRec		cInfoPB;

	err = FSpGetDirInfo(inFileSpec, &cInfoPB);
	if (err == noErr)
		*outDirID = cInfoPB.dirInfo.ioDrDirID;

	return err;
}

// Stolen from MoreFilesExtras

/*------------------------------------------------------------------
	GetVolumeInfoNoName

	Stolen from MoreFilesExtras for use in GetParentID.
------------------------------------------------------------------*/

static OSErr
GetVolumeInfoNoName(
	ConstStr255Param	inPathname,
	SInt16				inVRefNum,
	HParmBlkPtr			outPB)
{
	Str255	tempPathname;
	OSErr	err;
	
	/* Make sure pb parameter is not NULL */
	if (outPB != NULL)
	{
		outPB->volumeParam.ioVRefNum = inVRefNum;
		if (inPathname == NULL)
		{
			outPB->volumeParam.ioNamePtr	= NULL;
			outPB->volumeParam.ioVolIndex	= 0;	/* use ioVRefNum only */
		}
		else
		{
			::BlockMoveData(inPathname, tempPathname, inPathname[0] + 1);	/* make a copy of the string and */
			outPB->volumeParam.ioNamePtr	= tempPathname;					/* use the copy so the original isn't trashed */
			outPB->volumeParam.ioVolIndex	= -1;	/* use ioNamePtr/ioVRefNum combination */
		}
		err = ::PBHGetVInfoSync(outPB);
		outPB->volumeParam.ioNamePtr = NULL;	/* ioNamePtr may point to local tempPathname, so don't return it */
	}
	else
	{
		err = paramErr;
	}
	
	return err;
}


/*------------------------------------------------------------------
	DetermineVRefNum

	Stolen from MoreFilesExtras for use in GetParentID.
------------------------------------------------------------------*/

static OSErr
DetermineVRefNum(
	ConstStr255Param	inPathname,
	SInt16				inVRefNum,
	SInt16 *			outRealVRefNum)
{
	HParamBlockRec	pb;
	OSErr			err;
	
	err = GetVolumeInfoNoName(inPathname, inVRefNum, &pb);
	
	if (err == noErr)
	{
		*outRealVRefNum = pb.volumeParam.ioVRefNum;
	}
	
	return err;
}


/*------------------------------------------------------------------
	GetParentID

	Stolen from MoreFilesExtras.cpp
------------------------------------------------------------------*/

OSErr
GetParentID(
	SInt16					inVRefNum,
	SInt32					inDirID,
	ConstStrFileNameParam	inFileName,		/* leave empty for folders */
	SInt32 *				outParID)
{
	CInfoPBRec	pb;
	StrFileName	tempName;
	OSErr		err;
	SInt16		realVRefNum;
	
	/* Protection against File Sharing problem */
	if ((inFileName == NULL) || (inFileName[0] == 0))
	{
		tempName[0] = 0;
		pb.hFileInfo.ioNamePtr		= tempName;
		pb.hFileInfo.ioFDirIndex	= -1;		/* use ioDirID */
	}
	else
	{
		pb.hFileInfo.ioNamePtr		= (StringPtr)inFileName;
		pb.hFileInfo.ioFDirIndex	= 0;		/* use ioNamePtr and ioDirID */
	}
	pb.hFileInfo.ioVRefNum	= inVRefNum;
	pb.hFileInfo.ioDirID	= inDirID;
	err = ::PBGetCatInfoSync(&pb);
	
	if (err == noErr)
	{
		/*
		**	There's a bug in HFS where the wrong parent dir ID can be
		**	returned if multiple separators are used at the end of a
		**	pathname. For example, if the pathname:
		**		'volumeName:System Folder:Extensions::'
		**	is passed, the directory ID of the Extensions folder is
		**	returned in the ioFlParID field instead of fsRtDirID. Since
		**	multiple separators at the end of a pathname always specifies
		**	a directory, we only need to work-around cases where the
		**	object is a directory and there are multiple separators at
		**	the end of the name parameter.
		*/
		if ((pb.hFileInfo.ioFlAttrib & kioFlAttribDirMask) != 0)
		{
			/* It's a directory */
			
			/* is there a pathname? */
			if (pb.hFileInfo.ioNamePtr == inFileName)
			{
				/* could it contain multiple separators? */
				if (inFileName[0] >= 2)
				{
					/* does it contain multiple separators at the end? */
					if ((inFileName[inFileName[0]] == ':') && (inFileName[inFileName[0] - 1] == ':'))
					{
						/* OK, then do the extra stuff to get the correct parID */
						
						/* Get the real vRefNum (this should not fail) */
						err = DetermineVRefNum(inFileName, inVRefNum, &realVRefNum);
						if (err == noErr)
						{
							/* we don't need the parent's name, but protect against File Sharing problem */
							tempName[0] = 0;
							pb.dirInfo.ioNamePtr	= tempName;
							pb.dirInfo.ioVRefNum	= realVRefNum;
							/* pb.dirInfo.ioDrDirID already contains the */
							/* dirID of the directory object */
							pb.dirInfo.ioFDirIndex = -1;	/* get information about ioDirID */
							err = ::PBGetCatInfoSync(&pb);
							/* now, pb.dirInfo.ioDrParID contains the correct parID */
						}
					}
				}
			}
		}
		
		if (err == noErr)
		{
			/* if no errors, then pb.hFileInfo.ioFlParID (pb.dirInfo.ioDrParID) */
			/* contains the parent ID */
			*outParID = pb.hFileInfo.ioFlParID;
		}
	}
	
	return err;
}


/*------------------------------------------------------------------
	CountFolderContents

	This routine counts the number of items in a specified
	folder.

	Parameters:
		In:		inFolderSpec	- a valid folder FS spec
		Out:	outCount		- number of contained items
				(return)		- error
------------------------------------------------------------------*/

OSErr
CountFolderContents(	FSSpec*		inFolderSpec,
						uint16*		outCount)
{
	OSErr 			error;
	CInfoPBRec 		pb;

	pb.dirInfo.ioCompletion = NULL;						// Fill in the parameter block.
	pb.dirInfo.ioVRefNum = inFolderSpec->vRefNum;
	pb.dirInfo.ioDrDirID = inFolderSpec->parID;
	pb.dirInfo.ioNamePtr = inFolderSpec->name;
	
	// The old way (ROSWELL) way, was passing a different parameter in name
	// so now in the new way (DREAMLAND) we are handling it better.
	if (inFolderSpec->name != NULL)
		pb.dirInfo.ioFDirIndex = 0;
	else
		pb.dirInfo.ioFDirIndex = -1;

	error = PBGetCatInfoSync(&pb);						// Get the info on the directory.

	*outCount = pb.dirInfo.ioDrNmFls;

	return error;
}


/*------------------------------------------------------------------
	GetFileByIndex

	This routine returns the n'th item in the specified directory
	where "n" is defined by the inIndex parameter.

	Parameters:
		In:		inFolderSpec		- FS spec describing folder
				inIndex				- index of requested item
				inResolveAlias		- resolve aliases?
		Out:	outFileSpec			- FS spec of n'th file
				outIsFolder			- true if item was a folder
				outLogFileSize		- logical file size
				(return)			- noErr or file system error
------------------------------------------------------------------*/

OSErr
GetFileByIndex(	const FSSpec*	inFolderSpec,
				SInt16 			inIndex,
				byte		 	inResolveAlias,
				FSSpec*			outFileSpec,
				byte*			outIsFolder,
				UInt32*			outLogFileSize)
{
	CInfoPBRec 		pb;
	Str255 			fileName;
	OSErr 			error;
	byte 		wasAlias;

	*outIsFolder = false;

	fileName[0] = 0;
	pb.dirInfo.ioVRefNum = inFolderSpec->vRefNum;
	pb.dirInfo.ioDrDirID = inFolderSpec->parID;
	pb.dirInfo.ioNamePtr = fileName;
	pb.dirInfo.ioFDirIndex = inIndex;
	pb.dirInfo.ioFRefNum = 0;

	// Let PBCatInfo do most of the work
	error = PBGetCatInfoSync(&pb);

	if (error == noErr)
	{
		::CopyPString(pb.dirInfo.ioNamePtr, outFileSpec->name, sizeof(StrFileName) - 1);
		error = FSMakeFSSpec(inFolderSpec->vRefNum,
							 inFolderSpec->parID,
							 outFileSpec->name,
							 outFileSpec);

		if (error == noErr && inResolveAlias)
			error = ResolveAliasFile(outFileSpec, true, outIsFolder, &wasAlias);
		else
			*outIsFolder = ((pb.dirInfo.ioFlAttrib & ioDirMask) != 0);
	}

	*outLogFileSize = pb.hFileInfo.ioFlLgLen;

	return error;
}

/*------------------------------------------------------------------
	ResolveFolderFSSpec

	This routine resolves an FS spec belonging to a folder. The
	input FS spec includes the folder name. The output FS spec
	only has a parID and vRefNum so it can be used to specify
	the folder's contents.

	Parameters:
		In/Out:	ioFolderSpec		- spec for folder
------------------------------------------------------------------*/

OSErr
ResolveFolderFSSpec(FSSpec *ioFolderSpec)
{
	OSErr 			error;
	CInfoPBRec 		pb;

	pb.dirInfo.ioCompletion = NULL;
	pb.dirInfo.ioVRefNum = ioFolderSpec->vRefNum;
	pb.dirInfo.ioDrDirID = ioFolderSpec->parID;
	pb.dirInfo.ioFDirIndex = 0;
	pb.dirInfo.ioNamePtr = (StringPtr)(&(ioFolderSpec->name));

	error = PBGetCatInfoSync(&pb);
	if (error != noErr)
		return error;

	// Make sure it was a folder
	if (pb.dirInfo.ioFlAttrib & ioDirMask)
	{
		ioFolderSpec->parID = pb.dirInfo.ioDrDirID;
		ioFolderSpec->name[0] = '\0';
	}
	
	return noErr;
}

/*------------------------------------------------------------------
	FSMakeFSRef
	
	Create an FSRef for an existing object specified by a combination
    of volume refnum, parent directory, and pathname.
------------------------------------------------------------------*/

OSErr
FSMakeFSRef(
	short				inVRefNum, 
	long				inDirID, 
	ConstStr255Param	inFileName, 
	FSRef &				outFSRef)
{
	OSErr		status = noErr;
	
	// Make a local copy of the file name - this may get modified
	Str255		fileName;
	::CopyPString(inFileName, fileName, kPascalStringMaxLength);
	
	// Call the synchronous parameter block routine
	FSRefParam	paramBlock	= {0};	// zero struct
	paramBlock.ioNamePtr	= fileName;
	paramBlock.ioVRefNum	= inVRefNum;
	paramBlock.ioDirID		= inDirID;
	paramBlock.newRef		= &outFSRef;
	status = ::PBMakeFSRefSync(&paramBlock);
	
	return status;
}

#endif
