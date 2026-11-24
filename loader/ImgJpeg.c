#include "Loader.h"

#ifdef USE_IMG_JPEG

// --------------------------------   JPEG Support    -------------------------------- //
//
//		Self contained JPEG codec, no external library is required
//		Reading: baseline, extended, and progressive huffman files with 8 bit samples,
//		greyscale, YCbCr, RGB, CMYK, or YCCK color, any chroma subsampling, restart intervals
//		Writing: baseline YCbCr with standard tables, full chroma from quality 90 and half below
//		Not supported: arithmetic coding, lossless, hierarchical, and 12 bit files

#define JPEG_FAST_BITS 9 // huffman codes up to this length decode with one lookup
#define JPEG_MAX_PIXELS 0x10000000u // refuses images larger than 268 million pixels
#define JPEG_PI 3.14159265358979323846

static const uint8_t _zigzag[64] = { // position in the block for each coefficient in file order
	0, 1, 8, 16, 9, 2, 3, 10, 17, 24, 32, 25, 18, 11, 4, 5,
	12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13, 6, 7, 14, 21, 28,
	35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51,
	58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63
};

static float _dctBasis[8][8]; // [frequency][sample], shared by the forward and inverse transforms
static int _dctReady = FALSE;

static void dctSetup(void){
	if(_dctReady) return;
	for(unsigned u = 0; u < 8; u++)
		for(unsigned x = 0; x < 8; x++)
			_dctBasis[u][x] = (float)(((u == 0)? sqrt(0.125) : 0.5) * cos(((2.0 * x) + 1.0) * u * JPEG_PI / 16.0));
	_dctReady = TRUE;
}

static FILE* jpegOpen(const char* fileName, const char* mode){
#ifdef _WIN32
	FILE* file = NULL;
	if(fopen_s(&file, fileName, mode) != 0) return NULL;
	return file;
#else
	return fopen(fileName, mode);
#endif
}

static uint8_t clampByte(int value){ return (value < 0)? 0 : (value > 255)? 255 : (uint8_t)value; }

// --------------------------------   Reading: Bits and Huffman Codes    -------------------------------- //

typedef struct {
	const uint8_t* data;
	size_t size;
	size_t pos;
	uint32_t bits; // bit buffer, the next bit is the top bit
	int bitCount;
	int stopped; // a marker or the end of the file was reached, zero bits are supplied from there on
} Jpeg_Reader;

static void readerFill(Jpeg_Reader* reader){
	while(reader->bitCount <= 24){
		uint32_t byte = 0;
		if(!reader->stopped){
			if(reader->pos >= reader->size) reader->stopped = TRUE;
			else if(reader->data[reader->pos] != 0xFF) byte = reader->data[reader->pos++];
			else if(reader->pos + 1 < reader->size && reader->data[reader->pos + 1] == 0x00){ byte = 0xFF; reader->pos += 2; } // stuffed byte
			else reader->stopped = TRUE; // marker, position stays on it
		}
		reader->bits |= byte << (24 - reader->bitCount);
		reader->bitCount += 8;
	}
}

static int readerBits(Jpeg_Reader* reader, int count){ // count from 0 to 16
	if(count == 0) return 0;
	if(reader->bitCount < count) readerFill(reader);
	int value = (int)(reader->bits >> (32 - count));
	reader->bits <<= count;
	reader->bitCount -= count;
	return value;
}

static int readerMarker(Jpeg_Reader* reader){ // drops buffered bits and moves past the next marker, which is returned
	reader->bits = 0;
	reader->bitCount = 0;
	reader->stopped = FALSE;
	while(reader->pos + 1 < reader->size){
		if(reader->data[reader->pos] == 0xFF && reader->data[reader->pos + 1] != 0x00 && reader->data[reader->pos + 1] != 0xFF){
			reader->pos += 2;
			return reader->data[reader->pos - 1];
		}
		reader->pos++;
	}
	reader->pos = reader->size;
	return 0xD9; // end of image when the file is cut short
}

typedef struct {
	uint16_t lookup[1 << JPEG_FAST_BITS]; // (length << 8) | symbol for short codes, 0 for longer ones
	int maxCode[17]; // largest code of each length, -1 when there is none
	int valOffset[17]; // added to a code of each length to index vals
	uint8_t vals[256];
	int defined;
} Jpeg_Huffman;

static int huffmanBuild(Jpeg_Huffman* table, const uint8_t counts[16], const uint8_t* vals, unsigned total){
	memset(table, 0, sizeof(Jpeg_Huffman));
	memcpy(table->vals, vals, total);

	unsigned code = 0, index = 0;
	for(int length = 1; length <= 16; length++){
		table->valOffset[length] = (int)index - (int)code;
		for(unsigned c = 0; c < counts[length - 1]; c++, code++, index++){
			if(code >= (1u << length)) return FALSE; // more codes than the length allows
			if(length <= JPEG_FAST_BITS){
				unsigned first = code << (JPEG_FAST_BITS - length), span = 1u << (JPEG_FAST_BITS - length);
				for(unsigned s = 0; s < span; s++) table->lookup[first + s] = (uint16_t)((length << 8) | vals[index]);
			}
		}
		table->maxCode[length] = (counts[length - 1] > 0)? (int)code - 1 : -1;
		code <<= 1;
	}
	table->defined = TRUE;
	return TRUE;
}

