#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define bool char
#define true 1
#define false 0
#define DISPLAY_WIDTH   96
#define DISPLAY_HEIGHT  64

#define min(a, b) ((a) < (b) ? (a) : (b)) 
#define max(a, b) ((a) > (b) ? (a) : (b)) 
#define spicolor565(r,g,b) (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3))

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

static uint16_t read16(FILE* fd)
{
    uint16_t val = 0;
    fread(&val, sizeof(val), 1, fd);
    return val;
}

static uint32_t read32(FILE* fd)
{
    uint32_t val = 0;
    fread(&val, sizeof(val), 1, fd);
    return val;
}

#define BUFFPIXEL 32

char compareFrames(uint16_t* curr, uint16_t* prev, int* x, int* y, int* width, int* height)
{
    *y = 0;
    *height = DISPLAY_HEIGHT;
    // Find first mismatched y row
    for (int yy = 0; yy < DISPLAY_HEIGHT; yy++)
    {
        if (memcmp(&curr[DISPLAY_WIDTH*yy], &prev[DISPLAY_WIDTH*yy], DISPLAY_WIDTH*2) != 0)
        {
            *y = yy;
            *height -= yy;
            break;
        }
    }
    // Find last mismatched y row
    for (int yy = DISPLAY_HEIGHT-1; yy >= *y; yy--)
    {
        if (memcmp(&curr[DISPLAY_WIDTH*yy], &prev[DISPLAY_WIDTH*yy], DISPLAY_WIDTH*2) != 0)
        {
            *height -= DISPLAY_HEIGHT - yy;
            break;
        }
    }
    // Find first mismatched x row
    *x = DISPLAY_WIDTH;
    *width = 0;
    bool mismatch = false;
    for (int yy = *y; yy < *y + *height; yy++)
    {
        for (int xx = 0; xx < DISPLAY_WIDTH; xx++)
        {
            if (curr[DISPLAY_WIDTH*yy+xx] != prev[DISPLAY_WIDTH*yy+xx])
            {
                *x = min(xx, *x);
                mismatch = true;
                break;
            }
        }
        for (int xx = DISPLAY_WIDTH-1; xx >= *x; xx--)
        {
            if (curr[DISPLAY_WIDTH*yy+xx] != prev[DISPLAY_WIDTH*yy+xx])
            {
                *width = max(xx-*x, *width);
                mismatch = true;
                break;
            }
        }
    }
    // Return 0 if frames match
    if (!mismatch)
    {
        // printf("IDENTICAL FRAMES\n");
        *x = *y = *width = *height = 0;
        return 0;
    }
    // printf("DIFF [%d,%d,%d,%d]\n", *x, *y, *width, *height);
    if (*x == 0 && *y == 0 && *width == DISPLAY_WIDTH && *height == DISPLAY_HEIGHT)
    {
        // Return -1 for new keyframe
        return -1;
    }
    // Return 1 if incremental update
    return 1;
}

