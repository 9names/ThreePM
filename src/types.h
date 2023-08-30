#ifndef TYPES_H_
#define TYPES_H_

/**
 * Temporarily storing all the inner struct definitions here to avoid circular dependencies.
 * These were borrowed out of coder.h and mp3common.h
 */


/* determining MAINBUF_SIZE:
 *   max mainDataBegin = (2^9 - 1) bytes (since 9-bit offset) = 511
 *   max nSlots (concatenated with mainDataBegin bytes from before) = 1440 - 9 - 4 + 1 = 1428
 *   511 + 1428 = 1939, round up to 1940 (4-byte align)
 */
#define MAINBUF_SIZE	1940

#define MAX_NGRAN		2		/* max granules */
#define MAX_NCHAN		2		/* max channels */
#define MAX_NSAMP		576		/* max samples per channel, per granule */


#define MAX_SCFBD		4		/* max scalefactor bands per channel */
#define NGRANS_MPEG1	2
#define NGRANS_MPEG2	1

#define MAX_REORDER_SAMPS		((192-126)*3)		/* largest critical band for short blocks (see sfBandTable) */
#define BLOCK_SIZE				18
#define	NBANDS					32

#define VBUF_LENGTH				(17 * 2 * NBANDS)	/* for double-sized vbuf FIFO */

typedef struct _SFBandTable {
	short l[23];
	short s[14];
} SFBandTable;


/* map to 0,1,2 to make table indexing easier */
typedef enum {
	MPEG1 =  0,
	MPEG2 =  1,
	MPEG25 = 2
} MPEGVersion;


/* map these to the corresponding 2-bit values in the frame header */
typedef enum {
	Stereo = 0x00,	/* two independent channels, but L and R frames might have different # of bits */
	Joint = 0x01,	/* coupled channels - layer III: mix of M-S and intensity, Layers I/II: intensity and direct coding only */
	Dual = 0x02,	/* two independent channels, L and R always have exactly 1/2 the total bitrate */
	Mono = 0x03		/* one channel */
} StereoMode;

typedef struct _BitStreamInfo {
	const unsigned char *bytePtr;
	unsigned int iCache;
	int cachedBits;
	int nBytes;
} BitStreamInfo;

typedef struct _FrameHeader {
    MPEGVersion ver;	/* version ID */
    int layer;			/* layer index (1, 2, or 3) */
    int crc;			/* CRC flag: 0 = disabled, 1 = enabled */
    int brIdx;			/* bitrate index (0 - 15) */
    int srIdx;			/* sample rate index (0 - 2) */
    int paddingBit;		/* padding flag: 0 = no padding, 1 = single pad byte */
    int privateBit;		/* unused */
    StereoMode sMode;	/* mono/stereo mode */
    int modeExt;		/* used to decipher joint stereo mode */
    int copyFlag;		/* copyright flag: 0 = no, 1 = yes */
    int origFlag;		/* original flag: 0 = copy, 1 = original */
    int emphasis;		/* deemphasis mode */
    int CRCWord;		/* CRC word (16 bits, 0 if crc not enabled) */

	const SFBandTable *sfBand;
} FrameHeader;

typedef struct _SideInfoSub {
    int part23Length;		/* number of bits in main data */ 
    int nBigvals;			/* 2x this = first set of Huffman cw's (maximum amplitude can be > 1) */
    int globalGain;			/* overall gain for dequantizer */
    int sfCompress;			/* unpacked to figure out number of bits in scale factors */
    int winSwitchFlag;		/* window switching flag */
    int blockType;			/* block type */
    int mixedBlock;			/* 0 = regular block (all short or long), 1 = mixed block */
    int tableSelect[3];		/* index of Huffman tables for the big values regions */
    int subBlockGain[3];	/* subblock gain offset, relative to global gain */
    int region0Count;		/* 1+region0Count = num scale factor bands in first region of bigvals */
    int region1Count;		/* 1+region1Count = num scale factor bands in second region of bigvals */
    int preFlag;			/* for optional high frequency boost */
    int sfactScale;			/* scaling of the scalefactors */
    int count1TableSelect;	/* index of Huffman table for quad codewords */
} SideInfoSub;


