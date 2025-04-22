#ifndef JPEGWRAP_H
#define JPEGWRAP_H
#include <cstdio>
#include <string.h>
#include <stdlib.h>
#include <memory>
#include <jerror.h>
#include <jpeglib.h>
#include <setjmp.h>

namespace Jpeg {

    struct Error {
        struct jpeg_error_mgr pub;
        jmp_buf setjmp_buffer;
    };
    enum Align {
         ALIGN_0 = 0,
         ALIGN_4
    };
    enum Scale {
        SCALE_D1 = 1,
        SCALE_D2 = 2,
        SCALE_D4 = 4,
        SCALE_D8 = 8
    };

    void errorNotify(j_common_ptr cinfo);
    inline int align4(int width, int channel) {return (width*channel+3)/4*4;}
    int encode(unsigned char*& jpeg, unsigned long &totalsize,
               unsigned char* img, int w, int h, int quality=90);
    int decode(unsigned char *rgb, int &w, int &h,
               unsigned char *jpeg,  unsigned long totalsize, int scale = SCALE_D1, int align=ALIGN_4);
    int load(const char* filename, std::shared_ptr<uint8_t[]>& img, int &h, int &w, int &c);
    int save(const char* filename, unsigned char* img, int h, int w, int c, int quality=90);
}


#endif // JPEGWRAP_H
