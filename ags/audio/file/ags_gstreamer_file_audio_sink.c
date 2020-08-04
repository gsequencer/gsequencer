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

#include <ags/audio/file/ags_gstreamer_file_audio_sink.h>

#include <ags/i18n.h>

void ags_gstreamer_file_audio_sink_class_init(AgsGstreamerFileAudioSinkClass *gstreamer_file_audio_sink);
void ags_gstreamer_file_audio_sink_init(AgsGstreamerFileAudioSink *gstreamer_file_audio_sink);
void ags_gstreamer_file_audio_sink_dispose(GObject *gobject);
void ags_gstreamer_file_audio_sink_finalize(GObject *gobject);

gboolean ags_gstreamer_file_audio_sink_open(GstAudioSink *sink);
gboolean ags_gstreamer_file_audio_sink_prepare(GstAudioSink *sink, GstAudioRingBufferSpec *spec);
gboolean ags_gstreamer_file_audio_sink_unprepare(GstAudioSink *sink);
gboolean ags_gstreamer_file_audio_sink_close(GstAudioSink *sink);
gint ags_gstreamer_file_audio_sink_write(GstAudioSink *sink, gpointer data, guint length);
guint ags_gstreamer_file_audio_sink_delay(GstAudioSink *sink);
void ags_gstreamer_file_audio_sink_reset(GstAudioSink *sink);

/**
 * SECTION:ags_gstreamer_file_audio_sink
 * @short_description: gstreamer audio sink
 * @title: AgsGstreamerFileAudioSink
 * @section_id:
 * @include: ags/audio/audio_sink/ags_gstreamer_file_audio_sink.h
 *
 * #AgsGstreamerFileAudioSink is the base object to ineract with libgstreamer-1.0 audio sink.
 */

static gpointer ags_gstreamer_file_audio_sink_parent_class = NULL;

GType
ags_gstreamer_file_audio_sink_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_gstreamer_file_audio_sink = 0;

    static const GTypeInfo ags_gstreamer_file_audio_sink_info = {
      sizeof (AgsGstreamerFileAudioSinkClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_gstreamer_file_audio_sink_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsGstreamerFileAudioSink),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_gstreamer_file_audio_sink_init,
    };

    ags_type_gstreamer_file_audio_sink = g_type_register_static(GST_TYPE_AUDIO_SINK,
								"AgsGstreamerFileAudioSink",
								&ags_gstreamer_file_audio_sink_info,
								0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_gstreamer_file_audio_sink);
  }

  return g_define_type_id__volatile;
}

void
ags_gstreamer_file_audio_sink_class_init(AgsGstreamerFileAudioSinkClass *gstreamer_file_audio_sink)
{
  GObjectClass *gobject;
  GstAudioSinkClass *audio_sink;
  
  ags_gstreamer_file_audio_sink_parent_class = g_type_class_peek_parent(gstreamer_file_audio_sink);

  gobject = (GObjectClass *) gstreamer_file_audio_sink;

  gobject->dispose = ags_gstreamer_file_audio_sink_dispose;
  gobject->finalize = ags_gstreamer_file_audio_sink_finalize;

  /* GstAudioSinkClass */
  audio_sink = (GstAudioSinkClass *) gstreamer_file_audio_sink;
  
  audio_sink->open = ags_gstreamer_file_audio_sink_open;
  audio_sink->prepare = ags_gstreamer_file_audio_sink_prepare;
  audio_sink->unprepare = ags_gstreamer_file_audio_sink_unprepare;
  audio_sink->close = ags_gstreamer_file_audio_sink_close;
  audio_sink->write = ags_gstreamer_file_audio_sink_write;
  audio_sink->delay = ags_gstreamer_file_audio_sink_delay;
  audio_sink->reset = ags_gstreamer_file_audio_sink_reset;
}

void
ags_gstreamer_file_audio_sink_init(AgsGstreamerFileAudioSink *gstreamer_file_audio_sink)
{
  /* add gstreamer file audio sink mutex */
  g_rec_mutex_init(&(gstreamer_file_audio_sink->obj_mutex));  

  gstreamer_file_audio_sink->gstreamer_file = NULL;
}

void
ags_gstreamer_file_audio_sink_dispose(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_gstreamer_file_audio_sink_parent_class)->dispose(gobject);
}

void
ags_gstreamer_file_audio_sink_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_gstreamer_file_audio_sink_parent_class)->finalize(gobject);
}

gboolean
ags_gstreamer_file_audio_sink_open(GstAudioSink *sink)
{
  //TODO:JK: implement me

  return(TRUE);
}

gboolean
ags_gstreamer_file_audio_sink_prepare(GstAudioSink *sink, GstAudioRingBufferSpec *spec)
{
  //TODO:JK: implement me

  return(TRUE);
}

gboolean
ags_gstreamer_file_audio_sink_unprepare(GstAudioSink *sink)
{
  //TODO:JK: implement me

  return(TRUE);
}

gboolean
ags_gstreamer_file_audio_sink_close(GstAudioSink *sink)
{
  //TODO:JK: implement me

  return(TRUE);
}

gint
ags_gstreamer_file_audio_sink_write(GstAudioSink *sink, gpointer data, guint length)
{
  //TODO:JK: implement me

  return((gint) length);
}

guint
ags_gstreamer_file_audio_sink_delay(GstAudioSink *sink)
{
  //TODO:JK: implement me

  return(0);
}

void
ags_gstreamer_file_audio_sink_reset(GstAudioSink *sink)
{
  //TODO:JK: implement me
}

/**
 * ags_gstreamer_file_audio_sink_new:
 *
 * Creates a new instance of #AgsGstreamerFileAudioSink.
 *
 * Returns: the new #AgsGstreamerFileAudioSink.
 *
 * Since: 3.6.0
 */
AgsGstreamerFileAudioSink*
ags_gstreamer_file_audio_sink_new()
{
  AgsGstreamerFileAudioSink *gstreamer_file_audio_sink;

  gstreamer_file_audio_sink = (AgsGstreamerFileAudioSink *) g_object_new(AGS_TYPE_GSTREAMER_FILE_AUDIO_SINK,
									 NULL);

  return(gstreamer_file_audio_sink);
}
