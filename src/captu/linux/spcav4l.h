#ifndef SPCAV4L_H
#define SPCAV4L_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <math.h>
#include <stdarg.h>
#include <linux/types.h>
// is here for  __u16 __u8 ?
#include <linux/videodev.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "spca5xx.h"

#define BRIDGE_SPCA505 0
#define BRIDGE_SPCA506 1
#define BRIDGE_SPCA501 2
#define BRIDGE_SPCA508 3
#define BRIDGE_SPCA504 4
#define BRIDGE_SPCA500 5
#define BRIDGE_SPCA504B 6
#define BRIDGE_SPCA533 7
#define BRIDGE_SPCA504_PCCAM600 8
#define BRIDGE_SPCA561 9
#define BRIDGE_SPCA536 10
#define MAX_BRIDGE 11

struct vdIn {
	int fd;
	char *videodevice ;

	struct video_mmap vmmap;
	struct video_capability videocap;
	int mmapsize;
	struct video_mbuf videombuf;
	struct video_picture videopict;
	struct video_window videowin;
	struct video_channel videochan;
	
	unsigned int format ;	/* Format asked by apps for this frame */
	int cameratype ;		/* native in frame format */
	char *cameraname;
	char *bridge;
	int sizenative;
	int sizeothers;
	int palette;
	int norme ;
	int channel ;
	int grabMethod ;
	unsigned char *pFramebuffer;
	unsigned char *pixTmp;
	int framesizeIn ;
	int frame_cour;
	int bppIn;
	int  hdrwidth;
	int  hdrheight;
	int  formatIn;
	int flipUV;
	};

struct palette_list {
	int num;
	const char *name;
};

struct bridge_list {
	int num;
	const char *name;
};

static struct bridge_list Blist[]={

	{BRIDGE_SPCA505,"SPCA505"},
	{BRIDGE_SPCA506,"SPCA506"},
	{BRIDGE_SPCA501,"SPCA501"},
	{BRIDGE_SPCA508,"SPCA508"},
	{BRIDGE_SPCA504,"SPCA504"},
	{BRIDGE_SPCA500,"SPCA500"},
	{BRIDGE_SPCA504B,"SPCA504B"},
	{BRIDGE_SPCA533,"SPCA533"},
	{BRIDGE_SPCA504_PCCAM600,"SPCA504C"},
	{BRIDGE_SPCA561,"SPCA561"},
	{BRIDGE_SPCA536,"SPCA536"},
	{-1,NULL}
};
/* Camera type jpeg yuvy yyuv yuyv grey gbrg*/
static struct palette_list Plist[] ={
	{JPEG,"JPEG"},
	{YUVY,"YUVY"},
	{YYUV,"YYUV"},
	{YUYV,"YUYV"},
	{GREY,"GREY"},
	{GBRG,"GBRG"},
	{-1,NULL}
};

#define GET_EXT_MODES(bridge) (\
	(bridge) == BRIDGE_SPCA500 ? spca500_ext_modes : \
	(bridge) == BRIDGE_SPCA501 ? spca501_ext_modes : \
	(bridge) == BRIDGE_SPCA504 ? spca504_ext_modes : \
	(bridge) == BRIDGE_SPCA504B ? spca504_ext_modes : \
	(bridge) == BRIDGE_SPCA504_PCCAM600 ? spca504_pccam600_ext_modes : \
	(bridge) == BRIDGE_SPCA506 ? spca506_ext_modes : \
	(bridge) == BRIDGE_SPCA508 ? spca508_ext_modes : \
	(bridge) == BRIDGE_SPCA533 ? spca533_ext_modes : \
	(bridge) == BRIDGE_SPCA561 ? spca561_ext_modes : \
	(bridge) == BRIDGE_SPCA536 ? spca536_ext_modes : \
	spca50x_ext_modes)


static unsigned int spca500_ext_modes[][3] = {
	/* x , y , Standard*/
	{640, 480, 0x00},
	{352, 288, 0x00},
	{320, 240, 0x00},
	{176, 144, 0x00},
	{0, 0, 0}
};

static unsigned int spca501_ext_modes[][3] = {

	{640, 480, 0x00},
	{352, 288, 0x00},
	{320, 240, 0x00},
	{176, 144, 0x00},
	{160, 120, 0x00},
	{0, 0, 0}
};

