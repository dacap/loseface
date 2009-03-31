/****************************************************************************
#	 	SpcaGui:  Spca5xx Grabber                                   #
# 		Copyright (C) 2004 Michel Xhaard                            #
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

#include "spcav4l.h"

int
init_v4l (struct vdIn *vd)
{
  int f;
  int erreur = 0;

  if ((vd->fd = open (vd->videodevice, O_RDWR)) == -1)
    {
      perror ("ERROR opening V4L interface \n");
      return 1;
    }
  if (ioctl (vd->fd, VIDIOCGCAP, &(vd->videocap)) == -1)
    {
      printf ("wrong device\n");
      return 1;
    }
  printf ("Camera found: %s \n", vd->videocap.name);
  snprintf (vd->cameraname, 32, "%s", vd->videocap.name);

  erreur = GetVideoPict (vd);
  vd->formatIn = vd->videopict.palette;
  vd->bppIn = vd->videopict.depth;
  vd->flipUV = 0;
  /* To Compute the estimate frame size perhaps not need !!! 
  if ((vd->bppIn = GetDepth (vd->formatIn)) < 0)
    {
      perror ("getdepth  failed \n");
      return 1;
    }
   */
  if (ioctl (vd->fd, VIDIOCGCHAN, &vd->videochan) == -1)
    {
      printf ("Hmm did not support Video_channel\n");
      vd->cameratype = UNKNOW;
      /*
      vd->sizeothers = 0xa9;
      vd->hdrwidth = DWIDTH;
      vd->hdrheight = DHEIGHT; 
      */
    }
  else
    {

      printf ("Bridge found: %s \n", vd->videochan.name);
      snprintf (vd->bridge, 9, "%s", vd->videochan.name);
      vd->cameratype = GetStreamId (vd->videochan.name);
      
    }
  if (vd->cameratype == UNKNOW){
	 /* process others cams default parameters should be set */
	  printf ("StreamId: %d Unknow Camera\n", vd->cameratype);
	  if (probePaletteU (vd ) < 0) {
	  printf ("Sorry cannot probe Palette for that Unknow Camera\n");
	  	return 1;
	  }
	  if (probeSizeU (vd ) < 0) {
	  printf ("Sorry cannot probe Size for that Unknow Camera\n");
	  	return 1;
	  }
   }
  else
   {
	 /* process the spca5xx Cams */
	 printf ("StreamId: %s Camera\n", Plist[vd->cameratype].name);
	 if(vd->cameratype == JPEG){
	 	vd->palette = 31;
	 } else {
	 	vd->palette = 30;
	 }
	 /* test jpeg capability if not and jpeg ask without raw data
         set default format to YUVP */
      if (vd->formatIn == VIDEO_PALETTE_RAW_JPEG && vd->cameratype != JPEG)
	{
	  printf ("Camera unable to stream in JPEG mode switch to YUV420P\n");
	  vd->formatIn = VIDEO_PALETTE_YUV420P;
	  vd->bppIn = GetDepth (vd->formatIn);
	 
	}
      if (probeSize (vd) < 0)
	printf ("unable to probe size !!\n");
	 
   }


      
    
		/**********************************************************/
  /*          alloc the frame buffer to read                */
  vd->framesizeIn = (vd->hdrwidth * vd->hdrheight * (vd->bppIn >> 3));


		/***************************            *******************/
  if (vd->grabMethod)
    {
      printf (" grabbing method default MMAP asked \n");
      // MMAP VIDEO acquisition
      memset (&(vd->videombuf), 0, sizeof (vd->videombuf));
      if (ioctl (vd->fd, VIDIOCGMBUF, &(vd->videombuf)) < 0)
	{
	  perror (" init VIDIOCGMBUF FAILED\n");
	}
      printf ("VIDIOCGMBUF size %d  frames %d  offets[0]=%d offsets[1]=%d\n",
	      vd->videombuf.size, vd->videombuf.frames,
	      vd->videombuf.offsets[0], vd->videombuf.offsets[1]);
      vd->pFramebuffer =
	(unsigned char *) mmap (0, vd->videombuf.size, PROT_READ | PROT_WRITE,
				MAP_SHARED, vd->fd, 0);
      vd->mmapsize = vd->videombuf.size;
      vd->vmmap.height = vd->hdrheight;
      vd->vmmap.width = vd->hdrwidth;
      vd->vmmap.format = vd->formatIn;
      for (f = 0; f < vd->videombuf.frames; f++)
	{
	  vd->vmmap.frame = f;
	  if (ioctl (vd->fd, VIDIOCMCAPTURE, &(vd->vmmap)))
	    {
	      perror ("cmcapture");
	    }
	}

      vd->vmmap.frame = 0;



    }
  else
    {
      /* read method */
      printf (" grabbing method READ asked \n");
      if (ioctl (vd->fd, VIDIOCGWIN, &(vd->videowin)) < 0)
	perror ("VIDIOCGWIN failed \n");
      vd->videowin.height = vd->hdrheight;
      vd->videowin.width = vd->hdrwidth;
      if (ioctl (vd->fd, VIDIOCSWIN, &(vd->videowin)) < 0)
	perror ("VIDIOCSWIN failed \n");
      printf ("VIDIOCSWIN height %d  width %d \n",
	      vd->videowin.height, vd->videowin.width);
      /* why didn't use VIDIOCMCAPTURE to set palette ? */



    }
  vd->pixTmp = (unsigned char *) malloc (vd->framesizeIn);

  //sleep (1);
  //erreur = SetVideoPict ( vd );
  return erreur;
}

