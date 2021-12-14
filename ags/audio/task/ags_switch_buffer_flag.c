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

#include <ags/audio/task/ags_switch_buffer_flag.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_devin.h>
#include <ags/audio/ags_midiin.h>

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

void ags_switch_buffer_flag_class_init(AgsSwitchBufferFlagClass *switch_buffer_flag);
void ags_switch_buffer_flag_init(AgsSwitchBufferFlag *switch_buffer_flag);
void ags_switch_buffer_flag_set_property(GObject *gobject,
					 guint prop_id,
					 const GValue *value,
					 GParamSpec *param_spec);
void ags_switch_buffer_flag_get_property(GObject *gobject,
					 guint prop_id,
					 GValue *value,
					 GParamSpec *param_spec);
void ags_switch_buffer_flag_dispose(GObject *gobject);
void ags_switch_buffer_flag_finalize(GObject *gobject);

void ags_switch_buffer_flag_launch(AgsTask *task);

/**
 * SECTION:ags_switch_buffer_flag
 * @short_description: switch buffer flag of device
 * @title: AgsSwitchBufferFlag
 * @section_id:
 * @include: ags/audio/task/ags_switch_buffer_flag.h
 *
 * The #AgsSwitchBufferFlag task switches the buffer flag of device.
 */

static gpointer ags_switch_buffer_flag_parent_class = NULL;

enum{
  PROP_0,
  PROP_DEVICE,
};

GType
ags_switch_buffer_flag_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_switch_buffer_flag = 0;

    static const GTypeInfo ags_switch_buffer_flag_info = {
      sizeof(AgsSwitchBufferFlagClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_switch_buffer_flag_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsSwitchBufferFlag),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_switch_buffer_flag_init,
    };

    ags_type_switch_buffer_flag = g_type_register_static(AGS_TYPE_TASK,
							 "AgsSwitchBufferFlag",
							 &ags_switch_buffer_flag_info,
							 0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_switch_buffer_flag);
  }

  return g_define_type_id__volatile;
}

void
ags_switch_buffer_flag_class_init(AgsSwitchBufferFlagClass *switch_buffer_flag)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  GParamSpec *param_spec;

  ags_switch_buffer_flag_parent_class = g_type_class_peek_parent(switch_buffer_flag);

  /* gobject */
  gobject = (GObjectClass *) switch_buffer_flag;

  gobject->set_property = ags_switch_buffer_flag_set_property;
  gobject->get_property = ags_switch_buffer_flag_get_property;

  gobject->dispose = ags_switch_buffer_flag_dispose;
  gobject->finalize = ags_switch_buffer_flag_finalize;

  /* properties */
  /**
   * AgsSwitchBufferFlag:device:
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
  task = (AgsTaskClass *) switch_buffer_flag;

  task->launch = ags_switch_buffer_flag_launch;
}

void
ags_switch_buffer_flag_init(AgsSwitchBufferFlag *switch_buffer_flag)
{
  switch_buffer_flag->device = NULL;
}

void
ags_switch_buffer_flag_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec)
{
  AgsSwitchBufferFlag *switch_buffer_flag;

  switch_buffer_flag = AGS_SWITCH_BUFFER_FLAG(gobject);

  switch(prop_id){
  case PROP_DEVICE:
    {
      GObject *device;

      device = (GObject *) g_value_get_object(value);

      if(switch_buffer_flag->device == (GObject *) device){
	return;
      }

      if(switch_buffer_flag->device != NULL){
	g_object_unref(switch_buffer_flag->device);
      }

      if(device != NULL){
	g_object_ref(device);
      }

      switch_buffer_flag->device = (GObject *) device;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_switch_buffer_flag_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec)
{
  AgsSwitchBufferFlag *switch_buffer_flag;

  switch_buffer_flag = AGS_SWITCH_BUFFER_FLAG(gobject);

  switch(prop_id){
  case PROP_DEVICE:
    {
      g_value_set_object(value, switch_buffer_flag->device);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_switch_buffer_flag_dispose(GObject *gobject)
{
  AgsSwitchBufferFlag *switch_buffer_flag;

  switch_buffer_flag = AGS_SWITCH_BUFFER_FLAG(gobject);

  if(switch_buffer_flag->device != NULL){
    g_object_unref(switch_buffer_flag->device);

    switch_buffer_flag->device = NULL;
  }
  
  /* call parent*/
  G_OBJECT_CLASS(ags_switch_buffer_flag_parent_class)->dispose(gobject);
}

void
ags_switch_buffer_flag_finalize(GObject *gobject)
{
  AgsSwitchBufferFlag *switch_buffer_flag;

  switch_buffer_flag = AGS_SWITCH_BUFFER_FLAG(gobject);

  if(switch_buffer_flag->device != NULL){
    g_object_unref(switch_buffer_flag->device);
  }
  
  /* call parent*/
  G_OBJECT_CLASS(ags_switch_buffer_flag_parent_class)->finalize(gobject);
}

