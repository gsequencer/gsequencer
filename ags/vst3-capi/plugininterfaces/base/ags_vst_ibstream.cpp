/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/vst3-capi/plugininterfaces/base/ags_vst_funknown.h>

#include <plugininterfaces/base/ibstream.h>

extern "C" {

  AgsVstTUID
  ags_vst_ibstream_get_iid()
  {
    return(IBStream__iid);
  }

  gint32 ags_vst_ibstream_read(AgsVstIBStream *ibstream,
			       void *buffer, gint32 num_bytes, gint32 *num_bytes_read)
  {
    return(ibstream->read(buffer, num_bytes, num_bytes_read));
  }
	
  gint32 ags_vst_ibstream_write(AgsVstIBStream *ibstream,
				void *buffer, gint32 num_bytes, gint32 *num_bytes_written)
  {
    return(ibstream->write(buffer, num_bytes, num_bytes_read)));
  }
	
  gint32 ags_vst_ibstream_seek(AgsVstIBStream *ibstream,
			       gint64 pos, gint32 mode, gint64 *result)
  {
    return(ibstream->seek(pos, mode, result));
  }

  gint32 ags_vst_ibstream_tell(AgsVstIBStream *ibstream,
			       gint64 *pos)
  {
    return(ibstream->tell(pos));
  }

  AgsVstTUID
  ags_vst_isizeable_stream_get_iid()
  {
    return(ISizeableStream__iid);
  }
    
  gint32 ags_vst_isizeable_stream_get_stream_size(AgsVstISizeableStream *isizeable_stream,
						  gint64 *size)
  {
    return(ibstream->getStreamSize(size));
  }

  gint32 ags_vst_isizeable_stream_set_stream_size(AgsVstISizeableStream *isizeable_stream,
						  gint64 size)
  {
    return(ibstream->setStreamSize(size));
  }

}
