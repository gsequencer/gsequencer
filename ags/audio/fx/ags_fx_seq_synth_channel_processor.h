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

#ifndef __AGS_FX_SEQ_SYNTH_CHANNEL_PROCESSOR_H__
#define __AGS_FX_SEQ_SYNTH_CHANNEL_PROCESSOR_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_channel.h>

#include <ags/audio/fx/ags_fx_notation_channel_processor.h>

G_BEGIN_DECLS

#define AGS_TYPE_FX_SEQ_SYNTH_CHANNEL_PROCESSOR                (ags_fx_seq_synth_channel_processor_get_type())
#define AGS_FX_SEQ_SYNTH_CHANNEL_PROCESSOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FX_SEQ_SYNTH_CHANNEL_PROCESSOR, AgsFxSeqSynthChannelProcessor))
#define AGS_FX_SEQ_SYNTH_CHANNEL_PROCESSOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_FX_SEQ_SYNTH_CHANNEL_PROCESSOR, AgsFxSeqSynthChannelProcessorClass))
#define AGS_IS_FX_SEQ_SYNTH_CHANNEL_PROCESSOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_FX_SEQ_SYNTH_CHANNEL_PROCESSOR))
#define AGS_IS_FX_SEQ_SYNTH_CHANNEL_PROCESSOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_FX_SEQ_SYNTH_CHANNEL_PROCESSOR))
#define AGS_FX_SEQ_SYNTH_CHANNEL_PROCESSOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_FX_SEQ_SYNTH_CHANNEL_PROCESSOR, AgsFxSeqSynthChannelProcessorClass))

typedef struct _AgsFxSeqSynthChannelProcessor AgsFxSeqSynthChannelProcessor;
typedef struct _AgsFxSeqSynthChannelProcessorClass AgsFxSeqSynthChannelProcessorClass;

struct _AgsFxSeqSynthChannelProcessor
{
  AgsFxNotationChannelProcessor fx_notation_channel_processor;
};

struct _AgsFxSeqSynthChannelProcessorClass
{
  AgsFxNotationChannelProcessorClass fx_notation_channel_processor;
};

GType ags_fx_seq_synth_channel_processor_get_type();

/*  */
AgsFxSeqSynthChannelProcessor* ags_fx_seq_synth_channel_processor_new(AgsChannel *channel);

G_END_DECLS

#endif /*__AGS_FX_SEQ_SYNTH_CHANNEL_PROCESSOR_H__*/