static int huffmanDecode(Jpeg_Reader* reader, const Jpeg_Huffman* table){ // symbol, or -1 for an invalid code
	if(reader->bitCount < 16) readerFill(reader);

	unsigned entry = table->lookup[reader->bits >> (32 - JPEG_FAST_BITS)];
	if(entry != 0){
		reader->bits <<= (entry >> 8);
		reader->bitCount -= (int)(entry >> 8);
		return (int)(entry & 0xFF);
	}
	for(int length = JPEG_FAST_BITS + 1; length <= 16; length++){
		int code = (int)(reader->bits >> (32 - length));
		if(code <= table->maxCode[length]){
			reader->bits <<= length;
			reader->bitCount -= length;
			return table->vals[(table->valOffset[length] + code) & 0xFF];
		}
	}
	return -1;
}

static int extendValue(int value, int size){ return (value < (1 << (size - 1)))? value - (1 << size) + 1 : value; } // sign of a coefficient

// --------------------------------   Reading: Frames and Scans    -------------------------------- //

typedef struct {
	int id;
	int hSample, vSample; // sampling factors
	int quantId, dcId, acId; // table selectors
	unsigned width, height; // size in samples
	unsigned blocksWide, blocksHigh; // blocks that cover the samples, used by scans holding one component
	unsigned allocWide, allocHigh; // blocks allocated, covers whole MCUs
	int16_t* coefs; // 64 per block, not yet scaled by the quantization table
	uint8_t* plane; // decoded samples, 8 per block in each direction
	int dcPred;
} Jpeg_Component;

typedef struct {
	Jpeg_Reader reader;
	Jpeg_Huffman dcTables[4], acTables[4];
	uint16_t quant[4][64]; // block order
	int quantDefined[4];
	Jpeg_Component comps[4];
	unsigned width, height;
	int compCount; // 0 until a frame header is read
	int progressive;
	int hMax, vMax;
	unsigned mcusWide, mcusHigh;
	unsigned restartInterval;
	int adobeTransform; // -1 without an Adobe segment
	int hasJfif;
	int scanCount;
	// current scan
	int specStart, specEnd, approxHigh, approxLow;
	unsigned eobRun;
	const char* error;
} Jpeg_Decoder;

static unsigned read16(const uint8_t* data){ return ((unsigned)data[0] << 8) | data[1]; }

static int parseFrame(Jpeg_Decoder* decoder, const uint8_t* seg, unsigned length, int progressive){
	if(decoder->compCount != 0){ decoder->error = "more than one frame"; return FALSE; }
	if(length < 6){ decoder->error = "corrupt frame header"; return FALSE; }
	if(seg[0] != 8){ decoder->error = "only 8 bit samples are supported"; return FALSE; }

	decoder->height = read16(seg + 1);
	decoder->width = read16(seg + 3);
	int compCount = seg[5];
	if(decoder->width == 0 || decoder->height == 0){ decoder->error = "image has no size"; return FALSE; }
	if((size_t)decoder->width * decoder->height > JPEG_MAX_PIXELS){ decoder->error = "image is too large"; return FALSE; }
	if(compCount != 1 && compCount != 3 && compCount != 4){ decoder->error = "unsupported number of color components"; return FALSE; }
	if(length < 6 + (unsigned)compCount * 3){ decoder->error = "corrupt frame header"; return FALSE; }

	decoder->hMax = 1; decoder->vMax = 1;
	for(int c = 0; c < compCount; c++){
		Jpeg_Component* comp = &decoder->comps[c];
		comp->id = seg[6 + c * 3];
		comp->hSample = seg[7 + c * 3] >> 4;
		comp->vSample = seg[7 + c * 3] & 0xF;
		comp->quantId = seg[8 + c * 3];
		if(comp->hSample < 1 || comp->hSample > 4 || comp->vSample < 1 || comp->vSample > 4 || comp->quantId > 3){ decoder->error = "corrupt frame header"; return FALSE; }
		if(comp->hSample > decoder->hMax) decoder->hMax = comp->hSample;
		if(comp->vSample > decoder->vMax) decoder->vMax = comp->vSample;
	}
	if(compCount == 1){ decoder->comps[0].hSample = 1; decoder->comps[0].vSample = 1; decoder->hMax = 1; decoder->vMax = 1; } // one component is never interleaved

	decoder->mcusWide = (decoder->width + (8 * decoder->hMax) - 1) / (8 * decoder->hMax);
	decoder->mcusHigh = (decoder->height + (8 * decoder->vMax) - 1) / (8 * decoder->vMax);
	for(int c = 0; c < compCount; c++){
		Jpeg_Component* comp = &decoder->comps[c];
		comp->width = (decoder->width * comp->hSample + decoder->hMax - 1) / decoder->hMax;
		comp->height = (decoder->height * comp->vSample + decoder->vMax - 1) / decoder->vMax;
		comp->blocksWide = (comp->width + 7) / 8;
		comp->blocksHigh = (comp->height + 7) / 8;
		comp->allocWide = decoder->mcusWide * comp->hSample;
		comp->allocHigh = decoder->mcusHigh * comp->vSample;
		comp->coefs = (int16_t*)calloc((size_t)comp->allocWide * comp->allocHigh * 64, sizeof(int16_t));
		if(comp->coefs == NULL){ decoder->error = "out of memory"; return FALSE; }
	}
	decoder->compCount = compCount;
	decoder->progressive = progressive;
	return TRUE;
}

static int parseHuffman(Jpeg_Decoder* decoder, const uint8_t* seg, unsigned length){
	unsigned offset = 0;
	while(offset + 17 <= length){
		unsigned tableClass = seg[offset] >> 4, tableId = seg[offset] & 0xF;
		const uint8_t* counts = seg + offset + 1;
		unsigned total = 0;
		for(unsigned c = 0; c < 16; c++) total += counts[c];
		if(tableClass > 1 || tableId > 3 || total > 256 || offset + 17 + total > length){ decoder->error = "corrupt huffman table"; return FALSE; }

		Jpeg_Huffman* table = (tableClass == 0)? &decoder->dcTables[tableId] : &decoder->acTables[tableId];
		if(!huffmanBuild(table, counts, seg + offset + 17, total)){ decoder->error = "corrupt huffman table"; return FALSE; }
		offset += 17 + total;
	}
	return TRUE;
}

