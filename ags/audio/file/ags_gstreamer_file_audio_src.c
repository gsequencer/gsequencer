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

#include <ags/audio/file/ags_gstreamer_audio_src.h>

#include <ags/i18n.h>

void ags_gstreamer_audio_src_class_init(AgsGstreamerAudioSrcClass *gstreamer_audio_src);
void ags_gstreamer_audio_src_init(AgsGstreamerAudioSrc *gstreamer_audio_src);
void ags_gstreamer_audio_src_dispose(GObject *gobject);
void ags_gstreamer_audio_src_finalize(GObject *gobject);

/**
 * SECTION:ags_gstreamer_audio_src
 * @short_description: gstreamer audio src
 * @title: AgsGstreamerAudioSrc
 * @section_id:
 * @include: ags/audio/audio_src/ags_gstreamer_audio_src.h
 *
 * #AgsGstreamerAudioSrc is the base object to ineract with libgstreamer-1.0 audio src.
 */

static gpointer ags_gstreamer_audio_src_parent_class = NULL;

GType
ags_gstreamer_audio_src_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_gstreamer_audio_src = 0;

    static const GTypeInfo ags_gstreamer_audio_src_info = {
      sizeof (AgsGstreamerAudioSrcClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_gstreamer_audio_src_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsGstreamerAudioSrc),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_gstreamer_audio_src_init,
    };

    ags_type_gstreamer_audio_src = g_type_register_static(G_TYPE_OBJECT,
							  "AgsGstreamerAudioSrc",
							  &ags_gstreamer_audio_src_info,
							  0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_gstreamer_audio_src);
  }

  return g_define_type_id__volatile;
}

void
ags_gstreamer_audio_src_class_init(AgsGstreamerAudioSrcClass *gstreamer_audio_src)
{
  GObjectClass *gobject;

  ags_gstreamer_audio_src_parent_class = g_type_class_peek_parent(gstreamer_audio_src);

  gobject = (GObjectClass *) gstreamer_audio_src;

  gobject->dispose = ags_gstreamer_audio_src_dispose;
  gobject->finalize = ags_gstreamer_audio_src_finalize;
}

void
ags_gstreamer_audio_src_init(AgsGstreamerAudioSrc *gstreamer_audio_src)
{
  //TODO:JK: implement me
}

void
ags_gstreamer_audio_src_dispose(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_gstreamer_audio_src_parent_class)->dispose(gobject);
}

void
ags_gstreamer_audio_src_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_gstreamer_audio_src_parent_class)->finalize(gobject);
}

/**
 * ags_gstreamer_audio_src_new:
 *
 * Creates a new instance of #AgsGstreamerAudioSrc.
 *
 * Returns: the new #AgsGstreamerAudioSrc.
 *
 * Since: 3.6.0
 */
AgsGstreamerAudioSrc*
ags_gstreamer_audio_src_new()
{
  AgsGstreamerAudioSrc *gstreamer_audio_src;

  gstreamer_audio_src = (AgsGstreamerAudioSrc *) g_object_new(AGS_TYPE_GSTREAMER_AUDIO_SRC,
							      NULL);

  return(gstreamer_audio_src);
}
