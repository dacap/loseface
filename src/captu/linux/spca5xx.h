#ifndef SPCA5XX_H
#define SPCA5XX_H

#define DWIDTH 320
#define DHEIGHT 240

/* Input data output Palette define here */
#define VIDEO_PALETTE_RGB565 3		/* 16bit RGB */
#define VIDEO_PALETTE_RGB24  4		/* 24bit RGB */
#define VIDEO_PALETTE_RGB32  5		/* 32bit RGB */
#define VIDEO_PALETTE_YUV420P 15	/* YUV 420P */
#define VIDEO_PALETTE_RAW_JPEG  20

/* Camera type jpeg yuvy yyuv yuyv grey gbrg*/
enum {
	JPEG,
	YUVY,
	YYUV,
	YUYV,
	GREY,
	GBRG,
	UNKNOW,
};

struct pictparam {
	 int change;
	 int force_rgb;
         int gamma ;
	 int OffRed ;
	 int OffBlue;
	 int OffGreen;
	 int GRed ;
	 int GBlue ;
	 int GGreen ;
};
	
struct spca5xx_frame {
	unsigned char *data;		/* Frame buffer */
	unsigned char *tmpbuffer;	/* temporary buffer spca50x->tmpbuffer need for decoding*/
	int seq;                /* Frame sequence number */

	int depth;		/* Bytes per pixel */
	int width;		/* Width application is expecting */
	int height;		/* Height */

	int hdrwidth;		/* Width the frame actually is */
	int hdrheight;		/* Height */
	int method;		/* The decoding method for that frame 0 nothing 1 crop 2 div 4 mult */
	int cropx1;		/* value to be send with the frame for decoding feature */
	int cropx2;
	int cropy1;
	int cropy2;
	int x;
	int y;
	unsigned int format;	/* Format asked by apps for this frame */
	int cameratype;		/* native in frame format */
	struct pictparam pictsetting;	
	long scanlength;	/* uncompressed, raw data length of frame */	
};



#endif /* SPCA5XX_H */