static int parseQuant(Jpeg_Decoder* decoder, const uint8_t* seg, unsigned length){
	unsigned offset = 0;
	while(offset < length){
		unsigned precision = seg[offset] >> 4, tableId = seg[offset] & 0xF;
		unsigned bytes = (precision == 0)? 64 : 128;
		if(precision > 1 || tableId > 3 || offset + 1 + bytes > length){ decoder->error = "corrupt quantization table"; return FALSE; }
		for(unsigned i = 0; i < 64; i++)
			decoder->quant[tableId][_zigzag[i]] = (uint16_t)((precision == 0)? seg[offset + 1 + i] : read16(seg + offset + 1 + (i * 2)));
		decoder->quantDefined[tableId] = TRUE;
		offset += 1 + bytes;
	}
	return TRUE;
}

static int decodeBlock(Jpeg_Decoder* decoder, Jpeg_Component* comp, int16_t* block){
	Jpeg_Reader* reader = &decoder->reader;
	const Jpeg_Huffman* acTable = &decoder->acTables[comp->acId];
	int start = decoder->specStart, end = decoder->specEnd, low = decoder->approxLow;

	if(start == 0){ // DC coefficient
		if(decoder->approxHigh == 0){
			int size = huffmanDecode(reader, &decoder->dcTables[comp->dcId]);
			if(size < 0 || size > 15) return FALSE;
			if(size > 0) comp->dcPred += extendValue(readerBits(reader, size), size);
			block[0] = (int16_t)(comp->dcPred * (1 << low));
		}
		else if(readerBits(reader, 1)) block[0] |= (int16_t)(1 << low); // refinement bit
		if(decoder->progressive) return TRUE; // AC coefficients come in their own scans
		start = 1;
	}

	if(decoder->approxHigh == 0){ // first pass over this band
		if(decoder->eobRun > 0){ decoder->eobRun--; return TRUE; }
		for(int k = start; k <= end; ){
			int symbol = huffmanDecode(reader, acTable);
			if(symbol < 0) return FALSE;
			int run = symbol >> 4, size = symbol & 0xF;
			if(size == 0){
				if(run == 15){ k += 16; continue; } // sixteen zeros
				decoder->eobRun = (1u << run) - 1; // end of band for this and the following blocks
				if(run > 0) decoder->eobRun += (unsigned)readerBits(reader, run);
				if(!decoder->progressive) decoder->eobRun = 0; // sequential files end one block at a time
				break;
			}
			k += run;
			if(k > end) return FALSE;
			block[_zigzag[k++]] = (int16_t)(extendValue(readerBits(reader, size), size) * (1 << low));
		}
		return TRUE;
	}

	// refinement pass, adds one bit of precision to this band
	int plus = 1 << low, minus = -(1 << low);
	int k = start;
	if(decoder->eobRun == 0){
		for(; k <= end; k++){
			int symbol = huffmanDecode(reader, acTable);
			if(symbol < 0) return FALSE;
			int run = symbol >> 4, size = symbol & 0xF, value = 0;
			if(size != 0) value = readerBits(reader, 1)? plus : minus; // a new coefficient
			else if(run != 15){
				decoder->eobRun = 1u << run;
				if(run > 0) decoder->eobRun += (unsigned)readerBits(reader, run);
				break;
			}
			for(; k <= end; k++){ // skips zeros, correcting coefficients that are already set
				int16_t* coef = block + _zigzag[k];
				if(*coef != 0){
					if(readerBits(reader, 1) && (*coef & plus) == 0) *coef = (int16_t)(*coef + ((*coef >= 0)? plus : minus));
				}
				else if(--run < 0) break;
			}
			if(value != 0 && k <= end) block[_zigzag[k]] = (int16_t)value;
		}
	}
	if(decoder->eobRun > 0){ // remaining coefficients only receive corrections
		for(; k <= end; k++){
			int16_t* coef = block + _zigzag[k];
			if(*coef != 0 && readerBits(reader, 1) && (*coef & plus) == 0) *coef = (int16_t)(*coef + ((*coef >= 0)? plus : minus));
		}
		decoder->eobRun--;
	}
	return TRUE;
}

