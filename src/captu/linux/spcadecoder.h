
#ifndef SPCADECODER_H
#define SPCADECODER_H

#include "spca5xx.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ISHIFT 11

#define IFIX(a) ((long)((a) * (1 << ISHIFT) + .5))
#define IMULT(a, b) (((a) * (b)) >> ISHIFT)
#define ITOINT(a) ((a) >> ISHIFT)

/* special markers */
#define M_BADHUFF	-1

struct in {
	unsigned char *p;
	unsigned int bits;
	int left;
	int marker;
};

/*********************************/
struct dec_hufftbl;
struct enc_hufftbl;

union hufftblp {
	struct dec_hufftbl *dhuff;
	struct enc_hufftbl *ehuff;
};

struct scan {
	int dc;			/* old dc value */

	union hufftblp hudc;
	union hufftblp huac;
	int next;		/* when to switch to next scan */

	int cid;		/* component id */
	int hv;			/* horiz/vert, copied from comp */
	int tq;			/* quant tbl, copied from comp */
};

/*********************************/

#define DECBITS 10		/* seems to be the optimum */

struct dec_hufftbl {
	int maxcode[17];
	int valptr[16];
	unsigned char vals[256];
	unsigned int llvals[1 << DECBITS];
};


/*********************************/


/*********************************/

/*********************************/
struct comp {
	int cid;
	int hv;
	int tq;
};

#define MAXCOMP 4
#define ERR_NO_SOI 1
#define ERR_NOT_8BIT 2
#define ERR_HEIGHT_MISMATCH 3
#define ERR_WIDTH_MISMATCH 4
#define ERR_BAD_WIDTH_OR_HEIGHT 5
#define ERR_TOO_MANY_COMPPS 6
#define ERR_ILLEGAL_HV 7
#define ERR_QUANT_TABLE_SELECTOR 8
#define ERR_NOT_YCBCR_221111 9
#define ERR_UNKNOWN_CID_IN_SCAN 10
#define ERR_NOT_SEQUENTIAL_DCT 11
#define ERR_WRONG_MARKER 12
#define ERR_NO_EOI 13
#define ERR_BAD_TABLES 14
#define ERR_DEPTH_MISMATCH 15

int spca50x_outpicture (struct spca5xx_frame *myframe);

void init_jpeg_decoder(void);
void 
create_jpeg_from_data (unsigned char* dst, unsigned char * src, int qIndex,
		    int w, int h, unsigned char format, int o_size,
		    int *size, int omit_huffman_table);

#ifdef __cplusplus
}
#endif
	
#endif /* SPCADECODER_H */
