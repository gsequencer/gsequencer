/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#ifndef __AGS_FX_ANALYSE_CHANNEL_PROCESSOR_H__
#define __AGS_FX_ANALYSE_CHANNEL_PROCESSOR_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recall_channel_run.h>
#include <ags/audio/ags_audio_buffer_util.h>

G_BEGIN_DECLS

#define AGS_TYPE_FX_ANALYSE_CHANNEL_PROCESSOR                (ags_fx_analyse_channel_processor_get_type())
#define AGS_FX_ANALYSE_CHANNEL_PROCESSOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FX_ANALYSE_CHANNEL_PROCESSOR, AgsFxAnalyseChannelProcessor))
#define AGS_FX_ANALYSE_CHANNEL_PROCESSOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_FX_ANALYSE_CHANNEL_PROCESSOR, AgsFxAnalyseChannelProcessorClass))
#define AGS_IS_FX_ANALYSE_CHANNEL_PROCESSOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_FX_ANALYSE_CHANNEL_PROCESSOR))
#define AGS_IS_FX_ANALYSE_CHANNEL_PROCESSOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_FX_ANALYSE_CHANNEL_PROCESSOR))
#define AGS_FX_ANALYSE_CHANNEL_PROCESSOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_FX_ANALYSE_CHANNEL_PROCESSOR, AgsFxAnalyseChannelProcessorClass))

typedef struct _AgsFxAnalyseChannelProcessor AgsFxAnalyseChannelProcessor;
typedef struct _AgsFxAnalyseChannelProcessorClass AgsFxAnalyseChannelProcessorClass;

struct _AgsFxAnalyseChannelProcessor
{
  AgsRecallChannelRun recall_channel_run;

  AgsAudioBufferUtil audio_buffer_util;
};

struct _AgsFxAnalyseChannelProcessorClass
{
  AgsRecallChannelRunClass recall_channel_run;
};

GType ags_fx_analyse_channel_processor_get_type();

/*  */
AgsFxAnalyseChannelProcessor* ags_fx_analyse_channel_processor_new(AgsChannel *channel);

G_END_DECLS

#endif /*__AGS_FX_ANALYSE_CHANNEL_PROCESSOR_H__*/
