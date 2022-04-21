/**** ej's simple jpeg read code ****/

#include <stdio.h>
#include <memory.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>


bool ejSimpleJPEGRead(FILE *inputFile, void *inputMem, int inputMemSize, void **pOutData, unsigned int *pOutWidth, unsigned int *pOutHeight, unsigned int *pOutBPP)
{
	SDL_RWops* rwops = NULL;
	SDL_Surface* image = NULL;

	if (inputMem)
		rwops = SDL_RWFromMem(inputMem, inputMemSize);
	else
		rwops = SDL_RWFromFP(inputFile, SDL_FALSE);
	image = IMG_Load_RW(rwops, 0);

	if (image)
	{
		SDL_LockSurface(image); // todo: unlock this
		*pOutWidth = image->w;
		*pOutHeight = image->h;
		*pOutBPP = image->format->BitsPerPixel;
		size_t total_bytes = image->h * image->pitch;
		*pOutData = malloc(total_bytes);
		bool do_vertical_flip = true;
		if (do_vertical_flip)
		{
			for (int row = 0; row < image->h; ++row)
			{
				const char* src = (char*)image->pixels + row * image->pitch;
				char* dst = (char*)(*pOutData) + (image->h - (row + 1)) * image->pitch;
				memcpy(dst, src, image->pitch);
			}
		}
		else
		{
			memcpy(*pOutData, image->pixels, total_bytes);
		}
		SDL_UnlockSurface(image);
		SDL_FreeSurface(image);
		return true;
	}
	return false;
}
