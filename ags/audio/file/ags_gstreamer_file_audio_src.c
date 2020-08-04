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

#include <ags/audio/file/ags_gstreamer_file_audio_src.h>

#include <ags/i18n.h>

void ags_gstreamer_file_audio_src_class_init(AgsGstreamerFileAudioSrcClass *gstreamer_file_audio_src);
void ags_gstreamer_file_audio_src_init(AgsGstreamerFileAudioSrc *gstreamer_file_audio_src);
void ags_gstreamer_file_audio_src_dispose(GObject *gobject);
void ags_gstreamer_file_audio_src_finalize(GObject *gobject);

gboolean ags_gstreamer_file_audio_src_open(GstAudioSrc *src);
gboolean ags_gstreamer_file_audio_src_prepare(GstAudioSrc *src, GstAudioRingBufferSpec *spec);
gboolean ags_gstreamer_file_audio_src_unprepare(GstAudioSrc *src);
gboolean ags_gstreamer_file_audio_src_close(GstAudioSrc *src);
guint ags_gstreamer_file_audio_src_read(GstAudioSrc *src, gpointer data, guint length,
					GstClockTime *timestamp);
guint ags_gstreamer_file_audio_src_delay(GstAudioSrc *src);
void ags_gstreamer_file_audio_src_reset(GstAudioSrc *src);

/**
 * SECTION:ags_gstreamer_file_audio_src
 * @short_description: gstreamer audio src
 * @title: AgsGstreamerFileAudioSrc
 * @section_id:
 * @include: ags/audio/audio_src/ags_gstreamer_file_audio_src.h
 *
 * #AgsGstreamerFileAudioSrc is the base object to ineract with libgstreamer-1.0 audio src.
 */

static gpointer ags_gstreamer_file_audio_src_parent_class = NULL;

GType
ags_gstreamer_file_audio_src_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_gstreamer_file_audio_src = 0;

    static const GTypeInfo ags_gstreamer_file_audio_src_info = {
      sizeof (AgsGstreamerFileAudioSrcClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_gstreamer_file_audio_src_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsGstreamerFileAudioSrc),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_gstreamer_file_audio_src_init,
    };

    ags_type_gstreamer_file_audio_src = g_type_register_static(GST_TYPE_AUDIO_SRC,
							       "AgsGstreamerFileAudioSrc",
							       &ags_gstreamer_file_audio_src_info,
							       0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_gstreamer_file_audio_src);
  }

  return g_define_type_id__volatile;
}

void
ags_gstreamer_file_audio_src_class_init(AgsGstreamerFileAudioSrcClass *gstreamer_file_audio_src)
{
  GObjectClass *gobject;
  GstAudioSrcClass *audio_src;

  ags_gstreamer_file_audio_src_parent_class = g_type_class_peek_parent(gstreamer_file_audio_src);

  gobject = (GObjectClass *) gstreamer_file_audio_src;

  gobject->dispose = ags_gstreamer_file_audio_src_dispose;
  gobject->finalize = ags_gstreamer_file_audio_src_finalize;

  /* GstAudioSrcClass */
  audio_src = (GstAudioSrcClass *) gstreamer_file_audio_src;
  
  audio_src->open = ags_gstreamer_file_audio_src_open;
  audio_src->prepare = ags_gstreamer_file_audio_src_prepare;
  audio_src->unprepare = ags_gstreamer_file_audio_src_unprepare;
  audio_src->close = ags_gstreamer_file_audio_src_close;
  audio_src->read = ags_gstreamer_file_audio_src_read;
  audio_src->delay = ags_gstreamer_file_audio_src_delay;
  audio_src->reset = ags_gstreamer_file_audio_src_reset;
}

void
ags_gstreamer_file_audio_src_init(AgsGstreamerFileAudioSrc *gstreamer_file_audio_src)
{
  /* add gstreamer file audio src mutex */
  g_rec_mutex_init(&(gstreamer_file_audio_src->obj_mutex));  

  gstreamer_file_audio_src->gstreamer_file = NULL;
}

void
ags_gstreamer_file_audio_src_dispose(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_gstreamer_file_audio_src_parent_class)->dispose(gobject);
}

void
ags_gstreamer_file_audio_src_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_gstreamer_file_audio_src_parent_class)->finalize(gobject);
}

gboolean
ags_gstreamer_file_audio_src_open(GstAudioSrc *src)
{
  //TODO:JK: implement me

  return(TRUE);
}

gboolean
ags_gstreamer_file_audio_src_prepare(GstAudioSrc *src, GstAudioRingBufferSpec *spec)
{
  //TODO:JK: implement me

  return(TRUE);
}

gboolean
ags_gstreamer_file_audio_src_unprepare(GstAudioSrc *src)
{
  //TODO:JK: implement me

  return(TRUE);
}

gboolean
ags_gstreamer_file_audio_src_close(GstAudioSrc *src)
{
  //TODO:JK: implement me

  return(TRUE);
}

guint
ags_gstreamer_file_audio_src_read(GstAudioSrc *src, gpointer data, guint length,
				  GstClockTime *timestamp)
{
  //TODO:JK: implement me

  return(length);
}

guint
ags_gstreamer_file_audio_src_delay(GstAudioSrc *src)
{
  //TODO:JK: implement me

  return(0);
}

void
ags_gstreamer_file_audio_src_reset(GstAudioSrc *src)
{
  //TODO:JK: implement me
}

/**
 * ags_gstreamer_file_audio_src_new:
 *
 * Creates a new instance of #AgsGstreamerFileAudioSrc.
 *
 * Returns: the new #AgsGstreamerFileAudioSrc.
 *
 * Since: 3.6.0
 */
AgsGstreamerFileAudioSrc*
ags_gstreamer_file_audio_src_new()
{
  AgsGstreamerFileAudioSrc *gstreamer_file_audio_src;

  gstreamer_file_audio_src = (AgsGstreamerFileAudioSrc *) g_object_new(AGS_TYPE_GSTREAMER_FILE_AUDIO_SRC,
								       NULL);

  return(gstreamer_file_audio_src);
}
