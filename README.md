# Questions

## What's `stdint.h`?

<stdint.h> defines sets of integer types having specified widths and the corresponding sets of macros.

## What's the point of using `uint8_t`, `uint32_t`, `int32_t`, and `uint16_t` in a program?

To use different types of integers that can represent different ranges of integers.

## How many bytes is a `BYTE`, a `DWORD`, a `LONG`, and a `WORD`, respectively?

1, 4, 4, 2

## What (in ASCII, decimal, or hexadecimal) must the first two bytes of any BMP file be? Leading bytes used to identify file formats (with high probability) are generally called "magic numbers."

424d

## What's the difference between `bfSize` and `biSize`?

biSize refers to the size of the header (general information), while bfSize refers to the size of the whole BMP file (including the header and the image).

## What does it mean if `biHeight` is negative?

This indicates that the bitmap is top-down and its origin is the upper-left corner.

## What field in `BITMAPINFOHEADER` specifies the BMP's color depth (i.e., bits per pixel)?

biBitCount

## Why might `fopen` return `NULL` in lines 24 and 32 of `copy.c`?

Because opening the specified file could fail due to reasons including that the file doesn't exist, the access to the particular file is denied, etc.

## Why is the third argument to `fread` always `1` in our code?

There's only one BITMAPFILEHEADER and one BITMAPINFOHEADER respectively in each BMP file.

## What value does line 63 of `copy.c` assign to `padding` if `bi.biWidth` is `3`?

3

## What does `fseek` do?

It sets the file position indicator a specified number of bytes away from the specified origin in a specified file.

## What is `SEEK_CUR`?

It refers to the current position indicator.
