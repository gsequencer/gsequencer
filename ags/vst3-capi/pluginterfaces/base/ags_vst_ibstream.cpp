/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#include <ags/vst3-capi/pluginterfaces/base/ags_vst_ibstream.h>

#include <pluginterfaces/base/ibstream.h>

extern "C" {

  /**
   * Get IID.
   *
   * @return the Steinberg::TUID as AgsVstTUID
   *
   * @since 5.0.0
   */
  AgsVstTUID*
  ags_vst_ibstream_get_iid()
  {
    return((AgsVstTUID *) &(Steinberg::IBStream::iid.toTUID()));
  }

  /**
   * Read.
   * 
   * @param ib_stream the stream
   * @param buffer the buffer
   * @param num_bytes number of bytes
   * @param num_bytes_read number of read
   * @return the return value
   *
   * @since 5.0.0
   */
  AgsVstTResult
  ags_vst_ibstream_read(AgsVstIBStream *ib_stream, gpointer buffer, gint32 num_bytes, gint32 *num_bytes_read)
  {
    return((AgsVstTResult) static_cast<Steinberg::IBStream*>((void *) ib_stream)->read(buffer, num_bytes, num_bytes_read));
  }

  /**
   * Write.
   * 
   * @param ib_stream the stream
   * @param buffer the buffer
   * @param num_bytes number of bytes
   * @param num_bytes_written number of read
   * @return the return value
   *
   * @since 5.0.0
   */
  AgsVstTResult
  ags_vst_ibstream_write(AgsVstIBStream *ib_stream, gpointer buffer, gint32 num_bytes, gint32 *num_bytes_written)
  {
    return((AgsVstTResult) static_cast<Steinberg::IBStream*>((void *) ib_stream)->write(buffer, num_bytes, num_bytes_written));
  }

  /**
   * Seek.
   * 
   * @param ib_stream the stream
   * @param pos the position
   * @param mode mode
   * @param result the result
   * @return the return value
   *
   * @since 5.0.0
   */
  AgsVstTResult
  ags_vst_ibstream_seek(AgsVstIBStream *ib_stream, gint64 pos, gint32 mode, gint64 *result)
  {
    Steinberg::int64 tmp_result;
    Steinberg::tresult retval;

    retval = static_cast<Steinberg::IBStream*>((void *) ib_stream)->seek(pos, mode, &tmp_result);

    if(result != NULL){
      result[0] = (gint64) tmp_result;
    }
    
    return((AgsVstTResult) retval);
  }

  /**
   * Tell.
   * 
   * @param ib_stream the stream
   * @param pos the position
   * @return the return value
   *
   * @since 5.0.0
   */
  AgsVstTResult
  ags_vst_ibstream_tell(AgsVstIBStream *ib_stream, gint64 *pos)
  {
    Steinberg::int64 tmp_pos;
    Steinberg::tresult retval;

    retval = static_cast<Steinberg::IBStream*>((void *) ib_stream)->tell(&tmp_pos);

    if(pos != NULL){
      pos[0] = (gint64) tmp_pos;
    }
    
    return((AgsVstTResult) retval);
  }

  /**
   * Get IID.
   * 
   * @return the Steinberg::TUID as AgsVstTUID
   *
   * @since 5.0.0
   */
  AgsVstTUID*
  ags_vst_isizeable_stream_get_iid()
  {
    extern const Steinberg::TUID ISizeableStream__iid;

    return((AgsVstTUID *) &(Steinberg::ISizeableStream::iid.toTUID()));
  }

  /**
   * Set stream size.
   *
   * @param isizeable_stream the stream
   * @param stream_size stream size
   * @return the return code
   *
   * @since 5.0.0
   */
  AgsVstTResult
  ags_vst_isizeable_stream_set_stream_size(AgsVstISizeableStream *isizeable_stream, gint64 stream_size)
  {
    return((AgsVstTResult) static_cast<Steinberg::ISizeableStream*>((void *) isizeable_stream)->setStreamSize(stream_size));
  }

}
