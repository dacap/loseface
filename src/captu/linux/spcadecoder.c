/****************************************************************************
#	 	spcadecoder: Generic decoder for various input stream yyuv  #
# yuyv yuvy jpeg bayer rggb with gamma correct                              #
# and various output palette rgb16 rgb24 rgb32 yuv420p                      #
# various output size with crop feature                                     #
# 		Copyright (C) 2003 Michel Xhaard   mxhaard@magic.fr         #
#                                                                           #
# This program is free software; you can redistribute it and/or modify      #
# it under the terms of the GNU General Public License as published by      #
# the Free Software Foundation; either version 2 of the License, or         #
# (at your option) any later version.                                       #
#                                                                           #
# This program is distributed in the hope that it will be useful,           #
# but WITHOUT ANY WARRANTY; without even the implied warranty of            #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             #
# GNU General Public License for more details.                              #
#                                                                           #
# You should have received a copy of the GNU General Public License         #
# along with this program; if not, write to the Free Software               #
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA #
#                                                                           #
****************************************************************************/


#ifndef __KERNEL__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/types.h>
#else /* __KERNEL__ */
#include <linux/string.h>

#endif /* __KERNEL__ */


#include "spcadecoder.h"
#include "jpeg_header.h"
#include "spcagamma.h"
#include "spca5xx.h"


int spca50x_outpicture ( struct spca5xx_frame *myframe );

static int jpeg_decode(struct spca5xx_frame *myframe, int force_rgb);
static int yuv_decode(struct spca5xx_frame *myframe, int force_rgb);
static int bayer_decode(struct spca5xx_frame *myframe, int force_rgb);

inline static void idct (int *, int *, int *, long, int);
inline static void decode_mcus (struct in *, int *, int, struct scan *, int *);
static void dec_makehuff (struct dec_hufftbl *, int *, unsigned char *);
static void idctqtab (unsigned char *, int *);
static void scaleidctqtab (int *, int);

/*	Gtable[0][n] -> 2.2
*	Gtable[1][n] -> 1.7
*	Gtable[2][n] -> 1.45
*	Gtable[3][n] -> 1
*	Gtable[4][n] -> 0.6896
*	Gtable[5][n] -> 0.5882
*	Gtable[6][n] -> 0.4545
*	gCor coeff 0..6
*/
static int bgr = 0;
 
static struct comp comps[MAXCOMP] = {
	{0x01, 0x22, 0x00},
	{0x02, 0x11, 0x01},
	{0x03, 0x11, 0x01},
	{0x00, 0x00, 0x00}
};

static struct scan dscans[MAXCOMP];

static unsigned char quant[4][64];

static struct dec_hufftbl dhuff[4];

#define dec_huffdc (dhuff + 0)
#define dec_huffac (dhuff + 2)

static struct in in;

int dquant[3][64];

void
jpeg_reset_input_context (unsigned char *buf)
{
	/* set input context */
	in.p = buf;
	in.left = 0;
	in.bits = 0;
	in.marker = 0;
}


#define CLIP(color) (unsigned char)((color>0xFF)?0xff:((color<0)?0:color))
void 
create_jpeg_from_data ( unsigned char* dst,  unsigned char* src, int qIndex,
		    int w, int h, unsigned char format, int o_size,
		    int *size, int omit_huffman_table)
{

	int i = 0;
	unsigned char *start;
	unsigned char value;
	__u32 val;
	start = dst;
	/* copy the header from the template */
	 memcpy (dst, GsmartJPGDefaultHeaderPart1,
		  GSMART_JPG_DEFAULT_HEADER_PART1_LENGTH);

	/* modify quantization table */
	memcpy (dst + 7, GsmartQTable[qIndex * 2], 64);
	memcpy (dst + 72, GsmartQTable[qIndex * 2 + 1], 64);

	dst += GSMART_JPG_DEFAULT_HEADER_PART1_LENGTH;

	/* copy Huffman table */
	if (!omit_huffman_table)
	{
		memcpy (dst, GsmartJPGDefaultHeaderPart2,
			GSMART_JPG_DEFAULT_HEADER_PART2_LENGTH);
		dst += GSMART_JPG_DEFAULT_HEADER_PART2_LENGTH;
	}
		memcpy (dst, GsmartJPGDefaultHeaderPart3,
			GSMART_JPG_DEFAULT_HEADER_PART3_LENGTH);

	/* modify the image width, height */
	*(dst + 8) = w & 0xFF;	//Image width low byte
	*(dst + 7) = w >> 8 & 0xFF;	//Image width high byte
	*(dst + 6) = h & 0xFF;	//Image height low byte
	*(dst + 5) = h >> 8 & 0xFF;	//Image height high byte

	/* set the format */
	*(dst + 11) = format;

	/* point to real JPG compress data start position and copy */
	dst += GSMART_JPG_DEFAULT_HEADER_PART3_LENGTH;
	/* copy data since we found a end of frame 0x00 0x00 0x00 0x00*/
	/* seem to work with all my test but ... mxhaard 5 06 2003 */
	for (i = 0; i < o_size; i++) {
		value = *(src + i) & 0xFF;
		val = *((__u32*)(src+i));
		
		*(dst) = value;
		dst++;
		/* is there end ? */
		if (val == 0x00000000)
			break;
		if (value == 0xFF) {
			*(dst) = 0x00;
			dst++;
		}
	}

	/* Add end of image marker */
	*(dst++) = 0xFF;
	*(dst++) = 0xD9;

	*size = dst - start;
}

int spca50x_outpicture ( struct spca5xx_frame *myframe )
{	/* general idea keep a frame in the temporary buffer from the tasklet*/
	/* decode with native format at input and asked format at output */
	/* myframe->cameratype is the native input format */
	/* myframe->format is the asked format */
	int done = 0;
	int i;
	struct pictparam *gCorrect = &myframe->pictsetting;
	
	if (gCorrect->change) {
		if ( gCorrect->change == 0x01) {
		//printf ("change in jpeg decoder \n");
		/* Gamma setting change compute all case */
			memcpy (&GTable[7],&GTable[gCorrect->gamma],256);
			memcpy (&GTable[8],&GTable[gCorrect->gamma],256);
			memcpy (&GTable[9],&GTable[gCorrect->gamma],256);
			for (i =0; i < 256 ; i++){
				GTable[7][i] = CLIP(((GTable[7][i] + gCorrect->OffRed) * gCorrect->GRed) >> 8);
				GTable[8][i] = CLIP(((GTable[8][i] + gCorrect->OffGreen) * gCorrect->GGreen) >> 8);
				GTable[9][i] = CLIP(((GTable[9][i] + gCorrect->OffBlue) * gCorrect->GBlue) >> 8);
			
			}
			bgr = gCorrect->force_rgb;
			gCorrect->change = 0x00;
		}
		if ( gCorrect->change == 0x02) {
		/* Red setting change compute Red Value */
			memcpy (&GTable[7],&GTable[gCorrect->gamma],256);
			for (i =0; i < 256 ; i++){
				GTable[7][i] = CLIP(((GTable[7][i] + gCorrect->OffRed) * gCorrect->GRed) >> 8);
			}
			gCorrect->change &= ~0x02;
		}
		if ( gCorrect->change == 0x04) {
		/* Green setting change compute Green Value */
			memcpy (&GTable[8],&GTable[gCorrect->gamma],256);
			for (i =0; i < 256 ; i++){	
				GTable[8][i] = CLIP(((GTable[8][i] + gCorrect->OffGreen) * gCorrect->GGreen) >> 8);	
			}
			gCorrect->change &= ~0x04;
		}
		if ( gCorrect->change == 0x08) {
		/* Blue setting change compute Blue Value */
			memcpy (&GTable[9],&GTable[gCorrect->gamma],256);
			for (i =0; i < 256 ; i++){
				GTable[9][i] = CLIP(((GTable[9][i] + gCorrect->OffBlue) * gCorrect->GBlue) >> 8);
			}
			gCorrect->change &= ~0x08; 
		}
		if ( gCorrect->change == 0x10) {
		/* force_rgb setting change   */
			bgr = gCorrect->force_rgb;
			gCorrect->change &= ~0x10; 
		}
	}
	switch(myframe->cameratype){
		case JPEG: done = jpeg_decode( myframe, bgr);
		//printf ("jpeg decoder asked\n");
		break;
		case YUVY: 
		case YUYV: 
		case YYUV: done = yuv_decode( myframe, bgr);
		break;
		case GBRG: done = bayer_decode( myframe, bgr);
		break;
	default : done = -1;
	break;
	}
return done;	
}

