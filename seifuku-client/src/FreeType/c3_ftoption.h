/***************************************************************************/
/*                                                                         */
/*  ftoption.h                                                             */
/*                                                                         */
/*    User-selectable configuration macros (specification only).           */
/*                                                                         */
/*  Copyright 1996-2001, 2002, 2003, 2004, 2005 by                         */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/

#ifndef __FTOPTION_H__
#define __FTOPTION_H__


#include <ft2build.h>


FT_BEGIN_HEADER

#undef  FT_CONFIG_OPTION_FORCE_INT64

#define FT_CONFIG_OPTION_USE_LZW

#define FT_CONFIG_OPTION_USE_ZLIB

#define FT_CONFIG_OPTION_POSTSCRIPT_NAMES

#define FT_CONFIG_OPTION_ADOBE_GLYPH_LIST

#undef FT_CONFIG_OPTION_MAC_FONTS

#ifdef FT_CONFIG_OPTION_MAC_FONTS
#define FT_CONFIG_OPTION_GUESSING_EMBEDDED_RFORK
#endif

#define FT_RENDER_POOL_SIZE  16384L

#define FT_MAX_MODULES  32

#undef FT_CONFIG_OPTION_USE_MODULE_ERRORS

#define TT_CONFIG_OPTION_EMBEDDED_BITMAPS

#define TT_CONFIG_OPTION_POSTSCRIPT_NAMES

#define TT_CONFIG_OPTION_SFNT_NAMES

#define TT_CONFIG_CMAP_FORMAT_0
#define TT_CONFIG_CMAP_FORMAT_2
#define TT_CONFIG_CMAP_FORMAT_4
#define TT_CONFIG_CMAP_FORMAT_6
#define TT_CONFIG_CMAP_FORMAT_8
#define TT_CONFIG_CMAP_FORMAT_10
#define TT_CONFIG_CMAP_FORMAT_12

#define TT_CONFIG_OPTION_BYTECODE_INTERPRETER

#define TT_CONFIG_OPTION_UNPATENTED_HINTING

#define TT_CONFIG_OPTION_INTERPRETER_SWITCH

#undef TT_CONFIG_OPTION_COMPONENT_OFFSET_SCALED

#define TT_CONFIG_OPTION_GX_VAR_SUPPORT

#define T1_MAX_DICT_DEPTH  5

#define T1_MAX_SUBRS_CALLS  16

#define T1_MAX_CHARSTRINGS_OPERANDS  256

#undef T1_CONFIG_OPTION_NO_AFM

#undef T1_CONFIG_OPTION_NO_MM_SUPPORT

FT_END_HEADER

#endif
