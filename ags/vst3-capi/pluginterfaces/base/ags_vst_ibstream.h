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

#ifndef __AGS_VST_IBSTREAM_H__
#define __AGS_VST_IBSTREAM_H__

#include <ags/vst3-capi/pluginterfaces/base/ags_vst_funknown.h>

#define AGS_VST_IBSTREAM_IID (ags_vst_ibstream_get_iid())
#define AGS_VST_ISIZEABLE_STREAM_IID (ags_vst_isizeable_stream_get_iid())

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct IBStream AgsVstIBStream;

  typedef struct ISizeableStream AgsVstISizeableStream;  

  enum AgsVstIStreamSeekMode
  {
    AGS_VST_KIB_SEEK_SET = 0,
    AGS_VST_KIB_SEEK_CUR,
    AGS_VST_KIB_SEEK_END,
  };

  AgsVstTUID* ags_vst_ibstream_get_iid();

  AgsVstTResult ags_vst_ibstream_read(AgsVstIBStream *ib_stream, gpointer buffer, gint32 num_bytes, gint32 *num_bytes_read);
  AgsVstTResult ags_vst_ibstream_write(AgsVstIBStream *ib_stream, gpointer buffer, gint32 num_bytes, gint32 *num_bytes_written);

  AgsVstTResult ags_vst_ibstream_seek(AgsVstIBStream *ib_stream, gint64 pos, gint32 mode, gint64 *result);

  AgsVstTResult ags_vst_ibstream_tell(AgsVstIBStream *ib_stream, gint64 *pos);

  AgsVstTUID* ags_vst_isizeable_stream_get_iid();

  AgsVstTResult ags_vst_isizeable_stream_set_stream_size(AgsVstISizeableStream *isizeable_stream, gint64 stream_size);
  
#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_IBSTREAM_H__*/
