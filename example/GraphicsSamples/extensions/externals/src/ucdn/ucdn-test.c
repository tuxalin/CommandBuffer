/*
 * Copyright (C) 2012 Grigori Goronzy <greg@kinoho.net>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include "ucdn.h"

int main(int argc, char **argv)
{
    uint32_t codepoint, a, b;
    int i, len;
    uint32_t decomposed[18];

    if (argc < 2)
        return EXIT_FAILURE;

    codepoint = (uint32_t)strtol(argv[1], NULL, 0);

    printf("codepoint U+%04X\n", codepoint);
    printf("combining_class %d\n", ucdn_get_combining_class(codepoint));
    printf("general_category %d\n", ucdn_get_general_category(codepoint));
    if (ucdn_get_mirrored(codepoint))
        printf("mirrored U+%04X\n", ucdn_mirror(codepoint));
    printf("eastasian_width %d\n", ucdn_get_east_asian_width(codepoint));
    printf("script %d\n", ucdn_get_script(codepoint));
    printf("bidi_class %d\n", ucdn_get_bidi_class(codepoint));

    if ((len = ucdn_compat_decompose(codepoint, decomposed))) {
        printf("compatibility_decomposition");
        for (i = 0; i < len; i++) {
            printf(" U+%04X", decomposed[i]);
        }
        printf("\n");
    }

    if (ucdn_decompose(codepoint, &a, &b))
        printf("decomposition U+%04X U+%04X\n", a, b);

    if (ucdn_compose(&codepoint, a, b))
        printf("recomposition U+%04X\n", codepoint);

    return EXIT_SUCCESS;
}