static int 
yuv_decode(struct spca5xx_frame *myframe, int force_rgb )
{
	int cropx1,cropx2,cropy1,cropy2;
	int r_offset, g_offset, b_offset;
	int my , mx; /* scan input surface */
	unsigned char *pic, *pic1; /* output surface */
	__u16 *pix1,*pix2; /* same for 16 bits output */
	unsigned char *buf; /* input surface */
	unsigned char *U, *V; /* chroma output pointer */
	int inuv, inv, pocx; /* offset chroma input */
	int iny,iny1; /* offset luma input */
	int nextinline, nextoutline;
	int u1,v1,rg;
	unsigned char y,y1;
	char u,v;
	int width; 
	int height;
	int softwidth;
	int softheight;
	int method;
	int format;
	int bpp;
	int framesize, frameUsize;
	/* init usefull data */
	pic = myframe->data;
	buf = myframe->tmpbuffer;
	width = myframe->hdrwidth;
	height = myframe->hdrheight;
	softwidth = myframe->width;
	softheight = myframe->height;
	method = myframe->method;
	format = myframe->format;
	cropx1 = myframe->cropx1;
	cropx2 = myframe->cropx2;
	cropy1 = myframe->cropy1;
	cropy2 = myframe->cropy2;
	framesize = softwidth * softheight;
	frameUsize = framesize >> 2;
	/* rgb or bgr like U or V that's the question */
	if (force_rgb) {
		U = pic + framesize;
		V = U + frameUsize;
		r_offset = 2;
		g_offset = 1;
		b_offset = 0;
	} else {
		V = pic + framesize;
		U = V + frameUsize;
		r_offset = 0;
		g_offset = 1;
		b_offset = 2;
	}
	switch (myframe->cameratype) {
		case YUVY: {
			iny = 0;	   /********* iny **********/
			inuv = width;	   /*** inuv **** inv ******/
			nextinline = 3 * width;
			inv = ( nextinline >> 1);
			iny1 = width << 1; /********* iny1 *********/
			}
			break;
		case YUYV: {
			iny = 0;	   /********* iny **********/
			inuv = width;	   /*** inuv **** iny1 *****/
			nextinline = 3 * width;
			iny1 = ( nextinline >> 1);
			inv = iny1 + width ;/*** iny1 **** inv ******/
			}
			break;
		case YYUV: {
			iny = 0;	   /********* iny **********/
			iny1 = width;	   /********* iny1 *********/
			inuv = width << 1; /*** inuv **** inv ******/
			inv = inuv +(width >>1);
			nextinline = 3 * width;
			}
			break;
	default:	{
			iny = 0 ;	   /* make compiler happy */
			iny1 = 0;
			inuv = 0;
			inv = 0 ;
			nextinline = 0;
			}
		 break;
	}
	
	/* Decode to the correct format. */
	switch (format) {
		case VIDEO_PALETTE_RGB565:
			{	bpp = 2;
			/* initialize */
				
				pix1 = (__u16*) pic;
				pix2 = pix1 + softwidth;
				
				
				for ( my =0; my < height; my += 2){
					for ( mx = 0, pocx = 0; mx < width ; mx += 2, pocx++){
					/* test if we need to decode */
					  if ((my >= cropy1)
						    && (my < height - cropy2)
						    && (mx >= cropx1)
						    && (mx < width - cropx2)) {
						    /* yes decode */
						    if ( force_rgb ){
						    	u = buf [inuv + pocx] ;
						    	v = buf [inv + pocx] ;
						    } else {
						    	v = buf [inuv + pocx] ;
						    	u = buf [inv + pocx] ;
						    }
						    v1 = ((v << 10) + (v << 9)) >> 10;
						    rg = ((u << 8) + (u << 7) + (v << 9) + (v << 4)) >> 10;
						    u1 = ((u << 11) + (u << 4)) >> 10;
						    
						   
						    /* top pixel Right */
						    y1 = 128 +buf [iny + mx];		
							*pix1++ = ((GTable[7][CLIP((y1 + v1))] & 0xF8) >> 3 |
								  ((GTable[8][CLIP((y1 - rg))] & 0xFC) << 3) |
								  ((GTable[9][CLIP((y1 + u1))] & 0xF8) << 8)) ;		
						    /* top pixel Left */
						    y1 = 128 +buf [iny + mx +1];
							*pix1++ = ((GTable[7][CLIP((y1 + v1))] & 0xF8) >> 3 |
								  ((GTable[8][CLIP((y1 - rg))] & 0xFC) << 3) |
								  ((GTable[9][CLIP((y1 + u1))] & 0xF8) << 8)) ;		
						    /* bottom pixel Right */
						    y1 = 128 + buf [iny1 + mx];
							*pix2++ = ((GTable[7][CLIP((y1 + v1))] & 0xF8) >> 3 |
								  ((GTable[8][CLIP((y1 - rg))] & 0xFC) << 3) |
								  ((GTable[9][CLIP((y1 + u1))] & 0xF8) << 8)) ;		
						    /* bottom pixel Left */
						    y1 = 128 + buf [iny1 + mx + 1];
							*pix2++ = ((GTable[7][CLIP((y1 + v1))] & 0xF8) >> 3 |
								  ((GTable[8][CLIP((y1 - rg))] & 0xFC) << 3) |
								  ((GTable[9][CLIP((y1 + u1))] & 0xF8) << 8)) ;		
						    
						    
						    
						    
					  } // end test decode
					} // end mx loop
					iny += nextinline;
					inuv += nextinline ;
					inv += nextinline ;
					iny1 += nextinline;
					if (my >= cropy1){
						/* are we in a decode surface move the output pointer */
						pix1 += softwidth ;
						pix2 += softwidth ;
					}
					
				} // end my loop
			
			}
			break;
		case VIDEO_PALETTE_RGB32:
		case VIDEO_PALETTE_RGB24:
			{	bpp = (format == VIDEO_PALETTE_RGB32) ? 4 : 3;
				/* initialize */
				nextoutline  = bpp * softwidth;
				pic1 = pic + nextoutline;
				
				
				for ( my =0; my < height; my += 2){
					for ( mx = 0, pocx = 0; mx < width ; mx += 2, pocx++){
					/* test if we need to decode */
					  if ((my >= cropy1)
						    && (my < height - cropy2)
						    && (mx >= cropx1)
						    && (mx < width - cropx2)) {
						    /* yes decode */
						    v = buf [inuv + pocx] ;
						    u = buf [inv + pocx] ;
						    
						    v1 = ((v << 10) + (v << 9)) >> 10;
						    rg = ((u << 8) + (u << 7) + (v << 9) + (v << 4)) >> 10;
						    u1 = ((u << 11) + (u << 4)) >> 10;
						    
						    y = 128 +buf [iny + mx];
						    /* top pixel Right */
									
									pic[r_offset] = GTable[7][CLIP ((y + v1))];
									pic[g_offset] = GTable[8][CLIP ((y - rg))];
									pic[b_offset] = GTable[9][CLIP ((y + u1))];
									pic += bpp;
						    /* top pixel Left */
						    y = 128 +buf [iny + mx +1];
									pic[r_offset] = GTable[7][CLIP ((y + v1))];
									pic[g_offset] = GTable[8][CLIP ((y - rg))];
									pic[b_offset] = GTable[9][CLIP ((y + u1))];
									pic += bpp;
						    /* bottom pixel Right */
						    y1 = 128 + buf [iny1 + mx];
									pic1[r_offset] = GTable[7][CLIP ((y1 + v1))];
									pic1[g_offset] = GTable[8][CLIP ((y1 - rg))];
									pic1[b_offset] = GTable[9][CLIP ((y1 + u1))];
									pic1 += bpp;
						    /* bottom pixel Left */
						    y1 = 128 + buf [iny1 + mx + 1];
									pic1[r_offset] = GTable[7][CLIP ((y1 + v1))];
									pic1[g_offset] = GTable[8][CLIP ((y1 - rg))];
									pic1[b_offset] = GTable[9][CLIP ((y1 + u1))];
									pic1 += bpp;
						    
						    
						    
						    
					  } // end test decode
					} // end mx loop
					iny += nextinline;
					inuv += nextinline ;
					inv += nextinline ;
					iny1 += nextinline;
					if (my >= cropy1){
						/* are we in a decode surface move the output pointer */
						pic += nextoutline;
						pic1 += nextoutline;
					}
					
				} // end my loop
			}
			break;
		case VIDEO_PALETTE_YUV420P:
			{
				/* initialize */
				pic1 = pic + softwidth;
				
				for ( my =0; my < height; my += 2){
					for ( mx = 0, pocx=0; mx < width ; mx +=2, pocx++){
					/* test if we need to decode */
					  if ((my >= cropy1)
						    && (my < height - cropy2)
						    && (mx >= cropx1)
						    && (mx < width - cropx2)) {
						    /* yes decode */
						    *V++ = 128 + buf [inuv + pocx];
						    *U++ = 128 + buf [inv + pocx] ;
						    *pic++ = 128 +buf [iny + mx];
						    *pic++ = 128 +buf [iny + mx+1];
						    *pic1++ = 128 + buf [iny1 + mx];
						    *pic1++ = 128 + buf [iny1 + mx +1];
						    
					  } // end test decode
					} // end mx loop
					iny += nextinline;
					inuv += nextinline;
					inv += nextinline;
					iny1 += nextinline;
				
					if (my >= cropy1){
						/* are we in a decode surface move the output pointer */
						pic += softwidth;
						pic1 += softwidth;
					}
					
				} // end my loop
				
				
			}
			break;
		default:
			break;
	}// end case
	return 0;
}
/*
 *    linux/drivers/video/fbcon-jpegdec.c - a tiny jpeg decoder.
 *      
 *      (w) August 2001 by Michael Schroeder, <mls@suse.de>
 *
 *    I severly gutted this beast and hardcoded it to the palette and subset
 *    of jpeg needed for the spca50x driver. Also converted it from K&R style
 *    C to a more modern form ;). Michael can't be blamed for what is left.
 *    All nice features are his, all bugs are mine. - till
 *
 *    Change color space converter for YUVP and RGB -  
 *    Rework the IDCT implementation for best speed 
 *    For more details about idct look at :
 *    http://rnvs.informatik.tu-chemnitz.de/~jan/MPEG/HTML/IDCT.html 
 *    12/12/2003 mxhaard@magic.fr       
 */
