/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <stdint.h>

#include <stdlib.h>
#include <math.h>
#include <string.h>

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
void ags_audio_signal_connect(AgsConnectable *connectable);
void ags_audio_signal_disconnect(AgsConnectable *connectable);
void ags_audio_signal_dispose(GObject *gobject);
void ags_audio_signal_finalize(GObject *gobject);

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
 * #AgsAudioSignal organizes audio data within a #GList whereby data
 * pointing to the buffer.
 */

enum{
  PROP_0,
  PROP_OUTPUT_SOUNDCARD,
  PROP_INPUT_SOUNDCARD,
  PROP_RECYCLING,
  PROP_RECALL_ID,
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
  PROP_STREAM,
  PROP_STREAM_END,
  PROP_STREAM_CURRENT,
  PROP_NOTE,
  PROP_RT_TEMPLATE,
};

enum{
  ADD_NOTE,
  REMOVE_NOTE,
  LAST_SIGNAL,
};

static gpointer ags_audio_signal_parent_class = NULL;
static guint audio_signal_signals[LAST_SIGNAL];

GType
ags_audio_signal_get_type(void)
{
  static GType ags_type_audio_signal = 0;

  if(!ags_type_audio_signal){
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
  }

  return(ags_type_audio_signal);
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
   * AgsAudioSignal:recall-id:
   *
   * The assigned #AgsRecallID providing context.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("recall_id",
				   i18n_pspec("assigned recall id"),
				   i18n_pspec("The recall id it is assigned with"),
				   AGS_TYPE_RECALL_ID,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_ID,
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
   * AgsAudioSignal:stream:
   *
   * The stream it contains.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_pointer("stream",
				    i18n_pspec("containing stream"),
				    i18n_pspec("The stream it contains"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
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
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
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
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_STREAM_CURRENT,
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
}

void
ags_audio_signal_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_audio_signal_connect;
  connectable->disconnect = ags_audio_signal_disconnect;
}

void
ags_audio_signal_init(AgsAudioSignal *audio_signal)
{
  AgsConfig *config;
  
  gchar *str;
 
  audio_signal->flags = 0;

  audio_signal->output_soundcard = NULL;
  audio_signal->input_soundcard = NULL;

  audio_signal->recycling = NULL;
  audio_signal->recall_id = NULL;

  config = ags_config_get_instance();

  /* presets */
  audio_signal->samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  audio_signal->buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  audio_signal->format = AGS_SOUNDCARD_SIGNED_16_BIT;
  audio_signal->word_size = sizeof(gint16);
  
  /* samplerate */
  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "samplerate");
  
  if(str == NULL){
    str = ags_config_get_value(config,
			       AGS_CONFIG_SOUNDCARD_0,
			       "samplerate");
  }  

  if(str != NULL){
    audio_signal->samplerate = g_ascii_strtoull(str,
						NULL,
						10);
    free(str);
  }

  /* buffer-size */
  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "buffer-size");

  if(str == NULL){
    str = ags_config_get_value(config,
			       AGS_CONFIG_SOUNDCARD_0,
			       "buffer-size");
  }
  
  if(str != NULL){
    audio_signal->buffer_size = g_ascii_strtoull(str,
						 NULL,
						 10);
    free(str);
  }
  
  /* format */
  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "format");

  if(str == NULL){
    str = ags_config_get_value(config,
			       AGS_CONFIG_SOUNDCARD_0,
			       "format");
  }
  
  if(str != NULL){
    audio_signal->format = g_ascii_strtoull(str,
					    NULL,
					    10);
    free(str);

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
  }
  
  /*  */
  audio_signal->length = 0;
  audio_signal->first_frame = 0;
  audio_signal->last_frame = 0;

  audio_signal->frame_count = 0;
  audio_signal->loop_start = 0;
  audio_signal->loop_end = 0;

  audio_signal->delay = 0.0;
  audio_signal->attack = 0;

  audio_signal->stream_beginning = NULL;
  audio_signal->stream_current = NULL;
  audio_signal->stream_end = NULL;

  audio_signal->rt_template = NULL;
  audio_signal->note = NULL;
}

