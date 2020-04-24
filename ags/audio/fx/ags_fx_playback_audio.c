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

#include <ags/audio/fx/ags_fx_playback_audio.h>

#include <ags/i18n.h>

void ags_fx_playback_audio_class_init(AgsFxPlaybackAudioClass *fx_playback_audio);
void ags_fx_playback_audio_init(AgsFxPlaybackAudio *fx_playback_audio);
void ags_fx_playback_audio_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec);
void ags_fx_playback_audio_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec);
void ags_fx_playback_audio_dispose(GObject *gobject);
void ags_fx_playback_audio_finalize(GObject *gobject);

/**
 * SECTION:ags_fx_playback_audio
 * @short_description: fx playback audio
 * @title: AgsFxPlaybackAudio
 * @section_id:
 * @include: ags/audio/fx/ags_fx_playback_audio.h
 *
 * The #AgsFxPlaybackAudio class provides ports to the effect processor.
 */

static gpointer ags_fx_playback_audio_parent_class = NULL;

static const gchar *ags_fx_playback_audio_plugin_name = "ags-fx-playback";

static const gchar *ags_fx_playback_audio_specifier[] = {
  "./bpm[0]",
  "./tact[0]",
  "./delay[0]",
  "./duration[0]"
  "./loop[0]",
  "./loop-start[0]",
  "./loop-end[0]",
  "./export[0]",
  "./filename[0]",
  "./file-audio-channels[0]",
  "./file-samplerate[0]",
  "./file-buffer-size[0]",
  "./file-format[0]",
  NULL,
};

static const gchar *ags_fx_playback_audio_control_port[] = {
  "1/13",
  "2/13",
  "3/13",
  "4/13",
  "5/13",
  "6/13",
  "7/13",
  "8/13",
  "9/13",
  "10/13",
  "11/13",
  "12/13",
  "13/13",
  NULL,
};

enum{
  PROP_0,
  PROP_TACT,
  PROP_BPM,
  PROP_DELAY,
  PROP_DURATION,
  PROP_LOOP,
  PROP_LOOP_START,
  PROP_LOOP_END,
  PROP_EXPORT,
  PROP_FILENAME,
  PROP_FILE_AUDIO_CHANNELS,
  PROP_FILE_SAMPLERATE,
  PROP_FILE_BUFFER_SIZE,
  PROP_FILE_FORMAT,
};

