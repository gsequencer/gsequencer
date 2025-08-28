/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2025 Joël Krähemann
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

#include <ags/audio/fx/ags_fx_buffer_audio.h>

#include <ags/i18n.h>

void ags_fx_buffer_audio_class_init(AgsFxBufferAudioClass *fx_buffer_audio);
void ags_fx_buffer_audio_init(AgsFxBufferAudio *fx_buffer_audio);
void ags_fx_buffer_audio_dispose(GObject *gobject);
void ags_fx_buffer_audio_finalize(GObject *gobject);

/**
 * SECTION:ags_fx_buffer_audio
 * @short_description: fx buffer audio
 * @title: AgsFxBufferAudio
 * @section_id:
 * @include: ags/audio/fx/ags_fx_buffer_audio.h
 *
 * The #AgsFxBufferAudio class provides ports to the effect processor.
 */

static gpointer ags_fx_buffer_audio_parent_class = NULL;

const gchar *ags_fx_buffer_audio_plugin_name = "ags-fx-buffer";

GType
ags_fx_buffer_audio_get_type()
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_fx_buffer_audio = 0;

    static const GTypeInfo ags_fx_buffer_audio_info = {
      sizeof (AgsFxBufferAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_buffer_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio */
      sizeof (AgsFxBufferAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_buffer_audio_init,
    };

    ags_type_fx_buffer_audio = g_type_register_static(AGS_TYPE_RECALL_AUDIO,
						      "AgsFxBufferAudio",
						      &ags_fx_buffer_audio_info,
						      0);

    g_once_init_leave(&g_define_type_id__static, ags_type_fx_buffer_audio);
  }

  return(g_define_type_id__static);
}

void
ags_fx_buffer_audio_class_init(AgsFxBufferAudioClass *fx_buffer_audio)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_fx_buffer_audio_parent_class = g_type_class_peek_parent(fx_buffer_audio);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_buffer_audio;

  gobject->dispose = ags_fx_buffer_audio_dispose;
  gobject->finalize = ags_fx_buffer_audio_finalize;

  /* properties */
}

void
ags_fx_buffer_audio_init(AgsFxBufferAudio *fx_buffer_audio)
{
  guint i;

  AGS_RECALL(fx_buffer_audio)->name = "ags-fx-buffer";
  AGS_RECALL(fx_buffer_audio)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_buffer_audio)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_buffer_audio)->xml_type = "ags-fx-buffer-audio";

  fx_buffer_audio->scope_data_ready = FALSE;
  
  /* scope data */
  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    fx_buffer_audio->scope_data[i] = ags_fx_buffer_audio_scope_data_alloc();
      
    fx_buffer_audio->scope_data[i]->parent = fx_buffer_audio;
  }
}

void
ags_fx_buffer_audio_dispose(GObject *gobject)
{
  AgsFxBufferAudio *fx_buffer_audio;
  
  fx_buffer_audio = AGS_FX_BUFFER_AUDIO(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_buffer_audio_parent_class)->dispose(gobject);
}

void
ags_fx_buffer_audio_finalize(GObject *gobject)
{
  AgsFxBufferAudio *fx_buffer_audio;
  
  fx_buffer_audio = AGS_FX_BUFFER_AUDIO(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_buffer_audio_parent_class)->finalize(gobject);
}

/**
 * ags_fx_buffer_audio_scope_data_alloc:
 * 
 * Allocate #AgsFxBufferAudioScopeData-struct
 * 
 * Returns: (type gpointer) (transfer full): the new #AgsFxBufferAudioScopeData-struct
 * 
 * Since: 8.1.2
 */
AgsFxBufferAudioScopeData*
ags_fx_buffer_audio_scope_data_alloc()
{
  AgsFxBufferAudioScopeData *scope_data;

  scope_data = (AgsFxBufferAudioScopeData *) g_malloc(sizeof(AgsFxBufferAudioScopeData));

  g_rec_mutex_init(&(scope_data->strct_mutex));

  scope_data->parent = NULL;
  
  scope_data->destination = g_hash_table_new_full(g_direct_hash, g_direct_equal,
						  g_object_unref,
						  g_object_unref);


  scope_data->resample_cache = g_hash_table_new_full(g_direct_hash, g_direct_equal,
						     g_object_unref,
						     ags_stream_free);
  
  return(scope_data);
}

/**
 * ags_fx_buffer_audio_scope_data_free:
 * @scope_data: (type gpointer) (transfer full): the #AgsFxBufferAudioScopeData-struct
 * 
 * Free @scope_data.
 * 
 * Since: 8.1.2
 */
void
ags_fx_buffer_audio_scope_data_free(AgsFxBufferAudioScopeData *scope_data)
{
  if(scope_data == NULL){
    return;
  }

  g_hash_table_destroy(scope_data->destination);
  g_hash_table_destroy(scope_data->resample_cache);

  g_free(scope_data);
}

/**
 * ags_fx_buffer_audio_new:
 * @audio: the #AgsAudio
 *
 * Create a new instance of #AgsFxBufferAudio
 *
 * Returns: the new #AgsFxBufferAudio
 *
 * Since: 3.3.0
 */
AgsFxBufferAudio*
ags_fx_buffer_audio_new(AgsAudio *audio)
{
  AgsFxBufferAudio *fx_buffer_audio;

  fx_buffer_audio = (AgsFxBufferAudio *) g_object_new(AGS_TYPE_FX_BUFFER_AUDIO,
						      "audio", audio,
						      NULL);

  return(fx_buffer_audio);
}