static int
jpeg_decode (struct spca5xx_frame *myframe, int force_rgb)
{
	int mcusx, mcusy, mx, my;
	int dcts[6 * 64 + 16];
	int out[64 * 6];
	int max[6];
	int i;
	int bpp;
	int framesize, frameUsize;
	int k, j;
	int nextline, nextuv, nextblk, nextnewline;
	unsigned char *pic0, *pic1, *outv, *outu;
	__u16 *pix1,*pix2;
	int picy, picx, pocx, pocy;
	unsigned char *U, *V;
	int *outy, *inv, *inu;
	int outy1, outy2;
	int v, u, y1, v1, u1, u2;
	int r_offset, g_offset, b_offset;
	int cropx1,cropx2,cropy1,cropy2;
	unsigned char *pic;
	unsigned char *buf;
	int width; 
	int height;
	int softwidth;
	int softheight;
	int method;
	int format;
	/* init usefull data */
	pic = myframe->data;
	buf = myframe->tmpbuffer;
	width = myframe->hdrwidth;
	height = myframe->hdrheight;
	softwidth = myframe->width;
	softheight = myframe->height;
	method = myframe->method;
	format = myframe->format;
	cropx1 = myframe->cropx1;
	cropx2 = myframe->cropx2;
	cropy1 = myframe->cropy1;
	cropy2 = myframe->cropy2;
	
	if ((height & 15) || (width & 15))
		return 1;
	if (width < softwidth || height < softheight)
		return 1;
	//printf ("jpeg decoder process\n");
	mcusx = width >> 4;
	mcusy = height >> 4;
	framesize = softwidth * softheight;
	frameUsize = framesize >> 2;
	jpeg_reset_input_context (buf);

	/* for each component. Reset dc values. */
	for (i = 0; i < 3; i++)
		dscans[i].dc = 0;
	/* rgb or bgr like U or V that's the question */
	if (force_rgb) {
		U = pic + framesize;
		V = U + frameUsize;
		r_offset = 2;
		g_offset = 1;
		b_offset = 0;
	} else {
		V = pic + framesize;
		U = V + frameUsize;
		r_offset = 0;
		g_offset = 1;
		b_offset = 2;
	}

	/* Decode to the correct format. */
	switch (format) {
		case VIDEO_PALETTE_RGB565:
			{	bpp = 2;
				nextline = ((softwidth << 1) - 16);// *bpp;
				nextblk = bpp * (softwidth << 4);
				nextnewline = softwidth ; // *bpp;
				for (my = 0, picy = 0; my < mcusy; my++) {
					for (mx = 0, picx = 0; mx < mcusx; mx++) {

						decode_mcus (&in, dcts, 6,
							     dscans, max);
						if ((my >= cropy1)
						    && (my < mcusy - cropy2)
						    && (mx >= cropx1)
						    && (mx < mcusx - cropx2)) {
							idct (dcts, out,
							      dquant[0],
							      IFIX (128.5),
							      max[0]);
							idct (dcts + 64,
							      out + 64,
							      dquant[0],
							      IFIX (128.5),
							      max[1]);
							idct (dcts + 128,
							      out + 128,
							      dquant[0],
							      IFIX (128.5),
							      max[2]);
							idct (dcts + 192,
							      out + 192,
							      dquant[0],
							      IFIX (128.5),
							      max[3]);
							idct (dcts + 256,
							      out + 256,
							      dquant[1],
							      IFIX (0.5),
							      max[4]);
							idct (dcts + 320,
							      out + 320,
							      dquant[2],
							      IFIX (0.5),
							      max[5]);
							pix1 = (__u16 *)(pic + picx + picy);
							pix2 = pix1 + nextnewline;
							outy = out;
							outy1 = 0;
							outy2 = 8;
							inv = out + 64 * 4;
							inu = out + 64 * 5;
							for (j = 0; j < 8; j++) {
								for (k = 0;
								     k < 8;
								     k++) {
									if (k ==
									    4) {
										outy1 += 56;
										outy2 += 56;
									}
									/* outup 4 pixels */
									/* get the UV colors need to change UV order for force rgb? */
									if ( force_rgb){
										u = *inv++;
										v = *inu++;
									} else {
										v = *inv++;
										u = *inu++;
									}
									/* MX color space why not? */
									v1 = ((v << 10) + (v << 9)) >> 10;
									u1 = ((u << 8) + (u << 7) + (v << 9) + (v << 4)) >> 10;
									u2 = ((u << 11) + (u << 4)) >> 10;
									/* top pixel Right */
									y1 = outy[outy1++];
									*pix1++ = ((GTable[7][CLIP((y1 + v1))] & 0xF8) >> 3 |
									 	  ((GTable[8][CLIP((y1 - u1))] & 0xFC) << 3) |
									  	  ((GTable[9][CLIP((y1 + u2))] & 0xF8) << 8)) ;
									/* top pixel Left */
									y1 = outy[outy1++];	  
									*pix1++ = ((GTable[7][CLIP((y1 + v1))] & 0xF8) >> 3 | 
										  ((GTable[8][CLIP((y1 - u1))] & 0xFC) << 3) | 
										  ((GTable[9][CLIP((y1 + u2))] & 0xF8) << 8)) ;
									
									/* bottom pixel Right */
									y1 = outy[outy2++];
									*pix2++ = ((GTable[7][CLIP((y1 + v1))] & 0xF8) >> 3 |
									          ((GTable[8][CLIP((y1 - u1))] & 0xFC) << 3) | 
										  ((GTable[9][CLIP((y1 + u2))] & 0xF8) << 8)) ;
									/* bottom pixel Left */
									y1 = outy[outy2++];	  
									*pix2++ = ((GTable[7][CLIP((y1 + v1))] & 0xF8) >> 3| 
									          ((GTable[8][CLIP((y1 - u1))] & 0xFC) << 3) | 
										  ((GTable[9][CLIP((y1 + u2))] & 0xF8) << 8)) ;	

								}
								if (j == 3) {
									outy = out + 128;
								} else {
									outy += 16;
								}
								outy1 = 0;
								outy2 = 8;
								pix1 += nextline;
								pix2 += nextline;

							}
							picx += 16 * bpp;
						} 
					}
					if (my >= cropy1)
						picy += nextblk;

				}
			
			}
			break;
		case VIDEO_PALETTE_RGB32:
		case VIDEO_PALETTE_RGB24:
			{	//printf ("jpeg decoder process rgb24 \n");
				bpp = (format == VIDEO_PALETTE_RGB32) ? 4 : 3;
				nextline = bpp * ((softwidth << 1) - 16);
				nextblk = bpp * (softwidth << 4);
				nextnewline = bpp * softwidth;
				for (my = 0, picy = 0; my < mcusy; my++) {
					for (mx = 0, picx = 0; mx < mcusx; mx++) {

						decode_mcus (&in, dcts, 6,
							     dscans, max);
						if ((my >= cropy1)
						    && (my < mcusy - cropy2)
						    && (mx >= cropx1)
						    && (mx < mcusx - cropx2)) {
							idct (dcts, out,
							      dquant[0],
							      IFIX (128.5),
							      max[0]);
							idct (dcts + 64,
							      out + 64,
							      dquant[0],
							      IFIX (128.5),
							      max[1]);
							idct (dcts + 128,
							      out + 128,
							      dquant[0],
							      IFIX (128.5),
							      max[2]);
							idct (dcts + 192,
							      out + 192,
							      dquant[0],
							      IFIX (128.5),
							      max[3]);
							idct (dcts + 256,
							      out + 256,
							      dquant[1],
							      IFIX (0.5),
							      max[4]);
							idct (dcts + 320,
							      out + 320,
							      dquant[2],
							      IFIX (0.5),
							      max[5]);
							pic0 = pic + picx +
								picy;
							pic1 = pic0 + nextnewline;
							outy = out;
							outy1 = 0;
							outy2 = 8;
							inv = out + 64 * 4;
							inu = out + 64 * 5;
							for (j = 0; j < 8; j++) {
								for (k = 0;
								     k < 8;
								     k++) {
									if (k ==
									    4) {
										outy1 += 56;
										outy2 += 56;
									}
									/* outup 4 pixels */
									/* get the UV colors need to change UV order for force rgb? */
									v = *inv++;
									u = *inu++;
									/* MX color space why not? */
									v1 = ((v << 10) + (v << 9)) >> 10;
									u1 = ((u << 8) + (u << 7) + (v << 9) + (v << 4)) >> 10;
									u2 = ((u << 11) + (u << 4)) >> 10;
									/* top pixel Right */
									y1 = outy[outy1++];
									pic0[r_offset] = GTable[7][CLIP ((y1 + v1))];
									pic0[g_offset] = GTable[8][CLIP ((y1 - u1))];
									pic0[b_offset] = GTable[9][CLIP ((y1 + u2))];
									pic0 += bpp;
									/* top pixel Left */
									y1 = outy[outy1++];
									pic0[r_offset] = GTable[7][CLIP ((y1 + v1))];
									pic0[g_offset] = GTable[8][CLIP ((y1 - u1))];
									pic0[b_offset] = GTable[9][CLIP ((y1 + u2))];
									pic0 += bpp;
									/* bottom pixel Right */
									y1 = outy[outy2++];
									pic1[r_offset] = GTable[7][CLIP ((y1 + v1))];
									pic1[g_offset] = GTable[8][CLIP ((y1 - u1))];
									pic1[b_offset] = GTable[9][CLIP ((y1 + u2))];
									pic1 += bpp;
									/* bottom pixel Left */
									y1 = outy[outy2++];
									pic1[r_offset] = GTable[7][CLIP ((y1 + v1))];
									pic1[g_offset] = GTable[8][CLIP ((y1 - u1))];
									pic1[b_offset] = GTable[9][CLIP ((y1 + u2))];
									pic1 += bpp;

								}
								if (j == 3) {
									outy = out + 128;
								} else {
									outy += 16;
								}
								outy1 = 0;
								outy2 = 8;
								pic0 += nextline;
								pic1 += nextline;

							}
							picx += 16 * bpp;
						} 
					}
					if (my >= cropy1)
						picy += nextblk;

				}
				//printf ("finish jpeg decoder rgb24 \n");
			}
			break;
		case VIDEO_PALETTE_YUV420P:
			{
				nextline = (softwidth << 1) - 16;
				nextuv = (softwidth >> 1) - 8;
				nextblk = softwidth << 4;
				nextnewline = softwidth << 2;
				for (my = 0, picy = 0, pocy = 0; my < mcusy;
				     my++) {
					for (mx = 0, picx = 0, pocx = 0;
					     mx < mcusx; mx++) {
						decode_mcus (&in, dcts, 6,
							     dscans, max);
						if ((my >= cropy1)
						    && (my < mcusy - cropy2)
						    && (mx >= cropx1)
						    && (mx < mcusx - cropx2)) {
							idct (dcts, out,
							      dquant[0],
							      IFIX (128.5),
							      max[0]);
							idct (dcts + 64,
							      out + 64,
							      dquant[0],
							      IFIX (128.5),
							      max[1]);
							idct (dcts + 128,
							      out + 128,
							      dquant[0],
							      IFIX (128.5),
							      max[2]);
							idct (dcts + 192,
							      out + 192,
							      dquant[0],
							      IFIX (128.5),
							      max[3]);
							idct (dcts + 256,
							      out + 256,
							      dquant[1],
							      IFIX (0.5),
							      max[4]);
							idct (dcts + 320,
							      out + 320,
							      dquant[2],
							      IFIX (0.5),
							      max[5]);

							pic0 = pic + picx +
								picy;
							pic1 = pic0 + softwidth;
							outv = V + (pocx +
								    pocy);
							outu = U + (pocx +
								    pocy);
							outy = out;
							outy1 = 0;
							outy2 = 8;
							inv = out + 64 * 4;
							inu = out + 64 * 5;
							for (j = 0; j < 8; j++) {
								for (k = 0;
								     k < 8;
								     k++) {
									if (k ==
									    4) {
										outy1 += 56;
										outy2 += 56;
									}
									/* outup 4 pixels */

									*pic0++ = outy[outy1++];
									*pic0++ = outy[outy1++];
									*pic1++ = outy[outy2++];
									*pic1++ = outy[outy2++];
									*outv++ = 128 + *inv++;
									*outu++ = 128 + *inu++;
								}
								if (j == 3) {
									outy = out + 128;
								} else {
									outy += 16;
								}
								outy1 = 0;
								outy2 = 8;
								pic0 += nextline;
								pic1 += nextline;
								outv += nextuv;
								outu += nextuv;
							}
							picx += 16;
							pocx += 8;
						} 
					}
					if (my >= cropy1) {
						picy += nextblk;
						pocy += nextnewline;
					}
				}
			}
			break;
		default:
			break;
	}			// end case
	return 0;
}

