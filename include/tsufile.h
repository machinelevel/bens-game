/**** tsufile.h ****/

#ifndef _TSUFILE_H_
#define _TSUFILE_H_

void	tsuInit(void);
void	tsuTerminate(void);
void	tsuMake(void);
void	tsuLoad(void);
void	tsuMarkFile(char *fileName);
void	*tsuGetMemFile(char *fileName, uint32 *psize);


#endif /**** _TSUFILE_H_ ****/