int
close_v4l (struct vdIn *vd)
{
  if (vd->grabMethod)
    {
      printf ("unmapping frame buffer\n");
      munmap (vd->pFramebuffer, vd->mmapsize);
    }
  printf ("freeing frame buffer\n");
  free (vd->pixTmp);

  printf ("close v4l \n");
  close (vd->fd);
  return 0;
}

static void
flipUV (unsigned char *src, int format, int w, int h)
{
  __u32 *lpix;
  __u16 *pix;
  __u8 *V;
  __u8 *U;
  __u8 *savC = NULL;
  unsigned char tmp;
  int sizetransfert = 0;
  int i;
  if (format == VIDEO_PALETTE_RAW_JPEG)
    {
      /*nothing todo */
      return;
    }
  switch (format)
    {
    case VIDEO_PALETTE_YUV420P:
      {
	/* allocate a helper buffer */
	sizetransfert = (w * h >> 2);
	savC = (__u8 *) realloc (savC, (size_t) sizetransfert);
	U = src + (w * h);
	V = src + (w * h) + sizetransfert;
	/* save U */
	memcpy (savC, U, sizetransfert);
	/* flip UV */
	memcpy (U, V, sizetransfert);
	/* restore U */
	memcpy (V, savC, sizetransfert);
	free (savC);
	savC = NULL;
      }
      break;
    case VIDEO_PALETTE_RGB565:
      {
	pix = (__u16 *) src;
	for (i = 0; i < (w * h); i++)
	  {
	    pix[i] =
	      (((pix[i] & 0xF800) >> 11) | ((pix[i] & 0x001F) << 11) |
	       (pix[i] & 0x07E0));
	  }
      }
      break;
    case VIDEO_PALETTE_RGB24:
      {
	for (i = 0; i < (w * h * 3); i += 3)
	  {
	    tmp = src[i];
	    src[i] = src[i + 2];
	    src[i + 2] = tmp;
	  }
      }
      break;
    case VIDEO_PALETTE_RGB32:
      {
	lpix = (__u32 *) src;
	for (i = 0; i < (w * h); i++)
	  {
	    lpix[i] =
	      (((lpix[i] & 0x00FF0000) >> 16) | ((lpix[i] & 0x000000FF) << 16)
	       | (lpix[i] & 0x0000FF00));
	  }
      }
      break;
    default:
      break;
    }
}
int
SpcaGrab (struct vdIn *vd)
{
  int ff;
  int len;
  int status;
  int count = 0;
  int size;
  int erreur = 0;
  if (vd->grabMethod)
    {
      ff = vd->vmmap.frame;
      vd->vmmap.height = vd->hdrheight;
      vd->vmmap.width = vd->hdrwidth;
      vd->vmmap.format = vd->formatIn;
      if (ioctl (vd->fd, VIDIOCSYNC, &ff) < 0)
	{
	  perror ("cvsync err\n");
	  erreur = -1;
	}
      vd->vmmap.frame = ff;
      memcpy (vd->pixTmp,
	      vd->pFramebuffer +
	      vd->videombuf.offsets[vd->vmmap.frame], vd->framesizeIn);
      if (vd->flipUV)
	flipUV (vd->pixTmp, vd->formatIn, vd->hdrwidth, vd->hdrheight);
      if ((ioctl (vd->fd, VIDIOCMCAPTURE, &(vd->vmmap))) < 0)
	{
	  perror ("cmcapture");
	  printf (">>cmcapture err %d\n", status);
	  erreur = -1;
	}
      vd->vmmap.frame = (vd->vmmap.frame + 1) % vd->videombuf.frames;
      //printf("frame nb %d\n",vd->vmmap.frame);

    }
  else
    {
      /* read method */
      size = vd->framesizeIn;
      len = read (vd->fd, vd->pixTmp, size);
      if (len != size)
	{
	  printf ("v4l read error\n");
	  printf ("len %d asked %d \n", len, size);
	  erreur = -1;
	}
      if (vd->flipUV)
	flipUV (vd->pixTmp, vd->formatIn, vd->hdrwidth, vd->hdrheight);
    }
  return erreur;
}

