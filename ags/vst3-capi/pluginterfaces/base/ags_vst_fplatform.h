/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
 *
 * This file is part of GSequencer.
 *
 * GSequencer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GSequencer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GSequencer.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __AGS_VST_FPLATFORM_H__
#define __AGS_VST_FPLATFORM_H__

#include <glib.h>

#include <ags/ags_api_config.h>

#define AGS_VST_KLITTLE_ENDIAN    (0)
#define AGS_VST_KBIG_ENDIAN       (1)

#if defined(__APPLE__) || defined(__FreeBSD__) || defined(__DragonFly__)
#include <machine/endian.h>
#elif defined(AGS_W32API)
#else
#include <endian.h>
#endif

#if __BYTE_ORDER == __LITTLE_ENDIAN
#define AGS_VST_BYTEORDER AGS_VST_KLITTLE_ENDIAN
#else
#define AGS_VST_BYTEORDER AGS_VST_KBIG_ENDIAN
#endif

#ifdef __cplusplus
extern "C" {
#endif

  
  
#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_FPLATFORM_H__*/
