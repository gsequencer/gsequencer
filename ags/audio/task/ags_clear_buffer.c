/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#include <ags/audio/task/ags_clear_buffer.h>

#include <ags/audio/alsa/ags_alsa_devout.h>
#include <ags/audio/alsa/ags_alsa_devin.h>
#include <ags/audio/alsa/ags_alsa_midiin.h>

#include <ags/audio/oss/ags_oss_devout.h>
#include <ags/audio/oss/ags_oss_devin.h>
#include <ags/audio/oss/ags_oss_midiin.h>

#include <ags/audio/jack/ags_jack_devout.h>
#include <ags/audio/jack/ags_jack_devin.h>
#include <ags/audio/jack/ags_jack_midiin.h>

#include <ags/audio/pulse/ags_pulse_devout.h>
#include <ags/audio/pulse/ags_pulse_devin.h>

#include <ags/audio/wasapi/ags_wasapi_devout.h>
#include <ags/audio/wasapi/ags_wasapi_devin.h>

#include <ags/audio/core-audio/ags_core_audio_devout.h>
#include <ags/audio/core-audio/ags_core_audio_devin.h>
#include <ags/audio/core-audio/ags_core_audio_midiin.h>

#include <ags/i18n.h>

void ags_clear_buffer_class_init(AgsClearBufferClass *clear_buffer);
void ags_clear_buffer_init(AgsClearBuffer *clear_buffer);
void ags_clear_buffer_set_property(GObject *gobject,
					 guint prop_id,
					 const GValue *value,
					 GParamSpec *param_spec);
void ags_clear_buffer_get_property(GObject *gobject,
					 guint prop_id,
					 GValue *value,
					 GParamSpec *param_spec);
void ags_clear_buffer_dispose(GObject *gobject);
void ags_clear_buffer_finalize(GObject *gobject);

void ags_clear_buffer_launch(AgsTask *task);

/**
 * SECTION:ags_clear_buffer
 * @short_description: switch buffer flag of device
 * @title: AgsClearBuffer
 * @section_id:
 * @include: ags/audio/task/ags_clear_buffer.h
 *
 * The #AgsClearBuffer task switches the buffer flag of device.
 */

static gpointer ags_clear_buffer_parent_class = NULL;

enum{
  PROP_0,
  PROP_DEVICE,
};

GType
ags_clear_buffer_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_clear_buffer = 0;

    static const GTypeInfo ags_clear_buffer_info = {
      sizeof(AgsClearBufferClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_clear_buffer_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsClearBuffer),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_clear_buffer_init,
    };

    ags_type_clear_buffer = g_type_register_static(AGS_TYPE_TASK,
						   "AgsClearBuffer",
						   &ags_clear_buffer_info,
						   0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_clear_buffer);
  }

  return g_define_type_id__volatile;
}

void
ags_clear_buffer_class_init(AgsClearBufferClass *clear_buffer)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  GParamSpec *param_spec;

  ags_clear_buffer_parent_class = g_type_class_peek_parent(clear_buffer);

  /* gobject */
  gobject = (GObjectClass *) clear_buffer;

  gobject->set_property = ags_clear_buffer_set_property;
  gobject->get_property = ags_clear_buffer_get_property;

  gobject->dispose = ags_clear_buffer_dispose;
  gobject->finalize = ags_clear_buffer_finalize;

  /* properties */
  /**
   * AgsClearBuffer:device:
   *
   * The assigned #AgsSoundcard or #AgsSequencer
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("device",
				   i18n_pspec("device of change device"),
				   i18n_pspec("The device of change device task"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVICE,
				  param_spec);

  /* task */
  task = (AgsTaskClass *) clear_buffer;

  task->launch = ags_clear_buffer_launch;
}

void
ags_clear_buffer_init(AgsClearBuffer *clear_buffer)
{
  clear_buffer->device = NULL;
}

