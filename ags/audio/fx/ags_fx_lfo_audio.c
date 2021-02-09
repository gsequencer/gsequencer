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

#include <ags/audio/fx/ags_fx_lfo_audio.h>

#include <ags/plugin/ags_plugin_port.h>

#include <ags/i18n.h>

void ags_fx_lfo_audio_class_init(AgsFxLfoAudioClass *fx_lfo_audio);
void ags_fx_lfo_audio_init(AgsFxLfoAudio *fx_lfo_audio);
void ags_fx_lfo_audio_dispose(GObject *gobject);
void ags_fx_lfo_audio_finalize(GObject *gobject);

/**
 * SECTION:ags_fx_lfo_audio
 * @short_description: fx lfo audio
 * @title: AgsFxLfoAudio
 * @section_id:
 * @include: ags/audio/fx/ags_fx_lfo_audio.h
 *
 * The #AgsFxLfoAudio class provides ports to the effect processor.
 */

static gpointer ags_fx_lfo_audio_parent_class = NULL;

const gchar *ags_fx_lfo_audio_plugin_name = "ags-fx-lfo";

const gchar* ags_fx_lfo_audio_specifier[] = {
  NULL,
};

const gchar* ags_fx_lfo_audio_control_port[] = {
  NULL,
};

enum{
  PROP_0,
};

GType
ags_fx_lfo_audio_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_lfo_audio = 0;

    static const GTypeInfo ags_fx_lfo_audio_info = {
      sizeof (AgsFxLfoAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_lfo_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio */
      sizeof (AgsFxLfoAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_lfo_audio_init,
    };

    ags_type_fx_lfo_audio = g_type_register_static(AGS_TYPE_RECALL_AUDIO,
						   "AgsFxLfoAudio",
						   &ags_fx_lfo_audio_info,
						   0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_lfo_audio);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_lfo_audio_class_init(AgsFxLfoAudioClass *fx_lfo_audio)
{
  GObjectClass *gobject;

  ags_fx_lfo_audio_parent_class = g_type_class_peek_parent(fx_lfo_audio);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_lfo_audio;

  gobject->dispose = ags_fx_lfo_audio_dispose;
  gobject->finalize = ags_fx_lfo_audio_finalize;

  /* properties */
}

void
ags_fx_lfo_audio_init(AgsFxLfoAudio *fx_lfo_audio)
{
  AGS_RECALL(fx_lfo_audio)->name = "ags-fx-lfo";
  AGS_RECALL(fx_lfo_audio)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_lfo_audio)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_lfo_audio)->xml_type = "ags-fx-lfo-audio";
}

void
ags_fx_lfo_audio_dispose(GObject *gobject)
{
  AgsFxLfoAudio *fx_lfo_audio;
  
  fx_lfo_audio = AGS_FX_LFO_AUDIO(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_lfo_audio_parent_class)->dispose(gobject);
}

void
ags_fx_lfo_audio_finalize(GObject *gobject)
{
  AgsFxLfoAudio *fx_lfo_audio;
  
  fx_lfo_audio = AGS_FX_LFO_AUDIO(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_lfo_audio_parent_class)->finalize(gobject);
}

/**
 * ags_fx_lfo_audio_new:
 * @audio: the #AgsAudio
 *
 * Create a new instance of #AgsFxLfoAudio
 *
 * Returns: the new #AgsFxLfoAudio
 *
 * Since: 3.8.0
 */
AgsFxLfoAudio*
ags_fx_lfo_audio_new(AgsAudio *audio)
{
  AgsFxLfoAudio *fx_lfo_audio;

  fx_lfo_audio = (AgsFxLfoAudio *) g_object_new(AGS_TYPE_FX_LFO_AUDIO,
						"audio", audio,
						NULL);

  return(fx_lfo_audio);
}