static int 
bayer_decode(struct spca5xx_frame *myframe, int force_rgb )
{
	int cropx1,cropx2,cropy1,cropy2;
	int r_offset, g_offset, b_offset;
	int my , mx; /* scan input surface */
	unsigned char *pic, *pic1; /* output surface */
	__u16 *pix1,*pix2; /* same for 16 bits output */
	unsigned char *buf; /* input surface */
	unsigned char *U, *V; /* chroma output pointer */
	unsigned char inr, ing1, ing2, inb, ing; /* srgb input */
	int inl,inl1; /* offset line input */
	int nextinline, nextoutline;
	unsigned char r,b,y1,y2,y3,y4;
	int u,v;
	int width; 
	int height;
	int softwidth;
	int softheight;
	int method;
	int format;
	int bpp;
	int framesize, frameUsize;
	/* init usefull data */
	pic = myframe->data;
	buf = myframe->tmpbuffer;
	width = myframe->hdrwidth;
	height = myframe->hdrheight;
	softwidth = myframe->width;
	softheight = myframe->height;
	method = myframe->method;
	format = myframe->format;
	cropx1 = myframe->cropx1;
	cropx2 = myframe->cropx2;
	cropy1 = myframe->cropy1;
	cropy2 = myframe->cropy2;
	framesize = softwidth * softheight;
	frameUsize = framesize >> 2;
	/* rgb or bgr like U or V that's the question */
	if (force_rgb) {
		U = pic + framesize;
		V = U + frameUsize;		
		r_offset = 2;
		g_offset = 1;
		b_offset = 0;
	} else {
		V = pic + framesize;
		U = V + frameUsize;		
		r_offset = 0;
		g_offset = 1;
		b_offset = 2;
	}
	/* initialize input pointer */
	inl = 0;
	inl1 = width ;
	nextinline = width << 1;
	/* Decode to the correct format. */
	switch (format) {
		case VIDEO_PALETTE_RGB565:
			{	bpp = 2;
			/* initialize */				
				pix1 = (__u16*) pic;
				pix2 = pix1 + softwidth;				
				for ( my =0; my < height; my += 2){
					for ( mx = 0 ; mx < width ; mx += 2 ){
					/* test if we need to decode */
					  if ((my >= cropy1)
						    && (my < height - cropy2)
						    && (mx >= cropx1)
						    && (mx < width - cropx2)) {
						    /* yes decode */
						    ing1 = buf [inl + mx] ;
						if(force_rgb){						    
						    inr = buf [inl + 1 + mx] ;
						    inb = buf [inl1 + mx];
						    } else {
						    inb = buf [inl + 1 + mx] ;
						    inr = buf [inl1 + mx];
						    }
						    ing2 = buf [inl1 + 1 + mx];
						    ing = (ing1 + ing2) >> 1;
						   
						    /* top pixel Right */
						    		
							*pix1++ = ((GTable[7][inr] & 0xF8) >> 3 |
								  ((GTable[8][ing1] & 0xFC) << 3) |
								  ((GTable[9][inb] & 0xF8) << 8)) ;		
						    /* top pixel Left */
						    
							*pix1++ = ((GTable[7][inr] & 0xF8) >> 3 |
								  ((GTable[8][ing] & 0xFC) << 3) |
								  ((GTable[9][inb] & 0xF8) << 8)) ;		
						    /* bottom pixel Right */
						   
							*pix2++ = ((GTable[7][inr] & 0xF8) >> 3 |
								  ((GTable[8][ing] & 0xFC) << 3) |
								  ((GTable[9][inb] & 0xF8) << 8)) ;		
						    /* bottom pixel Left */
						    
							*pix2++ = ((GTable[7][inr] & 0xF8) >> 3 |
								  ((GTable[8][ing2] & 0xFC) << 3) |
								  ((GTable[9][inb] & 0xF8) << 8)) ;								    
						    
					  } // end test decode
					} // end mx loop
					inl += nextinline;
					inl1 += nextinline ;
					
					if (my >= cropy1){
						/* are we in a decode surface move the output pointer */
						pix1 += (softwidth);
						pix2 += (softwidth);
					}
					
				} // end my loop
			
			}
			break;
		case VIDEO_PALETTE_RGB32:
		case VIDEO_PALETTE_RGB24:
			{	bpp = (format == VIDEO_PALETTE_RGB32) ? 4 : 3;
				/* initialize */
				nextoutline  = bpp * softwidth;
				pic1 = pic + nextoutline;				
				for ( my =0; my < height; my += 2){
					for ( mx = 0 ; mx < width ; mx += 2 ){
					/* test if we need to decode */
					  if ((my >= cropy1)
						    && (my < height - cropy2)
						    && (mx >= cropx1)
						    && (mx < width - cropx2)) {
						    
						    
						    /* yes decode GBRG */
						    ing1 = buf [inl + mx] ;
						    inb = buf [inl+ 1 + mx] ;
						    inr = buf [inl1 + mx];
						    ing2 = buf [inl1 +1 + mx];
						    ing = (ing1 + ing2) >> 1;
						   
						   /* yes decode RGGB 
						    inr = buf [inl + mx] ;
						    ing1 = buf [inl+ 1 + mx] ;
						    ing2 = buf [inl1 + mx];
						    inb = buf [inl1 +1 + mx];
						    ing = (ing1 + ing2) >> 1;
						   */
						   /* yes decode GGBR 
						    ing1 = buf [inl + mx] ;
						    ing2 = buf [inl+ 1 + mx] ;
						    inb = buf [inl1 + mx];
						    inr = buf [inl1 +1 + mx];
						    ing = (ing1 + ing2) >> 1;
						   */
						   /* yes decode BRGG 
						    inr = buf [inl + mx] ;
						    inb = buf [inl+ 1 + mx] ;
						    ing1 = buf [inl1 + mx];
						    ing2 = buf [inl1 +1 + mx];
						    ing = (ing1 + ing2) >> 1;
						   */				    
						    /* top pixel Right */
									
									pic[r_offset] = GTable[7][inr];
									pic[g_offset] = GTable[8][ing1];
									pic[b_offset] = GTable[9][inb];
									pic += bpp;
						    /* top pixel Left */
						   
									pic[r_offset] = GTable[7][inr];
									pic[g_offset] = GTable[8][ing];
									pic[b_offset] = GTable[9][inb];
									pic += bpp;
						    /* bottom pixel Right */
						    
									pic1[r_offset] = GTable[7][inr];
									pic1[g_offset] = GTable[8][ing];
									pic1[b_offset] = GTable[9][inb];
									pic1 += bpp;
						    /* bottom pixel Left */
						    
									pic1[r_offset] = GTable[7][inr];
									pic1[g_offset] = GTable[8][ing2];
									pic1[b_offset] = GTable[9][inb];
									pic1 += bpp;						    
						    
					  } // end test decode
					} // end mx loop
					inl += nextinline;
					inl1 += nextinline ;
					
					if (my >= cropy1){
						/* are we in a decode surface move the output pointer */
						pic += (nextoutline);
						pic1 += (nextoutline);
					}
					
				} // end my loop
			}
			break;
		case VIDEO_PALETTE_YUV420P:
			{ /* Not yet implemented */
				nextoutline  = softwidth;
				pic1 = pic + nextoutline;

				for ( my =0; my < height; my += 2){
					for ( mx = 0; mx < width ; mx +=2 ){
					/* test if we need to decode */
					  if ((my >= cropy1)
						    && (my < height - cropy2)
						    && (mx >= cropx1)
						    && (mx < width - cropx2)) {
						    /* yes decode */
						    ing1 = buf [inl + mx] ;
						    b = buf [inl + 1 + mx] ;
						    r = buf [inl1 + mx];
						    ing2 = buf [inl1 + 1 + mx];
						    ing = (ing1 + ing2) >> 1;

							inr = ((r << 8)-( r << 4) -( r << 3)) >> 10;
							inb = (( b << 7) >>10);
							ing1 = ((ing1 <<9)+(ing1 << 7)+(ing1 << 5)) >> 10;
							ing2 = ((ing2 <<9)+(ing2 << 7)+(ing2 << 5)) >> 10;
							ing = ((ing <<9)+(ing << 7)+(ing << 5)) >> 10;

						    /* top pixel Right */
							y1= CLIP((inr+ing1+inb));
							*pic++ = y1;
						    /* top pixel Left */
						   	y2= CLIP((inr+ing+inb));
							*pic++ = y2;
						    /* bottom pixel Right */
							y3= CLIP((inr+ing+inb));
							*pic1++ = y3;
						    /* bottom pixel Left */
							y4= CLIP((inr+ing2+inb));
							*pic1++ = y4;
						/* U V plane */
							v = r - ((y1+y2+y3+y4) >> 2);
							u = ((v << 9) + (v <<7) + (v << 5)) >> 10;
							v = (b - ((y1+y2+y3+y4) >> 2)) >> 1;
							// *U++ = 128 + u;
							// *V++ = 128 + v;

							*U++ = 128 + v;
							*V++ = 128 + u;

					  } // end test decode
					} // end mx loop
					inl += nextinline;
					inl1 += nextinline ;

					if (my >= cropy1){
						/* are we in a decode surface move the output pointer */
						pic += softwidth ;
						pic1 += softwidth ;
					}

				} // end my loop
	
					
				
				
				
			}
			break;
		default:
			break;
	}// end case
	return 0;
} // end bayer_decode
/****************************************************************/
/**************       huffman decoder             ***************/
/****************************************************************/

