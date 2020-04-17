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

#include <ags/audio/fx/ags_fx_notation_audio_processor.h>

void ags_fx_notation_audio_processor_class_init(AgsFxNotationAudioProcessorClass *fx_notation_audio_processor);
void ags_fx_notation_audio_processor_init(AgsFxNotationAudioProcessor *fx_notation_audio_processor);
void ags_fx_notation_audio_processor_dispose(GObject *gobject);
void ags_fx_notation_audio_processor_finalize(GObject *gobject);

/**
 * SECTION:ags_fx_notation_audio_processor
 * @short_description: fx notation audio_processor
 * @title: AgsFxNotationAudioProcessor
 * @section_id:
 * @include: ags/audio/fx/ags_fx_notation_audio_processor.h
 *
 * The #AgsFxNotationAudioProcessor class provides ports to the effect processor.
 */

static gpointer ags_fx_notation_audio_processor_parent_class = NULL;

GType
ags_fx_notation_audio_processor_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_notation_audio_processor = 0;

    static const GTypeInfo ags_fx_notation_audio_processor_info = {
      sizeof (AgsFxNotationAudioProcessorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_notation_audio_processor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio_processor */
      sizeof (AgsFxNotationAudioProcessor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_notation_audio_processor_init,
    };

    ags_type_fx_notation_audio_processor = g_type_register_static(AGS_TYPE_RECALL_AUDIO_PROCESSOR,
								  "AgsFxNotationAudioProcessor",
								  &ags_fx_notation_audio_processor_info,
								  0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_notation_audio_processor);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_notation_audio_processor_class_init(AgsFxNotationAudioProcessorClass *fx_notation_audio_processor)
{
  GObjectClass *gobject;

  ags_fx_notation_audio_processor_parent_class = g_type_class_peek_parent(fx_notation_audio_processor);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_notation_audio_processor;

  gobject->dispose = ags_fx_notation_audio_processor_dispose;
  gobject->finalize = ags_fx_notation_audio_processor_finalize;

  /* AgsFxNotationAudioProcessorClass */
  fx_notation_audio_processor->key_on = ags_fx_notation_audio_processor_key_on;
  fx_notation_audio_processor->key_off = ags_fx_notation_audio_processor_key_off;
  fx_notation_audio_processor->key_pressure = ags_fx_notation_audio_processor_key_pressure;

  fx_notation_audio_processor->play = ags_fx_notation_audio_processor_play;
  fx_notation_audio_processor->record = ags_fx_notation_audio_processor_record;
  fx_notation_audio_processor->feed = ags_fx_notation_audio_processor_feed;
}

void
ags_fx_notation_audio_processor_init(AgsFxNotationAudioProcessor *fx_notation_audio_processor)
{
  guint i;

  for(i = 0; i < AGS_FX_NOTATION_AUDIO_PROCESSOR_MAX_CHANNELS; i++){
    fx_notation_audio_processor->audio_signal[i] = NULL;
  }
}

void
ags_fx_notation_audio_processor_dispose(GObject *gobject)
{
  AgsFxNotationAudioProcessor *fx_notation_audio_processor;
  
  fx_notation_audio_processor = AGS_FX_NOTATION_AUDIO_PROCESSOR(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_notation_audio_processor_parent_class)->dispose(gobject);
}

void
ags_fx_notation_audio_processor_finalize(GObject *gobject)
{
  AgsFxNotationAudioProcessor *fx_notation_audio_processor;
  
  fx_notation_audio_processor = AGS_FX_NOTATION_AUDIO_PROCESSOR(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_notation_audio_processor_parent_class)->finalize(gobject);
}

void
ags_fx_notation_audio_processor_key_on(AgsFxNotationAudioProcessor *fx_notation_audio_processor,
				       AgsNote *note,
				       guint velocity)
{
  //TODO:JK: implement me
}

void
ags_fx_notation_audio_processor_key_off(AgsFxNotationAudioProcessor *fx_notation_audio_processor,
					AgsNote *note,
					guint velocity)
{
  //TODO:JK: implement me
}

void
ags_fx_notation_audio_processor_key_pressure(AgsFxNotationAudioProcessor *fx_notation_audio_processor,
					     AgsNote *note,
					     guint velocity)
{
  //TODO:JK: implement me
}

void
ags_fx_notation_audio_processor_play(AgsFxNotationAudioProcessor *fx_notation_audio_processor)
{
  //TODO:JK: implement me
}

void
ags_fx_notation_audio_processor_record(AgsFxNotationAudioProcessor *fx_notation_audio_processor)
{
  //TODO:JK: implement me
}

void
ags_fx_notation_audio_processor_feed(AgsFxNotationAudioProcessor *fx_notation_audio_processor)
{
  //TODO:JK: implement me
}

/**
 * ags_fx_notation_audio_processor_new:
 * @audio: the #AgsAudio
 *
 * Create a new instance of #AgsFxNotationAudioProcessor
 *
 * Returns: the new #AgsFxNotationAudioProcessor
 *
 * Since: 3.3.0
 */
AgsFxNotationAudioProcessor*
ags_fx_notation_audio_processor_new(AgsAudio *audio)
{
  AgsFxNotationAudioProcessor *fx_notation_audio_processor;

  fx_notation_audio_processor = (AgsFxNotationAudioProcessor *) g_object_new(AGS_TYPE_FX_NOTATION_AUDIO_PROCESSOR,
									     "audio", audio,
									     NULL);

  return(fx_notation_audio_processor);
}