void
ags_audio_signal_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec)
{
  AgsAudioSignal *audio_signal;

  audio_signal = AGS_AUDIO_SIGNAL(gobject);

  switch(prop_id){
  case PROP_OUTPUT_SOUNDCARD:
    {
      GObject *output_soundcard;

      guint samplerate;
      guint buffer_size;
      guint format;
      
      output_soundcard = g_value_get_object(value);

      if(audio_signal->output_soundcard == output_soundcard){
	return;
      }

      if(audio_signal->output_soundcard != NULL){
	g_object_unref(audio_signal->output_soundcard);
      }
      
      if(output_soundcard != NULL){
	g_object_ref(output_soundcard);
      }
      
      audio_signal->output_soundcard = output_soundcard;

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
    break;
  case PROP_INPUT_SOUNDCARD:
    {
      GObject *input_soundcard;
      
      input_soundcard = g_value_get_object(value);

      if(audio_signal->input_soundcard == input_soundcard){
	return;
      }

      if(audio_signal->input_soundcard != NULL){
	g_object_unref(audio_signal->input_soundcard);
      }
      
      if(input_soundcard != NULL){
	g_object_ref(input_soundcard);
      }
      
      audio_signal->input_soundcard = input_soundcard;
    }
    break;
  case PROP_RECYCLING:
    {
      GObject *recycling;

      recycling = g_value_get_object(value);

      if(audio_signal->recycling == recycling){
	return;
      }

      if(audio_signal->recycling != NULL){
	g_object_unref(audio_signal->recycling);
      }
      
      if(recycling != NULL){
	g_object_ref(recycling);
      }
      
      audio_signal->recycling = recycling;
    }
    break;
  case PROP_RECALL_ID:
    {
      GObject *recall_id;

      recall_id = g_value_get_object(value);
      
      if(audio_signal->recall_id == recall_id)
	return;
      
      if(audio_signal->recall_id != NULL)
	g_object_unref(audio_signal->recall_id);

      if(recall_id != NULL)
	g_object_ref(recall_id);

      audio_signal->recall_id = recall_id;
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

      audio_signal->length = length;
    }
    break;
  case PROP_FIRST_FRAME:
    {
      guint first_frame;

      first_frame = g_value_get_uint(value);

      audio_signal->first_frame = first_frame;
    }
    break;
  case PROP_LAST_FRAME:
    {
      guint last_frame;

      last_frame = g_value_get_uint(value);

      audio_signal->last_frame = last_frame;
    }
    break;
  case PROP_FRAME_COUNT:
    {
      guint frame_count;

      frame_count = g_value_get_uint(value);

      audio_signal->frame_count = frame_count;
    }
    break;
  case PROP_LOOP_START:
    {
      guint loop_start;

      loop_start = g_value_get_uint(value);

      audio_signal->loop_start = loop_start;
    }
    break;
  case PROP_LOOP_END:
    {
      guint loop_end;

      loop_end = g_value_get_uint(value);

      audio_signal->loop_end = loop_end;
    }
    break;
  case PROP_DELAY:
    {
      gdouble delay;

      delay = g_value_get_double(value);

      audio_signal->delay = delay;
    }
    break;
  case PROP_ATTACK:
    {
      guint attack;

      attack = g_value_get_uint(value);

      audio_signal->attack = attack;
    }
    break;
  case PROP_STREAM:
    {
      gpointer data;

      data = g_value_get_pointer(value);

      if(data == NULL ||
	 g_list_find(audio_signal->stream_beginning,
		     data) != NULL){
      }

      audio_signal->stream_beginning = g_list_append(audio_signal->stream_beginning,
						     data);
      audio_signal->stream_end = g_list_last(audio_signal->stream_beginning);
    }
    break;
  case PROP_STREAM_CURRENT:
    {
      GList *current;

      current = g_value_get_pointer(value);

      audio_signal->stream_current = current;
    }
    break;
  case PROP_RT_TEMPLATE:
    {
      GObject *rt_template;

      rt_template = g_value_get_object(value);

      if(audio_signal->rt_template == rt_template){
	return;
      }

      if(audio_signal->rt_template != NULL){
	g_object_unref(audio_signal->rt_template);
      }
      
      if(rt_template != NULL){
	g_object_ref(rt_template);
      }
      
      audio_signal->rt_template = rt_template;
    }
    break;
  case PROP_NOTE:
    {
      GObject *note;

      note = g_value_get_pointer(value);

      if(g_list_find(audio_signal->note, note) != NULL){
	return;
      }

      ags_audio_signal_add_note(audio_signal,
				note);
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

  audio_signal = AGS_AUDIO_SIGNAL(gobject);

  switch(prop_id){
  case PROP_OUTPUT_SOUNDCARD:
    {
      g_value_set_object(value, audio_signal->output_soundcard);
    }
    break;
  case PROP_INPUT_SOUNDCARD:
    {
      g_value_set_object(value, audio_signal->input_soundcard);
    }
    break;
  case PROP_RECYCLING:
    {
      g_value_set_object(value, audio_signal->recycling);
    }
    break;
  case PROP_RECALL_ID:
    {
      g_value_set_object(value, audio_signal->recall_id);
    }
    break;
  case PROP_SAMPLERATE:
    {
      g_value_set_uint(value, audio_signal->samplerate);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      g_value_set_uint(value, audio_signal->buffer_size);
    }
  case PROP_FORMAT:
    {
      g_value_set_uint(value, audio_signal->format);
    }
    break;
  case PROP_WORD_SIZE:
    {
      g_value_set_uint(value, audio_signal->word_size);
    }
    break;
  case PROP_LENGTH:
    {
      g_value_set_uint(value, audio_signal->length);
    }
    break;
  case PROP_FIRST_FRAME:
    {
      g_value_set_uint(value, audio_signal->first_frame);
    }
    break;
  case PROP_LAST_FRAME:
    {
      g_value_set_uint(value, audio_signal->last_frame);
    }
    break;
  case PROP_FRAME_COUNT:
    {
      g_value_set_uint(value, audio_signal->frame_count);
    }
    break;
  case PROP_LOOP_START:
    {
      g_value_set_uint(value, audio_signal->loop_start);
    }
    break;
  case PROP_LOOP_END:
    {
      g_value_set_uint(value, audio_signal->loop_end);
    }
    break;
  case PROP_DELAY:
    {
      g_value_set_double(value, audio_signal->delay);
    }
    break;
  case PROP_ATTACK:
    {
      g_value_set_uint(value, audio_signal->attack);
    }
    break;
  case PROP_STREAM:
    {
      g_value_set_pointer(value, g_list_copy(audio_signal->stream_beginning));
    }
    break;
  case PROP_STREAM_END:
    {
      g_value_set_pointer(value, audio_signal->stream_end);
    }
    break;
  case PROP_STREAM_CURRENT:
    {
      g_value_set_pointer(value, audio_signal->stream_current);
    }
    break;
  case PROP_RT_TEMPLATE:
    {
      g_value_set_object(value, audio_signal->rt_template);
    }
    break;
  case PROP_NOTE:
    {
      g_value_set_pointer(value,
			  g_list_copy(audio_signal->note));
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_audio_signal_connect(AgsConnectable *connectable)
{
  AgsAudioSignal *audio_signal;

  audio_signal = AGS_AUDIO_SIGNAL(connectable);

  if((AGS_AUDIO_SIGNAL_CONNECTED & (audio_signal->flags)) != 0){
    return;
  }

  audio_signal->flags |= AGS_AUDIO_SIGNAL_CONNECTED;
}

void
ags_audio_signal_disconnect(AgsConnectable *connectable)
{
  AgsAudioSignal *audio_signal;

  audio_signal = AGS_AUDIO_SIGNAL(connectable);

  if((AGS_AUDIO_SIGNAL_CONNECTED & (audio_signal->flags)) == 0){
    return;
  }

  audio_signal->flags &= (~AGS_AUDIO_SIGNAL_CONNECTED);
}

void
ags_audio_signal_dispose(GObject *gobject)
{
  AgsAudioSignal *audio_signal;

  audio_signal = AGS_AUDIO_SIGNAL(gobject);

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
  
  /* recycling */
  if(audio_signal->recycling != NULL){
    g_object_unref(audio_signal->recycling);
    
    audio_signal->recycling = NULL;  
  }

  /* recall id */
  if(audio_signal->recall_id != NULL){
    g_object_unref(audio_signal->recall_id);

    audio_signal->recall_id = NULL;
  }

  /* rt-template */
  if(audio_signal->rt_template != NULL){
    g_object_unref(audio_signal->rt_template);
    
    audio_signal->rt_template = NULL;  
  }

  /* note */
  if(audio_signal->note != NULL){
    g_object_unref(audio_signal->note);

    audio_signal->note = NULL;
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
  g_message("finalize AgsAudioSignal");

  if((AGS_AUDIO_SIGNAL_TEMPLATE & (audio_signal->flags)) != 0){
    g_warning("AGS_AUDIO_SIGNAL_TEMPLATE: destroying\n");
  }
#endif

  /* disconnect */
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

  /* output soundcard */
  if(audio_signal->output_soundcard != NULL){
    g_object_unref(audio_signal->output_soundcard);
  }

  /* input soundcard */
  if(audio_signal->input_soundcard != NULL){
    g_object_unref(audio_signal->input_soundcard);
  }
  
  /* recycling */
  if(audio_signal->recycling != NULL){
    g_object_unref(audio_signal->recycling);
  }

  /* recall id */
  if(audio_signal->recall_id != NULL){
    g_object_unref(audio_signal->recall_id);
  }

  /* audio data */
  if(audio_signal->stream_beginning != NULL){
    g_list_free_full(audio_signal->stream_beginning,
		     (GDestroyNotify) ags_stream_free);
  }

  /* note */
  if(audio_signal->note != NULL){
    g_object_unref(audio_signal->note);
  }
  
  /* rt-template */
  if(audio_signal->rt_template != NULL){
    g_object_unref(audio_signal->rt_template);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_audio_signal_parent_class)->finalize(gobject);
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
  if(!AGS_IS_AUDIO_SIGNAL(audio_signal)){
    return;
  }
  
  audio_signal->samplerate = samplerate;
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

  guint old_buffer_size;
  guint word_size;
  
  if(!AGS_IS_AUDIO_SIGNAL(audio_signal)){
    return;
  }

  old_buffer_size = audio_signal->buffer_size;

  audio_signal->buffer_size = buffer_size;

  stream = audio_signal->stream_beginning;

  while(stream != NULL){
    switch(audio_signal->format){
    case AGS_SOUNDCARD_SIGNED_8_BIT:
      {
	stream->data = (gint8 *) realloc(stream->data,
					 buffer_size * sizeof(gint8));
	word_size = sizeof(gint8);
      }
      break;
    case AGS_SOUNDCARD_SIGNED_16_BIT:
      {
	stream->data = (gint16 *) realloc(stream->data,
					  buffer_size * sizeof(gint16));
	word_size = sizeof(gint16);
      }
      break;
    case AGS_SOUNDCARD_SIGNED_24_BIT:
      {
	stream->data = (gint32 *) realloc(stream->data,
					  buffer_size * sizeof(gint32));
	word_size = sizeof(gint32);
      }
      break;
    case AGS_SOUNDCARD_SIGNED_32_BIT:
      {
	stream->data = (gint32 *) realloc(stream->data,
					  buffer_size * sizeof(gint32));
	word_size = sizeof(gint32);
      }
      break;
    case AGS_SOUNDCARD_SIGNED_64_BIT:
      {
	stream->data = (gint64 *) realloc(stream->data,
					  buffer_size * sizeof(gint64));
	word_size = sizeof(gint64);
      }
      break;
    case AGS_SOUNDCARD_FLOAT:
      {
	stream->data = (gfloat *) realloc(stream->data,
					  buffer_size * sizeof(gfloat));
	word_size = sizeof(gfloat);
      }
      break;
    case AGS_SOUNDCARD_DOUBLE:
      {
	stream->data = (gdouble *) realloc(stream->data,
					  buffer_size * sizeof(gdouble));
	word_size = sizeof(gdouble);
      }
      break;
    default:
      g_warning("ags_audio_signal_set_buffer_size() - unsupported format");
    }

    if(old_buffer_size < buffer_size){
      memset(stream->data + old_buffer_size, 0, (buffer_size - old_buffer_size) * word_size);
    }
    
    stream = stream->next;
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

  guint copy_mode;

  if(!AGS_IS_AUDIO_SIGNAL(audio_signal)){
    return;
  }

  stream = audio_signal->stream_beginning;

  while(stream != NULL){
    data = ags_stream_alloc(audio_signal->buffer_size,
			    format);
    
    copy_mode = ags_audio_buffer_util_get_copy_mode(format,
						    audio_signal->format);
    
    ags_audio_buffer_util_copy_buffer_to_buffer(data, 1, 0,
						stream->data, 1, 0,
						audio_signal->buffer_size, copy_mode);

    free(stream->data);

    stream->data = data;
    
    stream = stream->next;
  }

  /* set format and apply word size */
  audio_signal->format = format;

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
}

/**
 * ags_audio_signal_get_length_till_current:
 * @audio_signal: an #AgsAudioSignal
 *
 * Counts the buffers from :stream-beginning upto :stream-current.
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

  if(!AGS_IS_AUDIO_SIGNAL(audio_signal)){
    return(0);
  }

  list = audio_signal->stream_beginning;
  length = 0;

  if(audio_signal->stream_current != NULL){
    stop = audio_signal->stream_current->next;
  }else{
    return(0);
  }

  while(list != stop){
    length++;
    list = list->next;
  }

  return(length);
}

/**
 * ags_audio_signal_add_stream:
 * @audio_signal: an #AgsAudioSignal
 *
 * Adds a buffer at the end of the stream.
 *
 * Since: 2.0.0
 */
void
ags_audio_signal_add_stream(AgsAudioSignal *audio_signal)
{
  GList *stream, *end_old;
  gint16 *buffer;

  if(!AGS_IS_AUDIO_SIGNAL(audio_signal)){
    return;
  }
  
  stream = g_list_alloc();
  buffer = ags_stream_alloc(audio_signal->buffer_size,
			    audio_signal->format);
  stream->data = buffer;

  if(audio_signal->stream_end != NULL){
    end_old = audio_signal->stream_end;

    stream->prev = end_old;
    end_old->next = stream;
  }else{
    audio_signal->stream_beginning = stream;
    audio_signal->stream_current = stream;
  }

  audio_signal->stream_end = stream;

  audio_signal->length += 1;
}

/**
 * ags_audio_signal_stream_resize:
 * @audio_signal: an #AgsAudioSignal to resize.
 * @length: a guint as the new length.
 *
 * Resizes an #AgsAudioSignal's stream but be carefull with shrinking.
 * This function may crash the application.
 *
 * Since: 2.0.0
 */
void
ags_audio_signal_stream_resize(AgsAudioSignal *audio_signal, guint length)
{
  guint i;

  if(!AGS_IS_AUDIO_SIGNAL(audio_signal)){
    return;
  }

  if(audio_signal->length < length){
    GList *stream, *end_old;
    gint16 *buffer;

    stream = NULL;

    for(i = audio_signal->length; i < length; i++){
      buffer = ags_stream_alloc(audio_signal->buffer_size,
				audio_signal->format);

      stream = g_list_prepend(stream,
			      buffer);
    }

    stream = g_list_reverse(stream);

    if(audio_signal->stream_end != NULL){
      end_old = audio_signal->stream_end;
      audio_signal->stream_end = g_list_last(stream);

      stream->prev = end_old;
      end_old->next = stream;
    }else{
      audio_signal->stream_beginning = stream;
      audio_signal->stream_end = g_list_last(stream);
      audio_signal->stream_current = stream;
    }
  }else if(audio_signal->length > length){
    GList *stream, *stream_end, *stream_next;
    gboolean check_current;

    stream = audio_signal->stream_beginning;
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
      audio_signal->stream_beginning = NULL;
      audio_signal->stream_current = NULL;
      audio_signal->stream_end = NULL;
    }

    stream->prev = NULL;
    g_list_free_full(stream,
		     g_free);
  }

  audio_signal->length = length;
}

/**
 * ags_audio_signal_stream_safe_resize:
 * @audio_signal: an #AgsAudioSignal
 * @length: a guint
 *
 * Resizes an #AgsAudioSignal's stream but doesn't shrink more than the
 * current stream position.
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
 * @audio_signal: destination #AgsAudioSignal
 * @template: source #AgsAudioSignal
 * 
 * Copy stream of one audio signal to another
 *
 * Since: 2.0.0
 */
void
ags_audio_signal_duplicate_stream(AgsAudioSignal *audio_signal,
				  AgsAudioSignal *template)
{
  if(!AGS_IS_AUDIO_SIGNAL(template)){
    if(!AGS_IS_AUDIO_SIGNAL(audio_signal)){
      return;
    }else{
      ags_audio_signal_stream_resize(audio_signal,
				     0);
      return;
    }
  }

  if(template->stream_beginning == NULL){
    ags_audio_signal_stream_resize(audio_signal,
				   0);
  }else{
    GList *template_stream, *stream;

    guint copy_mode;

    audio_signal->buffer_size = template->buffer_size;
    ags_audio_signal_stream_resize(audio_signal,
				   template->length);

    stream = audio_signal->stream_beginning;
    template_stream = template->stream_beginning;

    switch(audio_signal->format){
    case AGS_SOUNDCARD_SIGNED_8_BIT:
    case AGS_SOUNDCARD_SIGNED_16_BIT:
    case AGS_SOUNDCARD_SIGNED_24_BIT:
    case AGS_SOUNDCARD_SIGNED_32_BIT:
    case AGS_SOUNDCARD_SIGNED_64_BIT:
    case AGS_SOUNDCARD_FLOAT:
    case AGS_SOUNDCARD_DOUBLE:
      break;
    default:
      g_warning("unsupported word size");
      return;
    }

    switch(template->format){
    case AGS_SOUNDCARD_SIGNED_8_BIT:
    case AGS_SOUNDCARD_SIGNED_16_BIT:
    case AGS_SOUNDCARD_SIGNED_24_BIT:
    case AGS_SOUNDCARD_SIGNED_32_BIT:
    case AGS_SOUNDCARD_SIGNED_64_BIT:
    case AGS_SOUNDCARD_FLOAT:
    case AGS_SOUNDCARD_DOUBLE:
      break;
    default:
      g_warning("unsupported word size");
      return;
    }

    copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(audio_signal->format),
						    ags_audio_buffer_util_format_from_soundcard(template->format));
    
    while(template_stream != NULL){
      ags_audio_buffer_util_copy_buffer_to_buffer(stream->data, 1, 0,
						  template_stream->data, 1, 0,
						  template->buffer_size, copy_mode);
      stream = stream->next;
      template_stream = template_stream->next;
    }
  }
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

  if(!AGS_IS_AUDIO_SIGNAL(audio_signal) ||
     !AGS_IS_AUDIO_SIGNAL(template)){
    return;
  }
  
  old_length = audio_signal->length;
  old_last_frame = audio_signal->last_frame;
  old_frame_count = old_last_frame + (old_length * audio_signal->buffer_size) - audio_signal->first_frame;
  
  /* resize */
  if(template->loop_end > template->loop_start){
    loop_length = template->loop_end - template->loop_start;
    loop_frame_count = ((frame_count - template->loop_start) / loop_length) * template->buffer_size;

    ags_audio_signal_stream_resize(audio_signal,
				   (guint) ceil(frame_count / audio_signal->buffer_size) + 1);
    
  }else{
    ags_audio_signal_stream_resize(audio_signal,
				   (guint) ceil(frame_count / audio_signal->buffer_size) + 1);

    return;
  }

  audio_signal->last_frame = ((guint) (delay * audio_signal->buffer_size) + frame_count + attack) % audio_signal->buffer_size;

  if(template->length == 0){
    return;
  }

  delay = audio_signal->delay;
  attack = audio_signal->attack;

  /* generic copying */
  stream = g_list_nth(audio_signal->stream_beginning,
		      (guint) ((delay * audio_signal->buffer_size) + attack) / audio_signal->buffer_size);
  template_stream = template->stream_beginning;

  /* loop related copying */
  copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(audio_signal->format),
						  ags_audio_buffer_util_format_from_soundcard(template->format));
  
  for(i = 0, j = 0, k = attack, nth_loop = 0; i < frame_count;){    
    /* compute count of frames to copy */
    copy_n_frames = audio_signal->buffer_size;

    /* limit nth loop */
    if(i > template->loop_start &&
       i + copy_n_frames > template->loop_start + loop_length &&
       i + copy_n_frames < template->loop_start + loop_frame_count &&
       i + copy_n_frames >= template->loop_start + (nth_loop + 1) * loop_length){
      copy_n_frames = (template->loop_start + (nth_loop + 1) * loop_length) - i;
    }

    /* check boundaries */
    if((k % audio_signal->buffer_size) + copy_n_frames > audio_signal->buffer_size){
      copy_n_frames = audio_signal->buffer_size - (k % audio_signal->buffer_size);
    }

    if(j + copy_n_frames > audio_signal->buffer_size){
      copy_n_frames = audio_signal->buffer_size - j;
    }

    if(stream == NULL ||
       template_stream == NULL){
      break;
    }
    
    /* copy */
    ags_audio_buffer_util_copy_buffer_to_buffer(stream->data, 1, k % audio_signal->buffer_size,
						template_stream->data, 1, j,
						copy_n_frames, copy_mode);
    
    /* increment and iterate */
    if((i + copy_n_frames) % audio_signal->buffer_size == 0){
      stream = stream->next;
    }

    if(j + copy_n_frames == template->buffer_size){
      template_stream = template_stream->next;
    }
    
    if(template_stream == NULL ||
       (i > template->loop_start &&
	i + copy_n_frames > template->loop_start + loop_length &&
	i + copy_n_frames < template->loop_start + loop_frame_count &&
	i + copy_n_frames >= template->loop_start + (nth_loop + 1) * loop_length)){
      j = template->loop_start % template->buffer_size;
      template_stream = g_list_nth(template->stream_beginning,
				   floor(template->loop_start / template->buffer_size));

      nth_loop++;
    }else{
      j += copy_n_frames;
    }
    
    i += copy_n_frames;
    k += copy_n_frames;

    if(j == template->buffer_size){
      j = 0;
    }
  }
}

void
ags_audio_signal_real_add_note(AgsAudioSignal *audio_signal,
			       GObject *note)
{
  if(!(AGS_IS_NOTE(note))){
    return;
  }
  
  g_object_ref(note);  
  audio_signal->note = g_list_prepend(audio_signal->note,
				      note);
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
  if(!(AGS_IS_NOTE(note))){
    return;
  }
  
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
 * @audio_signal: a #GList containing #AgsAudioSignal
 *
 * Retrieve the template audio signal.
 *
 * Returns: the template #AgsAudioSignal
 *
 * Since: 2.0.0
 */
AgsAudioSignal*
ags_audio_signal_get_template(GList *audio_signal)
{
  GList *list;

  list = audio_signal;

  while(list != NULL){
    if(AGS_IS_AUDIO_SIGNAL(list->data) &&
       (AGS_AUDIO_SIGNAL_TEMPLATE & (AGS_AUDIO_SIGNAL(list->data)->flags)) != 0){
      return((AgsAudioSignal *) list->data);
    }

    list = list->next;
  }

  return(NULL);
}

/**
 * ags_audio_signal_find_stream_current:
 * @audio_signal: a #GList containing #AgsAudioSignal
 * @recall_id: the matching #AgsRecallID
 * 
 * Retrieve next current stream of #AgsAudioSignal list.
 *
 * Returns: next #GList matching #AgsRecallID
 *
 * Since: 2.0.0
 */
GList*
ags_audio_signal_find_stream_current(GList *list_audio_signal,
				     GObject *recall_id)
{
  AgsAudioSignal *audio_signal;
  GList *list;

  list = list_audio_signal;

  while(list != NULL){
    audio_signal = AGS_AUDIO_SIGNAL(list->data);

    if(!AGS_IS_AUDIO_SIGNAL(list->data) ||
       (AGS_AUDIO_SIGNAL_TEMPLATE & (audio_signal->flags)) != 0){
      list = list->next;
      
      continue;
    }

    if(audio_signal->stream_current != NULL && audio_signal->recall_id == recall_id){
      return(list);
    }

    list = list->next;
  }

  return(NULL);
}

/**
 * ags_audio_signal_find_by_recall_id:
 * @audio_signal: a #GList containing #AgsAudioSignal
 * @recall_id: matching #AgsRecallID
 *
 * Retrieve next audio signal refering to @recall_id
 *
 * Returns: matching #AgsAudioSignal
 *
 * Since: 2.0.0
 */
GList*
ags_audio_signal_find_by_recall_id(GList *list_audio_signal,
				   GObject *recall_id)
{
  AgsAudioSignal *audio_signal;
  GList *list;

  list = list_audio_signal;

  while(list != NULL){
    audio_signal = AGS_AUDIO_SIGNAL(list->data);

    if(!AGS_IS_AUDIO_SIGNAL(list->data) ||
       (AGS_AUDIO_SIGNAL_TEMPLATE & (audio_signal->flags)) != 0){
      list = list->next;
      
      continue;
    }

    if(audio_signal->recall_id == recall_id){
      return(list);
    }

    list = list->next;
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
  AgsAudioSignal *current;
  AgsRecyclingContext *recycling_context;
  
  if(!AGS_IS_RECALL_ID(recall_id) ||
     AGS_RECALL_ID(recall_id)->recycling_context == NULL){
    return(FALSE);
  }
  
  recycling_context = AGS_RECALL_ID(recall_id)->recycling_context;
  
  while(audio_signal != NULL){
    if(!AGS_IS_AUDIO_SIGNAL(audio_signal->data)){
      audio_signal = audio_signal->next;

      continue;
    }
    
    current = AGS_AUDIO_SIGNAL(audio_signal->data);
    
    if(current->recall_id != NULL &&
       AGS_RECALL_ID(current->recall_id)->recycling_context == recycling_context){
      return(TRUE);
    }
    
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
 * Creates a #AgsAudioSignal, with defaults of @soundcard, linking @recycling tree
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