static int fillbits (struct in *, int, unsigned int);
static int dec_rec2 (struct in *, struct dec_hufftbl *, int *, int, int);

static int
fillbits (struct in *in, int le, unsigned int bi)
{
	int b;

	if (in->marker) {
		if (le <= 16)
			in->bits = bi << 16, le += 16;
		return le;
	}
	while (le <= 24) {
		b = *in->p++;
		bi = bi << 8 | b;
		le += 8;
	}
	in->bits = bi;		/* tmp... 2 return values needed */
	return le;
}

#define LEBI_GET(in)	(le = in->left, bi = in->bits)
#define LEBI_PUT(in)	(in->left = le, in->bits = bi)

#define GETBITS(in, n) (					\
  (le < (n) ? le = fillbits(in, le, bi), bi = in->bits : 0),	\
  (le -= (n)),							\
  bi >> le & ((1 << (n)) - 1)					\
)

#define UNGETBITS(in, n) (	\
  le += (n)			\
)


static int
dec_rec2 (struct in *in, struct dec_hufftbl *hu, int *runp, int c, int i)
{
	int le, bi;

	le = in->left;
	bi = in->bits;
	if (i) {
		UNGETBITS (in, i & 127);
		*runp = i >> 8 & 15;
		i >>= 16;
	} else {
		for (i = DECBITS;
		     (c = ((c << 1) | GETBITS (in, 1))) >= (hu->maxcode[i]);
		     i++) ;
		if (i >= 16) {
			in->marker = M_BADHUFF;
			return 0;
		}
		i = hu->vals[hu->valptr[i] + c - hu->maxcode[i - 1] * 2];
		*runp = i >> 4;
		i &= 15;
	}
	if (i == 0) {		/* sigh, 0xf0 is 11 bit */
		LEBI_PUT (in);
		return 0;
	}
	/* receive part */
	c = GETBITS (in, i);
	if (c < (1 << (i - 1)))
		c += (-1 << i) + 1;
	LEBI_PUT (in);
	return c;
}