static int decodeScan(Jpeg_Decoder* decoder, const uint8_t* seg, unsigned length){
	if(decoder->compCount == 0){ decoder->error = "scan before frame header"; return FALSE; }
	int scanCount = (length > 0)? seg[0] : 0;
	if(scanCount < 1 || scanCount > decoder->compCount || length < 4 + (unsigned)scanCount * 2){ decoder->error = "corrupt scan header"; return FALSE; }

	Jpeg_Component* scanComps[4];
	for(int s = 0; s < scanCount; s++){
		scanComps[s] = NULL;
		for(int c = 0; c < decoder->compCount; c++) if(decoder->comps[c].id == seg[1 + s * 2]) scanComps[s] = &decoder->comps[c];
		if(scanComps[s] == NULL){ decoder->error = "scan names an unknown component"; return FALSE; }
		scanComps[s]->dcId = seg[2 + s * 2] >> 4;
		scanComps[s]->acId = seg[2 + s * 2] & 0xF;
		if(scanComps[s]->dcId > 3 || scanComps[s]->acId > 3){ decoder->error = "corrupt scan header"; return FALSE; }
		scanComps[s]->dcPred = 0;
	}
	decoder->specStart = seg[1 + scanCount * 2];
	decoder->specEnd = seg[2 + scanCount * 2];
	decoder->approxHigh = seg[3 + scanCount * 2] >> 4;
	decoder->approxLow = seg[3 + scanCount * 2] & 0xF;
	decoder->eobRun = 0;

	if(!decoder->progressive){ decoder->specStart = 0; decoder->specEnd = 63; decoder->approxHigh = 0; decoder->approxLow = 0; }
	else if(decoder->specStart > decoder->specEnd || decoder->specEnd > 63 || decoder->approxLow > 13
		|| (decoder->specStart == 0 && decoder->specEnd != 0) || (decoder->specStart > 0 && scanCount != 1)){ decoder->error = "corrupt progressive scan"; return FALSE; }

	for(int s = 0; s < scanCount; s++){ // tables used by this scan must exist
		int needsDc = decoder->specStart == 0 && decoder->approxHigh == 0;
		int needsAc = decoder->specEnd > 0;
		if((needsDc && !decoder->dcTables[scanComps[s]->dcId].defined) || (needsAc && !decoder->acTables[scanComps[s]->acId].defined)){ decoder->error = "scan uses a missing huffman table"; return FALSE; }
	}

	Jpeg_Reader* reader = &decoder->reader;
	reader->bits = 0; reader->bitCount = 0; reader->stopped = FALSE;

	unsigned unitsWide = (scanCount == 1)? scanComps[0]->blocksWide : decoder->mcusWide;
	unsigned unitsHigh = (scanCount == 1)? scanComps[0]->blocksHigh : decoder->mcusHigh;
	unsigned sinceRestart = 0;

	for(unsigned unitY = 0; unitY < unitsHigh; unitY++)
		for(unsigned unitX = 0; unitX < unitsWide; unitX++){
			if(decoder->restartInterval > 0 && sinceRestart == decoder->restartInterval){
				int marker = readerMarker(reader);
				if(marker < 0xD0 || marker > 0xD7){ reader->pos -= (reader->pos >= 2)? 2 : 0; return TRUE; } // data ends early, keeps what was decoded
				for(int s = 0; s < scanCount; s++) scanComps[s]->dcPred = 0;
				decoder->eobRun = 0;
				sinceRestart = 0;
			}
			sinceRestart++;

			if(scanCount == 1){
				Jpeg_Component* comp = scanComps[0];
				if(!decodeBlock(decoder, comp, comp->coefs + ((size_t)unitY * comp->allocWide + unitX) * 64)) return TRUE; // corrupt data, keeps what was decoded
			}
			else for(int s = 0; s < scanCount; s++){
				Jpeg_Component* comp = scanComps[s];
				for(int v = 0; v < comp->vSample; v++)
					for(int h = 0; h < comp->hSample; h++){
						size_t blockIndex = ((size_t)unitY * comp->vSample + v) * comp->allocWide + ((size_t)unitX * comp->hSample + h);
						if(!decodeBlock(decoder, comp, comp->coefs + blockIndex * 64)) return TRUE;
					}
			}
		}
	return TRUE;
}

// --------------------------------   Reading: Samples and Color    -------------------------------- //

static void inverseBlock(const int16_t* coefs, const uint16_t* quant, uint8_t* out, size_t stride){
	int flat = TRUE;
	for(unsigned i = 1; i < 64; i++) if(coefs[i] != 0){ flat = FALSE; break; }
	if(flat){ // only the average is set
		uint8_t value = clampByte((int)floor(((double)coefs[0] * quant[0] / 8.0) + 128.5));
		for(unsigned y = 0; y < 8; y++) memset(out + (y * stride), value, 8);
		return;
	}

	float temp[64];
	for(unsigned v = 0; v < 8; v++){ // rows
		float row[8];
		int rowZero = TRUE;
		for(unsigned u = 0; u < 8; u++){
			row[u] = (float)coefs[(v * 8) + u] * (float)quant[(v * 8) + u];
			if(row[u] != 0.0f) rowZero = FALSE;
		}
		for(unsigned x = 0; x < 8; x++){
			float sum = 0.0f;
			if(!rowZero) for(unsigned u = 0; u < 8; u++) sum += row[u] * _dctBasis[u][x];
			temp[(v * 8) + x] = sum;
		}
	}
	for(unsigned x = 0; x < 8; x++) // columns
		for(unsigned y = 0; y < 8; y++){
			float sum = 0.0f;
			for(unsigned v = 0; v < 8; v++) sum += temp[(v * 8) + x] * _dctBasis[v][y];
			out[(y * stride) + x] = clampByte((int)floorf(sum + 128.5f));
		}
}

typedef struct { unsigned* first; unsigned* second; unsigned* weight; } Jpeg_Resample; // per output column, weight of second out of 256

static int resampleSetup(Jpeg_Resample* resample, unsigned outSize, unsigned inSize, int sample, int sampleMax){
	resample->first = (unsigned*)malloc(outSize * sizeof(unsigned));
	resample->second = (unsigned*)malloc(outSize * sizeof(unsigned));
	resample->weight = (unsigned*)malloc(outSize * sizeof(unsigned));
	if(resample->first == NULL || resample->second == NULL || resample->weight == NULL) return FALSE;

	for(unsigned o = 0; o < outSize; o++){
		double pos = (((double)o + 0.5) * sample / sampleMax) - 0.5; // sample centers line up with pixel centers
		double base = floor(pos);
		long first = (long)base, second = first + 1;
		if(first < 0) first = 0;
		if(second < 0) second = 0;
		if(first > (long)inSize - 1) first = (long)inSize - 1;
		if(second > (long)inSize - 1) second = (long)inSize - 1;
		resample->first[o] = (unsigned)first;
		resample->second[o] = (unsigned)second;
		resample->weight[o] = (unsigned)((pos - base) * 256.0 + 0.5);
	}
	return TRUE;
}

