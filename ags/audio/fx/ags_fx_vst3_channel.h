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

#ifndef __AGS_FX_VST3_CHANNEL_H__
#define __AGS_FX_VST3_CHANNEL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>
#include <ags/libags-vst.h>

#include <alsa/seq_event.h>

#include <ags/plugin/ags_vst3_plugin.h>

#include <ags/audio/ags_port.h>
#include <ags/audio/ags_sound_enums.h>

#include <ags/audio/ags_channel.h>

#include <ags/audio/fx/ags_fx_notation_channel.h>

G_BEGIN_DECLS

#define AGS_TYPE_FX_VST3_CHANNEL                (ags_fx_vst3_channel_get_type())
#define AGS_FX_VST3_CHANNEL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FX_VST3_CHANNEL, AgsFxVst3Channel))
#define AGS_FX_VST3_CHANNEL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_FX_VST3_CHANNEL, AgsFxVst3ChannelClass))
#define AGS_IS_FX_VST3_CHANNEL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_FX_VST3_CHANNEL))
#define AGS_IS_FX_VST3_CHANNEL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_FX_VST3_CHANNEL))
#define AGS_FX_VST3_CHANNEL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_FX_VST3_CHANNEL, AgsFxVst3ChannelClass))

#define AGS_FX_VST3_CHANNEL_INPUT_DATA(ptr) ((AgsFxVst3ChannelInputData *)(ptr))
#define AGS_FX_VST3_CHANNEL_INPUT_DATA_GET_STRCT_MUTEX(ptr) (&(((AgsFxVst3ChannelInputData *)(ptr))->strct_mutex))

#define AGS_FX_VST3_CHANNEL_MAX_PARAMETER_CHANGES (1024)

typedef struct _AgsFxVst3Channel AgsFxVst3Channel;
typedef struct _AgsFxVst3ChannelInputData AgsFxVst3ChannelInputData;
typedef struct _AgsFxVst3ChannelClass AgsFxVst3ChannelClass;

struct _AgsFxVst3Channel
{
  AgsFxNotationChannel fx_notation_channel;

  guint output_port_count;
  guint *output_port;

  guint input_port_count;
  guint *input_port;

  struct{
    AgsVstParamID param_id;
    AgsVstParamValue param_value;
  }parameter_changes[AGS_FX_VST3_CHANNEL_MAX_PARAMETER_CHANGES];

  AgsFxVst3ChannelInputData* input_data[AGS_SOUND_SCOPE_LAST];

  AgsVst3Plugin *vst3_plugin;

  AgsPort **vst3_port;
};

struct _AgsFxVst3ChannelClass
{
  AgsFxNotationChannelClass fx_notation_channel;
};

struct _AgsFxVst3ChannelInputData
{
  GRecMutex strct_mutex;
  
  gpointer parent;

  float *output;
  float *input;
  
  AgsVstIComponent *icomponent;
  AgsVstIEditController *iedit_controller;
  AgsVstIAudioProcessor *iaudio_processor;

  AgsVstIEditControllerHostEditing *iedit_controller_host_editing;

  AgsVstIComponentHandler *icomponent_handler;
  
  AgsVstIComponentHandlerBeginEdit *begin_edit_callback;
  AgsVstIComponentHandlerPerformEdit *perform_edit_callback;
  AgsVstIComponentHandlerEndEdit *end_edit_callback;
  AgsVstIComponentHandlerRestartComponent *restart_component_callback;  

  AgsVstProcessData *process_data;
  AgsVstParameterChanges *input_parameter_changes;
};

GType ags_fx_vst3_channel_get_type();

/* runtime */
AgsFxVst3ChannelInputData* ags_fx_vst3_channel_input_data_alloc();
void ags_fx_vst3_channel_input_data_free(AgsFxVst3ChannelInputData *input_data);

/* load/unload */
void ags_fx_vst3_channel_load_plugin(AgsFxVst3Channel *fx_vst3_channel);
void ags_fx_vst3_channel_load_port(AgsFxVst3Channel *fx_vst3_channel);

/* instantiate */
AgsFxVst3Channel* ags_fx_vst3_channel_new(AgsChannel *channel);

G_END_DECLS

#endif /*__AGS_FX_VST3_CHANNEL_H__*/
