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

#include <ags/audio/fx/ags_fx_high_pass_audio.h>

#include <ags/plugin/ags_plugin_port.h>

#include <ags/i18n.h>

void ags_fx_high_pass_audio_class_init(AgsFxHighPassAudioClass *fx_high_pass_audio);
void ags_fx_high_pass_audio_init(AgsFxHighPassAudio *fx_high_pass_audio);
void ags_fx_high_pass_audio_dispose(GObject *gobject);
void ags_fx_high_pass_audio_finalize(GObject *gobject);

/**
 * SECTION:ags_fx_high_pass_audio
 * @short_description: fx high pass audio
 * @title: AgsFxHighPassAudio
 * @section_id:
 * @include: ags/audio/fx/ags_fx_high_pass_audio.h
 *
 * The #AgsFxHighPassAudio class provides ports to the effect processor.
 */

static gpointer ags_fx_high_pass_audio_parent_class = NULL;

const gchar *ags_fx_high_pass_audio_plugin_name = "ags-fx-high-pass";

const gchar* ags_fx_high_pass_audio_specifier[] = {
  NULL,
};

const gchar* ags_fx_high_pass_audio_control_port[] = {
  NULL,
};

GType
ags_fx_high_pass_audio_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_high_pass_audio = 0;

    static const GTypeInfo ags_fx_high_pass_audio_info = {
      sizeof (AgsFxHighPassAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_high_pass_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio */
      sizeof (AgsFxHighPassAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_high_pass_audio_init,
    };

    ags_type_fx_high_pass_audio = g_type_register_static(AGS_TYPE_RECALL_AUDIO,
							 "AgsFxHighPassAudio",
							 &ags_fx_high_pass_audio_info,
							 0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_high_pass_audio);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_high_pass_audio_class_init(AgsFxHighPassAudioClass *fx_high_pass_audio)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_fx_high_pass_audio_parent_class = g_type_class_peek_parent(fx_high_pass_audio);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_high_pass_audio;

  gobject->dispose = ags_fx_high_pass_audio_dispose;
  gobject->finalize = ags_fx_high_pass_audio_finalize;

  /* properties */
}

void
ags_fx_high_pass_audio_init(AgsFxHighPassAudio *fx_high_pass_audio)
{
  AGS_RECALL(fx_high_pass_audio)->name = "ags-fx-high-pass";
  AGS_RECALL(fx_high_pass_audio)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_high_pass_audio)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_high_pass_audio)->xml_type = "ags-fx-high-pass-audio";
}

void
ags_fx_high_pass_audio_dispose(GObject *gobject)
{
  AgsFxHighPassAudio *fx_high_pass_audio;
  
  fx_high_pass_audio = AGS_FX_HIGH_PASS_AUDIO(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_high_pass_audio_parent_class)->dispose(gobject);
}

void
ags_fx_high_pass_audio_finalize(GObject *gobject)
{
  AgsFxHighPassAudio *fx_high_pass_audio;
  
  fx_high_pass_audio = AGS_FX_HIGH_PASS_AUDIO(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_high_pass_audio_parent_class)->finalize(gobject);
}

/**
 * ags_fx_high_pass_audio_new:
 * @audio: the #AgsAudio
 *
 * Create a new instance of #AgsFxHighPassAudio
 *
 * Returns: the new #AgsFxHighPassAudio
 *
 * Since: 3.13.0
 */
AgsFxHighPassAudio*
ags_fx_high_pass_audio_new(AgsAudio *audio)
{
  AgsFxHighPassAudio *fx_high_pass_audio;

  fx_high_pass_audio = (AgsFxHighPassAudio *) g_object_new(AGS_TYPE_FX_HIGH_PASS_AUDIO,
							   "audio", audio,
							   NULL);

  return(fx_high_pass_audio);
}
