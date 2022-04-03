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

#include <ags/vst3-capi/public.sdk/source/common/ags_vst_memory_stream.h>

#include <public.sdk/source/common/memorystream.h>

extern "C" {

  AgsVstMemoryStream* ags_vst_memory_stream_new()
  {
    return((AgsVstMemoryStream *) (new Steinberg::MemoryStream()));
  }

  AgsVstMemoryStream* ags_vst_memory_stream_new_with_buffer(void *memory, gsize memory_size)
  {
    return((AgsVstMemoryStream *) (new Steinberg::MemoryStream(memory, (Steinberg::TSize) memory_size)));
  }

}