int
setPalette (struct vdIn *vd)
{

  vd->bppIn = GetDepth (vd->formatIn);
  vd->videopict.palette = vd->formatIn;
  vd->videopict.depth = vd->bppIn;
  SetVideoPict (vd);
  vd->framesizeIn = ((vd->hdrwidth * vd->hdrheight * vd->bppIn) >> 3);
  vd->pixTmp =
    (unsigned char *) realloc (vd->pixTmp, (size_t) vd->framesizeIn);

}

static
int probePaletteU ( struct vdIn *vd )
{	/* probe palette and set a default one for unknow cams*/
	int pal[] ={VIDEO_PALETTE_YUV420P,VIDEO_PALETTE_RGB565,VIDEO_PALETTE_RGB24,VIDEO_PALETTE_RGB32};
	struct video_picture pict;
	int masq = 0x2;
	int i;
	int availpal = 0;
	int defaut = 1;
	/* initialize the internal struct */
	if (ioctl (vd->fd, VIDIOCGPICT, &pict) < 0)
    		{
      		perror ("Couldnt get videopict params with VIDIOCGPICT\n");
      		return -1;
   		 }
	/* try each palette we have we skip raw_jpeg */
	for(i = 0; i < 4 ; i++){
	pict.palette = pal[i];
	/* maybe correct the bug on qca driver depth always 24 ? */	
	pict.depth = GetDepth (pal[i]);
	printf("try palette %d depth %d\n",pict.palette,pict.depth);
	if (ioctl (vd->fd, VIDIOCSPICT, &pict) < 0)
    		{
      		printf("Couldnt set palette first try %d \n", pal[i]);
      
    		}
	if (ioctl (vd->fd, VIDIOCGPICT, &pict) < 0)
    		{
      		printf("Couldnt get palette %d \n", pal[i]);
      
    		}
	if (pict.palette != pal[i]){
		printf("Damned second try fail \n");	
		} else {
		availpal = availpal | masq ;
		printf("Available  palette %d \n", pal[i]);
		if (defaut){
		 defaut = 0;
		 vd->formatIn = pal[i];
		 vd->bppIn = GetDepth (pal[i]);
		}
		}
		
	masq = masq << 1;
	}
	vd->palette = availpal;
	//should set default palette here ?
 return 1;	
}

static
int probeSizeU ( struct vdIn *vd )
{	/* probe size and set a default one for unknow cams */
	int size[] = { 640,480,384,288,352,288,320,240,192,144,176,144,160,120 };
	struct video_window win;
	int maxw,minw,maxh,minh;
	int masq = 0x1;
	int i = 0;
	int defaut = 1 ;
	/* initialize de parameters */
	maxw = vd->videocap.maxwidth;
	minw = vd->videocap.minwidth;
	maxh = vd->videocap.maxheight;
	minh = vd->videocap.minheight;
	printf("probe size in \n");
	while (size[i] > maxw){
	printf("skip size %d x %d\n",size[i],size[i+1]);
		i += 2;
		masq = masq << 1;
		if (i > 13) break;
	}
	/* initialize the internal struct */
	 if (ioctl (vd->fd, VIDIOCGWIN, &win) < 0) {
		perror ("VIDIOCGWIN failed \n");
		return -1;
		}
	/* now i is on the first possible width */
	while ((size[i] >= minw) && i < 13) {
	win.width = size[i];
	win.height = size[i+1];
	if (ioctl (vd->fd, VIDIOCSWIN, &win) < 0) {
      		printf ("VIDIOCSWIN reject width %d  height %d \n",
	      		win.width, win.height);	      
    	} else {
	vd->sizeothers = vd->sizeothers | masq;
	 printf ("Available Resolutions width %d  heigth %d \n",
	 		win.width, win.height);
	if (defaut)
		{

		  vd->hdrwidth = win.width;
		  vd->hdrheight = win.height;
		  defaut = 0;
		}
	}
	masq = masq << 1 ;
	i += 2;
	}
	
return 1;	
}

