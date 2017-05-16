/*
 * libAC780.inc
 *
 * vim:set ft=cpp:
 *
 * Copyright (C) 2017 Benoit Rapidel <benoit.rapidel+devs@exmachina.fr>
 *
 * Distributed under terms of the MIT license.
 */

const char UDC_bar_left_empty[] = {0x0,0xf,0x8,0x8,0x8,0x8,0xf,0x0};
const char UDC_bar_left_full[] = {0x0,0xf,0x8,0xb,0xb,0x8,0xf,0x0};
const char UDC_bar_middle_empty[] = {0x0,0x1f,0x0,0x0,0x0,0x0,0x1f,0x0};
const char UDC_bar_middle_full[] = {0x0,0x1f,0x0,0x1f,0x1f,0x0,0x1f,0x0};
const char UDC_bar_right_empty[] = {0x0,0x1e,0x2,0x2,0x2,0x2,0x1e,0x0};
const char UDC_bar_right_full[] = {0x0,0x1e,0x2,0x1a,0x1a,0x2,0x1e,0x0};