typedef struct _SideInfo {
	int mainDataBegin;
	int privateBits;
	int scfsi[MAX_NCHAN][MAX_SCFBD];				/* 4 scalefactor bands per channel */
	
	SideInfoSub	sis[MAX_NGRAN][MAX_NCHAN];
} SideInfo;

typedef struct {
    int cbType;		/* pure long = 0, pure short = 1, mixed = 2 */
    int cbEndS[3];	/* number nonzero short cb's, per subbblock */
	int cbEndSMax;	/* max of cbEndS[] */
    int cbEndL;		/* number nonzero long cb's  */
} CriticalBandInfo;

typedef struct _DequantInfo {
	int workBuf[MAX_REORDER_SAMPS];		/* workbuf for reordering short blocks */
	CriticalBandInfo cbi[MAX_NCHAN];	/* filled in dequantizer, used in joint stereo reconstruction */
} DequantInfo;

typedef struct _HuffmanInfo {
	int huffDecBuf[MAX_NCHAN][MAX_NSAMP];		/* used both for decoded Huffman values and dequantized coefficients */
	int nonZeroBound[MAX_NCHAN];				/* number of coeffs in huffDecBuf[ch] which can be > 0 */
	int gb[MAX_NCHAN];							/* minimum number of guard bits in huffDecBuf[ch] */
} HuffmanInfo;

typedef enum _HuffTabType {
	noBits,
	oneShot,
	loopNoLinbits,
	loopLinbits,
	quadA,
	quadB,
	invalidTab
} HuffTabType;

typedef struct _HuffTabLookup {
	int	linBits;
	HuffTabType tabType;
} HuffTabLookup;

typedef struct _IMDCTInfo {
	int outBuf[MAX_NCHAN][BLOCK_SIZE][NBANDS];	/* output of IMDCT */	
	int overBuf[MAX_NCHAN][MAX_NSAMP / 2];		/* overlap-add buffer (by symmetry, only need 1/2 size) */
	int numPrevIMDCT[MAX_NCHAN];				/* how many IMDCT's calculated in this channel on prev. granule */
	int prevType[MAX_NCHAN];
	int prevWinSwitch[MAX_NCHAN];
	int gb[MAX_NCHAN];
} IMDCTInfo;

typedef struct _BlockCount {
	int nBlocksLong;
	int nBlocksTotal;
	int nBlocksPrev; 
	int prevType;
	int prevWinSwitch;
	int currWinSwitch;
	int gbIn;
	int gbOut;
} BlockCount;

/* max bits in scalefactors = 5, so use char's to save space */
typedef struct _ScaleFactorInfoSub {
	char l[23];            /* [band] */
	char s[13][3];         /* [band][window] */
} ScaleFactorInfoSub;  

/* used in MPEG 2, 2.5 intensity (joint) stereo only */
typedef struct _ScaleFactorJS {
	int intensityScale;		
	int	slen[4];
	int	nr[4];
} ScaleFactorJS;

typedef struct _ScaleFactorInfo {
	ScaleFactorInfoSub sfis[MAX_NGRAN][MAX_NCHAN];
	ScaleFactorJS sfjs;
} ScaleFactorInfo;

/* NOTE - could get by with smaller vbuf if memory is more important than speed
 *  (in Subband, instead of replicating each block in FDCT32 you would do a memmove on the
 *   last 15 blocks to shift them down one, a hardware style FIFO)
 */ 
typedef struct _SubbandInfo {
	int vbuf[MAX_NCHAN * VBUF_LENGTH];		/* vbuf for fast DCT-based synthesis PQMF - double size for speed (no modulo indexing) */
	int vindex;								/* internal index for tracking position in vbuf */
} SubbandInfo;

#endif