#define DEC_REC(in, hu, r, i)	 (	\
  r = GETBITS(in, DECBITS),		\
  i = hu->llvals[r],			\
  i & 128 ?				\
    (					\
      UNGETBITS(in, i & 127),		\
      r = i >> 8 & 15,			\
      i >> 16				\
    )					\
  :					\
    (					\
      LEBI_PUT(in),			\
      i = dec_rec2(in, hu, &r, r, i),	\
      LEBI_GET(in),			\
      i					\
    )					\
)

inline static void
decode_mcus (struct in *in, int *dct, int n, struct scan *sc, int *maxp)
{
	struct dec_hufftbl *hu;
	int i, r, t;
	int le, bi;

	memset (dct, 0, n * 64 * sizeof (*dct));
	le = in->left;
	bi = in->bits;

	while (n-- > 0) {
		hu = sc->hudc.dhuff;
		*dct++ = (sc->dc += DEC_REC (in, hu, r, t));

		hu = sc->huac.dhuff;
		i = 63;
		while (i > 0) {
			t = DEC_REC (in, hu, r, t);
			if (t == 0 && r == 0) {
				dct += i;
				break;
			}
			dct += r;
			*dct++ = t;
			i -= r + 1;
		}
		*maxp++ = 64 - i;
		if (n == sc->next)
			sc++;
	}
	LEBI_PUT (in);
}