static unsigned int spca504_ext_modes[][3] =
{
	{ 640, 480, 0x01},
	{ 384, 288, 0x11},
	{ 352, 288, 0x11},
	{ 320, 240, 0x02},
	{ 192, 144, 0x12},
	{ 176, 144, 0x12},
	{ 0, 0, 0}
};

 
static unsigned int spca504_pccam600_ext_modes[][3] =
{	
	{ 1024, 768, 0x00},
	{ 640,  480, 0x01},
	{ 352,  288, 0x02},
	{ 320,  240, 0x03},
	{ 176,  144, 0x04},
	{ 0, 0, 0}
};


static unsigned int spca506_ext_modes[][3] =
{
	{ 640, 480, 0x00},
	{ 352, 288, 0x01},
	{ 320, 240, 0x02},
	{ 176, 144, 0x04},
	{ 160, 120, 0x05},
	{ 0, 0, 0}
};

static unsigned int spca508_ext_modes[][3] =
{
	{ 352, 288, 0x00},
	{ 320, 240, 0x01},
	{ 176, 144, 0x02},
	{ 160, 120, 0x03},
	{ 0, 0, 0}
};

static unsigned int spca533_ext_modes[][3] =
{
	//{ 640, 480, 0x41},
	{ 464, 480, 0x01},//PocketDVII unscaled resolution aspect ratio need to expand x axis
	{ 464, 352, 0x01},//Gsmart LCD3 feature good aspect ratio
	{ 384, 288, 0x11},
	{ 352, 288, 0x11},
	{ 320, 240, 0x02},
	{ 192, 144, 0x12},
	{ 176, 144, 0x12},
	{ 0, 0, 0}
};
static unsigned int spca536_ext_modes[][3] =
{
	{ 464, 480, 0x01},
	{ 464, 352, 0x01},
	{ 384, 288, 0x11},
	{ 352, 288, 0x11},
	{ 320, 240, 0x02},
	{ 192, 144, 0x12},
	{ 176, 144, 0x12},
	{ 0, 0, 0}
};
static unsigned int spca561_ext_modes[][3] =
{
	{ 352, 288, 0x00},
	{ 320, 240, 0x01},
	{ 176, 144, 0x11},
	{ 160, 120, 0x11},
	{ 0, 0, 0}
};

static unsigned int spca50x_ext_modes[][3] =
{	
	{ 352, 288, 0x01},
	{ 320, 240, 0x02}, 
	{ 176, 144, 0x04},
	{ 160, 120, 0x05}, 
	{ 0, 0, 0}
};
	
int init_v4l ( struct vdIn *vd );
int SpcaGrab (struct vdIn *vd );
int close_v4l (struct vdIn *vd);
int setPalette (struct vdIn *vd);
int changeSize (struct vdIn *vd);

__u8 SpcaGetBrightness ( struct vdIn *vdin);
void SpcaSetBrightness ( struct vdIn *vdin, __u8 bright);
__u8 SpcaGetContrast ( struct vdIn *vdin);
void SpcaSetContrast ( struct vdIn *vdin, __u8 contrast);
__u8 SpcaGetColors ( struct vdIn *vdin);
void SpcaSetColors ( struct vdIn *vdin, __u8 colors);
__u8 SpcaGetNorme ( struct vdIn *vdin);
void SpcaSetNorme (struct vdIn *vdin,__u8 norme);
__u8 SpcaGetChannel (struct vdIn *vdin);
void SpcaSetChannel( struct vdIn * vdin,__u8 channel);
	
static int GetVideoPict (struct vdIn *vd);
static int SetVideoPict (struct vdIn *vd);
static int GetDepth ( int format);
static
int probePaletteU ( struct vdIn *vd );
static
int probeSizeU ( struct vdIn *vd );	
static int isSpcaChip ( const char * BridgeName );
/* return Bridge otherwhise -1 */
static int GetStreamId ( const char * BridgeName );
/* return Stream_id otherwhise -1 */
static int probeSize ( struct vdIn *vdin );
/* return 1->ok otherwhise -1 */	

#ifdef __cplusplus
}
#endif

#endif /* SPCAV4L_H */
