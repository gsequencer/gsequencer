/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#include <ags/audio/fx/ags_fx_star_synth_audio_processor.h>

#include <ags/i18n.h>

void ags_fx_star_synth_audio_processor_class_init(AgsFxStarSynthAudioProcessorClass *fx_star_synth_audio_processor);
void ags_fx_star_synth_audio_processor_init(AgsFxStarSynthAudioProcessor *fx_star_synth_audio_processor);
void ags_fx_star_synth_audio_processor_dispose(GObject *gobject);
void ags_fx_star_synth_audio_processor_finalize(GObject *gobject);

/**
 * SECTION:ags_fx_star_synth_audio_processor
 * @short_description: fx star synth audio processor
 * @title: AgsFxStarSynthAudioProcessor
 * @section_id:
 * @include: ags/audio/fx/ags_fx_star_synth_audio_processor.h
 *
 * The #AgsFxStarSynthAudioProcessor class provides ports to the effect processor.
 */

static gpointer ags_fx_star_synth_audio_processor_parent_class = NULL;

GType
ags_fx_star_synth_audio_processor_get_type()
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_fx_star_synth_audio_processor = 0;

    static const GTypeInfo ags_fx_star_synth_audio_processor_info = {
      sizeof (AgsFxStarSynthAudioProcessorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_star_synth_audio_processor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio_processor */
      sizeof (AgsFxStarSynthAudioProcessor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_star_synth_audio_processor_init,
    };

    ags_type_fx_star_synth_audio_processor = g_type_register_static(AGS_TYPE_FX_NOTATION_AUDIO_PROCESSOR,
								    "AgsFxStarSynthAudioProcessor",
								    &ags_fx_star_synth_audio_processor_info,
								    0);

    g_once_init_leave(&g_define_type_id__static, ags_type_fx_star_synth_audio_processor);
  }

  return(g_define_type_id__static);
}

void
ags_fx_star_synth_audio_processor_class_init(AgsFxStarSynthAudioProcessorClass *fx_star_synth_audio_processor)
{
  GObjectClass *gobject;
  
  ags_fx_star_synth_audio_processor_parent_class = g_type_class_peek_parent(fx_star_synth_audio_processor);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_star_synth_audio_processor;

  gobject->dispose = ags_fx_star_synth_audio_processor_dispose;
  gobject->finalize = ags_fx_star_synth_audio_processor_finalize;
}

void
ags_fx_star_synth_audio_processor_init(AgsFxStarSynthAudioProcessor *fx_star_synth_audio_processor)
{
  AGS_RECALL(fx_star_synth_audio_processor)->name = "ags-fx-star-synth";
  AGS_RECALL(fx_star_synth_audio_processor)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_star_synth_audio_processor)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_star_synth_audio_processor)->xml_type = "ags-fx-star-synth-audio-processor";
}

void
ags_fx_star_synth_audio_processor_dispose(GObject *gobject)
{
  AgsFxStarSynthAudioProcessor *fx_star_synth_audio_processor;
  
  fx_star_synth_audio_processor = AGS_FX_STAR_SYNTH_AUDIO_PROCESSOR(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_star_synth_audio_processor_parent_class)->dispose(gobject);
}

void
ags_fx_star_synth_audio_processor_finalize(GObject *gobject)
{
  AgsFxStarSynthAudioProcessor *fx_star_synth_audio_processor;
  
  fx_star_synth_audio_processor = AGS_FX_STAR_SYNTH_AUDIO_PROCESSOR(gobject);  
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_star_synth_audio_processor_parent_class)->finalize(gobject);
}

/**
 * ags_fx_star_synth_audio_processor_new:
 * @audio: the #AgsAudio
 *
 * Create a new instance of #AgsFxStarSynthAudioProcessor
 *
 * Returns: the new #AgsFxStarSynthAudioProcessor
 *
 * Since: 7.4.0
 */
AgsFxStarSynthAudioProcessor*
ags_fx_star_synth_audio_processor_new(AgsAudio *audio)
{
  AgsFxStarSynthAudioProcessor *fx_star_synth_audio_processor;

  fx_star_synth_audio_processor = (AgsFxStarSynthAudioProcessor *) g_object_new(AGS_TYPE_FX_STAR_SYNTH_AUDIO_PROCESSOR,
										"audio", audio,
										NULL);

  return(fx_star_synth_audio_processor);
}
