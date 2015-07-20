#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
int StrToInt64(const char *string, int64_t *dest);
int StrToFloat(const char *string, float *dest);

/* Prefixed versions do NOT have '0', '0x', or '0b' in their strings, or a '-' or '+' in front.
 * They may also leave `dest' in an inconsistent state on error. */
int DecStrToInt64(const char *string, uint64_t *dest);
int HexStrToInt64(const char *string, uint64_t *dest);
int OctStrToInt64(const char *string, uint64_t *dest);
int BinStrToInt64(const char *string, uint64_t *dest);

int IsDecDigit(char c);
int IsHexDigit(char c);
int IsOctDigit(char c);
int IsBinDigit(char c);

/* Return undefined if !IsHexDigit(c) */
unsigned HexDigitValue(char c);

#ifdef __cplusplus
}
#endif
