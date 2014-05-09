// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GFX_TEXT_UTILS_H_
#define UI_GFX_TEXT_UTILS_H_

#include "base/strings/string16.h"
#include "ui/gfx/gfx_export.h"

namespace gfx {

class FontList;

// Strip the accelerator char (typically '&') from a menu string.  A double
// accelerator char ('&&') will be converted to a single char.  The out params
// |accelerated_char_pos| and |accelerated_char_span| will be set to the index
// and span of the last accelerated character, respectively, or -1 and 0 if
// there was none.
UI_EXPORT base::string16 RemoveAcceleratorChar(const base::string16& s,
                                               base::char16 accelerator_char,
                                               int* accelerated_char_pos,
                                               int* accelerated_char_span);

// Returns the number of horizontal pixels needed to display the specified
// |text| with |font_list|.
UI_EXPORT int GetStringWidth(const base::string16& text,
                             const FontList& font_list);

}  // namespace gfx

//#if 0 // NO_I18N
typedef int UChar32;
#define U16_IS_TRAIL(c) (((c)&0xfffffc00)==0xdc00)
#define U16_IS_LEAD(c) (((c)&0xfffffc00)==0xd800)

#define U16_SET_CP_START(s, start, i) { \
    if(U16_IS_TRAIL((s)[i]) && (i)>(start) && U16_IS_LEAD((s)[(i)-1])) { \
    --(i); \
    } \
}
#define U16_SET_CP_LIMIT(s, start, i, length) { \
    if((start)<(i) && (i)<(length) && U16_IS_LEAD((s)[(i)-1]) && U16_IS_TRAIL((s)[i])) { \
    ++(i); \
    } \
}
#define U16_SURROGATE_OFFSET ((0xd800<<10UL)+0xdc00-0x10000)
#define U16_GET_SUPPLEMENTARY(lead, trail) \
    (((UChar32)(lead)<<10UL)+(UChar32)(trail)-U16_SURROGATE_OFFSET)
#define U16_NEXT(s, i, length, c) { \
    (c)=(s)[(i)++]; \
    if(U16_IS_LEAD(c)) { \
    uint16_t __c2; \
    if((i)<(length) && U16_IS_TRAIL(__c2=(s)[(i)])) { \
    ++(i); \
    (c)=U16_GET_SUPPLEMENTARY((c), __c2); \
    } \
    } \
}

#endif  // UI_GFX_TEXT_UTILS_H_