int
changeSize (struct vdIn *vd)
{
  int erreur;
  erreur = GetVideoPict (vd);
  vd->formatIn = vd->videopict.palette;
  vd->bppIn = vd->videopict.depth;
  /* To Compute the estimate frame size perhaps not need !!! */
  if ((vd->bppIn = GetDepth (vd->formatIn)) < 0)
    {
      perror ("getdepth  failed \n");
      return 1;
    }
  if (vd->grabMethod)
    {
      vd->vmmap.height = vd->hdrheight;
      vd->vmmap.width = vd->hdrwidth;
      vd->vmmap.format = vd->formatIn;

    }
  else
    {

      if (ioctl (vd->fd, VIDIOCGWIN, &vd->videowin) < 0)
	perror ("VIDIOCGWIN failed \n");
      vd->videowin.height = vd->hdrheight;
      vd->videowin.width = vd->hdrwidth;
      if (ioctl (vd->fd, VIDIOCSWIN, &vd->videowin) < 0)
	perror ("VIDIOCSWIN failed \n");
      printf ("VIDIOCGWIN height %d  width %d \n",
	      vd->videowin.height, vd->videowin.width);
    }
  vd->framesizeIn = ((vd->hdrwidth * vd->hdrheight * vd->bppIn) >> 3);
  vd->pixTmp =
    (unsigned char *) realloc (vd->pixTmp, (size_t) vd->framesizeIn);

  return 0;
}

static int
isSpcaChip (const char *BridgeName)
{
  int i = -1;
  int size = 0;
  //size = strlen (BridgeName) - 1;
  /* Spca506 return more with channel video, cut it */
  //if (size > 10)
   // size = 7;
  /* return Bridge otherwhise -1 */
  for (i = 0; i < MAX_BRIDGE; i++)
    {
    size = strlen (Blist[i].name);
      if (strncmp (BridgeName, Blist[i].name, size) == 0)
	{
	  break;
	}
    }

  return i;
}

static int
GetStreamId (const char *BridgeName)
{
  int i = -1;
  int match = -1;
/* return Stream_id otherwhise -1 */
  if ((match = isSpcaChip (BridgeName)) < 0)
    {
      printf ("Not an Spca5xx Camera !!\n");
      return match;
    }
  switch (match)
    {
    case BRIDGE_SPCA505:
    case BRIDGE_SPCA506:
      i = YYUV;
      break;
    case BRIDGE_SPCA501:
      i = YUYV;
      break;
    case BRIDGE_SPCA508:
      i = YUVY;
      break;
    case BRIDGE_SPCA536:
    case BRIDGE_SPCA504:
    case BRIDGE_SPCA500:
    case BRIDGE_SPCA504B:
    case BRIDGE_SPCA533:
    case BRIDGE_SPCA504_PCCAM600:
      i = JPEG;
      break;
    case BRIDGE_SPCA561:
      i = GBRG;
      break;
    default:
      i = UNKNOW; // -1;
      printf ("Unable to find a StreamId !!\n");
      break;

    }
  return i;
}

