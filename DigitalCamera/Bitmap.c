//bitmap.c
#include "bitmap.h"
#include <string.h>
#include <stdlib.h>

bitmap_header_t *bmp_create_header(int w, int h) {
	bitmap_header_t *pbitmap  = (bitmap_header_t*)calloc(1, sizeof(bitmap_header_t));
	int _pixelbytesize = w * h * _bitsperpixel/8;
	int _filesize = _pixelbytesize+sizeof(bitmap_header_t);
  pbitmap->fileheader.signature[0] = 'H';
  pbitmap->fileheader.signature[1] = 'A';
	pbitmap->fileheader.filesize = _filesize;
	pbitmap->fileheader.fileoffset_to_pixelarray = sizeof(bitmap_header_t);
	pbitmap->bitmapinfoheader.dibheadersize = sizeof(bitmapinfoheader);
	pbitmap->bitmapinfoheader.width = w;
	pbitmap->bitmapinfoheader.height = h;
	pbitmap->bitmapinfoheader.planes = _planes;
	pbitmap->bitmapinfoheader.bitsperpixel = _bitsperpixel;
	pbitmap->bitmapinfoheader.compression = _compression;
	pbitmap->bitmapinfoheader.imagesize = _pixelbytesize;
	pbitmap->bitmapinfoheader.numcolorspallette = 0;
	return pbitmap;
}

const int16_t QQVGA_W = 160;
const int16_t QQVGA_H = 120;
const int16_t QQVGA_STARTX = 0;
const int16_t QQVGA_STARTY = 124;
reducedBitmap buildFromBuffer(uint8_t *buffer){
		
}

void startBmpBuild(){
	//uint16_t arrYay = [160][120];
	reducedBitmap image;
	image.h=120;
	image.w=160;
	image.x=0;
	image.y=124;
}
void addToBmpBuild(uint8_t,uint8_t,uint8_t);
const uint16_t *getBmpBuildImageArray(void);