static void resampleFree(Jpeg_Resample* resample){ free(resample->first); free(resample->second); free(resample->weight); }

static Rasteron_Image* decoderImage(Jpeg_Decoder* decoder, const char* fileName){
	unsigned width = decoder->width, height = decoder->height;
	int compCount = decoder->compCount;
	Jpeg_Resample columns[4], rows[4];
	uint8_t* line[4] = { NULL, NULL, NULL, NULL };
	memset(columns, 0, sizeof(columns));
	memset(rows, 0, sizeof(rows));

	int ok = TRUE;
	for(int c = 0; c < compCount && ok; c++){ // coefficients to samples
		Jpeg_Component* comp = &decoder->comps[c];
		if(!decoder->quantDefined[comp->quantId]){ decoder->error = "missing quantization table"; ok = FALSE; break; }
		size_t stride = (size_t)comp->allocWide * 8;
		comp->plane = (uint8_t*)malloc(stride * comp->allocHigh * 8);
		line[c] = (uint8_t*)malloc(width);
		if(comp->plane == NULL || line[c] == NULL){ decoder->error = "out of memory"; ok = FALSE; break; }

		for(unsigned by = 0; by < comp->allocHigh; by++)
			for(unsigned bx = 0; bx < comp->allocWide; bx++)
				inverseBlock(comp->coefs + ((size_t)by * comp->allocWide + bx) * 64, decoder->quant[comp->quantId], comp->plane + ((size_t)by * 8 * stride) + ((size_t)bx * 8), stride);

		if(!resampleSetup(&columns[c], width, comp->width, comp->hSample, decoder->hMax) || !resampleSetup(&rows[c], height, comp->height, comp->vSample, decoder->vMax)){ decoder->error = "out of memory"; ok = FALSE; }
	}

	Rasteron_Image* image = ok? RASTERON_ALLOC(fileName, height, width) : NULL;
	if(image != NULL && image->data == NULL){ free(image); image = NULL; decoder->error = "out of memory"; }

	// color handling follows the Adobe segment when present, otherwise component ids
	int isRgb = compCount == 3 && (decoder->adobeTransform == 0 || (decoder->adobeTransform < 0 && !decoder->hasJfif
		&& decoder->comps[0].id == 'R' && decoder->comps[1].id == 'G' && decoder->comps[2].id == 'B'));
	int isYcck = compCount == 4 && decoder->adobeTransform == 2;

	for(unsigned y = 0; image != NULL && y < height; y++){
		for(int c = 0; c < compCount; c++){ // one row of each component at full size
			const Jpeg_Component* comp = &decoder->comps[c];
			size_t stride = (size_t)comp->allocWide * 8;
			const uint8_t* top = comp->plane + (rows[c].first[y] * stride);
			const uint8_t* bottom = comp->plane + (rows[c].second[y] * stride);
			unsigned wy = rows[c].weight[y];

			if(comp->hSample == decoder->hMax && comp->vSample == decoder->vMax) memcpy(line[c], top, width);
			else for(unsigned x = 0; x < width; x++){
				unsigned x1 = columns[c].first[x], x2 = columns[c].second[x], wx = columns[c].weight[x];
				unsigned upper = (top[x1] * (256 - wx)) + (top[x2] * wx);
				unsigned lower = (bottom[x1] * (256 - wx)) + (bottom[x2] * wx);
				line[c][x] = (uint8_t)(((upper * (256 - wy)) + (lower * wy) + 32768) >> 16);
			}
		}

		uint32_t* dest = image->data + ((size_t)y * width);
		for(unsigned x = 0; x < width; x++){
			int red, green, blue;
			if(compCount == 1) red = green = blue = line[0][x];
			else if(isRgb){ red = line[0][x]; green = line[1][x]; blue = line[2][x]; }
			else if(compCount == 3 || isYcck){
				double luma = line[0][x], cb = line[1][x] - 128.0, cr = line[2][x] - 128.0;
				red = clampByte((int)floor(luma + (1.402 * cr) + 0.5));
				green = clampByte((int)floor(luma - (0.344136 * cb) - (0.714136 * cr) + 0.5));
				blue = clampByte((int)floor(luma + (1.772 * cb) + 0.5));
				if(isYcck){ red = 255 - red; green = 255 - green; blue = 255 - blue; } // back to stored CMY
			}
			else { red = line[0][x]; green = line[1][x]; blue = line[2][x]; }

			if(compCount == 4){ // CMYK is stored inverted by Adobe software, 255 means no ink
				int black = line[3][x];
				if(decoder->adobeTransform < 0){ red = 255 - red; green = 255 - green; blue = 255 - blue; black = 255 - black; } // plain CMYK without the Adobe segment
				red = (red * black + 127) / 255; green = (green * black + 127) / 255; blue = (blue * black + 127) / 255;
			}
			dest[x] = 0xFF000000 | ((uint32_t)red << 16) | ((uint32_t)green << 8) | (uint32_t)blue;
		}
	}

	for(int c = 0; c < 4; c++){
		resampleFree(&columns[c]);
		resampleFree(&rows[c]);
		free(line[c]);
	}
	return image;
}

