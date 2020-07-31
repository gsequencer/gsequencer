/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#ifndef __AGS_GSTREAMER_AUDIO_SRC_H__
#define __AGS_GSTREAMER_AUDIO_SRC_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/config.h>

#include <gst/gst.h>
#include <gst/audio/gstaudiosrc.h>

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_GSTREAMER_AUDIO_SRC                (ags_gstreamer_audio_src_get_type())
#define AGS_GSTREAMER_AUDIO_SRC(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_GSTREAMER_AUDIO_SRC, AgsGstreamerAudioSrc))
#define AGS_GSTREAMER_AUDIO_SRC_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_GSTREAMER_AUDIO_SRC, AgsGstreamerAudioSrcClass))
#define AGS_IS_GSTREAMER_AUDIO_SRC(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_GSTREAMER_AUDIO_SRC))
#define AGS_IS_GSTREAMER_AUDIO_SRC_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_GSTREAMER_AUDIO_SRC))
#define AGS_GSTREAMER_AUDIO_SRC_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_GSTREAMER_AUDIO_SRC, AgsGstreamerAudioSrcClass))

#define AGS_GSTREAMER_AUDIO_SRC_GET_OBJ_MUTEX(obj) (&(((AgsGstreamerAudioSrc *) obj)->obj_mutex))

typedef struct _AgsGstreamerAudioSrc AgsGstreamerAudioSrc;
typedef struct _AgsGstreamerAudioSrcClass AgsGstreamerAudioSrcClass;

struct _AgsGstreamerAudioSrc
{
  GstAudioSrc audio_src;
};

struct _AgsGstreamerAudioSrcClass
{
  GstAudioSrcClass audio_src;
};

GType ags_gstreamer_audio_src_get_type();

AgsGstreamerAudioSrc* ags_gstreamer_audio_src_new();

G_END_DECLS

#endif /*__AGS_GSTREAMER_AUDIO_SRC_H__*/
