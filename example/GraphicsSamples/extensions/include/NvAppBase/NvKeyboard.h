//----------------------------------------------------------------------------------
// File:        NvAppBase/NvKeyboard.h
// SDK Version: v3.00 
// Email:       gameworks@nvidia.com
// Site:        http://developer.nvidia.com/
//
// Copyright (c) 2014-2015, NVIDIA CORPORATION. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of NVIDIA CORPORATION nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//----------------------------------------------------------------------------------

#ifndef NV_KEYBOARD_H
#define NV_KEYBOARD_H

#include <NvSimpleTypes.h>

/// \file
/// Cross-platform keycode representation

/// Cross-platform keycodes.
struct NvKey {
    // this is currently designed around
    // us keyboard mappings for simplicity,
    // and quoted ASCII literals for values
    // where appropriate.
    // this could easily be changed to use
    // codes from say the USB HID usage table
    // for keyboard scan codes.

    // other design note is I've tried to keep
    // keycodes in the lower 16-bits, and bit
    // flags in the upper 16.  So if you mask
    // out the high 16, you'll get non-unique
    // modifiers, AND we can pass mod+key
    // into functions that don't accept
    // modifiers as bindable keys.
    enum Enum {
        K_UNKNOWN = 0x80000000, ///< high bit + zero flags unknown value.

        K_BACK = 0x04, // mapping android BACK button eventcode value.

        K_ESC = 0x1B, // ascii
        K_FKEY_BIT = 0x2000,
        K_F1 = K_FKEY_BIT+1,
        K_F2 = K_FKEY_BIT+2,
        K_F3 = K_FKEY_BIT+3,
        K_F4 = K_FKEY_BIT+4,
        K_F5 = K_FKEY_BIT+5,
        K_F6 = K_FKEY_BIT+6,
        K_F7 = K_FKEY_BIT+7,
        K_F8 = K_FKEY_BIT+8,
        K_F9 = K_FKEY_BIT+9,
        K_F10 = K_FKEY_BIT+10,
        K_F11 = K_FKEY_BIT+11,
        K_F12 = K_FKEY_BIT+12,

        K_EXTRA_KEY_BIT = 0x1000,
        K_PRINT_SCREEN = K_EXTRA_KEY_BIT+1,
        K_SCROLL_LOCK = K_EXTRA_KEY_BIT+2,
        K_PAUSE = K_EXTRA_KEY_BIT+3,

        K_INSERT = K_EXTRA_KEY_BIT+17,
        K_DELETE = K_EXTRA_KEY_BIT+18, // or ascii 0x7F
        K_HOME = K_EXTRA_KEY_BIT+19,
        K_END = K_EXTRA_KEY_BIT+20,
        K_PAGE_UP = K_EXTRA_KEY_BIT+21,
        K_PAGE_DOWN = K_EXTRA_KEY_BIT+22,

        K_ARROW_UP = K_EXTRA_KEY_BIT+33,
        K_ARROW_DOWN = K_EXTRA_KEY_BIT+34,
        K_ARROW_LEFT = K_EXTRA_KEY_BIT+35,
        K_ARROW_RIGHT = K_EXTRA_KEY_BIT+36,

        K_ACCENT_GRAVE = '`',
        K_0 = '0',
        K_1 = '1',
        K_2 = '2',
        K_3 = '3',
        K_4 = '4',
        K_5 = '5',
        K_6 = '6',
        K_7 = '7',
        K_8 = '8',
        K_9 = '9',
        K_MINUS = '-',
        K_EQUAL = '=',
        K_BACKSPACE = 0x08, // ascii

        K_TAB = '\t',
        K_Q = 'Q',
        K_W = 'W',
        K_E = 'E',
        K_R = 'R',
        K_T = 'T',
        K_Y = 'Y',
        K_U = 'U',
        K_I = 'I',
        K_O = 'O',
        K_P = 'P',
        K_LBRACKET = '[',
        K_RBRACKET = ']',
        K_BACKSLASH = '\\',

