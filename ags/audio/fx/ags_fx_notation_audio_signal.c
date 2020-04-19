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

#include <ags/audio/fx/ags_fx_notation_audio_signal.h>

#include <ags/i18n.h>

void ags_fx_notation_audio_signal_class_init(AgsFxNotationAudioSignalClass *fx_notation_audio_signal);
void ags_fx_notation_audio_signal_init(AgsFxNotationAudioSignal *fx_notation_audio_signal);
void ags_fx_notation_audio_signal_dispose(GObject *gobject);
void ags_fx_notation_audio_signal_finalize(GObject *gobject);

/**
 * SECTION:ags_fx_notation_audio_signal
 * @short_description: fx notation audio_signal
 * @title: AgsFxNotationAudioSignal
 * @section_id:
 * @include: ags/audio/fx/ags_fx_notation_audio_signal.h
 *
 * The #AgsFxNotationAudioSignal class provides ports to the effect processor.
 */

static gpointer ags_fx_notation_audio_signal_parent_class = NULL;

static const gchar *ags_fx_notation_audio_signal_plugin_name = "ags-fx-notation";

GType
ags_fx_notation_audio_signal_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_notation_audio_signal = 0;

    static const GTypeInfo ags_fx_notation_audio_signal_info = {
      sizeof (AgsFxNotationAudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_notation_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio_signal */
      sizeof (AgsFxNotationAudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_notation_audio_signal_init,
    };

    ags_type_fx_notation_audio_signal = g_type_register_static(AGS_TYPE_RECALL_AUDIO_SIGNAL,
							       "AgsFxNotationAudioSignal",
							       &ags_fx_notation_audio_signal_info,
							       0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_notation_audio_signal);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_notation_audio_signal_class_init(AgsFxNotationAudioSignalClass *fx_notation_audio_signal)
{
  GObjectClass *gobject;

  ags_fx_notation_audio_signal_parent_class = g_type_class_peek_parent(fx_notation_audio_signal);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_notation_audio_signal;

  gobject->dispose = ags_fx_notation_audio_signal_dispose;
  gobject->finalize = ags_fx_notation_audio_signal_finalize;
}

void
ags_fx_notation_audio_signal_init(AgsFxNotationAudioSignal *fx_notation_audio_signal)
{
  AGS_RECALL(fx_notation_audio_signal)->name = "ags-fx-notation";
  AGS_RECALL(fx_notation_audio_signal)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_notation_audio_signal)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_notation_audio_signal)->xml_type = "ags-fx-notation-audio-signal";
}

void
ags_fx_notation_audio_signal_dispose(GObject *gobject)
{
  AgsFxNotationAudioSignal *fx_notation_audio_signal;
  
  fx_notation_audio_signal = AGS_FX_NOTATION_AUDIO_SIGNAL(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_notation_audio_signal_parent_class)->dispose(gobject);
}

void
ags_fx_notation_audio_signal_finalize(GObject *gobject)
{
  AgsFxNotationAudioSignal *fx_notation_audio_signal;
  
  fx_notation_audio_signal = AGS_FX_NOTATION_AUDIO_SIGNAL(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_notation_audio_signal_parent_class)->finalize(gobject);
}

/**
 * ags_fx_notation_audio_signal_new:
 * @audio_signal: the #AgsAudioSignal
 *
 * Create a new instance of #AgsFxNotationAudioSignal
 *
 * Returns: the new #AgsFxNotationAudioSignal
 *
 * Since: 3.3.0
 */
AgsFxNotationAudioSignal*
ags_fx_notation_audio_signal_new(AgsAudioSignal *audio_signal)
{
  AgsFxNotationAudioSignal *fx_notation_audio_signal;

  fx_notation_audio_signal = (AgsFxNotationAudioSignal *) g_object_new(AGS_TYPE_FX_NOTATION_AUDIO_SIGNAL,
								       "audio_signal", audio_signal,
								       NULL);

  return(fx_notation_audio_signal);
}