GType
ags_fx_playback_audio_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_playback_audio = 0;

    static const GTypeInfo ags_fx_playback_audio_info = {
      sizeof (AgsFxPlaybackAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_playback_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio */
      sizeof (AgsFxPlaybackAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_playback_audio_init,
    };

    ags_type_fx_playback_audio = g_type_register_static(AGS_TYPE_RECALL_AUDIO,
							"AgsFxPlaybackAudio",
							&ags_fx_playback_audio_info,
							0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_playback_audio);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_playback_audio_class_init(AgsFxPlaybackAudioClass *fx_playback_audio)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_fx_playback_audio_parent_class = g_type_class_peek_parent(fx_playback_audio);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_playback_audio;

  gobject->set_property = ags_fx_playback_audio_set_property;
  gobject->get_property = ags_fx_playback_audio_get_property;

  gobject->dispose = ags_fx_playback_audio_dispose;
  gobject->finalize = ags_fx_playback_audio_finalize;

  /* properties */
  /**
   * AgsFxPlaybackAudio:bpm:
   *
   * The beats per minute.
   * 
   * Since: 3.3.0
   */
  param_spec = g_param_spec_object("bpm",
				   i18n_pspec("bpm of recall"),
				   i18n_pspec("The recall's bpm"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BPM,
				  param_spec);

  /**
   * AgsFxPlaybackAudio:tact:
   *
   * The tact segmentation.
   * 
   * Since: 3.3.0
   */
  param_spec = g_param_spec_object("tact",
				   i18n_pspec("tact of recall"),
				   i18n_pspec("The recall's tact"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TACT,
				  param_spec);

  /**
   * AgsFxPlaybackAudio:delay:
   *
   * The playback's delay.
   * 
   * Since: 3.3.0
   */
  param_spec = g_param_spec_object("delay",
				   i18n_pspec("delay of recall"),
				   i18n_pspec("The delay of the playback"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DELAY,
				  param_spec);

  /**
   * AgsFxPlaybackAudio:duration:
   *
   * The playback's duration.
   * 
   * Since: 3.3.0
   */
  param_spec = g_param_spec_object("duration",
				   i18n_pspec("duration of recall"),
				   i18n_pspec("The duration of the playback"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DURATION,
				  param_spec);

  /**
   * AgsFxPlaybackAudio:loop:
   *
   * Do loop.
   * 
   * Since: 3.3.0
   */
  param_spec = g_param_spec_object("loop",
				   i18n_pspec("loop playing"),
				   i18n_pspec("Play in a infinite loop"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LOOP,
				  param_spec);

  /**
   * AgsFxPlaybackAudio:loop-start:
   *
   * The playback's loop-start.
   * 
   * Since: 3.3.0
   */
  param_spec = g_param_spec_object("loop-start",
				   i18n_pspec("start beat of playback loop"),
				   i18n_pspec("The start beat of the playback loop"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LOOP_START,
				  param_spec);

  /**
   * AgsFxPlaybackAudio:loop-end:
   *
   * The playback's loop-end.
   * 
   * Since: 3.3.0
   */
  param_spec = g_param_spec_object("loop-end",
				   i18n_pspec("end beat of playback loop"),
				   i18n_pspec("The end beat of the playback loop"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LOOP_END,
				  param_spec);

  /**
   * AgsFxPlaybackAudio:export:
   *
   * The beats per minute.
   * 
   * Since: 3.3.0
   */
  param_spec = g_param_spec_object("export",
				   i18n_pspec("export of recall"),
				   i18n_pspec("The recall's export"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_EXPORT,
				  param_spec);

  /**
   * AgsFxPlaybackAudio:filename:
   *
   * The beats per minute.
   * 
   * Since: 3.3.0
   */
  param_spec = g_param_spec_object("filename",
				   i18n_pspec("filename of recall"),
				   i18n_pspec("The recall's filename"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILENAME,
				  param_spec);

  /**
   * AgsFxPlaybackAudio:file-audio-channels:
   *
   * The beats per minute.
   * 
   * Since: 3.3.0
   */
  param_spec = g_param_spec_object("file-audio-channels",
				   i18n_pspec("file audio channels of recall"),
				   i18n_pspec("The recall's file audio channels"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILE_AUDIO_CHANNELS,
				  param_spec);
  
  /**
   * AgsFxPlaybackAudio:file-samplerate:
   *
   * The beats per minute.
   * 
   * Since: 3.3.0
   */
  param_spec = g_param_spec_object("file-samplerate",
				   i18n_pspec("file samplerate of recall"),
				   i18n_pspec("The recall's file samplerate"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILE_SAMPLERATE,
				  param_spec);

  /**
   * AgsFxPlaybackAudio:file-buffer-size:
   *
   * The beats per minute.
   * 
   * Since: 3.3.0
   */
  param_spec = g_param_spec_object("file-buffer-size",
				   i18n_pspec("file buffer size of recall"),
				   i18n_pspec("The recall's file buffer size"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILE_BUFFER_SIZE,
				  param_spec);

  /**
   * AgsFxPlaybackAudio:file-format:
   *
   * The beats per minute.
   * 
   * Since: 3.3.0
   */
  param_spec = g_param_spec_object("file-format",
				   i18n_pspec("file format of recall"),
				   i18n_pspec("The recall's file format"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILE_FORMAT,
				  param_spec);
}

void
ags_fx_playback_audio_init(AgsFxPlaybackAudio *fx_playback_audio)
{
  gdouble bpm;
  gdouble delay;

  AGS_RECALL(fx_playback_audio)->name = "ags-fx-playback";
  AGS_RECALL(fx_playback_audio)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_playback_audio)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_playback_audio)->xml_type = "ags-fx-playback-audio";

  fx_playback_audio->flags = 0;
  
  fx_playback_audio->feed_audio_signal = NULL;
  fx_playback_audio->master_audio_signal = NULL;

  fx_playback_audio->audio_file = NULL;
  
  bpm = AGS_SOUNDCARD_DEFAULT_BPM;
  delay = AGS_SOUNDCARD_DEFAULT_DELAY;
  
  /* bpm */
  fx_playback_audio->bpm = g_object_new(AGS_TYPE_PORT,
					"plugin-name", ags_fx_playback_audio_plugin_name,
					"specifier", ags_fx_playback_audio_specifier[0],
					"control-port", ags_fx_playback_audio_control_port[0],
					"port-value-is-pointer", FALSE,
					"port-value-type", G_TYPE_DOUBLE,
					"port-value-size", sizeof(gdouble),
					"port-value-length", 1,
					NULL);
  g_object_ref(fx_playback_audio->bpm);
  
  fx_playback_audio->bpm->port_value.ags_port_double = bpm;

  ags_recall_add_port((AgsRecall *) fx_playback_audio,
		      fx_playback_audio->bpm);
  
  /* tact */
  fx_playback_audio->tact = g_object_new(AGS_TYPE_PORT,
					 "plugin-name", ags_fx_playback_audio_plugin_name,
					 "specifier", ags_fx_playback_audio_specifier[1],
					 "control-port", ags_fx_playback_audio_control_port[1],
					 "port-value-is-pointer", FALSE,
					 "port-value-type", G_TYPE_DOUBLE,
					 "port-value-size", sizeof(gdouble),
					 "port-value-length", 1,
					 NULL);
  g_object_ref(fx_playback_audio->tact);
  
  fx_playback_audio->tact->port_value.ags_port_double = AGS_SOUNDCARD_DEFAULT_TACT;

  ags_recall_add_port((AgsRecall *) fx_playback_audio,
		      fx_playback_audio->tact);

  /* delay */
  fx_playback_audio->delay = g_object_new(AGS_TYPE_PORT,
					  "plugin-name", ags_fx_playback_audio_plugin_name,
					  "specifier", ags_fx_playback_audio_specifier[2],
					  "control-port", ags_fx_playback_audio_control_port[2],
					  "port-value-is-pointer", FALSE,
					  "port-value-type", G_TYPE_DOUBLE,
					  "port-value-size", sizeof(gdouble),
					  "port-value-length", 1,
					  NULL);
  g_object_ref(fx_playback_audio->delay);
  
  fx_playback_audio->delay->port_value.ags_port_double = delay;

  ags_recall_add_port((AgsRecall *) fx_playback_audio,
		      fx_playback_audio->delay);
  
  /* duration */
  fx_playback_audio->duration = g_object_new(AGS_TYPE_PORT,
					     "plugin-name", ags_fx_playback_audio_plugin_name,
					     "specifier", ags_fx_playback_audio_specifier[3],
					     "control-port", ags_fx_playback_audio_control_port[3],
					     "port-value-is-pointer", FALSE,
					     "port-value-type", G_TYPE_UINT64,
					     "port-value-size", sizeof(guint64),
					     "port-value-length", 1,
					     NULL);
  g_object_ref(fx_playback_audio->duration);
  
  fx_playback_audio->duration->port_value.ags_port_double = ceil(AGS_NOTATION_DEFAULT_DURATION * delay);

  ags_recall_add_port((AgsRecall *) fx_playback_audio,
		      fx_playback_audio->duration);

  /* loop */
  fx_playback_audio->loop = g_object_new(AGS_TYPE_PORT,
					 "plugin-name", ags_fx_playback_audio_plugin_name,
					 "specifier", ags_fx_playback_audio_specifier[4],
					 "control-port", ags_fx_playback_audio_control_port[4],
					 "port-value-is-pointer", FALSE,
					 "port-value-type", G_TYPE_BOOLEAN,
					 "port-value-size", sizeof(gboolean),
					 "port-value-length", 1,
					 NULL);
  g_object_ref(fx_playback_audio->loop);
  
  fx_playback_audio->loop->port_value.ags_port_boolean = FALSE;

  ags_recall_add_port((AgsRecall *) fx_playback_audio,
		      fx_playback_audio->loop);

  /* loop-start */
  fx_playback_audio->loop_start = g_object_new(AGS_TYPE_PORT,
					       "plugin-name", ags_fx_playback_audio_plugin_name,
					       "specifier", ags_fx_playback_audio_specifier[5],
					       "control-port", ags_fx_playback_audio_control_port[5],
					       "port-value-is-pointer", FALSE,
					       "port-value-type", G_TYPE_UINT64,
					       "port-value-size", sizeof(guint64),
					       "port-value-length", 1,
					       NULL);
  g_object_ref(fx_playback_audio->loop_start);
  
  fx_playback_audio->loop_start->port_value.ags_port_uint = AGS_FX_PLAYBACK_AUDIO_DEFAULT_LOOP_START;

  ags_recall_add_port((AgsRecall *) fx_playback_audio,
		      fx_playback_audio->loop_start);
  
  /* loop-end */
  fx_playback_audio->loop_end = g_object_new(AGS_TYPE_PORT,
					     "plugin-name", ags_fx_playback_audio_plugin_name,
					     "specifier", ags_fx_playback_audio_specifier[6],
					     "control-port", ags_fx_playback_audio_control_port[6],
					     "port-value-is-pointer", FALSE,
					     "port-value-type", G_TYPE_UINT64,
					     "port-value-size", sizeof(guint64),
					     "port-value-length", 1,
					     NULL);
  g_object_ref(fx_playback_audio->loop_end);
  
  fx_playback_audio->loop_end->port_value.ags_port_uint = AGS_FX_PLAYBACK_AUDIO_DEFAULT_LOOP_END;

  ags_recall_add_port((AgsRecall *) fx_playback_audio,
		      fx_playback_audio->loop_end);

  /* export */
  fx_playback_audio->export = g_object_new(AGS_TYPE_PORT,
					   "plugin-name", ags_fx_playback_audio_plugin_name,
					   "specifier", ags_fx_playback_audio_specifier[7],
					   "control-port", ags_fx_playback_audio_control_port[7],
					   "port-value-is-pointer", FALSE,
					   "port-value-type", G_TYPE_BOOLEAN,
					   "port-value-size", sizeof(gboolean),
					   "port-value-length", 1,
					   NULL);
  g_object_ref(fx_playback_audio->export);
  
  fx_playback_audio->export->port_value.ags_port_boolean = FALSE;

  ags_recall_add_port((AgsRecall *) fx_playback_audio,
		      fx_playback_audio->export);

  /* filename */
  fx_playback_audio->filename = g_object_new(AGS_TYPE_PORT,
					     "plugin-name", ags_fx_playback_audio_plugin_name,
					     "specifier", ags_fx_playback_audio_specifier[7],
					     "control-port", ags_fx_playback_audio_control_port[7],
					     "port-value-is-pointer", FALSE,
					     "port-value-type", G_TYPE_POINTER,
					     NULL);
  g_object_ref(fx_playback_audio->filename);
  
  fx_playback_audio->filename->port_value.ags_port_pointer = NULL;

  ags_recall_add_port((AgsRecall *) fx_playback_audio,
		      fx_playback_audio->filename);
  
  /* file audio channels */
  fx_playback_audio->file_audio_channels = g_object_new(AGS_TYPE_PORT,
							"plugin-name", ags_fx_playback_audio_plugin_name,
							"specifier", ags_fx_playback_audio_specifier[8],
							"control-port", ags_fx_playback_audio_control_port[8],
							"port-value-is-pointer", FALSE,
							"port-value-type", G_TYPE_UINT64,
							"port-value-size", sizeof(guint64),
							"port-value-length", 1,
							NULL);
  g_object_ref(fx_playback_audio->file_audio_channels);
  
  fx_playback_audio->file_audio_channels->port_value.ags_port_uint = AGS_SOUNDCARD_DEFAULT_PCM_CHANNELS;

  ags_recall_add_port((AgsRecall *) fx_playback_audio,
		      fx_playback_audio->file_audio_channels);

  /* file samplerate */
  fx_playback_audio->file_samplerate = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_playback_audio_plugin_name,
						    "specifier", ags_fx_playback_audio_specifier[9],
						    "control-port", ags_fx_playback_audio_control_port[9],
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_UINT64,
						    "port-value-size", sizeof(guint64),
						    "port-value-length", 1,
						    NULL);
  g_object_ref(fx_playback_audio->file_samplerate);
  
  fx_playback_audio->file_samplerate->port_value.ags_port_uint = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;

  ags_recall_add_port((AgsRecall *) fx_playback_audio,
		      fx_playback_audio->file_samplerate);

  /* file buffer size */
  fx_playback_audio->file_buffer_size = g_object_new(AGS_TYPE_PORT,
						     "plugin-name", ags_fx_playback_audio_plugin_name,
						     "specifier", ags_fx_playback_audio_specifier[10],
						     "control-port", ags_fx_playback_audio_control_port[10],
						     "port-value-is-pointer", FALSE,
						     "port-value-type", G_TYPE_UINT64,
						     "port-value-size", sizeof(guint64),
						     "port-value-length", 1,
						     NULL);
  g_object_ref(fx_playback_audio->file_buffer_size);
  
  fx_playback_audio->file_buffer_size->port_value.ags_port_uint = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;

  ags_recall_add_port((AgsRecall *) fx_playback_audio,
		      fx_playback_audio->file_buffer_size);

  /* file format */
  fx_playback_audio->file_format = g_object_new(AGS_TYPE_PORT,
						"plugin-name", ags_fx_playback_audio_plugin_name,
						"specifier", ags_fx_playback_audio_specifier[11],
						"control-port", ags_fx_playback_audio_control_port[11],
						"port-value-is-pointer", FALSE,
						"port-value-type", G_TYPE_UINT64,
						"port-value-size", sizeof(guint64),
						"port-value-length", 1,
						NULL);
  g_object_ref(fx_playback_audio->file_format);
  
  fx_playback_audio->file_format->port_value.ags_port_uint = AGS_SOUNDCARD_DEFAULT_FORMAT;

  ags_recall_add_port((AgsRecall *) fx_playback_audio,
		      fx_playback_audio->file_format);
}

void
ags_fx_playback_audio_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec)
{
  AgsFxPlaybackAudio *fx_playback_audio;

  GRecMutex *recall_mutex;
  
  fx_playback_audio = AGS_FX_PLAYBACK_AUDIO(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_playback_audio);

  switch(prop_id){
  case PROP_BPM:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_playback_audio->bpm){
      g_rec_mutex_unlock(recall_mutex);

      return;
    }

    if(fx_playback_audio->bpm != NULL){
      g_object_unref(G_OBJECT(fx_playback_audio->bpm));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_playback_audio->bpm = port;

    g_rec_mutex_unlock(recall_mutex);
  }
  break;
  case PROP_TACT:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_playback_audio->tact){
      g_rec_mutex_unlock(recall_mutex);

      return;
    }

    if(fx_playback_audio->tact != NULL){
      g_object_unref(G_OBJECT(fx_playback_audio->tact));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_playback_audio->tact = port;

    g_rec_mutex_unlock(recall_mutex);
  }
  break;
  case PROP_DELAY:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_playback_audio->delay){
      g_rec_mutex_unlock(recall_mutex);

      return;
    }

    if(fx_playback_audio->delay != NULL){
      g_object_unref(G_OBJECT(fx_playback_audio->delay));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_playback_audio->delay = port;

    g_rec_mutex_unlock(recall_mutex);
  }
  break;
  case PROP_DURATION:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_playback_audio->duration){
      g_rec_mutex_unlock(recall_mutex);

      return;
    }

    if(fx_playback_audio->duration != NULL){
      g_object_unref(G_OBJECT(fx_playback_audio->duration));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_playback_audio->duration = port;

    g_rec_mutex_unlock(recall_mutex);
  }
  break;
  case PROP_LOOP:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_playback_audio->loop){
      g_rec_mutex_unlock(recall_mutex);

      return;
    }

    if(fx_playback_audio->loop != NULL){
      g_object_unref(G_OBJECT(fx_playback_audio->loop));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_playback_audio->loop = port;

    g_rec_mutex_unlock(recall_mutex);
  }
  break;
  case PROP_LOOP_START:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_playback_audio->loop_start){
      g_rec_mutex_unlock(recall_mutex);

      return;
    }

    if(fx_playback_audio->loop_start != NULL){
      g_object_unref(G_OBJECT(fx_playback_audio->loop_start));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_playback_audio->loop_start = port;

    g_rec_mutex_unlock(recall_mutex);
  }
  break;
  case PROP_LOOP_END:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_playback_audio->loop_end){
      g_rec_mutex_unlock(recall_mutex);

      return;
    }

    if(fx_playback_audio->loop_end != NULL){
      g_object_unref(G_OBJECT(fx_playback_audio->loop_end));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_playback_audio->loop_end = port;

    g_rec_mutex_unlock(recall_mutex);
  }
  break;
  case PROP_EXPORT:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_playback_audio->export){
      g_rec_mutex_unlock(recall_mutex);

      return;
    }

    if(fx_playback_audio->export != NULL){
      g_object_unref(G_OBJECT(fx_playback_audio->export));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_playback_audio->export = port;

    g_rec_mutex_unlock(recall_mutex);
  }
  break;
  case PROP_FILENAME:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_playback_audio->filename){
      g_rec_mutex_unlock(recall_mutex);

      return;
    }

    if(fx_playback_audio->filename != NULL){
      g_object_unref(G_OBJECT(fx_playback_audio->filename));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_playback_audio->filename = port;

    g_rec_mutex_unlock(recall_mutex);
  }
  break;
  case PROP_FILE_AUDIO_CHANNELS:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_playback_audio->file_audio_channels){
      g_rec_mutex_unlock(recall_mutex);

      return;
    }

    if(fx_playback_audio->file_audio_channels != NULL){
      g_object_unref(G_OBJECT(fx_playback_audio->file_audio_channels));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_playback_audio->file_audio_channels = port;

    g_rec_mutex_unlock(recall_mutex);
  }
  break;
  case PROP_FILE_SAMPLERATE:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_playback_audio->file_samplerate){
      g_rec_mutex_unlock(recall_mutex);

      return;
    }

    if(fx_playback_audio->file_samplerate != NULL){
      g_object_unref(G_OBJECT(fx_playback_audio->file_samplerate));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_playback_audio->file_samplerate = port;

    g_rec_mutex_unlock(recall_mutex);
  }
  break;
  case PROP_FILE_BUFFER_SIZE:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_playback_audio->file_buffer_size){
      g_rec_mutex_unlock(recall_mutex);

      return;
    }

    if(fx_playback_audio->file_buffer_size != NULL){
      g_object_unref(G_OBJECT(fx_playback_audio->file_buffer_size));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_playback_audio->file_buffer_size = port;

    g_rec_mutex_unlock(recall_mutex);
  }
  break;
  case PROP_FILE_FORMAT:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_playback_audio->file_format){
      g_rec_mutex_unlock(recall_mutex);

      return;
    }

    if(fx_playback_audio->file_format != NULL){
      g_object_unref(G_OBJECT(fx_playback_audio->file_format));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_playback_audio->file_format = port;

    g_rec_mutex_unlock(recall_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_fx_playback_audio_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec)
{
  AgsFxPlaybackAudio *fx_playback_audio;
  
  GRecMutex *recall_mutex;

  fx_playback_audio = AGS_FX_PLAYBACK_AUDIO(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_playback_audio);

  switch(prop_id){
  case PROP_BPM:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_playback_audio->bpm);

    g_rec_mutex_unlock(recall_mutex);
  }
  break;
  case PROP_TACT:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_playback_audio->tact);

    g_rec_mutex_unlock(recall_mutex);
  }
  break;
  case PROP_DELAY:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_playback_audio->delay);

    g_rec_mutex_unlock(recall_mutex);
  }
  break;
  case PROP_DURATION:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_playback_audio->duration);

    g_rec_mutex_unlock(recall_mutex);
  }
  break;
  case PROP_LOOP:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_playback_audio->loop);

    g_rec_mutex_unlock(recall_mutex);
  }
  break;
  case PROP_LOOP_START:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_playback_audio->loop_start);

    g_rec_mutex_unlock(recall_mutex);
  }
  break;
  case PROP_LOOP_END:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_playback_audio->loop_end);

    g_rec_mutex_unlock(recall_mutex);
  }
  break;
  case PROP_EXPORT:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_playback_audio->export);

    g_rec_mutex_unlock(recall_mutex);
  }
  break;
  case PROP_FILENAME:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_playback_audio->filename);

    g_rec_mutex_unlock(recall_mutex);
  }
  break;
  case PROP_FILE_AUDIO_CHANNELS:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_playback_audio->file_audio_channels);

    g_rec_mutex_unlock(recall_mutex);
  }
  break;
  case PROP_FILE_SAMPLERATE:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_playback_audio->file_samplerate);

    g_rec_mutex_unlock(recall_mutex);
  }
  break;
  case PROP_FILE_BUFFER_SIZE:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_playback_audio->file_buffer_size);

    g_rec_mutex_unlock(recall_mutex);
  }
  break;
  case PROP_FILE_FORMAT:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_playback_audio->file_format);

    g_rec_mutex_unlock(recall_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_fx_playback_audio_dispose(GObject *gobject)
{
  AgsFxPlaybackAudio *fx_playback_audio;
  
  fx_playback_audio = AGS_FX_PLAYBACK_AUDIO(gobject);

  if(fx_playback_audio->feed_audio_signal != NULL){
    g_list_free_full(fx_playback_audio->feed_audio_signal,
		     (GDestroyNotify) g_object_unref);

    fx_playback_audio->feed_audio_signal = NULL;
  }

  if(fx_playback_audio->master_audio_signal != NULL){
    g_list_free_full(fx_playback_audio->master_audio_signal,
		     (GDestroyNotify) g_object_unref);

    fx_playback_audio->master_audio_signal = NULL;
  }

  if(fx_playback_audio->bpm != NULL){
    g_object_unref(fx_playback_audio->bpm);

    fx_playback_audio->bpm = NULL;
  }

  if(fx_playback_audio->tact != NULL){
    g_object_unref(fx_playback_audio->tact);

    fx_playback_audio->tact = NULL;
  }

  if(fx_playback_audio->delay != NULL){
    g_object_unref(fx_playback_audio->delay);

    fx_playback_audio->delay = NULL;
  }

  if(fx_playback_audio->duration != NULL){
    g_object_unref(fx_playback_audio->duration);

    fx_playback_audio->duration = NULL;
  }

  if(fx_playback_audio->loop != NULL){
    g_object_unref(fx_playback_audio->loop);

    fx_playback_audio->loop = NULL;
  }

  if(fx_playback_audio->loop_start != NULL){
    g_object_unref(fx_playback_audio->loop_start);

    fx_playback_audio->loop_start = NULL;
  }

  if(fx_playback_audio->loop_end != NULL){
    g_object_unref(fx_playback_audio->loop_end);

    fx_playback_audio->loop_end = NULL;
  }

  if(fx_playback_audio->export != NULL){
    g_object_unref(fx_playback_audio->export);

    fx_playback_audio->export = NULL;
  }

  if(fx_playback_audio->filename != NULL){
    g_object_unref(fx_playback_audio->filename);

    fx_playback_audio->filename = NULL;
  }

  if(fx_playback_audio->file_audio_channels != NULL){
    g_object_unref(fx_playback_audio->file_audio_channels);

    fx_playback_audio->file_audio_channels = NULL;
  }

  if(fx_playback_audio->file_samplerate != NULL){
    g_object_unref(fx_playback_audio->file_samplerate);

    fx_playback_audio->file_samplerate = NULL;
  }

  if(fx_playback_audio->file_buffer_size != NULL){
    g_object_unref(fx_playback_audio->file_buffer_size);

    fx_playback_audio->file_buffer_size = NULL;
  }

  if(fx_playback_audio->file_format != NULL){
    g_object_unref(fx_playback_audio->file_format);

    fx_playback_audio->file_format = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_playback_audio_parent_class)->dispose(gobject);
}

void
ags_fx_playback_audio_finalize(GObject *gobject)
{
  AgsFxPlaybackAudio *fx_playback_audio;
  
  fx_playback_audio = AGS_FX_PLAYBACK_AUDIO(gobject);

  if(fx_playback_audio->feed_audio_signal != NULL){
    g_list_free_full(fx_playback_audio->feed_audio_signal,
		     (GDestroyNotify) g_object_unref);
  }

  if(fx_playback_audio->master_audio_signal != NULL){
    g_list_free_full(fx_playback_audio->master_audio_signal,
		     (GDestroyNotify) g_object_unref);
  }

  if(fx_playback_audio->audio_file != NULL){
    g_object_unref(fx_playback_audio->audio_file);
  }
  
  if(fx_playback_audio->bpm != NULL){
    g_object_unref(fx_playback_audio->bpm);
  }

  if(fx_playback_audio->tact != NULL){
    g_object_unref(fx_playback_audio->tact);
  }

  if(fx_playback_audio->delay != NULL){
    g_object_unref(fx_playback_audio->delay);
  }

  if(fx_playback_audio->duration != NULL){
    g_object_unref(fx_playback_audio->duration);
  }

  if(fx_playback_audio->loop != NULL){
    g_object_unref(fx_playback_audio->loop);
  }

  if(fx_playback_audio->loop_start != NULL){
    g_object_unref(fx_playback_audio->loop_start);
  }

  if(fx_playback_audio->loop_end != NULL){
    g_object_unref(fx_playback_audio->loop_end);
  }

  if(fx_playback_audio->export != NULL){
    g_object_unref(fx_playback_audio->export);
  }

  if(fx_playback_audio->filename != NULL){
    g_object_unref(fx_playback_audio->filename);
  }
  
  if(fx_playback_audio->file_audio_channels != NULL){
    g_object_unref(fx_playback_audio->file_audio_channels);
  }

  if(fx_playback_audio->file_samplerate != NULL){
    g_object_unref(fx_playback_audio->file_samplerate);
  }

  if(fx_playback_audio->file_buffer_size != NULL){
    g_object_unref(fx_playback_audio->file_buffer_size);
  }

  if(fx_playback_audio->file_format != NULL){
    g_object_unref(fx_playback_audio->file_format);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_playback_audio_parent_class)->finalize(gobject);
}

/**
 * ags_fx_playback_audio_test_flags:
 * @fx_playback_audio: the #AgsFxPlaybackAudio
 * @flags: the flags
 * 
 * Test @flags of @fx_playback_audio.
 * 
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 3.3.0
 */
gboolean
ags_fx_playback_audio_test_flags(AgsFxPlaybackAudio *fx_playback_audio, guint flags)
{
  gboolean success;
  
  GRecMutex *recall_mutex;
  
  if(!AGS_IS_FX_PLAYBACK_AUDIO(fx_playback_audio)){
    return(FALSE);
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_playback_audio);

  /* test flags */
  g_rec_mutex_lock(recall_mutex);

  success = ((flags & (fx_playback_audio->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(recall_mutex);

  return(success);
}

/**
 * ags_fx_playback_audio_set_flags:
 * @fx_playback_audio: the #AgsFxPlaybackAudio
 * @flags: the flags
 * 
 * Set @flags of @fx_playback_audio.
 * 
 * Since: 3.3.0
 */
void
ags_fx_playback_audio_set_flags(AgsFxPlaybackAudio *fx_playback_audio, guint flags)
{  
  GRecMutex *recall_mutex;

  if(!AGS_IS_FX_PLAYBACK_AUDIO(fx_playback_audio)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_playback_audio);

  /* set flags */
  g_rec_mutex_lock(recall_mutex);

  fx_playback_audio->flags |= flags;

  g_rec_mutex_unlock(recall_mutex);
}

/**
 * ags_fx_playback_audio_unset_flags:
 * @fx_playback_audio: the #AgsFxPlaybackAudio
 * @flags: the flags
 * 
 * Unset @flags of @fx_playback_audio.
 * 
 * Since: 3.3.0
 */
void
ags_fx_playback_audio_unset_flags(AgsFxPlaybackAudio *fx_playback_audio, guint flags)
{  
  GRecMutex *recall_mutex;

  if(!AGS_IS_FX_PLAYBACK_AUDIO(fx_playback_audio)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_playback_audio);

  /* unset flags */
  g_rec_mutex_lock(recall_mutex);

  fx_playback_audio->flags &= (~flags);

  g_rec_mutex_unlock(recall_mutex);
}

/**
 * ags_fx_playback_audio_get_feed_audio_signal:
 * @fx_playback_audio: the #AgsFxPlaybackAudio
 * 
 * Get feed audio_signal of @fx_playback_audio.
 * 
 * Returns: (element-type AgsAudio.AudioSignal) (transfer full): the #GList-struct containing feed audio_signal
 * 
 * Since: 3.3.0
 */
GList*
ags_fx_playback_audio_get_feed_audio_signal(AgsFxPlaybackAudio *fx_playback_audio)
{
  GList *feed_audio_signal;
  
  GRecMutex *recall_mutex;

  if(!AGS_IS_FX_PLAYBACK_AUDIO(fx_playback_audio)){
    return(NULL);
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_playback_audio);

  /* unset flags */
  g_rec_mutex_lock(recall_mutex);

  feed_audio_signal = g_list_copy_deep(fx_playback_audio->feed_audio_signal,
				       (GCopyFunc) g_object_ref,
				       NULL);

  g_rec_mutex_unlock(recall_mutex);

  return(feed_audio_signal);
}

/**
 * ags_fx_playback_audio_add_feed_audio_signal:
 * @fx_playback_audio: the #AgsFxPlaybackAudio
 * @audio_signal: the #AgsAudioSignal
 * 
 * Add @audio_signal to @fx_playback_audio.
 * 
 * Since: 3.3.0
 */
void
ags_fx_playback_audio_add_feed_audio_signal(AgsFxPlaybackAudio *fx_playback_audio,
					    AgsAudioSignal *audio_signal)
{
  GRecMutex *recall_mutex;

  if(!AGS_IS_FX_PLAYBACK_AUDIO(fx_playback_audio)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_playback_audio);

  /* add */
  g_rec_mutex_lock(recall_mutex);

  if(g_list_find(fx_playback_audio->feed_audio_signal, audio_signal) == NULL){
    g_object_ref(audio_signal);

    fx_playback_audio->feed_audio_signal = g_list_prepend(fx_playback_audio->feed_audio_signal,
							  audio_signal);
  }
  
  g_rec_mutex_unlock(recall_mutex);
}

/**
 * ags_fx_playback_audio_remove_feed_audio_signal:
 * @fx_playback_audio: the #AgsFxPlaybackAudio
 * @audio_signal: the #AgsAudioSignal
 * 
 * Remove @audio_signal from @fx_playback_audio.
 * 
 * Since: 3.3.0
 */
void
ags_fx_playback_audio_remove_feed_audio_signal(AgsFxPlaybackAudio *fx_playback_audio,
					       AgsAudioSignal *audio_signal)
{
  GRecMutex *recall_mutex;

  if(!AGS_IS_FX_PLAYBACK_AUDIO(fx_playback_audio)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_playback_audio);

  /* remove */
  g_rec_mutex_lock(recall_mutex);

  if(g_list_find(fx_playback_audio->feed_audio_signal, audio_signal) != NULL){
    fx_playback_audio->feed_audio_signal = g_list_remove(fx_playback_audio->feed_audio_signal,
							 audio_signal);
    g_object_unref(audio_signal);
  }
  
  g_rec_mutex_unlock(recall_mutex);
}

/**
 * ags_fx_playback_audio_get_master_audio_signal:
 * @fx_playback_audio: the #AgsFxPlaybackAudio
 * 
 * Get master audio_signal of @fx_playback_audio.
 * 
 * Returns: (element-type AgsAudio.AudioSignal) (transfer full): the #GList-struct containing master audio_signal
 * 
 * Since: 3.3.0
 */
GList*
ags_fx_playback_audio_get_master_audio_signal(AgsFxPlaybackAudio *fx_playback_audio)
{
  GList *master_audio_signal;
  
  GRecMutex *recall_mutex;

  if(!AGS_IS_FX_PLAYBACK_AUDIO(fx_playback_audio)){
    return(NULL);
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_playback_audio);

  /* unset flags */
  g_rec_mutex_lock(recall_mutex);

  master_audio_signal = g_list_copy_deep(fx_playback_audio->master_audio_signal,
					 (GCopyFunc) g_object_ref,
					 NULL);

  g_rec_mutex_unlock(recall_mutex);

  return(master_audio_signal);
}

/**
 * ags_fx_playback_audio_add_master_audio_signal:
 * @fx_playback_audio: the #AgsFxPlaybackAudio
 * @audio_signal: the #AgsAudioSignal
 * 
 * Add @audio_signal to @fx_playback_audio.
 * 
 * Since: 3.3.0
 */
void
ags_fx_playback_audio_add_master_audio_signal(AgsFxPlaybackAudio *fx_playback_audio,
					      AgsAudioSignal *audio_signal)
{
  GRecMutex *recall_mutex;

  if(!AGS_IS_FX_PLAYBACK_AUDIO(fx_playback_audio)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_playback_audio);

  /* add */
  g_rec_mutex_lock(recall_mutex);

  if(g_list_find(fx_playback_audio->master_audio_signal, audio_signal) == NULL){
    g_object_ref(audio_signal);

    fx_playback_audio->master_audio_signal = g_list_prepend(fx_playback_audio->master_audio_signal,
							    audio_signal);
  }
  
  g_rec_mutex_unlock(recall_mutex);
}

/**
 * ags_fx_playback_audio_remove_master_audio_signal:
 * @fx_playback_audio: the #AgsFxPlaybackAudio
 * @audio_signal: the #AgsAudioSignal
 * 
 * Remove @audio_signal from @fx_playback_audio.
 * 
 * Since: 3.3.0
 */
void
ags_fx_playback_audio_remove_master_audio_signal(AgsFxPlaybackAudio *fx_playback_audio,
						 AgsAudioSignal *audio_signal)
{
  GRecMutex *recall_mutex;

  if(!AGS_IS_FX_PLAYBACK_AUDIO(fx_playback_audio)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_playback_audio);

  /* remove */
  g_rec_mutex_lock(recall_mutex);

  if(g_list_find(fx_playback_audio->master_audio_signal, audio_signal) != NULL){
    fx_playback_audio->master_audio_signal = g_list_remove(fx_playback_audio->master_audio_signal,
							   audio_signal);
    g_object_unref(audio_signal);
  }
  
  g_rec_mutex_unlock(recall_mutex);
}

/**
 * ags_fx_playback_audio_open_audio_file:
 * @fx_playback_audio: the #AgsFxPlaybackAudio
 * 
 * Open audio file of @fx_playback_audio.
 * 
 * Since: 3.3.0
 */
void
ags_fx_playback_audio_open_audio_file(AgsFxPlaybackAudio *fx_playback_audio)
{
  //TODO:JK: implement me
}

/**
 * ags_fx_playback_audio_new:
 * @audio: the #AgsAudio
 *
 * Create a new instance of #AgsFxPlaybackAudio
 *
 * Returns: the new #AgsFxPlaybackAudio
 *
 * Since: 3.3.0
 */
AgsFxPlaybackAudio*
ags_fx_playback_audio_new(AgsAudio *audio)
{
  AgsFxPlaybackAudio *fx_playback_audio;

  fx_playback_audio = (AgsFxPlaybackAudio *) g_object_new(AGS_TYPE_FX_PLAYBACK_AUDIO,
							  "audio", audio,
							  NULL);

  return(fx_playback_audio);
}
