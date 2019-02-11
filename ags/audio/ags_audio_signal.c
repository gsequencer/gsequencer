/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#include <ags/audio/ags_audio_signal.h>

#include <ags/libags.h>

#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_recall_id.h>
#include <ags/audio/ags_audio_buffer_util.h>
#include <ags/audio/ags_note.h>

#include <libxml/tree.h>

#include <stdint.h>

#include <stdlib.h>
#include <string.h>

#include <math.h>
#include <complex.h>

#include <ags/i18n.h>

void ags_audio_signal_class_init(AgsAudioSignalClass *audio_signal_class);
void ags_audio_signal_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_audio_signal_init(AgsAudioSignal *audio_signal);
void ags_audio_signal_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec);
void ags_audio_signal_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec);
void ags_audio_signal_dispose(GObject *gobject);
void ags_audio_signal_finalize(GObject *gobject);

AgsUUID* ags_audio_signal_get_uuid(AgsConnectable *connectable);
gboolean ags_audio_signal_has_resource(AgsConnectable *connectable);
gboolean ags_audio_signal_is_ready(AgsConnectable *connectable);
void ags_audio_signal_add_to_registry(AgsConnectable *connectable);
void ags_audio_signal_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_audio_signal_list_resource(AgsConnectable *connectable);
xmlNode* ags_audio_signal_xml_compose(AgsConnectable *connectable);
void ags_audio_signal_xml_parse(AgsConnectable *connectable,
				xmlNode *node);
gboolean ags_audio_signal_is_connected(AgsConnectable *connectable);
void ags_audio_signal_connect(AgsConnectable *connectable);
void ags_audio_signal_disconnect(AgsConnectable *connectable);

void ags_audio_signal_real_add_note(AgsAudioSignal *audio_signal,
				    GObject *note);
void ags_audio_signal_real_remove_note(AgsAudioSignal *audio_signal,
				       GObject *note);

/**
 * SECTION:ags_audio_signal
 * @short_description: Contains the audio data and its alignment
 * @title: AgsAudioSignal
 * @section_id:
 * @include: ags/audio/ags_audio_signal.h
 *
 * #AgsAudioSignal organizes audio data within a #GList-struct whereby data
 * pointing to the buffer.
 */

enum{
  PROP_0,
  PROP_RECYCLING,
  PROP_OUTPUT_SOUNDCARD,
  PROP_OUTPUT_SOUNDCARD_CHANNEL,
  PROP_INPUT_SOUNDCARD,
  PROP_INPUT_SOUNDCARD_CHANNEL,
  PROP_SAMPLERATE,
  PROP_BUFFER_SIZE,
  PROP_FORMAT,
  PROP_WORD_SIZE,
  PROP_LENGTH,
  PROP_FIRST_FRAME,
  PROP_LAST_FRAME,
  PROP_FRAME_COUNT,
  PROP_LOOP_START,
  PROP_LOOP_END,
  PROP_DELAY,
  PROP_ATTACK,
  PROP_DAMPING,
  PROP_VIBRATION,
  PROP_TIMBRE_START,
  PROP_TIMBRE_END,
  PROP_TEMPLATE,
  PROP_RT_TEMPLATE,
  PROP_NOTE,
  PROP_RECALL_ID,
  PROP_STREAM,
  PROP_STREAM_END,
  PROP_STREAM_CURRENT,
};

enum{
  ADD_NOTE,
  REMOVE_NOTE,
  REFRESH_DATA,
  LAST_SIGNAL,
};

static gpointer ags_audio_signal_parent_class = NULL;
static guint audio_signal_signals[LAST_SIGNAL];

static pthread_mutex_t ags_audio_signal_class_mutex = PTHREAD_MUTEX_INITIALIZER;

GType
ags_audio_signal_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_audio_signal = 0;

    static const GTypeInfo ags_audio_signal_info = {
      sizeof (AgsAudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_audio_signal_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_audio_signal_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_audio_signal = g_type_register_static(G_TYPE_OBJECT,
						   "AgsAudioSignal",
						   &ags_audio_signal_info,
						   0);

    g_type_add_interface_static(ags_type_audio_signal,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_audio_signal);
  }

  return g_define_type_id__volatile;
}

void
ags_audio_signal_class_init(AgsAudioSignalClass *audio_signal)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_audio_signal_parent_class = g_type_class_peek_parent(audio_signal);

  /* GObjectClass */
  gobject = (GObjectClass *) audio_signal;

  gobject->set_property = ags_audio_signal_set_property;
  gobject->get_property = ags_audio_signal_get_property;

  gobject->dispose = ags_audio_signal_dispose;
  gobject->finalize = ags_audio_signal_finalize;

  /* properties */
  /**
   * AgsAudioSignal:recycling:
   *
   * The assigned #AgsRecycling linking tree.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("recycling",
				   i18n_pspec("assigned recycling"),
				   i18n_pspec("The recycling it is assigned with"),
				   AGS_TYPE_RECYCLING,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECYCLING,
				  param_spec);

  /**
   * AgsAudioSignal:output-soundcard:
   *
   * The assigned output #AgsSoundcard providing default settings.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("output-soundcard",
				   i18n_pspec("assigned output soundcard"),
				   i18n_pspec("The output soundcard it is assigned with"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_OUTPUT_SOUNDCARD,
				  param_spec);

  /**
   * AgsAudioSignal:output-soundcard-channel:
   *
   * The output soundcard channel.
   * 
   * Since: 2.0.0
   */
  param_spec =  g_param_spec_int("output-soundcard-channel",
				 i18n_pspec("output soundcard channel"),
				 i18n_pspec("The output soundcard channel"),
				 -1,
				 G_MAXINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_OUTPUT_SOUNDCARD_CHANNEL,
				  param_spec);

  /**
   * AgsAudioSignal:input-soundcard:
   *
   * The assigned input #AgsSoundcard.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("input-soundcard",
				   i18n_pspec("assigned input soundcard"),
				   i18n_pspec("The input soundcard it is assigned with"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_INPUT_SOUNDCARD,
				  param_spec);
  
  /**
   * AgsAudioSignal:input-soundcard-channel:
   *
   * The input soundcard channel.
   * 
   * Since: 2.0.0
   */
  param_spec =  g_param_spec_int("input-soundcard-channel",
				 i18n_pspec("input soundcard channel"),
				 i18n_pspec("The input soundcard channel"),
				 -1,
				 G_MAXINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_INPUT_SOUNDCARD_CHANNEL,
				  param_spec);  

  /**
   * AgsAudioSignal:samplerate:
   *
   * The samplerate to be used.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("samplerate",
				 i18n_pspec("using samplerate"),
				 i18n_pspec("The samplerate to be used"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SAMPLERATE,
				  param_spec);

  /**
   * AgsAudioSignal:buffer-size:
   *
   * The buffer size to be used.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("buffer-size",
				 i18n_pspec("using buffer size"),
				 i18n_pspec("The buffer size to be used"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER_SIZE,
				  param_spec);

  /**
   * AgsAudioSignal:format:
   *
   * The format to be used.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("format",
				 i18n_pspec("using format"),
				 i18n_pspec("The format to be used"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FORMAT,
				  param_spec);

  /**
   * AgsAudioSignal:word-size:
   *
   * The word size of frame.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("word-size",
				 i18n_pspec("frame word size"),
				 i18n_pspec("The word size of a frame"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_WORD_SIZE,
				  param_spec);

  /**
   * AgsAudioSignal:length:
   *
   * The length of the stream.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("length",
				 i18n_pspec("stream length"),
				 i18n_pspec("The length of the stream"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LENGTH,
				  param_spec);

  /**
   * AgsAudioSignal:first-frame:
   *
   * The first frame of stream.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("first-frame",
				 i18n_pspec("stream's first frame"),
				 i18n_pspec("The first frame of the stream"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FIRST_FRAME,
				  param_spec);

  /**
   * AgsAudioSignal:last-frame:
   *
   * The last frame of stream.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("last-frame",
				 i18n_pspec("stream's last frame"),
				 i18n_pspec("The last frame of the stream"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LAST_FRAME,
				  param_spec);

  /**
   * AgsAudioSignal:frame-count:
   *
   * The initial size of audio data.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("frame-count",
				 i18n_pspec("frame count of audio data"),
				 i18n_pspec("The initial frame count of audio data"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FRAME_COUNT,
				  param_spec);

  /**
   * AgsAudioSignal:loop-start:
   *
   * The loop start of stream.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("loop-start",
				 i18n_pspec("stream's loop start"),
				 i18n_pspec("The loop start of the stream"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LOOP_START,
				  param_spec);

  /**
   * AgsAudioSignal:loop-end:
   *
   * The loop end of stream.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("loop-end",
				 i18n_pspec("stream's loop end"),
				 i18n_pspec("The loop end of the stream"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LOOP_END,
				  param_spec);

  /**
   * AgsAudioSignal:delay:
   *
   * The delay to be used.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_double("delay",
				   i18n_pspec("using delay"),
				   i18n_pspec("The delay to be used"),
				   0.0,
				   G_MAXDOUBLE,
				   0.0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DELAY,
				  param_spec);

  /**
   * AgsAudioSignal:attack:
   *
   * The attack to be used.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("attack",
				 i18n_pspec("using attack"),
				 i18n_pspec("The attack to be used"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_ATTACK,
				  param_spec);

  /**
   * AgsAudioSignal:damping:
   *
   * Damping of timbre.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_boxed("damping",
				  i18n_pspec("damping"),
				  i18n_pspec("The timbre's damping"),
				  AGS_TYPE_COMPLEX,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DAMPING,
				  param_spec);

  /**
   * AgsAudioSignal:vibration:
   *
   * Vibration of timbre.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_boxed("vibration",
				  i18n_pspec("vibration"),
				  i18n_pspec("The timbre's vibration"),
				  AGS_TYPE_COMPLEX,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_VIBRATION,
				  param_spec);

  /**
   * AgsAudioSignal:timbre-start:
   *
   * The timbre's start frame.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("timbre-start",
				 i18n_pspec("timbre's start"),
				 i18n_pspec("The timbre's start frame"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TIMBRE_START,
				  param_spec);

  /**
   * AgsAudioSignal:timbre-end:
   *
   * The timbre's end frame.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("timbre-end",
				 i18n_pspec("timbre's end"),
				 i18n_pspec("The timbre's end frame"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TIMBRE_END,
				  param_spec);

  /**
   * AgsAudioSignal:template:
   *
   * The assigned #AgsAudioSignal template.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("template",
				   i18n_pspec("assigned template"),
				   i18n_pspec("The assigend template"),
				   AGS_TYPE_AUDIO_SIGNAL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TEMPLATE,
				  param_spec);

  /**
   * AgsAudioSignal:rt-template:
   *
   * The assigned #AgsAudioSignal realtime template.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("rt-template",
				   i18n_pspec("assigned realtime template"),
				   i18n_pspec("The assigend realtime template"),
				   AGS_TYPE_AUDIO_SIGNAL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RT_TEMPLATE,
				  param_spec);

  /**
   * AgsAudioSignal:note:
   *
   * The assigned #AgsNote providing default settings.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_pointer("note",
				    i18n_pspec("assigned note"),
				    i18n_pspec("The note it is assigned with"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_NOTE,
				  param_spec);

  /**
   * AgsAudioSignal:recall-id:
   *
   * The assigned #AgsRecallID providing context.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("recall-id",
				   i18n_pspec("assigned recall id"),
				   i18n_pspec("The recall id it is assigned with"),
				   AGS_TYPE_RECALL_ID,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_ID,
				  param_spec);

  /**
   * AgsAudioSignal:stream:
   *
   * The stream it contains.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_pointer("stream",
				    i18n_pspec("containing stream"),
				    i18n_pspec("The stream it contains"),
				    G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_STREAM,
				  param_spec);

  /**
   * AgsAudioSignal:stream-end:
   *
   * The end of stream.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_pointer("stream-end",
				    i18n_pspec("end of stream"),
				    i18n_pspec("The stream's end"),
				    G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_STREAM_END,
				  param_spec);

  /**
   * AgsAudioSignal:stream-current:
   *
   * The current stream.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_pointer("stream-current",
				    i18n_pspec("current stream"),
				    i18n_pspec("The current stream"),
				    G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_STREAM_CURRENT,
				  param_spec);

  /* AgsAudioSignalClass */
  audio_signal->add_note = ags_audio_signal_real_add_note;
  audio_signal->remove_note = ags_audio_signal_real_remove_note;
  
  /* signals */
  /**
   * AgsAudioSignal::add-note:
   * @audio_signal: the #AgsAudioSignal
   * @note: the #AgsNote
   *
   * The ::add-note signal notifies about adding @note.
   * 
   * Since: 2.0.0
   */
  audio_signal_signals[ADD_NOTE] =
    g_signal_new("add-note",
		 G_TYPE_FROM_CLASS(audio_signal),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsAudioSignalClass, add_note),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__OBJECT,
		 G_TYPE_NONE, 1,
		 G_TYPE_OBJECT);

  /**
   * AgsAudioSignal::remove-note:
   * @audio_signal: the #AgsAudioSignal
   * @note: the #AgsNote
   *
   * The ::remove-note signal notifies about removing @note.
   * 
   * Since: 2.0.0
   */
  audio_signal_signals[REMOVE_NOTE] =
    g_signal_new("remove-note",
		 G_TYPE_FROM_CLASS(audio_signal),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsAudioSignalClass, remove_note),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__OBJECT,
		 G_TYPE_NONE, 1,
		 G_TYPE_OBJECT);


  /**
   * AgsAudioSignal::refresh-data:
   * @audio_signal: the #AgsAudioSignal
   *
   * The ::refresh-data signal notifies about requesting to refresh data.
   * 
   * Since: 2.0.0
   */
  audio_signal_signals[REFRESH_DATA] =
    g_signal_new("refresh-data",
		 G_TYPE_FROM_CLASS(audio_signal),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsAudioSignalClass, refresh_data),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);
}

