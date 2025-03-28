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

#include <ags/audio/fx/ags_fx_ladspa_audio.h>

#include <ags/i18n.h>

void ags_fx_ladspa_audio_class_init(AgsFxLadspaAudioClass *fx_ladspa_audio);
void ags_fx_ladspa_audio_init(AgsFxLadspaAudio *fx_ladspa_audio);
void ags_fx_ladspa_audio_dispose(GObject *gobject);
void ags_fx_ladspa_audio_finalize(GObject *gobject);

/**
 * SECTION:ags_fx_ladspa_audio
 * @short_description: fx ladspa audio
 * @title: AgsFxLadspaAudio
 * @section_id:
 * @include: ags/audio/fx/ags_fx_ladspa_audio.h
 *
 * The #AgsFxLadspaAudio class provides ports to the effect processor.
 */

static gpointer ags_fx_ladspa_audio_parent_class = NULL;

const gchar *ags_fx_ladspa_audio_plugin_name = "ags-fx-ladspa";

const gchar *ags_fx_ladspa_audio_specifier[] = {
  NULL,
};

const gchar *ags_fx_ladspa_audio_control_port[] = {
  NULL,
};

enum{
  PROP_0,
};

GType
ags_fx_ladspa_audio_get_type()
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_fx_ladspa_audio = 0;

    static const GTypeInfo ags_fx_ladspa_audio_info = {
      sizeof (AgsFxLadspaAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_ladspa_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio */
      sizeof (AgsFxLadspaAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_ladspa_audio_init,
    };

    ags_type_fx_ladspa_audio = g_type_register_static(AGS_TYPE_RECALL_AUDIO,
						      "AgsFxLadspaAudio",
						      &ags_fx_ladspa_audio_info,
						      0);

    g_once_init_leave(&g_define_type_id__static, ags_type_fx_ladspa_audio);
  }

  return(g_define_type_id__static);
}

void
ags_fx_ladspa_audio_class_init(AgsFxLadspaAudioClass *fx_ladspa_audio)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_fx_ladspa_audio_parent_class = g_type_class_peek_parent(fx_ladspa_audio);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_ladspa_audio;

  gobject->dispose = ags_fx_ladspa_audio_dispose;
  gobject->finalize = ags_fx_ladspa_audio_finalize;

  /* properties */
}

void
ags_fx_ladspa_audio_init(AgsFxLadspaAudio *fx_ladspa_audio)
{
  AGS_RECALL(fx_ladspa_audio)->name = "ags-fx-ladspa";
  AGS_RECALL(fx_ladspa_audio)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_ladspa_audio)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_ladspa_audio)->xml_type = "ags-fx-ladspa-audio";
}

void
ags_fx_ladspa_audio_dispose(GObject *gobject)
{
  AgsFxLadspaAudio *fx_ladspa_audio;
  
  fx_ladspa_audio = AGS_FX_LADSPA_AUDIO(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_ladspa_audio_parent_class)->dispose(gobject);
}

void
ags_fx_ladspa_audio_finalize(GObject *gobject)
{
  AgsFxLadspaAudio *fx_ladspa_audio;
  
  fx_ladspa_audio = AGS_FX_LADSPA_AUDIO(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_ladspa_audio_parent_class)->finalize(gobject);
}

/**
 * ags_fx_ladspa_audio_new:
 * @audio: the #AgsAudio
 *
 * Create a new instance of #AgsFxLadspaAudio
 *
 * Returns: the new #AgsFxLadspaAudio
 *
 * Since: 3.3.0
 */
AgsFxLadspaAudio*
ags_fx_ladspa_audio_new(AgsAudio *audio)
{
  AgsFxLadspaAudio *fx_ladspa_audio;

  fx_ladspa_audio = (AgsFxLadspaAudio *) g_object_new(AGS_TYPE_FX_LADSPA_AUDIO,
						      "audio", audio,
						      NULL);

  return(fx_ladspa_audio);
}
