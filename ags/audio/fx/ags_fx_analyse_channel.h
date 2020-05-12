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

#ifndef __AGS_FX_ANALYSE_CHANNEL_H__
#define __AGS_FX_ANALYSE_CHANNEL_H__

#include <glib.h>
#include <glib-object.h>

#include <fftw3.h>

#include <ags/libags.h>

#include <ags/audio/ags_sound_enums.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recall_channel.h>

G_BEGIN_DECLS

#define AGS_TYPE_FX_ANALYSE_CHANNEL                (ags_fx_analyse_channel_get_type())
#define AGS_FX_ANALYSE_CHANNEL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FX_ANALYSE_CHANNEL, AgsFxAnalyseChannel))
#define AGS_FX_ANALYSE_CHANNEL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_FX_ANALYSE_CHANNEL, AgsFxAnalyseChannelClass))
#define AGS_IS_FX_ANALYSE_CHANNEL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_FX_ANALYSE_CHANNEL))
#define AGS_IS_FX_ANALYSE_CHANNEL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_FX_ANALYSE_CHANNEL))
#define AGS_FX_ANALYSE_CHANNEL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_FX_ANALYSE_CHANNEL, AgsFxAnalyseChannelClass))

#define AGS_FX_ANALYSE_CHANNEL_INPUT_DATA(ptr) ((AgsFxAnalyseChannelInputData *)(ptr))
#define AGS_FX_ANALYSE_CHANNEL_INPUT_DATA_GET_STRCT_MUTEX(ptr) (&(((AgsFxAnalyseChannelInputData *)(ptr))->strct_mutex))

typedef struct _AgsFxAnalyseChannel AgsFxAnalyseChannel;
typedef struct _AgsFxAnalyseChannelInputData AgsFxAnalyseChannelInputData;
typedef struct _AgsFxAnalyseChannelClass AgsFxAnalyseChannelClass;

struct _AgsFxAnalyseChannel
{
  AgsRecallChannel recall_channel;

  gboolean magnitude_cleared;
  
  AgsFxAnalyseChannelInputData* input_data[AGS_SOUND_SCOPE_LAST];

  AgsPort *frequency;
  AgsPort *magnitude;
};

struct _AgsFxAnalyseChannelClass
{
  AgsRecallChannelClass recall_channel;
};

struct _AgsFxAnalyseChannelInputData
{
  GRecMutex strct_mutex;
  
  gpointer parent;

  fftw_plan plan;
  fftw_complex *comout;

  double *in;
  double *out;
};

GType ags_fx_analyse_channel_get_type();

/* runtime */
AgsFxAnalyseChannelInputData* ags_fx_analyse_channel_input_data_alloc();
void ags_fx_analyse_channel_input_data_free(AgsFxAnalyseChannelInputData *input_data);

/* get/set AgsFxAnalyseChannelInputData */
gpointer ags_fx_analyse_channel_input_get_parent(AgsFxAnalyseChannelInputData *input_data);

gpointer ags_fx_analyse_channel_input_get_in(AgsFxAnalyseChannelInputData *input_data);
gpointer ags_fx_analyse_channel_input_get_out(AgsFxAnalyseChannelInputData *input_data);

/* get/set AgsFxAnalyseChannel */
AgsFxAnalyseChannelInputData* ags_fx_analyse_channel_get_input_data(AgsFxAnalyseChannel *fx_analyse_channel,
								    gint sound_scope);

/* instantiate */
AgsFxAnalyseChannel* ags_fx_analyse_channel_new(AgsChannel *channel);

G_END_DECLS

#endif /*__AGS_FX_ANALYSE_CHANNEL_H__*/