Rasteron_Image* loadImgOp_jpeg(const char* fileName){
	FILE* jpegFile = jpegOpen(fileName, "rb");
	if(jpegFile == NULL) return NULL; // cannot open file

	fseek(jpegFile, 0, SEEK_END);
	long fileSize = ftell(jpegFile);
	fseek(jpegFile, 0, SEEK_SET);
	uint8_t* fileData = (fileSize > 4)? (uint8_t*)malloc((size_t)fileSize) : NULL;
	if(fileData == NULL || fread(fileData, 1, (size_t)fileSize, jpegFile) != (size_t)fileSize || fileData[0] != 0xFF || fileData[1] != 0xD8){
		fclose(jpegFile);
		free(fileData);
		return NULL; // invalid file format
	}
	fclose(jpegFile);

	dctSetup();
	Jpeg_Decoder* decoder = (Jpeg_Decoder*)calloc(1, sizeof(Jpeg_Decoder));
	if(decoder == NULL){ free(fileData); return NULL; }
	Jpeg_Reader* reader = &decoder->reader;
	reader->data = fileData;
	reader->size = (size_t)fileSize;
	reader->pos = 2;
	decoder->adobeTransform = -1;

	int done = FALSE;
	while(!done && decoder->error == NULL){
		int marker = readerMarker(reader);
		if(marker == 0xD9) break; // end of image
		if(marker == 0x01 || (marker >= 0xD0 && marker <= 0xD8)) continue; // markers without a segment

		if(reader->pos + 2 > reader->size) break;
		unsigned length = read16(reader->data + reader->pos);
		if(length < 2 || reader->pos + length > reader->size) break; // file is cut short
		const uint8_t* seg = reader->data + reader->pos + 2;
		reader->pos += length;
		length -= 2;

		switch(marker){
		case 0xC0: case 0xC1: parseFrame(decoder, seg, length, FALSE); break;
		case 0xC2: parseFrame(decoder, seg, length, TRUE); break;
		case 0xC4: parseHuffman(decoder, seg, length); break;
		case 0xDB: parseQuant(decoder, seg, length); break;
		case 0xDD: if(length >= 2) decoder->restartInterval = read16(seg); break;
		case 0xDA:
			if(decodeScan(decoder, seg, length)) decoder->scanCount++;
			break;
		case 0xE0: if(length >= 5 && memcmp(seg, "JFIF", 5) == 0) decoder->hasJfif = TRUE; break;
		case 0xEE: if(length >= 12 && memcmp(seg, "Adobe", 5) == 0) decoder->adobeTransform = seg[11]; break;
		case 0xC3: case 0xC5: case 0xC6: case 0xC7: case 0xC9: case 0xCA: case 0xCB: case 0xCD: case 0xCE: case 0xCF:
			decoder->error = "lossless, hierarchical, and arithmetic coded files are not supported";
			break;
		default: break; // application data and comments are skipped
		}
	}

	Rasteron_Image* image = NULL;
	if(decoder->error == NULL && decoder->scanCount == 0) decoder->error = "no image data";
	if(decoder->error == NULL) image = decoderImage(decoder, fileName);
	if(image == NULL) fprintf(stderr, "Cannot read %s: %s\n", fileName, (decoder->error != NULL)? decoder->error : "out of memory");

	for(int c = 0; c < 4; c++){
		free(decoder->comps[c].coefs);
		free(decoder->comps[c].plane);
	}
	free(decoder);
	free(fileData);
	return image;
}

// --------------------------------   Writing    -------------------------------- //

// Tables from Annex K of the JPEG standard, quantization tables in block order

static const uint8_t _lumaQuant[64] = {
	16, 11, 10, 16, 24, 40, 51, 61, 12, 12, 14, 19, 26, 58, 60, 55,
	14, 13, 16, 24, 40, 57, 69, 56, 14, 17, 22, 29, 51, 87, 80, 62,
	18, 22, 37, 56, 68, 109, 103, 77, 24, 35, 55, 64, 81, 104, 113, 92,
	49, 64, 78, 87, 103, 121, 120, 101, 72, 92, 95, 98, 112, 100, 103, 99
};
static const uint8_t _chromaQuant[64] = {
	17, 18, 24, 47, 99, 99, 99, 99, 18, 21, 26, 66, 99, 99, 99, 99,
	24, 26, 56, 99, 99, 99, 99, 99, 47, 66, 99, 99, 99, 99, 99, 99,
	99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
	99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99
};

static const uint8_t _dcLumaCounts[16] = { 0, 1, 5, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 };
static const uint8_t _dcChromaCounts[16] = { 0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 };
static const uint8_t _dcVals[12] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

static const uint8_t _acLumaCounts[16] = { 0, 2, 1, 3, 3, 2, 4, 3, 5, 5, 4, 4, 0, 0, 1, 0x7D };
static const uint8_t _acLumaVals[162] = {
	0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12, 0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07,
	0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xA1, 0x08, 0x23, 0x42, 0xB1, 0xC1, 0x15, 0x52, 0xD1, 0xF0,
	0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0A, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x25, 0x26, 0x27, 0x28,
	0x29, 0x2A, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
	0x4A, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
	0x6A, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
	0x8A, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7,
	0xA8, 0xA9, 0xAA, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xC2, 0xC3, 0xC4, 0xC5,
	0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xE1, 0xE2,
	0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8,
	0xF9, 0xFA
};

static const uint8_t _acChromaCounts[16] = { 0, 2, 1, 2, 4, 4, 3, 4, 7, 5, 4, 4, 0, 1, 2, 0x77 };
static const uint8_t _acChromaVals[162] = {
	0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21, 0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71,
	0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91, 0xA1, 0xB1, 0xC1, 0x09, 0x23, 0x33, 0x52, 0xF0,
	0x15, 0x62, 0x72, 0xD1, 0x0A, 0x16, 0x24, 0x34, 0xE1, 0x25, 0xF1, 0x17, 0x18, 0x19, 0x1A, 0x26,
	0x27, 0x28, 0x29, 0x2A, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
	0x49, 0x4A, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
	0x69, 0x6A, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
	0x88, 0x89, 0x8A, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0xA2, 0xA3, 0xA4, 0xA5,
	0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xC2, 0xC3,
	0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA,
	0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8,
	0xF9, 0xFA
};

