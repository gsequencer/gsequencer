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

#include <ags/audio/fx/ags_fx_lv2_audio_signal.h>

#include <ags/i18n.h>

void ags_fx_lv2_audio_signal_class_init(AgsFxLv2AudioSignalClass *fx_lv2_audio_signal);
void ags_fx_lv2_audio_signal_init(AgsFxLv2AudioSignal *fx_lv2_audio_signal);
void ags_fx_lv2_audio_signal_dispose(GObject *gobject);
void ags_fx_lv2_audio_signal_finalize(GObject *gobject);

void ags_fx_lv2_audio_signal_real_run_inter(AgsRecall *recall);

/**
 * SECTION:ags_fx_lv2_audio_signal
 * @short_description: fx lv2 audio signal
 * @title: AgsFxLv2AudioSignal
 * @section_id:
 * @include: ags/audio/fx/ags_fx_lv2_audio_signal.h
 *
 * The #AgsFxLv2AudioSignal class provides ports to the effect processor.
 */

static gpointer ags_fx_lv2_audio_signal_parent_class = NULL;

static const gchar *ags_fx_lv2_audio_signal_plugin_name = "ags-fx-lv2";

GType
ags_fx_lv2_audio_signal_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_lv2_audio_signal = 0;

    static const GTypeInfo ags_fx_lv2_audio_signal_info = {
      sizeof (AgsFxLv2AudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_lv2_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio_signal */
      sizeof (AgsFxLv2AudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_lv2_audio_signal_init,
    };

    ags_type_fx_lv2_audio_signal = g_type_register_static(AGS_TYPE_RECALL_AUDIO_SIGNAL,
							  "AgsFxLv2AudioSignal",
							  &ags_fx_lv2_audio_signal_info,
							  0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_lv2_audio_signal);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_lv2_audio_signal_class_init(AgsFxLv2AudioSignalClass *fx_lv2_audio_signal)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_fx_lv2_audio_signal_parent_class = g_type_class_peek_parent(fx_lv2_audio_signal);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_lv2_audio_signal;

  gobject->dispose = ags_fx_lv2_audio_signal_dispose;
  gobject->finalize = ags_fx_lv2_audio_signal_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) fx_lv2_audio_signal;
  
  recall->run_inter = ags_fx_lv2_audio_signal_real_run_inter;
}

void
ags_fx_lv2_audio_signal_init(AgsFxLv2AudioSignal *fx_lv2_audio_signal)
{
  AGS_RECALL(fx_lv2_audio_signal)->name = "ags-fx-lv2";
  AGS_RECALL(fx_lv2_audio_signal)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_lv2_audio_signal)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_lv2_audio_signal)->xml_type = "ags-fx-lv2-audio-signal";
}

void
ags_fx_lv2_audio_signal_dispose(GObject *gobject)
{
  AgsFxLv2AudioSignal *fx_lv2_audio_signal;
  
  fx_lv2_audio_signal = AGS_FX_LV2_AUDIO_SIGNAL(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_lv2_audio_signal_parent_class)->dispose(gobject);
}

void
ags_fx_lv2_audio_signal_finalize(GObject *gobject)
{
  AgsFxLv2AudioSignal *fx_lv2_audio_signal;
  
  fx_lv2_audio_signal = AGS_FX_LV2_AUDIO_SIGNAL(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_lv2_audio_signal_parent_class)->finalize(gobject);
}

void
ags_fx_lv2_audio_signal_real_run_inter(AgsRecall *recall)
{
  /* call parent */
  AGS_RECALL_CLASS(ags_fx_lv2_audio_signal_parent_class)->run_inter(recall);
}

/**
 * ags_fx_lv2_audio_signal_new:
 * @audio_signal: the #AgsAudioSignal
 *
 * Create a new instance of #AgsFxLv2AudioSignal
 *
 * Returns: the new #AgsFxLv2AudioSignal
 *
 * Since: 3.3.0
 */
AgsFxLv2AudioSignal*
ags_fx_lv2_audio_signal_new(AgsAudioSignal *audio_signal)
{
  AgsFxLv2AudioSignal *fx_lv2_audio_signal;

  fx_lv2_audio_signal = (AgsFxLv2AudioSignal *) g_object_new(AGS_TYPE_FX_LV2_AUDIO_SIGNAL,
							     "audio-signal", audio_signal,
							     NULL);

  return(fx_lv2_audio_signal);
}
