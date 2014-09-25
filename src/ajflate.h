
#ifndef AJFLATE_H
#define AJFLATE_H

#ifdef __cplusplus
extern "C" {
#endif


unsigned char* make_gzip(unsigned int* sizeOut, const unsigned char* src, unsigned int srcSize);


#ifdef __cplusplus
}
#endif

#endif /* AJFLATE_H */

