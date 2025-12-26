#include "common.h"

// src から dst に n バイト分コピーする
void *memcpy(void *dst, const void *src, size_t n) {
    uint8_t *d = (uint8_t *) dst;
    const uint8_t *s = (const uint8_t *) src;
    while (n--)
        *d++ = *s++;
    return dst;
}

// buf の先頭から n バイト分を c で埋める
void *memset(void *buf, char c, size_t n) {
    uint8_t *p = (uint8_t *) buf;
    while (n--)
        *p++ = c; // *p=c; p++;
    return buf;
}

// src を dst にコピーする (\0 終端まで)
// ※ メモリリークの危険があるため、通常は使用しない.
// src のサイズが dst を超えないことを仮定する
char *strcpy(char *dst, const char *src) {
    char *d = dst;
    while (*src)
        *d++ = *src++;
    *d = '\0';
    return dst;
}

// s1 と s2 を比較する
// 0(false) のとき等しい.
int strcmp(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        if (*s1 != *s2)
            break;
        s1++;
        s2++;
    }

    // POSIX の仕様に準拠し, `unsigned char*` にキャストしてから比較する
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}