void
ags_audio_signal_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_audio_signal_get_uuid;
  connectable->has_resource = ags_audio_signal_has_resource;
  connectable->is_ready = ags_audio_signal_is_ready;

  connectable->add_to_registry = ags_audio_signal_add_to_registry;
  connectable->remove_from_registry = ags_audio_signal_remove_from_registry;

  connectable->list_resource = ags_audio_signal_list_resource;
  connectable->xml_compose = ags_audio_signal_xml_compose;
  connectable->xml_parse = ags_audio_signal_xml_parse;

  connectable->is_connected = ags_audio_signal_is_connected;
  
  connectable->connect = ags_audio_signal_connect;
  connectable->disconnect = ags_audio_signal_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_audio_signal_init(AgsAudioSignal *audio_signal)
{
  AgsConfig *config;
  
  gchar *str;

  complex z;

  pthread_mutex_t *mutex;
  pthread_mutexattr_t *attr;

  audio_signal->flags = 0;

  /* add audio signal mutex */
  audio_signal->obj_mutexattr = 
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(attr,
				PTHREAD_PRIO_INHERIT);
#endif

  audio_signal->obj_mutex = 
    mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     attr);  

  /* uuid */
  audio_signal->uuid = ags_uuid_alloc();
  ags_uuid_generate(audio_signal->uuid);

  /* recycling */
  audio_signal->recycling = NULL;

  /* config */
  config = ags_config_get_instance();

  /* base init */
  audio_signal->output_soundcard = NULL;
  audio_signal->output_soundcard_channel = 0;

  audio_signal->input_soundcard = NULL;
  audio_signal->input_soundcard_channel = 0;

  /* presets */
  audio_signal->samplerate = (guint) ags_soundcard_helper_config_get_samplerate(config);
  audio_signal->buffer_size = (guint) ags_soundcard_helper_config_get_buffer_size(config);
  audio_signal->format = (guint) ags_soundcard_helper_config_get_format(config);

  audio_signal->word_size = sizeof(gint16);

  /* format */
  switch(audio_signal->format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
  {
    audio_signal->word_size = sizeof(gint8);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
  {
    audio_signal->word_size = sizeof(gint16);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
  {
    audio_signal->word_size = sizeof(gint32);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
  {
    audio_signal->word_size = sizeof(gint32);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
  {
    audio_signal->word_size = sizeof(gint64);
  }
  break;
  case AGS_SOUNDCARD_FLOAT:
  {
    audio_signal->word_size = sizeof(gfloat);
  }
  break;
  case AGS_SOUNDCARD_DOUBLE:
  {
    audio_signal->word_size = sizeof(gdouble);
  }
  break;
  }    

  /* duration */
  audio_signal->length = 0;
  audio_signal->first_frame = 0;
  audio_signal->last_frame = 0;

  audio_signal->frame_count = 0;
  audio_signal->loop_start = 0;
  audio_signal->loop_end = 0;

  /* offset */
  audio_signal->delay = 0.0;
  audio_signal->attack = 0;

  /* timbre */
  z = 0.0 + I * 1.0;  
  ags_complex_set(&(audio_signal->damping),
		  z);
  
  z = 0.0 + I * 1.0;
  ags_complex_set(&(audio_signal->vibration),
		  z);

  audio_signal->timbre_start = 0;
  audio_signal->timbre_end = 0;

  /* template */
  audio_signal->template = NULL;
  
  /* realtime fields */
  audio_signal->rt_template = NULL;
  audio_signal->note = NULL;

  /* recall id */
  audio_signal->recall_id = NULL;
  
  /* stream */
  audio_signal->stream_mutexattr = 
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

  audio_signal->stream_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(audio_signal->stream_mutex,
		     attr);

  audio_signal->stream = NULL;
  audio_signal->stream_current = NULL;
  audio_signal->stream_end = NULL;
}

void
ags_audio_signal_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec)
{
  AgsAudioSignal *audio_signal;

  pthread_mutex_t *audio_signal_mutex;

  audio_signal = AGS_AUDIO_SIGNAL(gobject);

  /* get audio signal mutex */
  pthread_mutex_lock(ags_audio_signal_get_class_mutex());
  
  audio_signal_mutex = audio_signal->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_signal_get_class_mutex());
  
  switch(prop_id){
  case PROP_RECYCLING:
  {
    GObject *recycling;

    recycling = g_value_get_object(value);

    pthread_mutex_lock(audio_signal_mutex);

    if(audio_signal->recycling == recycling){
      pthread_mutex_unlock(audio_signal_mutex);

      return;
    }

    if(audio_signal->recycling != NULL){
      g_object_unref(audio_signal->recycling);
    }
      
    if(recycling != NULL){
      g_object_ref(recycling);
    }
      
    audio_signal->recycling = recycling;

    pthread_mutex_unlock(audio_signal_mutex);
  }
  break;
  case PROP_OUTPUT_SOUNDCARD:
  {
    GObject *output_soundcard;

    output_soundcard = g_value_get_object(value);

    ags_audio_signal_set_output_soundcard(audio_signal,
					  output_soundcard);
  }
  break;
  case PROP_OUTPUT_SOUNDCARD_CHANNEL:
  {
    pthread_mutex_lock(audio_signal_mutex);

    audio_signal->output_soundcard_channel = g_value_get_int(value);

    pthread_mutex_unlock(audio_signal_mutex);
  }
  break;
  case PROP_INPUT_SOUNDCARD:
  {
    GObject *input_soundcard;
      
    input_soundcard = g_value_get_object(value);

    ags_audio_signal_set_input_soundcard(audio_signal,
					 input_soundcard);
  }
  break;
  case PROP_INPUT_SOUNDCARD_CHANNEL:
  {
    pthread_mutex_lock(audio_signal_mutex);

    audio_signal->input_soundcard_channel = g_value_get_int(value);

    pthread_mutex_unlock(audio_signal_mutex);
  }
  break;
  case PROP_SAMPLERATE:
  {
    guint samplerate;

    samplerate = g_value_get_uint(value);

    ags_audio_signal_set_samplerate(audio_signal,
				    samplerate);
  }
  break;
  case PROP_BUFFER_SIZE:
  {
    guint buffer_size;

    buffer_size = g_value_get_uint(value);

    ags_audio_signal_set_buffer_size(audio_signal,
				     buffer_size);
  }
  break;
  case PROP_FORMAT:
  {
    guint format;

    format = g_value_get_uint(value);

    ags_audio_signal_set_format(audio_signal,
				format);
  }
  break;
  case PROP_LENGTH:
  {
    guint length;

    length = g_value_get_uint(value);

    pthread_mutex_lock(audio_signal_mutex);

    audio_signal->length = length;

    pthread_mutex_unlock(audio_signal_mutex);
  }
  break;
  case PROP_FIRST_FRAME:
  {
    guint first_frame;

    first_frame = g_value_get_uint(value);

    pthread_mutex_lock(audio_signal_mutex);

    audio_signal->first_frame = first_frame;

    pthread_mutex_unlock(audio_signal_mutex);
  }
  break;
  case PROP_LAST_FRAME:
  {
    guint last_frame;

    last_frame = g_value_get_uint(value);

    pthread_mutex_lock(audio_signal_mutex);

    audio_signal->last_frame = last_frame;

    pthread_mutex_unlock(audio_signal_mutex);
  }
  break;
  case PROP_FRAME_COUNT:
  {
    guint frame_count;

    frame_count = g_value_get_uint(value);

    pthread_mutex_lock(audio_signal_mutex);

    audio_signal->frame_count = frame_count;

    pthread_mutex_unlock(audio_signal_mutex);
  }
  break;
  case PROP_LOOP_START:
  {
    guint loop_start;

    loop_start = g_value_get_uint(value);

    pthread_mutex_lock(audio_signal_mutex);

    audio_signal->loop_start = loop_start;

    pthread_mutex_unlock(audio_signal_mutex);
  }
  break;
  case PROP_LOOP_END:
  {
    guint loop_end;

    loop_end = g_value_get_uint(value);

    pthread_mutex_lock(audio_signal_mutex);

    audio_signal->loop_end = loop_end;

    pthread_mutex_unlock(audio_signal_mutex);
  }
  break;
  case PROP_DELAY:
  {
    gdouble delay;

    delay = g_value_get_double(value);

    pthread_mutex_lock(audio_signal_mutex);

    audio_signal->delay = delay;

    pthread_mutex_unlock(audio_signal_mutex);
  }
  break;
  case PROP_ATTACK:
  {
    guint attack;

    attack = g_value_get_uint(value);

    pthread_mutex_lock(audio_signal_mutex);

    audio_signal->attack = attack;

    pthread_mutex_unlock(audio_signal_mutex);
  }
  break;
  case PROP_DAMPING:
  {
    AgsComplex *damping;

    damping = (AgsComplex *) g_value_get_boxed(value);

    pthread_mutex_lock(audio_signal_mutex);

    ags_complex_set(&(audio_signal->damping),
		    ags_complex_get(damping));      

    pthread_mutex_unlock(audio_signal_mutex);
  }
  break;
  case PROP_VIBRATION:
  {
    AgsComplex *vibration;

    vibration = (AgsComplex *) g_value_get_boxed(value);

    pthread_mutex_lock(audio_signal_mutex);

    ags_complex_set(&(audio_signal->vibration),
		    ags_complex_get(vibration));      

    pthread_mutex_unlock(audio_signal_mutex);
  }
  break;
  case PROP_TIMBRE_START:
  {
    pthread_mutex_lock(audio_signal_mutex);

    audio_signal->timbre_start = g_value_get_uint(value);      

    pthread_mutex_unlock(audio_signal_mutex);
  }
  break;
  case PROP_TIMBRE_END:
  {
    pthread_mutex_lock(audio_signal_mutex);

    audio_signal->timbre_end = g_value_get_uint(value);      

    pthread_mutex_unlock(audio_signal_mutex);
  }
  break;
  case PROP_TEMPLATE:
  {
    GObject *template;

    template = g_value_get_object(value);

    pthread_mutex_lock(audio_signal_mutex);

    if(audio_signal->template == template){
      pthread_mutex_unlock(audio_signal_mutex);
	
      return;
    }

    if(audio_signal->template != NULL){
      g_object_unref(audio_signal->template);
    }
      
    if(template != NULL){
      g_object_ref(template);
    }
      
    audio_signal->template = template;

    pthread_mutex_unlock(audio_signal_mutex);
  }
  break;
  case PROP_RT_TEMPLATE:
  {
    GObject *rt_template;

    rt_template = g_value_get_object(value);

    pthread_mutex_lock(audio_signal_mutex);

    if(audio_signal->rt_template == rt_template){
      pthread_mutex_unlock(audio_signal_mutex);
	
      return;
    }

    if(audio_signal->rt_template != NULL){
      g_object_unref(audio_signal->rt_template);
    }
      
    if(rt_template != NULL){
      g_object_ref(rt_template);
    }
      
    audio_signal->rt_template = rt_template;

    pthread_mutex_unlock(audio_signal_mutex);
  }
  break;
  case PROP_NOTE:
  {
    GObject *note;

    note = g_value_get_pointer(value);

    pthread_mutex_lock(audio_signal_mutex);

    if(g_list_find(audio_signal->note, note) != NULL){
      pthread_mutex_unlock(audio_signal_mutex);
	
      return;
    }

    pthread_mutex_unlock(audio_signal_mutex);

    ags_audio_signal_add_note(audio_signal,
			      note);
  }
  break;
  case PROP_RECALL_ID:
  {
    GObject *recall_id;

    recall_id = g_value_get_object(value);
      
    pthread_mutex_lock(audio_signal_mutex);

    if(audio_signal->recall_id == recall_id){
      pthread_mutex_unlock(audio_signal_mutex);
	
      return;
    }
      
    if(audio_signal->recall_id != NULL){
      g_object_unref(audio_signal->recall_id);
    }
      
    if(recall_id != NULL){
      g_object_ref(recall_id);
    }
      
    audio_signal->recall_id = recall_id;

    pthread_mutex_unlock(audio_signal_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_audio_signal_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec)
{
  AgsAudioSignal *audio_signal;

  pthread_mutex_t *audio_signal_mutex;
  pthread_mutex_t *stream_mutex;

  audio_signal = AGS_AUDIO_SIGNAL(gobject);

  /* get audio signal mutex */
  pthread_mutex_lock(ags_audio_signal_get_class_mutex());
  
  audio_signal_mutex = audio_signal->obj_mutex;
  stream_mutex = audio_signal->stream_mutex;
  
  pthread_mutex_unlock(ags_audio_signal_get_class_mutex());

  switch(prop_id){
  case PROP_RECYCLING:
  {
    pthread_mutex_lock(audio_signal_mutex);

    g_value_set_object(value, audio_signal->recycling);

    pthread_mutex_unlock(audio_signal_mutex);
  }
  break;
  case PROP_OUTPUT_SOUNDCARD:
  {
    pthread_mutex_lock(audio_signal_mutex);

    g_value_set_object(value, audio_signal->output_soundcard);

    pthread_mutex_unlock(audio_signal_mutex);
  }
  break;
  case PROP_OUTPUT_SOUNDCARD_CHANNEL:
  {
    pthread_mutex_lock(audio_signal_mutex);

    g_value_set_int(value, audio_signal->output_soundcard_channel);

    pthread_mutex_unlock(audio_signal_mutex);
  }
  break;
  case PROP_INPUT_SOUNDCARD:
  {
    pthread_mutex_lock(audio_signal_mutex);

    g_value_set_object(value, audio_signal->input_soundcard);

    pthread_mutex_unlock(audio_signal_mutex);
  }
  break;
  case PROP_INPUT_SOUNDCARD_CHANNEL:
  {
    pthread_mutex_lock(audio_signal_mutex);

    g_value_set_int(value, audio_signal->input_soundcard_channel);

    pthread_mutex_unlock(audio_signal_mutex);
  }
  break;
  case PROP_SAMPLERATE:
  {
    pthread_mutex_lock(audio_signal_mutex);

    g_value_set_uint(value, audio_signal->samplerate);

    pthread_mutex_unlock(audio_signal_mutex);
  }
  break;
  case PROP_BUFFER_SIZE:
  {
    pthread_mutex_lock(audio_signal_mutex);

    g_value_set_uint(value, audio_signal->buffer_size);

    pthread_mutex_unlock(audio_signal_mutex);
  }
  break;
  case PROP_FORMAT:
  {
    pthread_mutex_lock(audio_signal_mutex);

    g_value_set_uint(value, audio_signal->format);

    pthread_mutex_unlock(audio_signal_mutex);
  }
  break;
  case PROP_WORD_SIZE:
  {
    pthread_mutex_lock(audio_signal_mutex);

    g_value_set_uint(value, audio_signal->word_size);

    pthread_mutex_unlock(audio_signal_mutex);
  }
  break;
  case PROP_LENGTH:
  {
    pthread_mutex_lock(audio_signal_mutex);

    g_value_set_uint(value, audio_signal->length);

    pthread_mutex_unlock(audio_signal_mutex);
  }
  break;
  case PROP_FIRST_FRAME:
  {
    pthread_mutex_lock(audio_signal_mutex);

    g_value_set_uint(value, audio_signal->first_frame);

    pthread_mutex_unlock(audio_signal_mutex);
  }
  break;
  case PROP_LAST_FRAME:
  {
    pthread_mutex_lock(audio_signal_mutex);

    g_value_set_uint(value, audio_signal->last_frame);

    pthread_mutex_unlock(audio_signal_mutex);
  }
  break;
  case PROP_FRAME_COUNT:
  {
    pthread_mutex_lock(audio_signal_mutex);

    g_value_set_uint(value, audio_signal->frame_count);

    pthread_mutex_unlock(audio_signal_mutex);
  }
  break;
  case PROP_LOOP_START:
  {
    pthread_mutex_lock(audio_signal_mutex);

    g_value_set_uint(value, audio_signal->loop_start);

    pthread_mutex_unlock(audio_signal_mutex);
  }
  break;
  case PROP_LOOP_END:
  {
    pthread_mutex_lock(audio_signal_mutex);

    g_value_set_uint(value, audio_signal->loop_end);

    pthread_mutex_unlock(audio_signal_mutex);
  }
  break;
  case PROP_DELAY:
  {
    pthread_mutex_lock(audio_signal_mutex);

    g_value_set_double(value, audio_signal->delay);

    pthread_mutex_unlock(audio_signal_mutex);
  }
  break;
  case PROP_ATTACK:
  {
    pthread_mutex_lock(audio_signal_mutex);

    g_value_set_uint(value, audio_signal->attack);

    pthread_mutex_unlock(audio_signal_mutex);
  }
  break;
  case PROP_DAMPING:
  {
    pthread_mutex_lock(audio_signal_mutex);

    g_value_set_boxed(value, audio_signal->damping);

    pthread_mutex_unlock(audio_signal_mutex);
  }
  break;
  case PROP_VIBRATION:
  {
    pthread_mutex_lock(audio_signal_mutex);

    g_value_set_boxed(value, audio_signal->vibration);

    pthread_mutex_unlock(audio_signal_mutex);
  }
  break;
  case PROP_TIMBRE_START:
  {
    pthread_mutex_lock(audio_signal_mutex);

    g_value_set_uint(value, audio_signal->timbre_start);

    pthread_mutex_unlock(audio_signal_mutex);
  }
  break;
  case PROP_TIMBRE_END:
  {
    pthread_mutex_lock(audio_signal_mutex);

    g_value_set_uint(value, audio_signal->timbre_end);

    pthread_mutex_unlock(audio_signal_mutex);
  }
  break;
  case PROP_TEMPLATE:
  {
    pthread_mutex_lock(audio_signal_mutex);

    g_value_set_object(value, audio_signal->template);

    pthread_mutex_unlock(audio_signal_mutex);
  }
  break;
  case PROP_RT_TEMPLATE:
  {
    pthread_mutex_lock(audio_signal_mutex);

    g_value_set_object(value, audio_signal->rt_template);

    pthread_mutex_unlock(audio_signal_mutex);
  }
  break;
  case PROP_NOTE:
  {
    pthread_mutex_lock(audio_signal_mutex);

    g_value_set_pointer(value,
			g_list_copy_deep(audio_signal->note,
					 (GCopyFunc) g_object_ref,
					 NULL));

    pthread_mutex_unlock(audio_signal_mutex);
  }
  break;
  case PROP_RECALL_ID:
  {
    pthread_mutex_lock(audio_signal_mutex);

    g_value_set_object(value, audio_signal->recall_id);

    pthread_mutex_unlock(audio_signal_mutex);
  }
  break;
  case PROP_STREAM:
  {
    pthread_mutex_lock(stream_mutex);

    g_value_set_pointer(value, audio_signal->stream);

    pthread_mutex_unlock(stream_mutex);
  }
  break;
  case PROP_STREAM_END:
  {
    pthread_mutex_lock(stream_mutex);

    g_value_set_pointer(value, audio_signal->stream_end);

    pthread_mutex_unlock(stream_mutex);
  }
  break;
  case PROP_STREAM_CURRENT:
  {
    pthread_mutex_lock(stream_mutex);

    g_value_set_pointer(value, audio_signal->stream_current);

    pthread_mutex_unlock(stream_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_audio_signal_dispose(GObject *gobject)
{
  AgsAudioSignal *audio_signal;

  audio_signal = AGS_AUDIO_SIGNAL(gobject);
  
  /* recycling */
  if(audio_signal->recycling != NULL){
    g_object_unref(audio_signal->recycling);
    
    audio_signal->recycling = NULL;  
  }

  /* output soundcard */
  if(audio_signal->output_soundcard != NULL){
    g_object_unref(audio_signal->output_soundcard);

    audio_signal->output_soundcard = NULL;
  }

  /* input soundcard */
  if(audio_signal->input_soundcard != NULL){
    g_object_unref(audio_signal->input_soundcard);

    audio_signal->input_soundcard = NULL;
  }

  /* template */
  if(audio_signal->template != NULL){
    g_object_unref(audio_signal->template);
    
    audio_signal->template = NULL;  
  }

  /* rt-template */
  if(audio_signal->rt_template != NULL){
    g_object_unref(audio_signal->rt_template);
    
    audio_signal->rt_template = NULL;  
  }

  /* note */
  if(audio_signal->note != NULL){
    g_list_free_full(audio_signal->note,
		     g_object_unref);

    audio_signal->note = NULL;
  }

  /* recall id */
  if(audio_signal->recall_id != NULL){
    g_object_unref(audio_signal->recall_id);

    audio_signal->recall_id = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_audio_signal_parent_class)->dispose(gobject);
}

void
ags_audio_signal_finalize(GObject *gobject)
{
  AgsAudioSignal *audio_signal;

  guint *ids;
  guint i, n_ids;

  audio_signal = AGS_AUDIO_SIGNAL(gobject);

#ifdef AGS_DEBUG
  g_message("fin %x", audio_signal);
  
  if((AGS_AUDIO_SIGNAL_TEMPLATE & (audio_signal->flags)) != 0){
    g_warning("AGS_AUDIO_SIGNAL_TEMPLATE: destroying\n");
  }
#endif

  pthread_mutex_destroy(audio_signal->obj_mutex);
  free(audio_signal->obj_mutex);

  pthread_mutexattr_destroy(audio_signal->obj_mutexattr);
  free(audio_signal->obj_mutexattr);

  ags_uuid_free(audio_signal->uuid);

  /* disconnect */
#if 0
  ids = g_signal_list_ids(AGS_TYPE_AUDIO_SIGNAL,
			  &n_ids);
  
  for(i = 0; i < n_ids; i++){
    g_signal_handlers_disconnect_matched(gobject,
					 G_SIGNAL_MATCH_ID,
					 ids[i],
					 0,
					 NULL,
					 NULL,
					 NULL);
  }

  g_free(ids);
#endif

  /* recycling */
  if(audio_signal->recycling != NULL){
    g_object_unref(audio_signal->recycling);
  }
  
  /* output soundcard */
  if(audio_signal->output_soundcard != NULL){
    g_object_unref(audio_signal->output_soundcard);
  }

  /* input soundcard */
  if(audio_signal->input_soundcard != NULL){
    g_object_unref(audio_signal->input_soundcard);
  }

  /* template */
  if(audio_signal->template != NULL){
    g_object_unref(audio_signal->template);
  }

  /* rt-template */
  if(audio_signal->rt_template != NULL){
    g_object_unref(audio_signal->rt_template);
  }

  /* note */
  if(audio_signal->note != NULL){
    g_list_free_full(audio_signal->note,
		     g_object_unref);
  }  
  
  /* recall id */
  if(audio_signal->recall_id != NULL){
    g_object_unref(audio_signal->recall_id);
  }

  /* audio data */
  pthread_mutex_destroy(audio_signal->stream_mutex);
  free(audio_signal->stream_mutex);

  pthread_mutexattr_destroy(audio_signal->stream_mutexattr);
  free(audio_signal->stream_mutexattr);

  g_list_free_full(audio_signal->stream,
		   (GDestroyNotify) ags_stream_free);

  /* call parent */
  G_OBJECT_CLASS(ags_audio_signal_parent_class)->finalize(gobject);
}

AgsUUID*
ags_audio_signal_get_uuid(AgsConnectable *connectable)
{
  AgsAudioSignal *audio_signal;
  
  AgsUUID *ptr;

  pthread_mutex_t *audio_signal_mutex;

  audio_signal = AGS_AUDIO_SIGNAL(connectable);

  /* get audio signal mutex */
  pthread_mutex_lock(ags_audio_signal_get_class_mutex());
  
  audio_signal_mutex = audio_signal->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_signal_get_class_mutex());

  /* get UUID */
  pthread_mutex_lock(audio_signal_mutex);

  ptr = audio_signal->uuid;

  pthread_mutex_unlock(audio_signal_mutex);
  
  return(ptr);
}

gboolean
ags_audio_signal_has_resource(AgsConnectable *connectable)
{
  return(TRUE);
}

gboolean
ags_audio_signal_is_ready(AgsConnectable *connectable)
{
  AgsAudioSignal *audio_signal;
  
  gboolean is_ready;

  pthread_mutex_t *audio_signal_mutex;

  audio_signal = AGS_AUDIO_SIGNAL(connectable);

  /* get audio signal mutex */
  pthread_mutex_lock(ags_audio_signal_get_class_mutex());
  
  audio_signal_mutex = audio_signal->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_signal_get_class_mutex());

  /* check is ready */
  pthread_mutex_lock(audio_signal_mutex);
  
  is_ready = (((AGS_AUDIO_SIGNAL_ADDED_TO_REGISTRY & (audio_signal->flags)) != 0) ? TRUE: FALSE);
  
  pthread_mutex_unlock(audio_signal_mutex);

  return(is_ready);
}

void
ags_audio_signal_add_to_registry(AgsConnectable *connectable)
{
  AgsAudioSignal *audio_signal;

  AgsRegistry *registry;
  AgsRegistryEntry *entry;

  AgsApplicationContext *application_context;

  if(ags_connectable_is_ready(connectable)){
    return;
  }

  audio_signal = AGS_AUDIO_SIGNAL(connectable);

  ags_audio_signal_set_flags(audio_signal, AGS_AUDIO_SIGNAL_ADDED_TO_REGISTRY);

  application_context = ags_application_context_get_instance();

  registry = (AgsRegistry *) ags_service_provider_get_registry(AGS_SERVICE_PROVIDER(application_context));

  if(registry != NULL){
    entry = ags_registry_entry_alloc(registry);
    g_value_set_object(entry->entry,
		       (gpointer) audio_signal);
    ags_registry_add_entry(registry,
			   entry);
  }  
}

void
ags_audio_signal_remove_from_registry(AgsConnectable *connectable)
{
  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  //TODO:JK: implement me
}

xmlNode*
ags_audio_signal_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_audio_signal_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_audio_signal_xml_parse(AgsConnectable *connectable,
			   xmlNode *node)
{
  //TODO:JK: implement me  
}

gboolean
ags_audio_signal_is_connected(AgsConnectable *connectable)
{
  AgsAudioSignal *audio_signal;
  
  gboolean is_connected;

  pthread_mutex_t *audio_signal_mutex;

  audio_signal = AGS_AUDIO_SIGNAL(connectable);

  /* get audio signal mutex */
  pthread_mutex_lock(ags_audio_signal_get_class_mutex());
  
  audio_signal_mutex = audio_signal->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_signal_get_class_mutex());

  /* check is connected */
  pthread_mutex_lock(audio_signal_mutex);

  is_connected = (((AGS_AUDIO_SIGNAL_CONNECTED & (audio_signal->flags)) != 0) ? TRUE: FALSE);
  
  pthread_mutex_unlock(audio_signal_mutex);

  return(is_connected);
}

void
ags_audio_signal_connect(AgsConnectable *connectable)
{
  AgsAudioSignal *audio_signal;

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  audio_signal = AGS_AUDIO_SIGNAL(connectable);
  
  ags_audio_signal_set_flags(audio_signal, AGS_AUDIO_SIGNAL_CONNECTED);
}

void
ags_audio_signal_disconnect(AgsConnectable *connectable)
{
  AgsAudioSignal *audio_signal;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  audio_signal = AGS_AUDIO_SIGNAL(connectable);

  ags_audio_signal_unset_flags(audio_signal, AGS_AUDIO_SIGNAL_CONNECTED);
}

/**
 * ags_audio_signal_get_class_mutex:
 * 
 * Use this function's returned mutex to access mutex fields.
 *
 * Returns: the class mutex
 * 
 * Since: 2.0.0
 */
pthread_mutex_t*
ags_audio_signal_get_class_mutex()
{
  return(&ags_audio_signal_class_mutex);
}

/**
 * ags_audio_signal_test_flags:
 * @audio_signal: the #AgsAudioSignal
 * @flags: the flags
 *
 * Test @flags to be set on @audio_signal.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 2.0.0
 */
gboolean
ags_audio_signal_test_flags(AgsAudioSignal *audio_signal, guint flags)
{
  gboolean retval;  
  
  pthread_mutex_t *audio_signal_mutex;

  if(!AGS_IS_AUDIO_SIGNAL(audio_signal)){
    return(FALSE);
  }

  /* get audio_signal mutex */
  pthread_mutex_lock(ags_audio_signal_get_class_mutex());
  
  audio_signal_mutex = audio_signal->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_signal_get_class_mutex());

  /* test */
  pthread_mutex_lock(audio_signal_mutex);

  retval = (flags & (audio_signal->flags)) ? TRUE: FALSE;
  
  pthread_mutex_unlock(audio_signal_mutex);

  return(retval);
}

/**
 * ags_audio_signal_set_flags:
 * @audio_signal: the #AgsAudioSignal
 * @flags: see #AgsAudioSignalFlags-enum
 *
 * Enable a feature of @audio_signal.
 *
 * Since: 2.0.0
 */
void
ags_audio_signal_set_flags(AgsAudioSignal *audio_signal, guint flags)
{
  pthread_mutex_t *audio_signal_mutex;

  if(!AGS_IS_AUDIO_SIGNAL(audio_signal)){
    return;
  }

  /* get audio_signal mutex */
  pthread_mutex_lock(ags_audio_signal_get_class_mutex());
  
  audio_signal_mutex = audio_signal->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_signal_get_class_mutex());

  //TODO:JK: add more?

  /* set flags */
  pthread_mutex_lock(audio_signal_mutex);

  audio_signal->flags |= flags;
  
  pthread_mutex_unlock(audio_signal_mutex);
}
    
/**
 * ags_audio_signal_unset_flags:
 * @audio_signal: the #AgsAudioSignal
 * @flags: see #AgsAudioSignalFlags-enum
 *
 * Disable a feature of @audio_signal.
 *
 * Since: 2.0.0
 */
void
ags_audio_signal_unset_flags(AgsAudioSignal *audio_signal, guint flags)
{  
  pthread_mutex_t *audio_signal_mutex;

  if(!AGS_IS_AUDIO_SIGNAL(audio_signal)){
    return;
  }

  /* get audio_signal mutex */
  pthread_mutex_lock(ags_audio_signal_get_class_mutex());
  
  audio_signal_mutex = audio_signal->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_signal_get_class_mutex());

  //TODO:JK: add more?

  /* unset flags */
  pthread_mutex_lock(audio_signal_mutex);

  audio_signal->flags &= (~flags);
  
  pthread_mutex_unlock(audio_signal_mutex);
}

/**
 * ags_stream_alloc:
 * @buffer_size: the buffer size
 * @format: the format
 *
 * Allocs an audio buffer.
 *
 * Returns: the audio data array
 *
 * Since: 2.0.0
 */
void*
ags_stream_alloc(guint buffer_size,
		 guint format)
{
  void *buffer;
  guint word_size;
  
  switch(format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
    {
      buffer = (gint8 *) malloc(buffer_size * sizeof(gint8));
      word_size = sizeof(gint8);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
    {
      buffer = (gint16 *) malloc(buffer_size * sizeof(gint16));
      word_size = sizeof(gint16);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
    {
      buffer = (gint32 *) malloc(buffer_size * sizeof(gint32));
      //NOTE:JK: The 24-bit linear samples use 32-bit physical space
      word_size = sizeof(gint32);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
    {
      buffer = (gint32 *) malloc(buffer_size * sizeof(gint32));
      word_size = sizeof(gint32);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
    {
      buffer = (gint64 *) malloc(buffer_size * sizeof(gint64));
      word_size = sizeof(gint64);
    }
    break;
  case AGS_SOUNDCARD_FLOAT:
    {
      buffer = (gfloat *) malloc(buffer_size * sizeof(gfloat));
      word_size = sizeof(gfloat);
    }
    break;
  case AGS_SOUNDCARD_DOUBLE:
    {
      buffer = (gdouble *) malloc(buffer_size * sizeof(gdouble));
      word_size = sizeof(gdouble);
    }
    break;
  default:
    g_warning("ags_stream_alloc(): unsupported word size");
    return(NULL);
  }

  memset(buffer, 0, buffer_size * word_size);

  return(buffer);
}

/**
 * ags_stream_free:
 * @buffer_size: the buffer size
 *
 * Frees an audio buffer.
 *
 * Since: 2.0.0
 */
void
ags_stream_free(void *buffer)
{
  g_free(buffer);
}

/**
 * ags_audio_signal_set_output_soundcard:
 * @audio_signal: the #AgsAudioSignal
 * @output_soundcard: the #GObject implementing #AgsSoundcard
 * 
 * Set output soundcarod of @audio_signal.
 * 
 * Since: 2.0.0
 */
void
ags_audio_signal_set_output_soundcard(AgsAudioSignal *audio_signal, GObject *output_soundcard)
{
  guint samplerate;
  guint buffer_size;
  guint format;

  pthread_mutex_t *audio_signal_mutex;

  if(!AGS_IS_AUDIO_SIGNAL(audio_signal) ||
     !AGS_IS_SOUNDCARD(output_soundcard)){
    return;
  }
  
  /* get audio signal mutex */
  pthread_mutex_lock(ags_audio_signal_get_class_mutex());
  
  audio_signal_mutex = audio_signal->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_signal_get_class_mutex());

  /* set output soundcard */
  pthread_mutex_lock(audio_signal_mutex);

  if(audio_signal->output_soundcard == output_soundcard){
    pthread_mutex_unlock(audio_signal_mutex);
    
    return;
  }

  if(audio_signal->output_soundcard != NULL){
    g_object_unref(audio_signal->output_soundcard);
  }
      
  if(output_soundcard != NULL){
    g_object_ref(output_soundcard);
  }
      
  audio_signal->output_soundcard = output_soundcard;

  pthread_mutex_unlock(audio_signal_mutex);

  /* apply presets */
  if(output_soundcard != NULL){
    ags_soundcard_get_presets(AGS_SOUNDCARD(output_soundcard),
			      NULL,
			      &samplerate,
			      &buffer_size,
			      &format);

    g_object_set(audio_signal,
		 "samplerate", samplerate,
		 "buffer-size", buffer_size,
		 "format", format,
		 NULL);
  }
}

/**
 * ags_audio_signal_set_input_soundcard:
 * @audio_signal: the #AgsAudioSignal
 * @input_soundcard: the #GObject implementing #AgsSoundcard
 * 
 * Set input soundcarod of @audio_signal.
 * 
 * Since: 2.0.0
 */
void
ags_audio_signal_set_input_soundcard(AgsAudioSignal *audio_signal, GObject *input_soundcard)
{
  pthread_mutex_t *audio_signal_mutex;

  if(!AGS_IS_AUDIO_SIGNAL(audio_signal) ||
     !AGS_IS_SOUNDCARD(input_soundcard)){
    return;
  }
  
  /* get audio signal mutex */
  pthread_mutex_lock(ags_audio_signal_get_class_mutex());
  
  audio_signal_mutex = audio_signal->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_signal_get_class_mutex());

  /* set input soundcard */
  pthread_mutex_lock(audio_signal_mutex);

  if(audio_signal->input_soundcard == input_soundcard){
    pthread_mutex_unlock(audio_signal_mutex);
    
    return;
  }

  if(audio_signal->input_soundcard != NULL){
    g_object_unref(audio_signal->input_soundcard);
  }
      
  if(input_soundcard != NULL){
    g_object_ref(input_soundcard);
  }
      
  audio_signal->input_soundcard = input_soundcard;

  pthread_mutex_unlock(audio_signal_mutex);
}

/**
 * ags_audio_signal_set_samplerate:
 * @audio_signal: the #AgsAudioSignal
 * @samplerate: the samplerate
 *
 * Set samplerate.
 *
 * Since: 2.0.0
 */
void
ags_audio_signal_set_samplerate(AgsAudioSignal *audio_signal, guint samplerate)
{
  GList *stream;

  void *data, *resampled_data;

  guint stream_length;
  guint end_offset;
  guint buffer_size;
  guint old_samplerate;
  guint format;
  guint offset;
  guint copy_mode;
  
  pthread_mutex_t *audio_signal_mutex;
  pthread_mutex_t *stream_mutex;

  if(!AGS_IS_AUDIO_SIGNAL(audio_signal)){
    return;
  }
  
  /* get audio signal mutex */
  pthread_mutex_lock(ags_audio_signal_get_class_mutex());
  
  audio_signal_mutex = audio_signal->obj_mutex;
  stream_mutex = audio_signal->stream_mutex;
  
  pthread_mutex_unlock(ags_audio_signal_get_class_mutex());

  /* check resample */
  pthread_mutex_lock(audio_signal_mutex);

  old_samplerate = audio_signal->samplerate;

  pthread_mutex_unlock(audio_signal_mutex);

  if(old_samplerate == samplerate){
    return;
  }
  
  /* set samplerate */
  pthread_mutex_lock(audio_signal_mutex);

  buffer_size = audio_signal->buffer_size;
  format = audio_signal->format;

  audio_signal->samplerate = samplerate;

  audio_signal->loop_start = (guint) floor((double) samplerate * ((double) audio_signal->loop_start / (double) old_samplerate));
  audio_signal->loop_end = (guint) floor((double) samplerate * ((double) audio_signal->loop_end / (double) old_samplerate));

  pthread_mutex_unlock(audio_signal_mutex);

  /* resample buffer */
  pthread_mutex_lock(stream_mutex);

  data = NULL;
  
  stream_length = g_list_length(audio_signal->stream);

  switch(format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
    {
      data = (gint8 *) malloc(stream_length * buffer_size * sizeof(gint8));
      memset(data, 0, stream_length * buffer_size * sizeof(gint8));
      
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S8;
    }
    break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
    {
      data = (gint16 *) malloc(stream_length * buffer_size * sizeof(gint16));
      memset(data, 0, stream_length * buffer_size * sizeof(gint16));

      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S16;
    }
    break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
    {
      data = (gint32 *) malloc(stream_length * buffer_size * sizeof(gint32));
      memset(data, 0, stream_length * buffer_size * sizeof(gint32));

      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S32;
    }
    break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
    {
      data = (gint32 *) malloc(stream_length * buffer_size * sizeof(gint32));
      memset(data, 0, stream_length * buffer_size * sizeof(gint32));

      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S32;
    }
    break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
    {
      data = (gint64 *) malloc(stream_length * buffer_size * sizeof(gint64));
      memset(data, 0, stream_length * buffer_size * sizeof(gint64));

      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S64;
    }
    break;
  case AGS_SOUNDCARD_FLOAT:
    {
      data = (gfloat *) malloc(stream_length * buffer_size * sizeof(gfloat));
      memset(data, 0, stream_length * buffer_size * sizeof(gfloat));

      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_FLOAT;
    }
    break;
  case AGS_SOUNDCARD_DOUBLE:
    {
      data = (gdouble *) malloc(stream_length * buffer_size * sizeof(gdouble));
      memset(data, 0, stream_length * buffer_size * sizeof(gdouble));

      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_DOUBLE;
    }
    break;
  default:
    g_warning("ags_audio_signal_set_samplerate() - unsupported format");
  }

  stream = audio_signal->stream;

  offset = 0;
  
  while(stream != NULL){
    ags_audio_buffer_util_copy_buffer_to_buffer(data, 1, offset,
						stream->data, 1, 0,
						buffer_size, copy_mode);

    /* iterate */
    stream = stream->next;

    offset += buffer_size;
  }
  
  pthread_mutex_unlock(stream_mutex);

  resampled_data = ags_audio_buffer_util_resample(data, 1,
						  ags_audio_buffer_util_format_from_soundcard(format), old_samplerate,
						  stream_length * buffer_size,
						  samplerate);

  if(data != NULL){
    free(data);
  }

  ags_audio_signal_stream_resize(audio_signal,
				 (guint) ceil((samplerate * (stream_length * buffer_size / old_samplerate)) / buffer_size));
  
  pthread_mutex_lock(stream_mutex);

  stream = audio_signal->stream;

  stream_length = g_list_length(audio_signal->stream);
  end_offset = stream_length * buffer_size;	

  offset = 0;

  while(stream != NULL && offset < stream_length * buffer_size){
    ags_audio_buffer_util_clear_buffer(stream->data, 1,
				       buffer_size, ags_audio_buffer_util_format_from_soundcard(format));
    
    if(offset + buffer_size < stream_length * buffer_size){
      ags_audio_buffer_util_copy_buffer_to_buffer(stream->data, 1, 0,
						  resampled_data, 1, offset,
						  buffer_size, copy_mode);
    }else{
      if(end_offset > offset){
	ags_audio_buffer_util_copy_buffer_to_buffer(stream->data, 1, 0,
						    resampled_data, 1, offset,
						    end_offset - offset, copy_mode);
      }
    }
    
    /* iterate */
    stream = stream->next;

    offset += buffer_size;
  }
  
  pthread_mutex_unlock(stream_mutex);

  if(resampled_data != NULL){
    free(resampled_data);
  }
}

/**
 * ags_audio_signal_set_buffer_size:
 * @audio_signal: the #AgsAudioSignal
 * @buffer_size: the buffer size
 *
 * Set buffer size.
 *
 * Since: 2.0.0
 */
void
ags_audio_signal_set_buffer_size(AgsAudioSignal *audio_signal, guint buffer_size)
{
  GList *stream;

  void *data;

  guint stream_length;
  guint end_offset;
  guint offset;
  guint format;
  guint old_buffer_size;
  guint word_size;
  guint copy_mode;
  
  pthread_mutex_t *audio_signal_mutex;
  pthread_mutex_t *stream_mutex;

  if(!AGS_IS_AUDIO_SIGNAL(audio_signal)){
    return;
  }

  /* get audio signal mutex */
  pthread_mutex_lock(ags_audio_signal_get_class_mutex());
  
  audio_signal_mutex = audio_signal->obj_mutex;
  stream_mutex = audio_signal->stream_mutex;
  
  pthread_mutex_unlock(ags_audio_signal_get_class_mutex());

  /* check buffer size */
  pthread_mutex_lock(audio_signal_mutex);

  old_buffer_size = audio_signal->buffer_size;

  pthread_mutex_unlock(audio_signal_mutex);

  if(old_buffer_size == buffer_size){
    return;
  }
  
  /* get some fields and set buffer size */
  pthread_mutex_lock(audio_signal_mutex);

  format = audio_signal->format;

  audio_signal->buffer_size = buffer_size;
  
  pthread_mutex_unlock(audio_signal_mutex);

  /* resize buffer */
  pthread_mutex_lock(stream_mutex);

  data = NULL;
  
  stream_length = g_list_length(audio_signal->stream);
  word_size = 1;  
  
  switch(format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
    {
      data = (gint8 *) malloc(stream_length * old_buffer_size * sizeof(gint8));
      memset(data, 0, stream_length * old_buffer_size * sizeof(gint8));
      
      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S8_TO_S8;
      word_size = sizeof(gint8);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
    {
      data = (gint16 *) malloc(stream_length * old_buffer_size * sizeof(gint16));
      memset(data, 0, stream_length * old_buffer_size * sizeof(gint16));

      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S16_TO_S16;
      word_size = sizeof(gint16);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
    {
      data = (gint32 *) malloc(stream_length * old_buffer_size * sizeof(gint32));
      memset(data, 0, stream_length * old_buffer_size * sizeof(gint32));

      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S32;
      word_size = sizeof(gint32);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
    {
      data = (gint32 *) malloc(stream_length * old_buffer_size * sizeof(gint32));
      memset(data, 0, stream_length * old_buffer_size * sizeof(gint32));

      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S32_TO_S32;
      word_size = sizeof(gint32);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
    {
      data = (gint64 *) malloc(stream_length * old_buffer_size * sizeof(gint64));
      memset(data, 0, stream_length * old_buffer_size * sizeof(gint64));

      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_S64_TO_S64;
      word_size = sizeof(gint64);
    }
    break;
  case AGS_SOUNDCARD_FLOAT:
    {
      data = (gfloat *) malloc(stream_length * old_buffer_size * sizeof(gfloat));
      memset(data, 0, stream_length * old_buffer_size * sizeof(gfloat));

      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_FLOAT;
      word_size = sizeof(gfloat);
    }
    break;
  case AGS_SOUNDCARD_DOUBLE:
    {
      data = (gdouble *) malloc(stream_length * old_buffer_size * sizeof(gdouble));
      memset(data, 0, stream_length * old_buffer_size * sizeof(gdouble));

      copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_DOUBLE_TO_DOUBLE;
      word_size = sizeof(gdouble);
    }
    break;
  default:
    g_warning("ags_audio_signal_set_buffer_size() - unsupported format");
  }

  stream = audio_signal->stream;

  offset = 0;
  
  while(stream != NULL){
    ags_audio_buffer_util_copy_buffer_to_buffer(data, 1, offset,
						stream->data, 1, 0,
						old_buffer_size, copy_mode);

    /* iterate */
    stream = stream->next;

    offset += old_buffer_size;
  }

  pthread_mutex_unlock(stream_mutex);
  
  ags_audio_signal_stream_resize(audio_signal,
				 (guint) ceil(((double) stream_length * (double) old_buffer_size) / buffer_size));

  pthread_mutex_lock(stream_mutex);

  stream = audio_signal->stream;

  stream_length = g_list_length(audio_signal->stream);
  end_offset = stream_length * old_buffer_size;	

  offset = 0;
  
  while(stream != NULL && offset < stream_length * buffer_size){
    switch(format){
    case AGS_SOUNDCARD_SIGNED_8_BIT:
      {
	stream->data = (gint8 *) realloc(stream->data,
					 buffer_size * sizeof(gint8));
	memset(stream->data, 0, buffer_size * sizeof(gint8));
      }
      break;
    case AGS_SOUNDCARD_SIGNED_16_BIT:
      {
	stream->data = (gint16 *) realloc(stream->data,
					  buffer_size * sizeof(gint16));
	memset(stream->data, 0, buffer_size * sizeof(gint16));
      }
      break;
    case AGS_SOUNDCARD_SIGNED_24_BIT:
      {
	stream->data = (gint32 *) realloc(stream->data,
					  buffer_size * sizeof(gint32));
	memset(stream->data, 0, buffer_size * sizeof(gint32));
      }
      break;
    case AGS_SOUNDCARD_SIGNED_32_BIT:
      {
	stream->data = (gint32 *) realloc(stream->data,
					  buffer_size * sizeof(gint32));
	memset(stream->data, 0, buffer_size * sizeof(gint32));
      }
      break;
    case AGS_SOUNDCARD_SIGNED_64_BIT:
      {
	stream->data = (gint64 *) realloc(stream->data,
					  buffer_size * sizeof(gint64));
	memset(stream->data, 0, buffer_size * sizeof(gint64));
      }
      break;
    case AGS_SOUNDCARD_FLOAT:
      {
	stream->data = (gfloat *) realloc(stream->data,
					  buffer_size * sizeof(gfloat));
	memset(stream->data, 0, buffer_size * sizeof(gfloat));
      }
      break;
    case AGS_SOUNDCARD_DOUBLE:
      {
	stream->data = (gdouble *) realloc(stream->data,
					   buffer_size * sizeof(gdouble));
	memset(stream->data, 0, buffer_size * sizeof(gdouble));
      }
      break;
    default:
      g_warning("ags_audio_signal_set_buffer_size() - unsupported format");
    }

    ags_audio_buffer_util_clear_buffer(stream->data, 1,
				       buffer_size, ags_audio_buffer_util_format_from_soundcard(format));
    
    if(offset + buffer_size < end_offset){
      ags_audio_buffer_util_copy_buffer_to_buffer(stream->data, 1, 0,
						  data, 1, offset,
						  buffer_size, copy_mode);
    }else{
      if(end_offset > offset){
	ags_audio_buffer_util_copy_buffer_to_buffer(stream->data, 1, 0,
						    data, 1, offset,
						    end_offset - offset, copy_mode);
      }
    }
    
    /* iterate */
    stream = stream->next;

    offset += buffer_size;
  }

  pthread_mutex_unlock(stream_mutex);

  if(data != NULL){
    free(data);
  }
}

/**
 * ags_audio_signal_set_format:
 * @audio_signal: the #AgsAudioSignal
 * @format: the format
 *
 * Set format.
 *
 * Since: 2.0.0
 */
void
ags_audio_signal_set_format(AgsAudioSignal *audio_signal, guint format)
{
  GList *stream;

  void *data;

  guint buffer_size;
  guint old_format;
  guint copy_mode;

  pthread_mutex_t *audio_signal_mutex;
  pthread_mutex_t *stream_mutex;

  if(!AGS_IS_AUDIO_SIGNAL(audio_signal)){
    return;
  }

  /* get audio signal mutex */
  pthread_mutex_lock(ags_audio_signal_get_class_mutex());
  
  audio_signal_mutex = audio_signal->obj_mutex;
  stream_mutex = audio_signal->stream_mutex;
  
  pthread_mutex_unlock(ags_audio_signal_get_class_mutex());

  /* get some fields and set format */
  pthread_mutex_lock(audio_signal_mutex);

  buffer_size = audio_signal->buffer_size;
  old_format = audio_signal->format;
  
  audio_signal->format = format;

  /* set word size */
  switch(format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
    {
      audio_signal->word_size = sizeof(gint8);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
    {
      audio_signal->word_size = sizeof(gint16);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
    {
      audio_signal->word_size = sizeof(gint32);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
    {
      audio_signal->word_size = sizeof(gint32);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
    {
      audio_signal->word_size = sizeof(gint64);
    }
    break;
  case AGS_SOUNDCARD_FLOAT:
    {
      audio_signal->word_size = sizeof(gfloat);
    }
    break;
  case AGS_SOUNDCARD_DOUBLE:
    {
      audio_signal->word_size = sizeof(gdouble);
    }
    break;
  }

  pthread_mutex_unlock(audio_signal_mutex);

  if(old_format == 0){
    return;
  }
  
  /* resize buffer */
  pthread_mutex_lock(stream_mutex);

  stream = audio_signal->stream;
    
  copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(format),
						  ags_audio_buffer_util_format_from_soundcard(old_format));

  while(stream != NULL){
    data = ags_stream_alloc(audio_signal->buffer_size,
			    format);
    
    ags_audio_buffer_util_copy_buffer_to_buffer(data, 1, 0,
						stream->data, 1, 0,
						buffer_size, copy_mode);

    free(stream->data);
    
    stream->data = data;

    /* iterate */
    stream = stream->next;
  }

  pthread_mutex_unlock(stream_mutex);
}

/**
 * ags_audio_signal_get_length_till_current:
 * @audio_signal: the #AgsAudioSignal
 *
 * Counts the buffers from :stream upto :stream-current.
 *
 * Returns: the counted length.
 *
 * Since: 2.0.0
 */
guint
ags_audio_signal_get_length_till_current(AgsAudioSignal *audio_signal)
{
  GList *list, *stop;

  guint length;

  pthread_mutex_t *stream_mutex;

  if(!AGS_IS_AUDIO_SIGNAL(audio_signal)){
    return(0);
  }

  /* get stream mutex */
  pthread_mutex_lock(ags_audio_signal_get_class_mutex());
  
  stream_mutex = audio_signal->stream_mutex;
  
  pthread_mutex_unlock(ags_audio_signal_get_class_mutex());

  /* count entries */
  pthread_mutex_lock(stream_mutex);

  list = audio_signal->stream;
  length = 0;

  if(audio_signal->stream_current != NULL){
    stop = audio_signal->stream_current->next;

    while(list != stop){
      length++;
      list = list->next;
    }
  }
  
  pthread_mutex_unlock(stream_mutex);

  return(length);
}

/**
 * ags_audio_signal_add_stream:
 * @audio_signal: the #AgsAudioSignal
 *
 * Adds a buffer at the end of the stream.
 *
 * Since: 2.0.0
 */
void
ags_audio_signal_add_stream(AgsAudioSignal *audio_signal)
{
  GList *stream, *end_old;

  void *buffer;

  guint buffer_size;
  guint format;
  
  pthread_mutex_t *audio_signal_mutex;
  pthread_mutex_t *stream_mutex;

  if(!AGS_IS_AUDIO_SIGNAL(audio_signal)){
    return;
  }

  /* get audio signal mutex */
  pthread_mutex_lock(ags_audio_signal_get_class_mutex());
  
  audio_signal_mutex = audio_signal->obj_mutex;
  stream_mutex = audio_signal->stream_mutex;
  
  pthread_mutex_unlock(ags_audio_signal_get_class_mutex());

  /* get some fields and increment length */
  pthread_mutex_lock(audio_signal_mutex);

  buffer_size = audio_signal->buffer_size;
  format = audio_signal->format;

  audio_signal->length += 1;

  pthread_mutex_unlock(audio_signal_mutex);

  /* allocate stream and buffer */
  pthread_mutex_lock(stream_mutex);

  stream = g_list_alloc();
  buffer = ags_stream_alloc(buffer_size,
			    format);
  stream->data = buffer;

  if(audio_signal->stream_end != NULL){
    end_old = audio_signal->stream_end;

    stream->prev = end_old;
    end_old->next = stream;
  }else{
    audio_signal->stream = stream;
    audio_signal->stream_current = stream;
  }

  audio_signal->stream_end = stream;

  pthread_mutex_unlock(stream_mutex);
}

/**
 * ags_audio_signal_stream_resize:
 * @audio_signal: the #AgsAudioSignal
 * @length: the new length
 *
 * Resize stream of @audio_signal to @length number of buffers.
 *
 * Since: 2.0.0
 */
void
ags_audio_signal_stream_resize(AgsAudioSignal *audio_signal, guint length)
{
  guint buffer_size;
  guint format;
  guint old_length;
  guint i;

  pthread_mutex_t *audio_signal_mutex;
  pthread_mutex_t *stream_mutex;

  if(!AGS_IS_AUDIO_SIGNAL(audio_signal)){
    return;
  }

  /* get audio signal mutex */
  pthread_mutex_lock(ags_audio_signal_get_class_mutex());
  
  audio_signal_mutex = audio_signal->obj_mutex;
  stream_mutex = audio_signal->stream_mutex;
  
  pthread_mutex_unlock(ags_audio_signal_get_class_mutex());

  /* get some fields and set length */
  pthread_mutex_lock(audio_signal_mutex);

  buffer_size = audio_signal->buffer_size;
  format = audio_signal->format;

  old_length = audio_signal->length;

  audio_signal->length = length;

  pthread_mutex_unlock(audio_signal_mutex);

  /* resize stream */
  if(old_length < length){
    GList *stream, *end_old;

    void *buffer;

    stream = NULL;

    for(i = old_length; i < length; i++){
      buffer = ags_stream_alloc(buffer_size,
				format);

      stream = g_list_prepend(stream,
			      buffer);
    }

    stream = g_list_reverse(stream);

    /* concat */
    pthread_mutex_lock(stream_mutex);
    
    if(audio_signal->stream_end != NULL){
      end_old = audio_signal->stream_end;
      audio_signal->stream_end = g_list_last(stream);

      stream->prev = end_old;
      end_old->next = stream;
    }else{
      audio_signal->stream = stream;
      audio_signal->stream_end = g_list_last(stream);
      audio_signal->stream_current = stream;
    }

    pthread_mutex_unlock(stream_mutex);
  }else if(old_length > length){
    GList *stream, *stream_end, *stream_next;

    gboolean check_current;

    pthread_mutex_lock(stream_mutex);
    
    stream = audio_signal->stream;
    check_current = TRUE;

    for(i = 0; i < length; i++){
      if(check_current && stream == audio_signal->stream_current){
	audio_signal->stream_current = NULL;
	check_current = FALSE;
      }

      stream = stream->next;
    }

    if(length != 0){
      stream_end = stream->prev;
      stream_end->next = NULL;
      audio_signal->stream_end = stream_end;
    }else{
      audio_signal->stream = NULL;
      audio_signal->stream_current = NULL;
      audio_signal->stream_end = NULL;
    }

    pthread_mutex_unlock(stream_mutex);
    
    stream->prev = NULL;
    g_list_free_full(stream,
		     ags_stream_free);
  }
}

/**
 * ags_audio_signal_stream_safe_resize:
 * @audio_signal: the #AgsAudioSignal
 * @length: the new length
 *
 * Resize stream of @audio_signal to @length number of buffers. But doesn't shrink
 * more than the current stream position.
 *
 * Since: 2.0.0
 */
void
ags_audio_signal_stream_safe_resize(AgsAudioSignal *audio_signal, guint length)
{
  guint length_till_current;

  if(!AGS_IS_AUDIO_SIGNAL(audio_signal)){
    return;
  }

  length_till_current = ags_audio_signal_get_length_till_current(audio_signal);

  if(length_till_current < length){
    ags_audio_signal_stream_resize(audio_signal,
				   length);
  }else{
    ags_audio_signal_stream_resize(audio_signal,
				   length_till_current);
  }
}

/**
 * ags_audio_signal_duplicate_stream:
 * @audio_signal: the #AgsAudioSignal
 * @template: the template #AgsAudioSignal
 * 
 * Apply @template audio data to @audio_signal. Note should only be invoked
 * by proper recall context because only the stream of @template is locked.
 *
 * Since: 2.0.0
 */
void
ags_audio_signal_duplicate_stream(AgsAudioSignal *audio_signal,
				  AgsAudioSignal *template)
{
  GList *template_stream, *stream;

  guint samplerate;
  guint buffer_size;
  guint template_format, format;
  guint length;
  guint copy_mode;

  pthread_mutex_t *template_mutex;
  pthread_mutex_t *audio_signal_mutex;
  pthread_mutex_t *template_stream_mutex;

  if(!AGS_IS_AUDIO_SIGNAL(template)){
    if(!AGS_IS_AUDIO_SIGNAL(audio_signal)){
      return;
    }else{
      ags_audio_signal_stream_resize(audio_signal,
				     0);
      return;
    }
  }
  
  /* get audio signal mutex */
  pthread_mutex_lock(ags_audio_signal_get_class_mutex());
  
  template_mutex = template->obj_mutex;
  audio_signal_mutex = audio_signal->obj_mutex;
  
  template_stream_mutex = template->stream_mutex;

  pthread_mutex_unlock(ags_audio_signal_get_class_mutex());

  /* check stream presence */
  pthread_mutex_lock(template_stream_mutex);

  if(template->stream == NULL){
    pthread_mutex_unlock(template_stream_mutex);
    
    ags_audio_signal_stream_resize(audio_signal,
				   0);

    return;
  }
  
  pthread_mutex_unlock(template_stream_mutex);
  
  /* get some fields */
  pthread_mutex_lock(template_mutex);

  samplerate = template->samplerate;
  buffer_size = template->buffer_size;
  template_format = template->format;

  length = template->length;
  
  pthread_mutex_unlock(template_mutex);

  /* get some fields */
  pthread_mutex_lock(audio_signal_mutex);

  format = audio_signal->format;
  
  pthread_mutex_unlock(audio_signal_mutex);

  /*  */
  g_object_set(audio_signal,
	       "samplerate", samplerate,
	       "buffer-size", buffer_size,
	       NULL);
  ags_audio_signal_stream_resize(audio_signal,
				 length);

  copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(format),
						  ags_audio_buffer_util_format_from_soundcard(template_format));


  //NOTE:JK: lock only template
  pthread_mutex_lock(template_stream_mutex);

  template_stream = template->stream;
  stream = audio_signal->stream;
    
  while(template_stream != NULL){
    ags_audio_buffer_util_copy_buffer_to_buffer(stream->data, 1, 0,
						template_stream->data, 1, 0,
						buffer_size, copy_mode);

    /* iterate */
    stream = stream->next;
    template_stream = template_stream->next;
  }

  pthread_mutex_unlock(template_stream_mutex);
}

/**
 * ags_audio_signal_feed:
 * @audio_signal: the #AgsAudioSignal
 * @template: the template #AgsAudioSignal
 * @frame_count: the new frame count
 * 
 * Feed audio signal to grow upto frame count.
 * 
 * Since: 2.0.0
 */
void
ags_audio_signal_feed(AgsAudioSignal *audio_signal,
		      AgsAudioSignal *template,
		      guint frame_count)
{
  GList *stream, *template_stream;

  guint template_format, format;
  guint loop_start, loop_end;
  guint template_buffer_size, buffer_size;
  guint template_length;
  guint old_length;
  guint old_last_frame;
  guint old_frame_count;
  gdouble delay;
  guint attack;
  guint loop_length;
  guint loop_frame_count;
  guint n_frames;
  guint copy_n_frames;
  guint nth_loop;
  guint i, j, k;
  guint copy_mode;

  pthread_mutex_t *template_mutex;
  pthread_mutex_t *audio_signal_mutex;
  pthread_mutex_t *template_stream_mutex;

  if(!AGS_IS_AUDIO_SIGNAL(audio_signal) ||
     !AGS_IS_AUDIO_SIGNAL(template)){
    return;
  }
  
  /* get audio signal mutex */
  pthread_mutex_lock(ags_audio_signal_get_class_mutex());
  
  template_mutex = template->obj_mutex;
  audio_signal_mutex = audio_signal->obj_mutex;
  
  template_stream_mutex = template->stream_mutex;

  pthread_mutex_unlock(ags_audio_signal_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(template_mutex);

  template_buffer_size = template->buffer_size;
  template_format = template->format;
  
  loop_start = template->loop_start;
  loop_end = template->loop_end;

  template_length = template->length;
  
  pthread_mutex_unlock(template_mutex);

  /* get some fields */
  pthread_mutex_lock(audio_signal_mutex);

  buffer_size = audio_signal->buffer_size;
  format = audio_signal->format;
  
  old_length = audio_signal->length;
  old_last_frame = audio_signal->last_frame;
  old_frame_count = old_last_frame + (old_length * audio_signal->buffer_size) - audio_signal->first_frame;

  pthread_mutex_unlock(audio_signal_mutex);

  if(template_buffer_size != buffer_size){
    g_warning("template buffer size != buffer size");

    return;
  }
  
  /* resize */
  if(loop_end > loop_start){
    loop_length = loop_end - loop_start;
    loop_frame_count = ((frame_count - loop_start) / loop_length) * buffer_size;

    ags_audio_signal_stream_resize(audio_signal,
				   (guint) ceil(frame_count / buffer_size) + 1);
    
  }else{
    ags_audio_signal_stream_resize(audio_signal,
				   (guint) ceil(frame_count / buffer_size) + 1);

    return;
  }

  pthread_mutex_lock(audio_signal_mutex);

  delay = audio_signal->delay;
  attack = audio_signal->attack;
  
  audio_signal->last_frame = ((guint) (delay * buffer_size) + frame_count + attack) % buffer_size;

  pthread_mutex_unlock(audio_signal_mutex);

  if(template_length == 0){
    return;
  }
  
  //NOTE:JK: lock only template
  pthread_mutex_lock(template_stream_mutex);

  /* generic copying */
  stream = g_list_nth(audio_signal->stream,
		      (guint) ((delay * buffer_size) + attack) / buffer_size);
  template_stream = template->stream;

  /* loop related copying */
  copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(format),
						  ags_audio_buffer_util_format_from_soundcard(template_format));
  
  for(i = 0, j = 0, k = attack, nth_loop = 0; i < frame_count;){    
    /* compute count of frames to copy */
    copy_n_frames = buffer_size;

    /* limit nth loop */
    if(i > template->loop_start &&
       i + copy_n_frames > loop_start + loop_length &&
       i + copy_n_frames < loop_start + loop_frame_count &&
       i + copy_n_frames >= loop_start + (nth_loop + 1) * loop_length){
      copy_n_frames = (loop_start + (nth_loop + 1) * loop_length) - i;
    }

    /* check boundaries */
    if((k % buffer_size) + copy_n_frames > buffer_size){
      copy_n_frames = buffer_size - (k % buffer_size);
    }

    if(j + copy_n_frames > buffer_size){
      copy_n_frames = buffer_size - j;
    }

    if(stream == NULL ||
       template_stream == NULL){
      break;
    }
    
    /* copy */
    ags_audio_buffer_util_copy_buffer_to_buffer(stream->data, 1, k % buffer_size,
						template_stream->data, 1, j,
						copy_n_frames, copy_mode);
    
    /* increment and iterate */
    if((i + copy_n_frames) % buffer_size == 0){
      stream = stream->next;
    }

    if(j + copy_n_frames == buffer_size){
      template_stream = template_stream->next;
    }
    
    if(template_stream == NULL ||
       (i > template->loop_start &&
	i + copy_n_frames > loop_start + loop_length &&
	i + copy_n_frames < loop_start + loop_frame_count &&
	i + copy_n_frames >= loop_start + (nth_loop + 1) * loop_length)){
      j = loop_start % buffer_size;
      template_stream = g_list_nth(stream,
				   floor(loop_start / buffer_size));

      nth_loop++;
    }else{
      j += copy_n_frames;
    }
    
    i += copy_n_frames;
    k += copy_n_frames;

    if(j == buffer_size){
      j = 0;
    }
  }

  pthread_mutex_unlock(template_stream_mutex);
}

void
ags_audio_signal_real_add_note(AgsAudioSignal *audio_signal,
			       GObject *note)
{
  if(g_list_find(audio_signal->note, note) == NULL){
    g_object_ref(note);
    audio_signal->note = g_list_prepend(audio_signal->note,
					note);
  }
}

/**
 * ags_audio_signal_add_note:
 * @audio_signal: the #AgsAudioSignal
 * @note: the #AgsNote
 * 
 * Add note to @audio_signal.
 * 
 * Since: 2.0.0
 */
void
ags_audio_signal_add_note(AgsAudioSignal *audio_signal,
			  GObject *note)
{
  g_return_if_fail(AGS_IS_AUDIO_SIGNAL(audio_signal));
  g_object_ref(G_OBJECT(audio_signal));
  g_signal_emit(G_OBJECT(audio_signal),
		audio_signal_signals[ADD_NOTE], 0,
		note);
  g_object_unref(G_OBJECT(audio_signal));
}

void
ags_audio_signal_real_remove_note(AgsAudioSignal *audio_signal,
				  GObject *note)
{  
  if(g_list_find(audio_signal->note, note) != NULL){
    audio_signal->note = g_list_remove(audio_signal->note,
				       note);
    g_object_unref(note);  
  }
}

/**
 * ags_audio_signal_remove_note:
 * @audio_signal: the #AgsAudioSignal
 * @note: the #AgsNote
 * 
 * Remove note from @audio_signal.
 * 
 * Since: 2.0.0
 */
void
ags_audio_signal_remove_note(AgsAudioSignal *audio_signal,
			     GObject *note)
{
  g_return_if_fail(AGS_IS_AUDIO_SIGNAL(audio_signal));
  g_object_ref(G_OBJECT(audio_signal));
  g_signal_emit(G_OBJECT(audio_signal),
		audio_signal_signals[REMOVE_NOTE], 0,
		note);
  g_object_unref(G_OBJECT(audio_signal));
}

/**
 * ags_audio_signal_get_template:
 * @audio_signal: the #GList-struct containing #AgsAudioSignal
 *
 * Retrieve the template audio signal.
 *
 * Returns: the template #AgsAudioSignal or %NULL if not found
 *
 * Since: 2.0.0
 */
AgsAudioSignal*
ags_audio_signal_get_template(GList *audio_signal)
{
  AgsAudioSignal *current_audio_signal;
  
  guint audio_signal_flags;
  
  pthread_mutex_t *audio_signal_mutex;

  while(audio_signal != NULL){
    current_audio_signal = audio_signal->data;
    
    /* get audio signal mutex */
    pthread_mutex_lock(ags_audio_signal_get_class_mutex());
  
    audio_signal_mutex = current_audio_signal->obj_mutex;

    pthread_mutex_unlock(ags_audio_signal_get_class_mutex());

    /* get some fields */
    pthread_mutex_lock(audio_signal_mutex);

    audio_signal_flags = current_audio_signal->flags;
  
    pthread_mutex_unlock(audio_signal_mutex);

    /* check flags */
    if((AGS_AUDIO_SIGNAL_TEMPLATE & (audio_signal_flags)) != 0){
      return(current_audio_signal);
    }
    
    /* iterate */
    audio_signal = audio_signal->next;
  }

  return(NULL);
}

/**
 * ags_audio_signal_get_template:
 * @audio_signal: a #GList-struct containing #AgsAudioSignal
 *
 * Retrieve the realtime template audio signal.
 *
 * Returns: the rt-templates as #GList-struct containing #AgsAudioSignal
 *
 * Since: 2.0.0
 */
GList*
ags_audio_signal_get_rt_template(GList *audio_signal)
{
  AgsAudioSignal *current_audio_signal;
  
  GList *rt_template;

  guint audio_signal_flags;
  
  pthread_mutex_t *audio_signal_mutex;

  rt_template = NULL;
  
  while(audio_signal != NULL){
    current_audio_signal = audio_signal->data;
    
    /* get audio signal mutex */
    pthread_mutex_lock(ags_audio_signal_get_class_mutex());
  
    audio_signal_mutex = current_audio_signal->obj_mutex;

    pthread_mutex_unlock(ags_audio_signal_get_class_mutex());

    /* get some fields */
    pthread_mutex_lock(audio_signal_mutex);

    audio_signal_flags = current_audio_signal->flags;
  
    pthread_mutex_unlock(audio_signal_mutex);

    /* check flags */
    if((AGS_AUDIO_SIGNAL_RT_TEMPLATE & (audio_signal_flags)) != 0){
      rt_template = g_list_prepend(rt_template,
				   current_audio_signal);
    }

    /* iterate */
    audio_signal = audio_signal->next;
  }

  rt_template = g_list_reverse(rt_template);
  
  return(rt_template);
}

/**
 * ags_audio_signal_find_by_recall_id:
 * @audio_signal: a #GList-struct containing #AgsAudioSignal
 * @recall_id: matching #AgsRecallID
 *
 * Retrieve next audio signal refering to @recall_id
 *
 * Returns: matching #AgsAudioSignal
 *
 * Since: 2.0.0
 */
GList*
ags_audio_signal_find_by_recall_id(GList *audio_signal,
				   GObject *recall_id)
{
  AgsAudioSignal *current_audio_signal;
  AgsRecallID *current_recall_id;

  pthread_mutex_t *audio_signal_mutex;

  while(audio_signal != NULL){
    current_audio_signal = audio_signal->data;

    /* get audio signal mutex */
    pthread_mutex_lock(ags_audio_signal_get_class_mutex());
  
    audio_signal_mutex = current_audio_signal->obj_mutex;

    pthread_mutex_unlock(ags_audio_signal_get_class_mutex());

    /* get some fields */
    pthread_mutex_lock(audio_signal_mutex);

    current_recall_id = (AgsRecallID *) current_audio_signal->recall_id;
  
    pthread_mutex_unlock(audio_signal_mutex);

    /* check recall id */
    if(current_recall_id == (AgsRecallID *) recall_id){
      return(audio_signal);
    }

    /* iterate */
    audio_signal = audio_signal->next;
  }

  return(NULL);
}

/**
 * ags_audio_signal_find_stream_current:
 * @audio_signal: the #GList-struct containing #AgsAudioSignal
 * @recall_id: the matching #AgsRecallID
 * 
 * Retrieve next current stream of #AgsAudioSignal list. Warning this function does not
 * lock the stream mutex.
 *
 * Returns: next #GList-struct matching #AgsRecallID
 *
 * Since: 2.0.0
 */
GList*
ags_audio_signal_find_stream_current(GList *audio_signal,
				     GObject *recall_id)
{
  AgsAudioSignal *current_audio_signal;
  AgsRecallID *current_recall_id;

  GList *stream_current;
  
  while((audio_signal = ags_audio_signal_find_by_recall_id(audio_signal, recall_id)) != NULL){
    current_audio_signal = audio_signal->data;

    /* get some fields */
    stream_current = current_audio_signal->stream_current;
    
    if(stream_current != NULL){
      return(audio_signal);
    }

    /* iterate */
    audio_signal = audio_signal->next;
  }

  return(NULL);
}

/**
 * ags_audio_signal_is_active:
 * @audio_signal: the #GList-struct containing #AgsAudioSignal
 * @recall_id: the #AgsRecallID
 * 
 * Check if is active.
 * 
 * Returns: %TRUE if related audio signal to recall id is available, otherwise %FALSE
 * 
 * Since: 2.0.0
 */
gboolean
ags_audio_signal_is_active(GList *audio_signal,
			   GObject *recall_id)
{
  AgsAudioSignal *current_audio_signal;
  AgsRecallID *current_recall_id;
  AgsRecyclingContext *current_recycling_context, *recycling_context;
  
  pthread_mutex_t *audio_signal_mutex;
  pthread_mutex_t *recall_id_mutex;

  if(!AGS_IS_RECALL_ID(recall_id)){
    return(FALSE);
  }

  /* get audio signal mutex */
  pthread_mutex_lock(ags_recall_id_get_class_mutex());
  
  recall_id_mutex = AGS_RECALL_ID(recall_id)->obj_mutex;

  pthread_mutex_unlock(ags_recall_id_get_class_mutex());

  /* get recycling context */
  pthread_mutex_lock(recall_id_mutex);
  
  recycling_context = AGS_RECALL_ID(recall_id)->recycling_context;

  pthread_mutex_unlock(recall_id_mutex);
  
  while(audio_signal != NULL){
    current_audio_signal = audio_signal->data;

    /* get audio signal mutex */
    pthread_mutex_lock(ags_audio_signal_get_class_mutex());
  
    audio_signal_mutex = current_audio_signal->obj_mutex;

    pthread_mutex_unlock(ags_audio_signal_get_class_mutex());

    /* get some fields */
    pthread_mutex_lock(audio_signal_mutex);

    current_recall_id = (AgsRecallID *) current_audio_signal->recall_id;

    pthread_mutex_unlock(audio_signal_mutex);

    /* get current recycling context */
    current_recycling_context = NULL;
    
    if(current_recall_id != NULL){
      /* get recall id mutex */
      pthread_mutex_lock(ags_recall_id_get_class_mutex());
  
      recall_id_mutex = current_recall_id->obj_mutex;

      pthread_mutex_unlock(ags_recall_id_get_class_mutex());

      /* get some fields */
      pthread_mutex_lock(recall_id_mutex);
      
      current_recycling_context = current_recall_id->recycling_context;

      pthread_mutex_unlock(recall_id_mutex);
    }
    
    if(current_recycling_context == recycling_context){
      return(TRUE);
    }

    /* iterate */
    audio_signal = audio_signal->next;
  }
  
  return(FALSE);
}

/**
 * ags_audio_signal_new:
 * @output_soundcard: the assigned output #AgsSoundcard
 * @recycling: the #AgsRecycling
 * @recall_id: the #AgsRecallID, it can be NULL if %AGS_AUDIO_SIGNAL_TEMPLATE is set
 *
 * Creates a #AgsAudioSignal, with defaults of @output_soundcard, linking @recycling tree
 * and refering to @recall_id.
 *
 * Returns: a new #AgsAudioSignal
 *
 * Since: 2.0.0
 */
AgsAudioSignal*
ags_audio_signal_new(GObject *output_soundcard,
		     GObject *recycling,
		     GObject *recall_id)
{
  AgsAudioSignal *audio_signal;

  audio_signal = (AgsAudioSignal *) g_object_new(AGS_TYPE_AUDIO_SIGNAL,
						 "output-soundcard", output_soundcard,
						 "recycling", recycling,
						 "recall-id", recall_id,
						 NULL);

  return(audio_signal);
}

/**
 * ags_audio_signal_new_with_length:
 * @output_soundcard: the assigned output #AgsSoundcard
 * @recycling: the #AgsRecycling
 * @recall_id: the #AgsRecallID, it can be NULL if %AGS_AUDIO_SIGNAL_TEMPLATE is set
 * @length: audio data frame count
 *
 * Creates a #AgsAudioSignal, with defaults of @soundcard, linking @recycling tree
 * and refering to @recall_id.
 * The audio data is tiled to @length frame count.
 *
 * Returns: a new #AgsAudioSignal
 *
 * Since: 2.0.0
 */
AgsAudioSignal*
ags_audio_signal_new_with_length(GObject *output_soundcard,
				 GObject *recycling,
				 GObject *recall_id,
				 guint length)
{
  AgsAudioSignal *audio_signal, *template;

  audio_signal = (AgsAudioSignal *) g_object_new(AGS_TYPE_AUDIO_SIGNAL,
						 "output-soundcard", output_soundcard,
						 "recycling", recycling,
						 "recall-id", recall_id,
						 NULL);

  ags_audio_signal_stream_resize(audio_signal,
				 length);
  
  return(audio_signal);
}
