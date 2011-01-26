/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __AGS_COPY_STREAM_TO_STREAM_H__
#define __AGS_COPY_STREAM_TO_STREAM_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_audio_signal.h>

#define AGS_TYPE_COPY_STREAM_TO_STREAM                (ags_copy_stream_to_stream_get_type())
#define AGS_COPY_STREAM_TO_STREAM(obj)                (G_TYPE_CHECK_INSTANCE_CAST(obj, AGS_TYPE_COPY_STREAM_TO_STREAM, AgsCopyStreamToStream))
#define AGS_COPY_STREAM_TO_STREAM_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_COPY_STREAM_TO_STREAM, AgsCopyStreamToStream))

typedef struct _AgsCopyStreamToStream AgsCopyStreamToStream;
typedef struct _AgsCopyStreamToStreamClass AgsCopyStreamToStreamClass;

struct _AgsCopyStreamToStream{
  AgsRecall recall;

  AgsAudioSignal *destination;
  AgsAudioSignal *source;

  GList *stream0;
  GList *stream1;
};

struct _AgsCopyStreamToStreamClass{
  AgsRecallClass recall;
};

GType ags_copy_stream_to_stream_get_type();

void ags_copy_stream_to_stream(AgsRecall *recall);

AgsCopyStreamToStream* ags_copy_stream_to_stream_new();

#endif /*__AGS_COPY_STREAM_TO_STREAM_H__*/