        K_CAPS_LOCK = K_EXTRA_KEY_BIT+4, // !!!!TBD after K_PAUSE
        K_A = 'A',
        K_S = 'S',
        K_D = 'D',
        K_F = 'F',
        K_G = 'G',
        K_H = 'H',
        K_J = 'J',
        K_K = 'K',
        K_L = 'L',
        K_SEMICOLON = ';',
        K_APOSTROPHE = '\'',
        K_ENTER = '\n', // ascii LF 0x10

        K_MOD_BASE_BIT = 0x8000, // all modifier keys are in this 'class'.
        // if we want left/right distinguished only by high flags, keep this in upper 16
        // otherwise, can drop it to say 0x100 and make L/R keys unique.
        K_MOD_RIGHT_BIT = 0x100000, // 'flag' bit for the 'right' version of left/right keys
        K_MOD_SHIFT_BIT = 0x200000, // high 'flag' bit for shift
        K_MOD_CTRL_BIT = 0x400000, // high 'flag' bit for ctrl
        K_MOD_ALT_BIT = 0x800000, // high 'flag' bit for alt

        K_MOD_WINDOWS_BIT = 0x1000000, // 'flag' bit for windows key for now. !!!!TBD

        K_SHIFT_ANY = K_MOD_BASE_BIT + 1,
        K_CTRL_ANY = K_MOD_BASE_BIT + 2,
        K_ALT_ANY = K_MOD_BASE_BIT + 3,
        K_WINDOWS_ANY = K_MOD_BASE_BIT + 4,

        K_SHIFT_LEFT = K_MOD_SHIFT_BIT + K_SHIFT_ANY, // so this is curr 0x00208001
        K_Z = 'Z',
        K_X = 'X',
        K_C = 'C',
        K_V = 'V',
        K_B = 'B',
        K_N = 'N',
        K_M = 'M',
        K_COMMA = ',',
        K_PERIOD = '.',
        K_SLASH = '/',
        K_SHIFT_RIGHT = K_MOD_SHIFT_BIT + K_SHIFT_ANY + K_MOD_RIGHT_BIT,

        K_CTRL_LEFT = K_MOD_CTRL_BIT + K_CTRL_ANY,
        K_WINDOWS_LEFT = K_MOD_WINDOWS_BIT + K_WINDOWS_ANY,
        K_ALT_LEFT = K_MOD_ALT_BIT + K_ALT_ANY,
        K_SPACE = ' ',
        K_ALT_RIGHT = K_MOD_ALT_BIT + K_ALT_ANY + K_MOD_RIGHT_BIT,
        K_WINDOWS_RIGHT = K_MOD_WINDOWS_BIT + K_WINDOWS_ANY + K_MOD_RIGHT_BIT,
        K_CONTEXT = K_EXTRA_KEY_BIT+5, // !!!!TBD context menu key, put after caps
        K_CTRL_RIGHT = K_MOD_CTRL_BIT + K_CTRL_ANY + K_MOD_RIGHT_BIT,

        K_KP_BIT = 0x4000,
        K_NUMLOCK = K_KP_BIT+ 0x101, // !!!!TBD just something...
        K_KP_DIVIDE = K_KP_BIT + '/',
        K_KP_MULTIPLY = K_KP_BIT + '*',
        K_KP_SUBTRACT = K_KP_BIT + '-',
        K_KP_ADD = K_KP_BIT + '+',
        K_KP_ENTER = K_KP_BIT + '/',
        K_KP_DECIMAL = K_KP_BIT + '.',
        K_KP_0 = K_KP_BIT + '0',
        K_KP_1 = K_KP_BIT + '1',
        K_KP_2 = K_KP_BIT + '2',
        K_KP_3 = K_KP_BIT + '3',
        K_KP_4 = K_KP_BIT + '4',
        K_KP_5 = K_KP_BIT + '5',
        K_KP_6 = K_KP_BIT + '6',
        K_KP_7 = K_KP_BIT + '7',
        K_KP_8 = K_KP_BIT + '8',
        K_KP_9 = K_KP_BIT + '9',

        // !!!!TBD TODO ???? other common keys? windows, win8, ios, winrt, android...
        // volume: +/-/mute
        // media: next/prev/pause/play/launch-player
        // web: home/refresh/stop/next/prev
        // other launcher keys: mail, calc, file explorer.  photos/gallery.
        // lock key, unlock key
    };
};

#endif