typedef struct { uint16_t code[256]; uint8_t size[256]; } Jpeg_Codes; // huffman code for each symbol

static void codesBuild(Jpeg_Codes* codes, const uint8_t counts[16], const uint8_t* vals){
	memset(codes, 0, sizeof(Jpeg_Codes));
	unsigned code = 0, index = 0;
	for(unsigned length = 1; length <= 16; length++){
		for(unsigned c = 0; c < counts[length - 1]; c++, code++, index++){
			codes->code[vals[index]] = (uint16_t)code;
			codes->size[vals[index]] = (uint8_t)length;
		}
		code <<= 1;
	}
}

typedef struct { FILE* file; uint32_t bits; int bitCount; } Jpeg_Writer;

static void writerBits(Jpeg_Writer* writer, unsigned value, int size){ // size up to 16
	writer->bits = (writer->bits << size) | (value & ((1u << size) - 1));
	writer->bitCount += size;
	while(writer->bitCount >= 8){
		unsigned byte = (writer->bits >> (writer->bitCount - 8)) & 0xFF;
		fputc((int)byte, writer->file);
		if(byte == 0xFF) fputc(0x00, writer->file); // keeps data from looking like a marker
		writer->bitCount -= 8;
	}
}

static void write16(FILE* file, unsigned value){ fputc((int)((value >> 8) & 0xFF), file); fputc((int)(value & 0xFF), file); }

static void writeHuffman(FILE* file, unsigned tableId, const uint8_t counts[16], const uint8_t* vals){
	unsigned total = 0;
	for(unsigned c = 0; c < 16; c++) total += counts[c];
	fputc(0xFF, file); fputc(0xC4, file);
	write16(file, 19 + total);
	fputc((int)tableId, file);
	fwrite(counts, 1, 16, file);
	fwrite(vals, 1, total, file);
}

static int bitSize(int value){ // bits needed for the magnitude
	int size = 0;
	for(value = abs(value); value > 0; value >>= 1) size++;
	return size;
}

static int writeBlock(Jpeg_Writer* writer, const float* samples, const uint8_t* quant, const Jpeg_Codes* dcCodes, const Jpeg_Codes* acCodes, int dcPred){
	float temp[64];
	int coefs[64];

	for(unsigned y = 0; y < 8; y++) // rows
		for(unsigned u = 0; u < 8; u++){
			float sum = 0.0f;
			for(unsigned x = 0; x < 8; x++) sum += samples[(y * 8) + x] * _dctBasis[u][x];
			temp[(y * 8) + u] = sum;
		}
	for(unsigned u = 0; u < 8; u++) // columns, then quantization
		for(unsigned v = 0; v < 8; v++){
			float sum = 0.0f;
			for(unsigned y = 0; y < 8; y++) sum += temp[(y * 8) + u] * _dctBasis[v][y];
			int value = (int)floorf((sum / (float)quant[(v * 8) + u]) + 0.5f);
			int limit = (u == 0 && v == 0)? 1024 : 1023; // largest values the standard tables can code
			coefs[(v * 8) + u] = (value < -limit)? -limit : (value > 1023)? 1023 : value;
		}

	int diff = coefs[0] - dcPred;
	int size = bitSize(diff);
	writerBits(writer, dcCodes->code[size], dcCodes->size[size]);
	if(size > 0) writerBits(writer, (unsigned)((diff < 0)? diff + (1 << size) - 1 : diff), size);

	int run = 0;
	for(unsigned k = 1; k < 64; k++){
		int value = coefs[_zigzag[k]];
		if(value == 0){ run++; continue; }
		for(; run > 15; run -= 16) writerBits(writer, acCodes->code[0xF0], acCodes->size[0xF0]); // sixteen zeros
		size = bitSize(value);
		writerBits(writer, acCodes->code[(run << 4) | size], acCodes->size[(run << 4) | size]);
		writerBits(writer, (unsigned)((value < 0)? value + (1 << size) - 1 : value), size);
		run = 0;
	}
	if(run > 0) writerBits(writer, acCodes->code[0x00], acCodes->size[0x00]); // end of block
	return coefs[0];
}

