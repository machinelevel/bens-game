#if 0
/* 
 * 	MacUtils.h
 *
 *	Macintosh utility files for Ben's game.
 */	
 
#include <stdio.h>
#include "types.h"
#include "macsound.h"

#define fopen	mac_fopen
 
extern Boolean mac_init(); 
extern void mac_teardown();
extern uint32 mac_get_time();

extern FILE *mac_fopen( const char *inFileName, const char *inPermissions);
extern FILE *mac_open_resource_file( const char *inFileName, const char *inPermissions);
extern uint16 mac_count_folder_contents(char *inFolderName);
Boolean mac_get_filename_by_index(char *inFolderName, char *inFileName, uint16 inFileIndex);

void mac_LaunchURL(char *urlStr);
#endif