static void
dec_makehuff (struct dec_hufftbl *hu, int *hufflen, unsigned char *huffvals)
{
	int code, k, i, j, d, x, c, v;

	for (i = 0; i < (1 << DECBITS); i++)
		hu->llvals[i] = 0;

/*
 * llvals layout:
 *
 * value v already known, run r, backup u bits:
 *  vvvvvvvvvvvvvvvv 0000 rrrr 1 uuuuuuu
 * value unknown, size b bits, run r, backup u bits:
 *  000000000000bbbb 0000 rrrr 0 uuuuuuu
 * value and size unknown:
 *  0000000000000000 0000 0000 0 0000000
 */
	code = 0;
	k = 0;
	for (i = 0; i < 16; i++, code <<= 1) {	/* sizes */
		hu->valptr[i] = k;
		for (j = 0; j < hufflen[i]; j++) {
			hu->vals[k] = *huffvals++;
			if (i < DECBITS) {
				c = code << (DECBITS - 1 - i);
				v = hu->vals[k] & 0x0f;	/* size */
				for (d = 1 << (DECBITS - 1 - i); --d >= 0;) {
					if (v + i < DECBITS) {	/* both fit in table */
						x = d >> (DECBITS - 1 - v - i);
						if (v && x < (1 << (v - 1)))
							x += (-1 << v) + 1;
						x = x << 16 | (hu->
							       vals[k] & 0xf0)
							<< 4 | (DECBITS -
								(i + 1 +
								 v)) | 128;
					} else
						x = v << 16 | (hu->
							       vals[k] & 0xf0)
							<< 4 | (DECBITS -
								(i + 1));
					hu->llvals[c | d] = x;
				}
			}
			code++;
			k++;
		}
		hu->maxcode[i] = code;
	}
	hu->maxcode[16] = 0x20000;	/* always terminate decode */
}

/****************************************************************/
/**************             idct                  ***************/
/****************************************************************/


#define S22 ((long)IFIX(2 * 0.382683432))
#define C22 ((long)IFIX(2 * 0.923879532))
#define IC4 ((long)IFIX(1 / 0.707106781))

static unsigned char zig2[64] = {
	0, 2, 3, 9, 10, 20, 21, 35,
	14, 16, 25, 31, 39, 46, 50, 57,
	5, 7, 12, 18, 23, 33, 37, 48,
	27, 29, 41, 44, 52, 55, 59, 62,
	15, 26, 30, 40, 45, 51, 56, 58,
	1, 4, 8, 11, 19, 22, 34, 36,
	28, 42, 43, 53, 54, 60, 61, 63,
	6, 13, 17, 24, 32, 38, 47, 49
};