void writeFileImageRaw_jpegExt(const char* fileName, unsigned height, unsigned width, unsigned* data, unsigned quality){
	if(data == NULL || width == 0 || height == 0 || width > 65535 || height > 65535){
		printf("Error writing %s, jpeg size must be between 1 and 65535 pixels per side", fileName);
		return;
	}
	FILE* jpegFile = jpegOpen(fileName, "wb");
	if(jpegFile == NULL){
		printf("Error opening %s", fileName);
		return;
	}
	dctSetup();

	if(quality < 1) quality = 1;
	if(quality > 100) quality = 100;
	unsigned scale = (quality < 50)? 5000 / quality : 200 - (quality * 2);
	unsigned chromaStep = (quality >= 90)? 1 : 2; // color is stored at half size below quality 90

	uint8_t quant[2][64];
	for(unsigned i = 0; i < 64; i++){
		unsigned luma = ((_lumaQuant[i] * scale) + 50) / 100, chroma = ((_chromaQuant[i] * scale) + 50) / 100;
		quant[0][i] = (uint8_t)((luma < 1)? 1 : (luma > 255)? 255 : luma);
		quant[1][i] = (uint8_t)((chroma < 1)? 1 : (chroma > 255)? 255 : chroma);
	}

	// Writing Meta-Data

	static const uint8_t jfifHeader[20] = { 0xFF, 0xD8, 0xFF, 0xE0, 0, 16, 'J', 'F', 'I', 'F', 0, 1, 1, 0, 0, 1, 0, 1, 0, 0 };
	fwrite(jfifHeader, 1, sizeof(jfifHeader), jpegFile);

	for(unsigned t = 0; t < 2; t++){ // quantization tables
		fputc(0xFF, jpegFile); fputc(0xDB, jpegFile);
		write16(jpegFile, 67);
		fputc((int)t, jpegFile);
		for(unsigned i = 0; i < 64; i++) fputc(quant[t][_zigzag[i]], jpegFile);
	}

	fputc(0xFF, jpegFile); fputc(0xC0, jpegFile); // frame header
	write16(jpegFile, 17);
	fputc(8, jpegFile);
	write16(jpegFile, height);
	write16(jpegFile, width);
	fputc(3, jpegFile);
	fputc(1, jpegFile); fputc((int)((chromaStep << 4) | chromaStep), jpegFile); fputc(0, jpegFile);
	fputc(2, jpegFile); fputc(0x11, jpegFile); fputc(1, jpegFile);
	fputc(3, jpegFile); fputc(0x11, jpegFile); fputc(1, jpegFile);

	writeHuffman(jpegFile, 0x00, _dcLumaCounts, _dcVals);
	writeHuffman(jpegFile, 0x10, _acLumaCounts, _acLumaVals);
	writeHuffman(jpegFile, 0x01, _dcChromaCounts, _dcVals);
	writeHuffman(jpegFile, 0x11, _acChromaCounts, _acChromaVals);

	static const uint8_t scanHeader[14] = { 0xFF, 0xDA, 0, 12, 3, 1, 0x00, 2, 0x11, 3, 0x11, 0, 63, 0 };
	fwrite(scanHeader, 1, sizeof(scanHeader), jpegFile);

	// Writing Data

	Jpeg_Codes* codes = (Jpeg_Codes*)malloc(4 * sizeof(Jpeg_Codes)); // DC luma, AC luma, DC chroma, AC chroma
	if(codes == NULL){ fclose(jpegFile); return; }
	codesBuild(&codes[0], _dcLumaCounts, _dcVals);
	codesBuild(&codes[1], _acLumaCounts, _acLumaVals);
	codesBuild(&codes[2], _dcChromaCounts, _dcVals);
	codesBuild(&codes[3], _acChromaCounts, _acChromaVals);

	Jpeg_Writer writer = { jpegFile, 0, 0 };
	unsigned mcuSize = 8 * chromaStep;
	int dcPred[3] = { 0, 0, 0 };
	float luma[256], cb[256], cr[256], block[64]; // one MCU, up to 16 by 16 pixels

	for(unsigned mcuY = 0; mcuY < height; mcuY += mcuSize)
		for(unsigned mcuX = 0; mcuX < width; mcuX += mcuSize){
			for(unsigned y = 0; y < mcuSize; y++){ // color conversion, edge pixels repeat past the border
				unsigned srcY = (mcuY + y < height)? mcuY + y : height - 1;
				for(unsigned x = 0; x < mcuSize; x++){
					unsigned srcX = (mcuX + x < width)? mcuX + x : width - 1;
					unsigned color = *(data + ((size_t)srcY * width) + srcX);
					float red = (float)((color >> 16) & 0xFF), green = (float)((color >> 8) & 0xFF), blue = (float)(color & 0xFF);
					luma[(y * mcuSize) + x] = (0.299f * red) + (0.587f * green) + (0.114f * blue) - 128.0f;
					cb[(y * mcuSize) + x] = (-0.168736f * red) - (0.331264f * green) + (0.5f * blue);
					cr[(y * mcuSize) + x] = (0.5f * red) - (0.418688f * green) - (0.081312f * blue);
				}
			}

			for(unsigned by = 0; by < chromaStep; by++) // brightness blocks
				for(unsigned bx = 0; bx < chromaStep; bx++){
					for(unsigned y = 0; y < 8; y++)
						for(unsigned x = 0; x < 8; x++) block[(y * 8) + x] = luma[(((by * 8) + y) * mcuSize) + (bx * 8) + x];
					dcPred[0] = writeBlock(&writer, block, quant[0], &codes[0], &codes[1], dcPred[0]);
				}

			for(unsigned c = 0; c < 2; c++){ // color blocks, averaged when stored at half size
				const float* chroma = (c == 0)? cb : cr;
				for(unsigned y = 0; y < 8; y++)
					for(unsigned x = 0; x < 8; x++){
						float sum = 0.0f;
						for(unsigned sy = 0; sy < chromaStep; sy++)
							for(unsigned sx = 0; sx < chromaStep; sx++) sum += chroma[(((y * chromaStep) + sy) * mcuSize) + (x * chromaStep) + sx];
						block[(y * 8) + x] = sum / (float)(chromaStep * chromaStep);
					}
				dcPred[c + 1] = writeBlock(&writer, block, quant[1], &codes[2], &codes[3], dcPred[c + 1]);
			}
		}

	if(writer.bitCount > 0) writerBits(&writer, 0xFF, 8 - writer.bitCount); // pads the last byte with ones
	fputc(0xFF, jpegFile); fputc(0xD9, jpegFile); // end of image

	free(codes);
	fclose(jpegFile);
}

void writeFileImageRaw_jpeg(const char* fileName, unsigned height, unsigned width, unsigned* data){
	writeFileImageRaw_jpegExt(fileName, height, width, data, JPEG_QUALITY);
}

#endif