static int
probeSize (struct vdIn *vd)
{
  int size[] = { 640, 384, 352, 320, 192, 176, 160 };
  int bridge = -1;
  int i, j;
  unsigned int intwidth;
  unsigned int intheight;
  unsigned int intformatN;
  int defaut = 1;
  int masq = 0x01;

  vd->sizenative = 0;
  vd->sizeothers = 0;
/* return 1->ok otherwhise -1 */
  if ((bridge = isSpcaChip (vd->videochan.name)) < 0)
    {
      printf ("Cannot Probe Size !! maybe not an Spca5xx Camera\n");
      vd->sizeothers = 0xa9;
      return -1;
    }

  i = 0;
  j = 0;
  while ((unsigned int) (GET_EXT_MODES (bridge)[i][0]))
    {
      intwidth = GET_EXT_MODES (bridge)[i][0];
      intheight = GET_EXT_MODES (bridge)[i][1];
      intformatN = (GET_EXT_MODES (bridge)[i][2] & 0xF0) >> 4;
      if (intwidth == size[j])
	{
	  j++;
	  if (!intformatN)
	    {
	      if (defaut)
		{

		  vd->hdrwidth = intwidth;
		  vd->hdrheight = intheight;
		  defaut = 0;
		}
	      vd->sizenative = vd->sizenative | masq;
	    }
	  vd->sizeothers = vd->sizeothers | masq;
	  masq = masq << 1;
	  printf ("Available Resolutions width %d  heigth %d %s\n",
		  intwidth, intheight, (intformatN) ? "decoded" : "native");
	}
      if (intwidth > size[j])
	{
	  i++;
	}
      if (intwidth < size[j])
	{
	  j++;
	  if (j > 6)
	    break;
	  masq = masq << 1;
	}
    }
  return 0;
}
static int
GetVideoPict (struct vdIn *vd)
{
  if (ioctl (vd->fd, VIDIOCGPICT, &vd->videopict) < 0)
    {
      perror ("Couldnt get videopict params with VIDIOCGPICT\n");
      return -1;
    }

  printf ("VIDIOCGPICT brightnes=%d hue=%d color=%d contrast=%d whiteness=%d"
	  "depth=%d palette=%d\n", vd->videopict.brightness,
	  vd->videopict.hue, vd->videopict.colour, vd->videopict.contrast,
	  vd->videopict.whiteness, vd->videopict.depth,
	  vd->videopict.palette);

  return 0;
}

static int
SetVideoPict (struct vdIn *vd)
{
  if (ioctl (vd->fd, VIDIOCSPICT, &vd->videopict) < 0)
    {
      perror ("Couldnt set videopict params with VIDIOCSPICT\n");
      return -1;
    }

  printf ("VIDIOCSPICT brightnes=%d hue=%d color=%d contrast=%d whiteness=%d"
	  "depth=%d palette=%d\n", vd->videopict.brightness,
	  vd->videopict.hue, vd->videopict.colour, vd->videopict.contrast,
	  vd->videopict.whiteness, vd->videopict.depth,
	  vd->videopict.palette);

  return 0;
}

static int
GetDepth (int format)
{
  int depth;
  switch (format)
    {
    case VIDEO_PALETTE_RAW_JPEG:
      {
	depth = 8;		// be sure spca50x ask raw data
      }
      break;
    case VIDEO_PALETTE_YUV420P:
      {
	depth = (8 * 3) >> 1;
      }
      break;
    case VIDEO_PALETTE_RGB565:
      depth = 16;
      break;
    case VIDEO_PALETTE_RGB24:
      depth = 24;
      break;
    case VIDEO_PALETTE_RGB32:
      {
	depth = 32;
      }
      break;
    default:
      depth = -1;
      break;
    }
  return depth;
}

__u8
SpcaGetBrightness (struct vdIn * vdin)
{
  if (GetVideoPict (vdin) < 0)
    {
      printf (" Error getBrightness \n");
      return 0;
    }
  return ((vdin->videopict.brightness) >> 8);
}

void
SpcaSetBrightness (struct vdIn *vdin, __u8 bright)
{
  vdin->videopict.brightness = bright << 8;
  if (SetVideoPict (vdin) < 0)
    {
      printf (" Error setBrightness \n");
    }

}
__u8
SpcaGetContrast (struct vdIn *vdin)
{
  if (GetVideoPict (vdin) < 0)
    {
      printf (" Error getContrast \n");
      return 0;
    }
  return ((vdin->videopict.contrast) >> 8);
}

void
SpcaSetContrast (struct vdIn *vdin, __u8 contrast)
{
  vdin->videopict.contrast = contrast << 8;
  if (SetVideoPict (vdin) < 0)
    {
      printf (" Error setContrast \n");
    }
}
__u8
SpcaGetColors (struct vdIn *vdin)
{
  if (GetVideoPict (vdin) < 0)
    {
      printf (" Error getColors \n");
      return 0;
    }
  return ((vdin->videopict.colour) >> 8);
}

void
SpcaSetColors (struct vdIn *vdin, __u8 colors)
{
  vdin->videopict.colour = colors << 8;
  if (SetVideoPict (vdin) < 0)
    {
      printf (" Error setColors \n");
    }
}
