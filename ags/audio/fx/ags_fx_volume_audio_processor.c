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

#include <ags/audio/fx/ags_fx_volume_audio_processor.h>

#include <ags/i18n.h>

void ags_fx_volume_audio_processor_class_init(AgsFxVolumeAudioProcessorClass *fx_volume_audio_processor);
void ags_fx_volume_audio_processor_init(AgsFxVolumeAudioProcessor *fx_volume_audio_processor);
void ags_fx_volume_audio_processor_dispose(GObject *gobject);
void ags_fx_volume_audio_processor_finalize(GObject *gobject);

/**
 * SECTION:ags_fx_volume_audio_processor
 * @short_description: fx volume audio processor
 * @title: AgsFxVolumeAudioProcessor
 * @section_id:
 * @include: ags/audio/fx/ags_fx_volume_audio_processor.h
 *
 * The #AgsFxVolumeAudioProcessor class provides ports to the effect processor.
 */

static gpointer ags_fx_volume_audio_processor_parent_class = NULL;

GType
ags_fx_volume_audio_processor_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_volume_audio_processor = 0;

    static const GTypeInfo ags_fx_volume_audio_processor_info = {
      sizeof (AgsFxVolumeAudioProcessorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_volume_audio_processor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio_processor */
      sizeof (AgsFxVolumeAudioProcessor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_volume_audio_processor_init,
    };

    ags_type_fx_volume_audio_processor = g_type_register_static(AGS_TYPE_RECALL_AUDIO_RUN,
								"AgsFxVolumeAudioProcessor",
								&ags_fx_volume_audio_processor_info,
								0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_volume_audio_processor);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_volume_audio_processor_class_init(AgsFxVolumeAudioProcessorClass *fx_volume_audio_processor)
{
  GObjectClass *gobject;
  
  ags_fx_volume_audio_processor_parent_class = g_type_class_peek_parent(fx_volume_audio_processor);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_volume_audio_processor;

  gobject->dispose = ags_fx_volume_audio_processor_dispose;
  gobject->finalize = ags_fx_volume_audio_processor_finalize;
}

void
ags_fx_volume_audio_processor_init(AgsFxVolumeAudioProcessor *fx_volume_audio_processor)
{
  AGS_RECALL(fx_volume_audio_processor)->name = "ags-fx-volume";
  AGS_RECALL(fx_volume_audio_processor)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_volume_audio_processor)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_volume_audio_processor)->xml_type = "ags-fx-volume-audio-processor";
}

void
ags_fx_volume_audio_processor_dispose(GObject *gobject)
{
  AgsFxVolumeAudioProcessor *fx_volume_audio_processor;
  
  fx_volume_audio_processor = AGS_FX_VOLUME_AUDIO_PROCESSOR(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_volume_audio_processor_parent_class)->dispose(gobject);
}

void
ags_fx_volume_audio_processor_finalize(GObject *gobject)
{
  AgsFxVolumeAudioProcessor *fx_volume_audio_processor;
  
  fx_volume_audio_processor = AGS_FX_VOLUME_AUDIO_PROCESSOR(gobject);  
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_volume_audio_processor_parent_class)->finalize(gobject);
}

/**
 * ags_fx_volume_audio_processor_new:
 * @audio: the #AgsAudio
 *
 * Create a new instance of #AgsFxVolumeAudioProcessor
 *
 * Returns: the new #AgsFxVolumeAudioProcessor
 *
 * Since: 3.3.0
 */
AgsFxVolumeAudioProcessor*
ags_fx_volume_audio_processor_new(AgsAudio *audio)
{
  AgsFxVolumeAudioProcessor *fx_volume_audio_processor;

  fx_volume_audio_processor = (AgsFxVolumeAudioProcessor *) g_object_new(AGS_TYPE_FX_VOLUME_AUDIO_PROCESSOR,
									 "audio", audio,
									 NULL);

  return(fx_volume_audio_processor);
}
