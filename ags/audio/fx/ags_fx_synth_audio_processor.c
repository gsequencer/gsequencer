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

#include <ags/audio/fx/ags_fx_synth_audio_processor.h>

#include <ags/i18n.h>

void ags_fx_synth_audio_processor_class_init(AgsFxSynthAudioProcessorClass *fx_synth_audio_processor);
void ags_fx_synth_audio_processor_init(AgsFxSynthAudioProcessor *fx_synth_audio_processor);
void ags_fx_synth_audio_processor_dispose(GObject *gobject);
void ags_fx_synth_audio_processor_finalize(GObject *gobject);

/**
 * SECTION:ags_fx_synth_audio_processor
 * @short_description: fx synth audio processor
 * @title: AgsFxSynthAudioProcessor
 * @section_id:
 * @include: ags/audio/fx/ags_fx_synth_audio_processor.h
 *
 * The #AgsFxSynthAudioProcessor class provides ports to the effect processor.
 */

static gpointer ags_fx_synth_audio_processor_parent_class = NULL;

GType
ags_fx_synth_audio_processor_get_type()
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_fx_synth_audio_processor = 0;

    static const GTypeInfo ags_fx_synth_audio_processor_info = {
      sizeof (AgsFxSynthAudioProcessorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_synth_audio_processor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio_processor */
      sizeof (AgsFxSynthAudioProcessor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_synth_audio_processor_init,
    };

    ags_type_fx_synth_audio_processor = g_type_register_static(AGS_TYPE_FX_NOTATION_AUDIO_PROCESSOR,
							       "AgsFxSynthAudioProcessor",
							       &ags_fx_synth_audio_processor_info,
							       0);

    g_once_init_leave(&g_define_type_id__static, ags_type_fx_synth_audio_processor);
  }

  return(g_define_type_id__static);
}

void
ags_fx_synth_audio_processor_class_init(AgsFxSynthAudioProcessorClass *fx_synth_audio_processor)
{
  GObjectClass *gobject;
  
  ags_fx_synth_audio_processor_parent_class = g_type_class_peek_parent(fx_synth_audio_processor);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_synth_audio_processor;

  gobject->dispose = ags_fx_synth_audio_processor_dispose;
  gobject->finalize = ags_fx_synth_audio_processor_finalize;
}

void
ags_fx_synth_audio_processor_init(AgsFxSynthAudioProcessor *fx_synth_audio_processor)
{
  AGS_RECALL(fx_synth_audio_processor)->name = "ags-fx-synth";
  AGS_RECALL(fx_synth_audio_processor)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_synth_audio_processor)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_synth_audio_processor)->xml_type = "ags-fx-synth-audio-processor";
}

void
ags_fx_synth_audio_processor_dispose(GObject *gobject)
{
  AgsFxSynthAudioProcessor *fx_synth_audio_processor;
  
  fx_synth_audio_processor = AGS_FX_SYNTH_AUDIO_PROCESSOR(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_synth_audio_processor_parent_class)->dispose(gobject);
}

void
ags_fx_synth_audio_processor_finalize(GObject *gobject)
{
  AgsFxSynthAudioProcessor *fx_synth_audio_processor;
  
  fx_synth_audio_processor = AGS_FX_SYNTH_AUDIO_PROCESSOR(gobject);  
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_synth_audio_processor_parent_class)->finalize(gobject);
}

/**
 * ags_fx_synth_audio_processor_new:
 * @audio: the #AgsAudio
 *
 * Create a new instance of #AgsFxSynthAudioProcessor
 *
 * Returns: the new #AgsFxSynthAudioProcessor
 *
 * Since: 3.14.0
 */
AgsFxSynthAudioProcessor*
ags_fx_synth_audio_processor_new(AgsAudio *audio)
{
  AgsFxSynthAudioProcessor *fx_synth_audio_processor;

  fx_synth_audio_processor = (AgsFxSynthAudioProcessor *) g_object_new(AGS_TYPE_FX_SYNTH_AUDIO_PROCESSOR,
								       "audio", audio,
								       NULL);

  return(fx_synth_audio_processor);
}
