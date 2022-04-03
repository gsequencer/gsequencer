/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#ifndef __AGS_VST_MEMORY_STREAM_H__
#define __AGS_VST_MEMORY_STREAM_H__

#include <ags/vst3-capi/pluginterfaces/base/ags_vst_ibstream.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct MemoryStream AgsVstMemoryStream;

  AgsVstMemoryStream* ags_vst_memory_stream_new();
  AgsVstMemoryStream* ags_vst_memory_stream_new_with_buffer(void *memory, gsize memory_size);
  
#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_MEMORY_STREAM_H__*/