inline static void
idct (int *in, int *out, int *quant, long off, int max)
{
	long t0, t1, t2, t3, t4, t5, t6, t7;	// t ;
	long tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6;
	long tmp[64], *tmpp;
	int i, j, te;
	unsigned char *zig2p;

	t0 = off;
	if (max == 1) {
		t0 += in[0] * quant[0];
		for (i = 0; i < 64; i++)
			out[i] = ITOINT (t0);
		return;
	}
	zig2p = zig2;
	tmpp = tmp;
	for (i = 0; i < 8; i++) {
		j = *zig2p++;
		t0 += in[j] * (long) quant[j];
		j = *zig2p++;
		t5 = in[j] * (long) quant[j];
		j = *zig2p++;
		t2 = in[j] * (long) quant[j];
		j = *zig2p++;
		t7 = in[j] * (long) quant[j];
		j = *zig2p++;
		t1 = in[j] * (long) quant[j];
		j = *zig2p++;
		t4 = in[j] * (long) quant[j];
		j = *zig2p++;
		t3 = in[j] * (long) quant[j];
		j = *zig2p++;
		t6 = in[j] * (long) quant[j];


		if ((t1 | t2 | t3 | t4 | t5 | t6 | t7) == 0) {

			tmpp[0 * 8] = t0;
			tmpp[1 * 8] = t0;
			tmpp[2 * 8] = t0;
			tmpp[3 * 8] = t0;
			tmpp[4 * 8] = t0;
			tmpp[5 * 8] = t0;
			tmpp[6 * 8] = t0;
			tmpp[7 * 8] = t0;

			tmpp++;
			t0 = 0;
			continue;
		}
		//IDCT;
		tmp0 = t0 + t1;
		t1 = t0 - t1;
		tmp2 = t2 - t3;
		t3 = t2 + t3;
		tmp2 = IMULT (tmp2, IC4) - t3;
		tmp3 = tmp0 + t3;
		t3 = tmp0 - t3;
		tmp1 = t1 + tmp2;
		tmp2 = t1 - tmp2;
		tmp4 = t4 - t7;
		t7 = t4 + t7;
		tmp5 = t5 + t6;
		t6 = t5 - t6;
		tmp6 = tmp5 - t7;
		t7 = tmp5 + t7;
		tmp5 = IMULT (tmp6, IC4);
		tmp6 = IMULT ((tmp4 + t6), S22);
		tmp4 = IMULT (tmp4, (C22 - S22)) + tmp6;
		t6 = IMULT (t6, (C22 + S22)) - tmp6;
		t6 = t6 - t7;
		t5 = tmp5 - t6;
		t4 = tmp4 - t5;

		tmpp[0 * 8] = tmp3 + t7;	//t0;
		tmpp[1 * 8] = tmp1 + t6;	//t1;
		tmpp[2 * 8] = tmp2 + t5;	//t2;
		tmpp[3 * 8] = t3 + t4;	//t3;
		tmpp[4 * 8] = t3 - t4;	//t4;
		tmpp[5 * 8] = tmp2 - t5;	//t5;
		tmpp[6 * 8] = tmp1 - t6;	//t6;
		tmpp[7 * 8] = tmp3 - t7;	//t7;
		tmpp++;
		t0 = 0;
	}
	for (i = 0, j = 0; i < 8; i++) {
		t0 = tmp[j + 0];
		t1 = tmp[j + 1];
		t2 = tmp[j + 2];
		t3 = tmp[j + 3];
		t4 = tmp[j + 4];
		t5 = tmp[j + 5];
		t6 = tmp[j + 6];
		t7 = tmp[j + 7];
		if ((t1 | t2 | t3 | t4 | t5 | t6 | t7) == 0) {
			te = ITOINT (t0);
			out[j + 0] = te;
			out[j + 1] = te;
			out[j + 2] = te;
			out[j + 3] = te;
			out[j + 4] = te;
			out[j + 5] = te;
			out[j + 6] = te;
			out[j + 7] = te;
			j += 8;
			continue;
		}
		//IDCT;
		tmp0 = t0 + t1;
		t1 = t0 - t1;
		tmp2 = t2 - t3;
		t3 = t2 + t3;
		tmp2 = IMULT (tmp2, IC4) - t3;
		tmp3 = tmp0 + t3;
		t3 = tmp0 - t3;
		tmp1 = t1 + tmp2;
		tmp2 = t1 - tmp2;
		tmp4 = t4 - t7;
		t7 = t4 + t7;
		tmp5 = t5 + t6;
		t6 = t5 - t6;
		tmp6 = tmp5 - t7;
		t7 = tmp5 + t7;
		tmp5 = IMULT (tmp6, IC4);
		tmp6 = IMULT ((tmp4 + t6), S22);
		tmp4 = IMULT (tmp4, (C22 - S22)) + tmp6;
		t6 = IMULT (t6, (C22 + S22)) - tmp6;
		t6 = t6 - t7;
		t5 = tmp5 - t6;
		t4 = tmp4 - t5;

		out[j + 0] = ITOINT (tmp3 + t7);
		out[j + 1] = ITOINT (tmp1 + t6);
		out[j + 2] = ITOINT (tmp2 + t5);
		out[j + 3] = ITOINT (t3 + t4);
		out[j + 4] = ITOINT (t3 - t4);
		out[j + 5] = ITOINT (tmp2 - t5);
		out[j + 6] = ITOINT (tmp1 - t6);
		out[j + 7] = ITOINT (tmp3 - t7);
		j += 8;
	}

}

static unsigned char zig[64] = {
	0, 1, 5, 6, 14, 15, 27, 28,
	2, 4, 7, 13, 16, 26, 29, 42,
	3, 8, 12, 17, 25, 30, 41, 43,
	9, 11, 18, 24, 31, 40, 44, 53,
	10, 19, 23, 32, 39, 45, 52, 54,
	20, 22, 33, 38, 46, 51, 55, 60,
	21, 34, 37, 47, 50, 56, 59, 61,
	35, 36, 48, 49, 57, 58, 62, 63
};

static int aaidct[8] = {
	IFIX (0.3535533906), IFIX (0.4903926402),
	IFIX (0.4619397663), IFIX (0.4157348062),
	IFIX (0.3535533906), IFIX (0.2777851165),
	IFIX (0.1913417162), IFIX (0.0975451610)
};


inline static void
idctqtab (unsigned char *qin, int *qout)
{
	int i, j;

	for (i = 0; i < 8; i++)
		for (j = 0; j < 8; j++)
			qout[zig[i * 8 + j]] = qin[zig[i * 8 + j]] *
				IMULT (aaidct[i], aaidct[j]);
}

inline static void
scaleidctqtab (int *q, int sc)
{
	int i;

	for (i = 0; i < 64; i++)
		q[i] = IMULT (q[i], sc);
}

/* Reduce to the necessary minimum. FIXME */
void
init_jpeg_decoder ()
{
	unsigned int i, j, k, l;
	unsigned int qIndex = 2;
	int tc, th, tt, tac, tdc;
	unsigned char *ptr;

	/* set up a quantization table */
	for (i = 0; i < 2; i++) {
		for (j = 0; j < 64; j++) {
			quant[i][j] = GsmartQTable[qIndex * 2 + i][j];
		}
	}

	/* set up the huffman table */
	ptr = (unsigned char *) GsmartJPEGHuffmanTable;
	l = GSMART_JPG_HUFFMAN_TABLE_LENGTH;
	while (l > 0) {
		int hufflen[16];
		unsigned char huffvals[256];

		tc = *ptr++;
		th = tc & 15;
		tc >>= 4;
		tt = tc * 2 + th;
		if (tc > 1 || th > 1) {
			//printf("died whilst setting up huffman table.\n");
			//abort();
		}
		for (i = 0; i < 16; i++)
			hufflen[i] = *ptr++;
		l -= 1 + 16;
		k = 0;
		for (i = 0; i < 16; i++) {
			for (j = 0; j < (unsigned int) hufflen[i]; j++)
				huffvals[k++] = *ptr++;
			l -= hufflen[i];
		}
		dec_makehuff (dhuff + tt, hufflen, huffvals);
	}

	/* set up the scan table */
	ptr = (unsigned char *) GsmartJPEGScanTable;
	for (i = 0; i < 3; i++) {
		dscans[i].cid = *ptr++;
		tdc = *ptr++;
		tac = tdc & 15;
		tdc >>= 4;
		if (tdc > 1 || tac > 1) {
			//printf("died whilst setting up scan table.\n");
			//abort();
		}
		/* for each component */
		for (j = 0; j < 3; j++)
			if (comps[j].cid == dscans[i].cid)
				break;

		dscans[i].hv = comps[j].hv;
		dscans[i].tq = comps[j].tq;
		dscans[i].hudc.dhuff = dec_huffdc + tdc;
		dscans[i].huac.dhuff = dec_huffac + tac;
	}

	if (dscans[0].cid != 1 || dscans[1].cid != 2 || dscans[2].cid != 3) {
		//printf("invalid cid found.\n");
		//abort();
	}

	if (dscans[0].hv != 0x22 || dscans[1].hv != 0x11
	    || dscans[2].hv != 0x11) {
		//printf("invalid hv found.\n");
		//abort();
	}

	idctqtab (quant[dscans[0].tq], dquant[0]);
	idctqtab (quant[dscans[1].tq], dquant[1]);
	idctqtab (quant[dscans[2].tq], dquant[2]);
	/* rescale qtab */
	scaleidctqtab (dquant[0], IFIX (0.7));
	scaleidctqtab (dquant[1], IFIX (0.7));
	scaleidctqtab (dquant[2], IFIX (0.7));

	dscans[0].next = 6 - 4;
	dscans[1].next = 6 - 4 - 1;
	dscans[2].next = 6 - 4 - 1 - 1;	/* 411 encoding */

}