void
ags_clear_buffer_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec)
{
  AgsClearBuffer *clear_buffer;

  clear_buffer = AGS_CLEAR_BUFFER(gobject);

  switch(prop_id){
  case PROP_DEVICE:
    {
      GObject *device;

      device = (GObject *) g_value_get_object(value);

      if(clear_buffer->device == (GObject *) device){
	return;
      }

      if(clear_buffer->device != NULL){
	g_object_unref(clear_buffer->device);
      }

      if(device != NULL){
	g_object_ref(device);
      }

      clear_buffer->device = (GObject *) device;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_clear_buffer_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec)
{
  AgsClearBuffer *clear_buffer;

  clear_buffer = AGS_CLEAR_BUFFER(gobject);

  switch(prop_id){
  case PROP_DEVICE:
    {
      g_value_set_object(value, clear_buffer->device);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_clear_buffer_dispose(GObject *gobject)
{
  AgsClearBuffer *clear_buffer;

  clear_buffer = AGS_CLEAR_BUFFER(gobject);

  if(clear_buffer->device != NULL){
    g_object_unref(clear_buffer->device);

    clear_buffer->device = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_clear_buffer_parent_class)->dispose(gobject);
}

void
ags_clear_buffer_finalize(GObject *gobject)
{
  AgsClearBuffer *clear_buffer;

  clear_buffer = AGS_CLEAR_BUFFER(gobject);

  if(clear_buffer->device != NULL){
    g_object_unref(clear_buffer->device);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_clear_buffer_parent_class)->finalize(gobject);
}

void
ags_clear_buffer_launch(AgsTask *task)
{
  AgsClearBuffer *clear_buffer;

  guint buffer_size;
  guint pcm_channels;
  guint format;
  guint nth_buffer;
  guint word_size;

  clear_buffer = AGS_CLEAR_BUFFER(task);

  g_return_if_fail(AGS_IS_SOUNDCARD(clear_buffer->device));
  
  ags_soundcard_get_presets(AGS_SOUNDCARD(clear_buffer->device),
			    &pcm_channels,
			    NULL,
			    &buffer_size,
			    &format);

  switch(format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
    {
      word_size = sizeof(gint8);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
    {
      word_size = sizeof(gint16);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
    {
      word_size = sizeof(gint32);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
    {
      word_size = sizeof(gint32);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
    {
      word_size = sizeof(gint64);
    }
    break;
  default:
    g_warning("ags_clear_buffer_launch(): unsupported word size");
      
    return;
  }

  nth_buffer = 0;

  if(AGS_IS_ALSA_DEVOUT(clear_buffer->device)){
    AgsAlsaDevout *alsa_devout;

    alsa_devout = (AgsAlsaDevout *) clear_buffer->device;

    /* retrieve nth buffer */
    nth_buffer = alsa_devout->app_buffer_mode;

    ags_soundcard_lock_buffer(AGS_SOUNDCARD(clear_buffer->device), alsa_devout->app_buffer[nth_buffer]);
    
    memset(alsa_devout->app_buffer[nth_buffer], 0, (size_t) pcm_channels * buffer_size * word_size);

    ags_soundcard_unlock_buffer(AGS_SOUNDCARD(clear_buffer->device), alsa_devout->app_buffer[nth_buffer]);
  }else if(AGS_IS_ALSA_DEVIN(clear_buffer->device)){
    AgsAlsaDevin *alsa_devin;

    alsa_devin = (AgsAlsaDevin *) clear_buffer->device;    

    /* retrieve nth buffer */
    if(alsa_devin->app_buffer_mode == AGS_ALSA_DEVIN_APP_BUFFER_3){
      nth_buffer = AGS_ALSA_DEVIN_APP_BUFFER_0;
    }else{
      nth_buffer = alsa_devin->app_buffer_mode + 1;
    }
    
    ags_soundcard_lock_buffer(AGS_SOUNDCARD(clear_buffer->device), alsa_devin->app_buffer[nth_buffer]);
    
    memset(alsa_devin->app_buffer[nth_buffer], 0, (size_t) pcm_channels * buffer_size * word_size);

    ags_soundcard_unlock_buffer(AGS_SOUNDCARD(clear_buffer->device), alsa_devin->app_buffer[nth_buffer]);
  }else if(AGS_IS_OSS_DEVOUT(clear_buffer->device)){
    AgsOssDevout *oss_devout;

    oss_devout = (AgsOssDevout *) clear_buffer->device;

    /* retrieve nth buffer */
    nth_buffer = oss_devout->app_buffer_mode;

    ags_soundcard_lock_buffer(AGS_SOUNDCARD(clear_buffer->device), oss_devout->app_buffer[nth_buffer]);
    
    memset(oss_devout->app_buffer[nth_buffer], 0, (size_t) pcm_channels * buffer_size * word_size);

    ags_soundcard_unlock_buffer(AGS_SOUNDCARD(clear_buffer->device), oss_devout->app_buffer[nth_buffer]);
  }else if(AGS_IS_OSS_DEVIN(clear_buffer->device)){
    AgsOssDevin *oss_devin;

    oss_devin = (AgsOssDevin *) clear_buffer->device;    

    /* retrieve nth buffer */
    if(oss_devin->app_buffer_mode == AGS_OSS_DEVIN_APP_BUFFER_3){
      nth_buffer = AGS_OSS_DEVIN_APP_BUFFER_0;
    }else{
      nth_buffer = oss_devin->app_buffer_mode + 1;
    }
    
    ags_soundcard_lock_buffer(AGS_SOUNDCARD(clear_buffer->device), oss_devin->app_buffer[nth_buffer]);
    
    memset(oss_devin->app_buffer[nth_buffer], 0, (size_t) pcm_channels * buffer_size * word_size);

    ags_soundcard_unlock_buffer(AGS_SOUNDCARD(clear_buffer->device), oss_devin->app_buffer[nth_buffer]);
  }else if(AGS_IS_JACK_DEVOUT(clear_buffer->device)){
    AgsJackDevout *jack_devout;

    GRecMutex *jack_devout_mutex;
    
    jack_devout = (AgsJackDevout *) clear_buffer->device;

    /* get jack devout mutex */
    jack_devout_mutex = AGS_JACK_DEVOUT_GET_OBJ_MUTEX(jack_devout);
    
    /* retrieve nth buffer */    
    g_rec_mutex_lock(jack_devout_mutex);
    
    if(jack_devout->app_buffer_mode == AGS_JACK_DEVOUT_APP_BUFFER_0){
      nth_buffer = 2;
    }else if(jack_devout->app_buffer_mode == AGS_JACK_DEVOUT_APP_BUFFER_1){
      nth_buffer = 3;
    }else if(jack_devout->app_buffer_mode == AGS_JACK_DEVOUT_APP_BUFFER_2){
      nth_buffer = 0;
    }else if(jack_devout->app_buffer_mode == AGS_JACK_DEVOUT_APP_BUFFER_3){
      nth_buffer = 1;
    }
            
    g_rec_mutex_unlock(jack_devout_mutex);
    
    ags_soundcard_lock_buffer(AGS_SOUNDCARD(clear_buffer->device), jack_devout->app_buffer[nth_buffer]);
    
    memset(jack_devout->app_buffer[nth_buffer], 0, (size_t) pcm_channels * buffer_size * word_size);

    ags_soundcard_unlock_buffer(AGS_SOUNDCARD(clear_buffer->device), jack_devout->app_buffer[nth_buffer]);
  }else if(AGS_IS_JACK_DEVIN(clear_buffer->device)){
    AgsJackDevin *jack_devin;

    GRecMutex *jack_devin_mutex;
    
    jack_devin = (AgsJackDevin *) clear_buffer->device;

    /* get jack devin mutex */
    jack_devin_mutex = AGS_JACK_DEVIN_GET_OBJ_MUTEX(jack_devin);
    
    /* retrieve nth buffer */    
    g_rec_mutex_lock(jack_devin_mutex);
    
    /* retrieve nth buffer */    
    if(jack_devin->app_buffer_mode == AGS_JACK_DEVIN_APP_BUFFER_0){
      nth_buffer = 2;
    }else if(jack_devin->app_buffer_mode == AGS_JACK_DEVIN_APP_BUFFER_1){
      nth_buffer = 3;
    }else if(jack_devin->app_buffer_mode == AGS_JACK_DEVIN_APP_BUFFER_2){
      nth_buffer = 0;
    }else if(jack_devin->app_buffer_mode == AGS_JACK_DEVIN_APP_BUFFER_3){
      nth_buffer = 1;
    }
    
    g_rec_mutex_unlock(jack_devin_mutex);

    ags_soundcard_lock_buffer(AGS_SOUNDCARD(clear_buffer->device), jack_devin->app_buffer[nth_buffer]);
    
    memset(jack_devin->app_buffer[nth_buffer], 0, (size_t) pcm_channels * buffer_size * word_size);

    ags_soundcard_unlock_buffer(AGS_SOUNDCARD(clear_buffer->device), jack_devin->app_buffer[nth_buffer]);
  }else if(AGS_IS_PULSE_DEVOUT(clear_buffer->device)){
    AgsPulseDevout *pulse_devout;

    GRecMutex *pulse_devout_mutex;
    
    pulse_devout = (AgsPulseDevout *) clear_buffer->device;

    /* get pulse devout mutex */
    pulse_devout_mutex = AGS_PULSE_DEVOUT_GET_OBJ_MUTEX(pulse_devout);
    
    /* retrieve nth buffer */    
    g_rec_mutex_lock(pulse_devout_mutex);

    if(pulse_devout->app_buffer_mode == AGS_PULSE_DEVOUT_APP_BUFFER_0){
      nth_buffer = 2;
    }else if(pulse_devout->app_buffer_mode == AGS_PULSE_DEVOUT_APP_BUFFER_1){
      nth_buffer = 3;
    }else if(pulse_devout->app_buffer_mode == AGS_PULSE_DEVOUT_APP_BUFFER_2){
      nth_buffer = 4;
    }else if(pulse_devout->app_buffer_mode == AGS_PULSE_DEVOUT_APP_BUFFER_3){
      nth_buffer = 5;
    }else if(pulse_devout->app_buffer_mode == AGS_PULSE_DEVOUT_APP_BUFFER_4){
      nth_buffer = 6;
    }else if(pulse_devout->app_buffer_mode == AGS_PULSE_DEVOUT_APP_BUFFER_5){
      nth_buffer = 7;
    }else if(pulse_devout->app_buffer_mode == AGS_PULSE_DEVOUT_APP_BUFFER_6){
      nth_buffer = 0;
    }else if(pulse_devout->app_buffer_mode == AGS_PULSE_DEVOUT_APP_BUFFER_7){
      nth_buffer = 1;
    }
      
    g_rec_mutex_unlock(pulse_devout_mutex);

    ags_soundcard_lock_buffer(AGS_SOUNDCARD(clear_buffer->device), pulse_devout->app_buffer[nth_buffer]);
    
    memset(pulse_devout->app_buffer[nth_buffer], 0, (size_t) pcm_channels * buffer_size * word_size);

    ags_soundcard_unlock_buffer(AGS_SOUNDCARD(clear_buffer->device), pulse_devout->app_buffer[nth_buffer]);
  }else if(AGS_IS_PULSE_DEVIN(clear_buffer->device)){
    AgsPulseDevin *pulse_devin;

    GRecMutex *pulse_devin_mutex;
    
    pulse_devin = (AgsPulseDevin *) clear_buffer->device;

    /* get pulse devin mutex */
    pulse_devin_mutex = AGS_PULSE_DEVIN_GET_OBJ_MUTEX(pulse_devin);
    
    /* retrieve nth buffer */    
    g_rec_mutex_lock(pulse_devin_mutex);

    if(pulse_devin->app_buffer_mode == AGS_PULSE_DEVIN_APP_BUFFER_0){
      nth_buffer = 2;
    }else if(pulse_devin->app_buffer_mode == AGS_PULSE_DEVIN_APP_BUFFER_1){
      nth_buffer = 3;
    }else if(pulse_devin->app_buffer_mode == AGS_PULSE_DEVIN_APP_BUFFER_2){
      nth_buffer = 4;
    }else if(pulse_devin->app_buffer_mode == AGS_PULSE_DEVIN_APP_BUFFER_3){
      nth_buffer = 5;
    }else if(pulse_devin->app_buffer_mode == AGS_PULSE_DEVIN_APP_BUFFER_4){
      nth_buffer = 6;
    }else if(pulse_devin->app_buffer_mode == AGS_PULSE_DEVIN_APP_BUFFER_5){
      nth_buffer = 7;
    }else if(pulse_devin->app_buffer_mode == AGS_PULSE_DEVIN_APP_BUFFER_6){
      nth_buffer = 0;
    }else if(pulse_devin->app_buffer_mode == AGS_PULSE_DEVIN_APP_BUFFER_7){
      nth_buffer = 1;
    }

    g_rec_mutex_unlock(pulse_devin_mutex);
      
    ags_soundcard_lock_buffer(AGS_SOUNDCARD(clear_buffer->device), pulse_devin->app_buffer[nth_buffer]);
    
    memset(pulse_devin->app_buffer[nth_buffer], 0, (size_t) pcm_channels * buffer_size * word_size);

    ags_soundcard_unlock_buffer(AGS_SOUNDCARD(clear_buffer->device), pulse_devin->app_buffer[nth_buffer]);
  }else if(AGS_IS_WASAPI_DEVOUT(clear_buffer->device)){
    AgsWasapiDevout *wasapi_devout;

    GRecMutex *wasapi_devout_mutex;
    
    wasapi_devout = (AgsWasapiDevout *) clear_buffer->device;

    /* get wasapi devout mutex */
    wasapi_devout_mutex = AGS_WASAPI_DEVOUT_GET_OBJ_MUTEX(wasapi_devout);
    
    /* retrieve nth buffer */    
    g_rec_mutex_lock(wasapi_devout_mutex);

    if(wasapi_devout->app_buffer_mode == AGS_WASAPI_DEVOUT_APP_BUFFER_0){
      nth_buffer = 2;
    }else if(wasapi_devout->app_buffer_mode == AGS_WASAPI_DEVOUT_APP_BUFFER_1){
      nth_buffer = 3;
    }else if(wasapi_devout->app_buffer_mode == AGS_WASAPI_DEVOUT_APP_BUFFER_2){
      nth_buffer = 4;
    }else if(wasapi_devout->app_buffer_mode == AGS_WASAPI_DEVOUT_APP_BUFFER_3){
      nth_buffer = 5;
    }else if(wasapi_devout->app_buffer_mode == AGS_WASAPI_DEVOUT_APP_BUFFER_4){
      nth_buffer = 6;
    }else if(wasapi_devout->app_buffer_mode == AGS_WASAPI_DEVOUT_APP_BUFFER_5){
      nth_buffer = 7;
    }else if(wasapi_devout->app_buffer_mode == AGS_WASAPI_DEVOUT_APP_BUFFER_6){
      nth_buffer = 0;
    }else if(wasapi_devout->app_buffer_mode == AGS_WASAPI_DEVOUT_APP_BUFFER_7){
      nth_buffer = 1;
    }
    
    g_rec_mutex_unlock(wasapi_devout_mutex);
            
    ags_soundcard_lock_buffer(AGS_SOUNDCARD(clear_buffer->device), wasapi_devout->app_buffer[nth_buffer]);
    
    memset(wasapi_devout->app_buffer[nth_buffer], 0, (size_t) pcm_channels * buffer_size * word_size);

    ags_soundcard_unlock_buffer(AGS_SOUNDCARD(clear_buffer->device), wasapi_devout->app_buffer[nth_buffer]);
  }else if(AGS_IS_WASAPI_DEVIN(clear_buffer->device)){
    AgsWasapiDevin *wasapi_devin;

    GRecMutex *wasapi_devin_mutex;
    
    wasapi_devin = (AgsWasapiDevin *) clear_buffer->device;
    
    /* get wasapi devin mutex */
    wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);
    
    /* retrieve nth buffer */    
    g_rec_mutex_lock(wasapi_devin_mutex);

    if(wasapi_devin->app_buffer_mode == AGS_WASAPI_DEVIN_APP_BUFFER_0){
      nth_buffer = 2;
    }else if(wasapi_devin->app_buffer_mode == AGS_WASAPI_DEVIN_APP_BUFFER_1){
      nth_buffer = 3;
    }else if(wasapi_devin->app_buffer_mode == AGS_WASAPI_DEVIN_APP_BUFFER_2){
      nth_buffer = 4;
    }else if(wasapi_devin->app_buffer_mode == AGS_WASAPI_DEVIN_APP_BUFFER_3){
      nth_buffer = 5;
    }else if(wasapi_devin->app_buffer_mode == AGS_WASAPI_DEVIN_APP_BUFFER_4){
      nth_buffer = 6;
    }else if(wasapi_devin->app_buffer_mode == AGS_WASAPI_DEVIN_APP_BUFFER_5){
      nth_buffer = 7;
    }else if(wasapi_devin->app_buffer_mode == AGS_WASAPI_DEVIN_APP_BUFFER_6){
      nth_buffer = 0;
    }else if(wasapi_devin->app_buffer_mode == AGS_WASAPI_DEVIN_APP_BUFFER_7){
      nth_buffer = 1;
    }
    
    g_rec_mutex_unlock(wasapi_devin_mutex);
    
    ags_soundcard_lock_buffer(AGS_SOUNDCARD(clear_buffer->device), wasapi_devin->app_buffer[nth_buffer]);
    
    memset(wasapi_devin->app_buffer[nth_buffer], 0, (size_t) pcm_channels * buffer_size * word_size);

    ags_soundcard_unlock_buffer(AGS_SOUNDCARD(clear_buffer->device), wasapi_devin->app_buffer[nth_buffer]);
  }else if(AGS_IS_CORE_AUDIO_DEVOUT(clear_buffer->device)){
    AgsCoreAudioDevout *core_audio_devout;
    
    core_audio_devout = (AgsCoreAudioDevout *) clear_buffer->device;

    /* retrieve nth buffer */    
    if(ags_core_audio_devout_test_flags(core_audio_devout, AGS_CORE_AUDIO_DEVOUT_BUFFER0)){
      nth_buffer = 2;
    }else if(ags_core_audio_devout_test_flags(core_audio_devout, AGS_CORE_AUDIO_DEVOUT_BUFFER1)){
      nth_buffer = 3;
    }else if(ags_core_audio_devout_test_flags(core_audio_devout, AGS_CORE_AUDIO_DEVOUT_BUFFER2)){
      nth_buffer = 4;
    }else if(ags_core_audio_devout_test_flags(core_audio_devout, AGS_CORE_AUDIO_DEVOUT_BUFFER3)){
      nth_buffer = 5;
    }else if(ags_core_audio_devout_test_flags(core_audio_devout, AGS_CORE_AUDIO_DEVOUT_BUFFER4)){
      nth_buffer = 6;
    }else if(ags_core_audio_devout_test_flags(core_audio_devout, AGS_CORE_AUDIO_DEVOUT_BUFFER5)){
      nth_buffer = 7;
    }else if(ags_core_audio_devout_test_flags(core_audio_devout, AGS_CORE_AUDIO_DEVOUT_BUFFER6)){
      nth_buffer = 0;
    }else if(ags_core_audio_devout_test_flags(core_audio_devout, AGS_CORE_AUDIO_DEVOUT_BUFFER7)){
      nth_buffer = 1;
    }
      
    ags_soundcard_lock_buffer(AGS_SOUNDCARD(clear_buffer->device), core_audio_devout->buffer[nth_buffer]);
    
    memset(core_audio_devout->buffer[nth_buffer], 0, (size_t) pcm_channels * buffer_size * word_size);

    ags_soundcard_unlock_buffer(AGS_SOUNDCARD(clear_buffer->device), core_audio_devout->buffer[nth_buffer]);
  }else if(AGS_IS_CORE_AUDIO_DEVIN(clear_buffer->device)){
    AgsCoreAudioDevin *core_audio_devin;
    
    core_audio_devin = (AgsCoreAudioDevin *) clear_buffer->device;

    /* retrieve nth buffer */    
    if(ags_core_audio_devin_test_flags(core_audio_devin, AGS_CORE_AUDIO_DEVIN_BUFFER0)){
      nth_buffer = 2;
    }else if(ags_core_audio_devin_test_flags(core_audio_devin, AGS_CORE_AUDIO_DEVIN_BUFFER1)){
      nth_buffer = 3;
    }else if(ags_core_audio_devin_test_flags(core_audio_devin, AGS_CORE_AUDIO_DEVIN_BUFFER2)){
      nth_buffer = 4;
    }else if(ags_core_audio_devin_test_flags(core_audio_devin, AGS_CORE_AUDIO_DEVIN_BUFFER3)){
      nth_buffer = 5;
    }else if(ags_core_audio_devin_test_flags(core_audio_devin, AGS_CORE_AUDIO_DEVIN_BUFFER4)){
      nth_buffer = 6;
    }else if(ags_core_audio_devin_test_flags(core_audio_devin, AGS_CORE_AUDIO_DEVIN_BUFFER5)){
      nth_buffer = 7;
    }else if(ags_core_audio_devin_test_flags(core_audio_devin, AGS_CORE_AUDIO_DEVIN_BUFFER6)){
      nth_buffer = 0;
    }else if(ags_core_audio_devin_test_flags(core_audio_devin, AGS_CORE_AUDIO_DEVIN_BUFFER7)){
      nth_buffer = 1;
    }
      
    ags_soundcard_lock_buffer(AGS_SOUNDCARD(clear_buffer->device), core_audio_devin->buffer[nth_buffer]);
    
    memset(core_audio_devin->buffer[nth_buffer], 0, (size_t) pcm_channels * buffer_size * word_size);

    ags_soundcard_unlock_buffer(AGS_SOUNDCARD(clear_buffer->device), core_audio_devin->buffer[nth_buffer]);
  }else if(AGS_IS_JACK_MIDIIN(clear_buffer->device)){
    //TODO:JK: implement me
  }else if(AGS_IS_CORE_AUDIO_MIDIIN(clear_buffer->device)){
    //TODO:JK: implement me
  }
}

/**
 * ags_clear_buffer_new:
 * @device: the #AgsSoundcard or #AgsSequencer
 *
 * Create a new instance of #AgsClearBuffer.
 *
 * Returns: the new #AgsClearBuffer.
 *
 * Since: 3.0.0
 */
AgsClearBuffer*
ags_clear_buffer_new(GObject *device)
{
  AgsClearBuffer *clear_buffer;

  clear_buffer = (AgsClearBuffer *) g_object_new(AGS_TYPE_CLEAR_BUFFER,
						 "device", device,
						 NULL);

  return(clear_buffer);
}