int convertBmp(FILE* inFD, FILE* outFD, uint16_t* previousBuffer, uint32_t frameCount)
{
    int x = 0, y = 0;
    uint16_t outBuffer[DISPLAY_WIDTH*DISPLAY_HEIGHT];
    uint16_t* out = outBuffer;
    memset(outBuffer, 0, sizeof(outBuffer));
    FILE* bmpFile = inFD;
    if (read16(bmpFile) == 0x4D42)
    {
        bool flip = true;
        uint32_t fileSize = read32(bmpFile);
        uint32_t creatorBytes = read32(bmpFile);
        uint32_t bmpImageOffset = read32(bmpFile);
        uint32_t headerSize = read32(bmpFile);
        int bmpWidth = (int)read32(bmpFile);
        int bmpHeight = (int)read32(bmpFile);
        uint8_t sdbuffer[4*BUFFPIXEL];
        uint8_t buffidx = sizeof(sdbuffer);
        uint32_t pos = 0;
        if (read16(bmpFile) == 1)
        {
            uint16_t bmpDepth = read16(bmpFile);
            if (bmpDepth == 32 && read32(bmpFile) == 0) // uncompressed 32-bit
            {
                uint32_t rowSize = bmpWidth * 4;
                if (bmpHeight < 0)
                {
                    bmpHeight = -bmpHeight;
                    flip = false;
                }
                int w = bmpWidth;
                int h = bmpHeight;
                if ((x+w-1) >= DISPLAY_WIDTH) w = DISPLAY_WIDTH - x;
                if ((y+h-1) >= DISPLAY_HEIGHT) h = DISPLAY_HEIGHT - y;
                for (int row = 0; row < h; row++)
                {
                    if (flip)
                        pos = bmpImageOffset + (bmpHeight - 1 - row) * rowSize;
                    else
                        pos = bmpImageOffset + row * rowSize;
                    if (ftell(bmpFile) != pos)
                    {
                        fseek(bmpFile, pos, SEEK_SET);
                        buffidx = sizeof(sdbuffer);
                    }
                    for (int col = 0; col < w; col++)
                    {
                        if (buffidx >= sizeof(sdbuffer))
                        {
                            fread(sdbuffer, 1, sizeof(sdbuffer), bmpFile);
                            buffidx = 0;
                        }
                        uint8_t b = sdbuffer[buffidx++];
                        uint8_t g = sdbuffer[buffidx++];
                        uint8_t r = sdbuffer[buffidx++];
                        buffidx++;

                        *out++ = spicolor565(r, g, b);
                    }
                }
                char frameType = (frameCount > 0) ? compareFrames(outBuffer, previousBuffer, &x, &y, &bmpWidth, &bmpHeight) : -1;
                printf("frameType : %d\n", frameType);
                fwrite(&frameType, 1, sizeof(frameType), outFD);
                if (frameType == 1)
                {
                    char cx = (char)x;
                    char cy = (char)y;
                    char cw = (char)bmpWidth;
                    char ch = (char)bmpHeight;
                    fwrite(&cx, 1, 1, outFD);
                    fwrite(&cy, 1, 1, outFD);
                    fwrite(&cw, 1, 1, outFD);
                    fwrite(&ch, 1, 1, outFD);
                    for (int yy = y; yy < y+bmpHeight; yy++)
                    {
                        fwrite(&outBuffer[yy*DISPLAY_WIDTH+x], sizeof(uint16_t), bmpWidth, outFD);
                    }
                }
                else if (frameType == -1)
                {
                    fwrite(outBuffer, 1, sizeof(outBuffer), outFD);
                }
                memcpy(previousBuffer, outBuffer, sizeof(outBuffer));
                return 0;
            }
        }
    }
    printf("Unsupported format\n");
    return 1;
}

int main(int argc, const char* argv[])
{
    int failed = 0;
    int32_t frameCount = 0;
    char fnameBuffer[1000];
    FILE* bmpFile;
    FILE* bd2File = NULL;
    /* Frame rate defaults to 10 which is about all the Arduino can reliably handle */
    const char* sourceFileDir = argv[1];
    int32_t fps = (argc == 3) ? atoi(argv[2]) : 10;
    if (argc != 2)
    {
        fprintf(stderr, "Usage:\n%s <directoryname> [fps]\n", argv[0]);
        return 1;
    }
    do
    {
        sprintf(fnameBuffer, "%s/%d.bmp", argv[1], frameCount+1);
        bmpFile = fopen(fnameBuffer, "rb");
        if (bmpFile != NULL)
        {
            printf("Reading %s\n", fnameBuffer);
            if (bd2File == NULL)
            {
                // write initial frameCount to reserve space in output file
                sprintf(fnameBuffer, "%s.bd2", argv[1]);
                bd2File = fopen(fnameBuffer, "wb+");
                fwrite(&frameCount, sizeof(frameCount), 1, bd2File);
                fwrite(&fps, sizeof(fps), 1, bd2File);
            }
            static uint16_t sPreviousFrame[DISPLAY_WIDTH*DISPLAY_HEIGHT];
            if (convertBmp(bmpFile, bd2File, sPreviousFrame, frameCount) != 0)
            {
                failed = 1;
            }
            fclose(bmpFile);
            frameCount++;
        }
    }
    while (!failed && bmpFile != NULL);
    if (bd2File != NULL)
    {
        // update final framecount
        fseek(bd2File, 0, SEEK_SET);
        fwrite(&frameCount, sizeof(frameCount), 1, bd2File);
        fclose(bmpFile);
    }
    return failed;
}
