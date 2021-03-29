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

#ifndef __AGS_FX_TWO_PASS_ALIASE_CHANNEL_H__
#define __AGS_FX_TWO_PASS_ALIASE_CHANNEL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_sound_enums.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recall_channel.h>

G_BEGIN_DECLS

#define AGS_TYPE_FX_TWO_PASS_ALIASE_CHANNEL                (ags_fx_two_pass_aliase_channel_get_type())
#define AGS_FX_TWO_PASS_ALIASE_CHANNEL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FX_TWO_PASS_ALIASE_CHANNEL, AgsFxTwoPassAliaseChannel))
#define AGS_FX_TWO_PASS_ALIASE_CHANNEL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_FX_TWO_PASS_ALIASE_CHANNEL, AgsFxTwoPassAliaseChannelClass))
#define AGS_IS_FX_TWO_PASS_ALIASE_CHANNEL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_FX_TWO_PASS_ALIASE_CHANNEL))
#define AGS_IS_FX_TWO_PASS_ALIASE_CHANNEL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_FX_TWO_PASS_ALIASE_CHANNEL))
#define AGS_FX_TWO_PASS_ALIASE_CHANNEL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_FX_TWO_PASS_ALIASE_CHANNEL, AgsFxTwoPassAliaseChannelClass))

#define AGS_FX_TWO_PASS_ALIASE_CHANNEL_INPUT_DATA(ptr) ((AgsFxTwoPassAliaseChannelInputData *)(ptr))
#define AGS_FX_TWO_PASS_ALIASE_CHANNEL_INPUT_DATA_GET_STRCT_MUTEX(ptr) (&(((AgsFxTwoPassAliaseChannelInputData *)(ptr))->strct_mutex))

typedef struct _AgsFxTwoPassAliaseChannel AgsFxTwoPassAliaseChannel;
typedef struct _AgsFxTwoPassAliaseChannelInputData AgsFxTwoPassAliaseChannelInputData;
typedef struct _AgsFxTwoPassAliaseChannelClass AgsFxTwoPassAliaseChannelClass;

struct _AgsFxTwoPassAliaseChannel
{
  AgsRecallChannel recall_channel;

  AgsFxTwoPassAliaseChannelInputData* input_data[AGS_SOUND_SCOPE_LAST];

  AgsPort *enabled;

  AgsPort *a_amount;
  AgsPort *a_phase;

  AgsPort *b_amount;
  AgsPort *b_phase;
};

struct _AgsFxTwoPassAliaseChannelClass
{
  AgsRecallChannelClass recall_channel;
};

struct _AgsFxTwoPassAliaseChannelInputData
{
  GRecMutex strct_mutex;

  gpointer parent;

  gdouble *orig_buffer;

  gdouble *a_buffer;
  gdouble *b_buffer;

  gdouble *a_mix;
  gdouble *b_mix;

  gdouble *final_mix;
};

GType ags_fx_two_pass_aliase_channel_get_type();

/* runtime */
AgsFxTwoPassAliaseChannelInputData* ags_fx_two_pass_aliase_channel_input_data_alloc();
void ags_fx_two_pass_aliase_channel_input_data_free(AgsFxTwoPassAliaseChannelInputData *input_data);

/* get/set AgsFxTwoPassAliaseChannelInputData */
GRecMutex* ags_fx_two_pass_aliase_channel_input_data_get_strct_mutex(AgsFxTwoPassAliaseChannelInputData *input_data);

gpointer ags_fx_two_pass_aliase_channel_input_get_parent(AgsFxTwoPassAliaseChannelInputData *input_data);

gpointer ags_fx_two_pass_aliase_channel_input_get_orig_buffer(AgsFxTwoPassAliaseChannelInputData *input_data);

gpointer ags_fx_two_pass_aliase_channel_input_get_a_buffer(AgsFxTwoPassAliaseChannelInputData *input_data);
gpointer ags_fx_two_pass_aliase_channel_input_get_b_buffer(AgsFxTwoPassAliaseChannelInputData *input_data);

gpointer ags_fx_two_pass_aliase_channel_input_get_a_mix(AgsFxTwoPassAliaseChannelInputData *input_data);
gpointer ags_fx_two_pass_aliase_channel_input_get_b_mix(AgsFxTwoPassAliaseChannelInputData *input_data);

gpointer ags_fx_two_pass_aliase_channel_input_get_final_mix(AgsFxTwoPassAliaseChannelInputData *input_data);

/* instantiate */
AgsFxTwoPassAliaseChannel* ags_fx_two_pass_aliase_channel_new(AgsChannel *channel);

G_END_DECLS

#endif /*__AGS_FX_TWO_PASS_ALIASE_CHANNEL_H__*/