void
ags_switch_buffer_flag_launch(AgsTask *task)
{
  AgsSwitchBufferFlag *switch_buffer_flag;

  switch_buffer_flag = AGS_SWITCH_BUFFER_FLAG(task);

  /* switch buffer flag */
  if(AGS_IS_DEVOUT(switch_buffer_flag->device)){
    ags_devout_switch_buffer_flag((AgsDevout *) switch_buffer_flag->device);
  }else if(AGS_IS_DEVIN(switch_buffer_flag->device)){
    ags_devin_switch_buffer_flag((AgsDevin *) switch_buffer_flag->device);
  }else if(AGS_IS_ALSA_DEVOUT(switch_buffer_flag->device)){
    ags_alsa_devout_switch_buffer((AgsAlsaDevout *) switch_buffer_flag->device);
  }else if(AGS_IS_ALSA_DEVIN(switch_buffer_flag->device)){
    ags_alsa_devin_switch_buffer_flag((AgsAlsaDevin *) switch_buffer_flag->device);
  }else if(AGS_IS_OSS_DEVOUT(switch_buffer_flag->device)){
    ags_oss_devout_switch_buffer_flag((AgsOssDevout *) switch_buffer_flag->device);
  }else if(AGS_IS_OSS_DEVIN(switch_buffer_flag->device)){
    ags_oss_devin_switch_buffer_flag((AgsOssDevin *) switch_buffer_flag->device);
  }else if(AGS_IS_JACK_DEVOUT(switch_buffer_flag->device)){
    ags_jack_devout_switch_buffer_flag((AgsJackDevout *) switch_buffer_flag->device);
  }else if(AGS_IS_JACK_DEVIN(switch_buffer_flag->device)){
    ags_jack_devin_switch_buffer_flag((AgsJackDevin *) switch_buffer_flag->device);
  }else if(AGS_IS_PULSE_DEVOUT(switch_buffer_flag->device)){
    ags_pulse_devout_switch_buffer_flag((AgsPulseDevout *) switch_buffer_flag->device);
  }else if(AGS_IS_PULSE_DEVIN(switch_buffer_flag->device)){
    ags_pulse_devin_switch_buffer_flag((AgsPulseDevin *) switch_buffer_flag->device);
  }else if(AGS_IS_WASAPI_DEVOUT(switch_buffer_flag->device)){
    ags_wasapi_devout_switch_buffer_flag((AgsWasapiDevout *) switch_buffer_flag->device);
  }else if(AGS_IS_WASAPI_DEVIN(switch_buffer_flag->device)){
    ags_wasapi_devin_switch_buffer_flag((AgsWasapiDevin *) switch_buffer_flag->device);
  }else if(AGS_IS_CORE_AUDIO_DEVOUT(switch_buffer_flag->device)){
    ags_core_audio_devout_switch_buffer_flag((AgsCoreAudioDevout *) switch_buffer_flag->device);
  }else if(AGS_IS_CORE_AUDIO_DEVIN(switch_buffer_flag->device)){
    ags_core_audio_devin_switch_buffer_flag((AgsCoreAudioDevin *) switch_buffer_flag->device);
  }else if(AGS_IS_MIDIIN(switch_buffer_flag->device)){
    ags_midiin_switch_buffer_flag((AgsMidiin *) switch_buffer_flag->device);
  }else if(AGS_IS_ALSA_MIDIIN(switch_buffer_flag->device)){
    ags_alsa_midiin_switch_buffer_flag((AgsAlsaMidiin *) switch_buffer_flag->device);
  }else if(AGS_IS_OSS_MIDIIN(switch_buffer_flag->device)){
    ags_oss_midiin_switch_buffer_flag((AgsOssMidiin *) switch_buffer_flag->device);
  }else if(AGS_IS_JACK_MIDIIN(switch_buffer_flag->device)){
    ags_jack_midiin_switch_buffer_flag((AgsJackMidiin *) switch_buffer_flag->device);
  }else if(AGS_IS_CORE_AUDIO_MIDIIN(switch_buffer_flag->device)){
    ags_core_audio_midiin_switch_buffer_flag((AgsCoreAudioMidiin *) switch_buffer_flag->device);
  }
}

/**
 * ags_switch_buffer_flag_new:
 * @device: the #AgsSoundcard or #AgsSequencer
 *
 * Creates an #AgsSwitchBufferFlag.
 *
 * Returns: an new #AgsSwitchBufferFlag.
 *
 * Since: 3.0.0
 */
AgsSwitchBufferFlag*
ags_switch_buffer_flag_new(GObject *device)
{
  AgsSwitchBufferFlag *switch_buffer_flag;

  switch_buffer_flag = (AgsSwitchBufferFlag *) g_object_new(AGS_TYPE_SWITCH_BUFFER_FLAG,
							    "device", device,
							    NULL);

  return(switch_buffer_flag);
}
