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

#ifndef __AGS_FX_NOTATION_AUDIO_SIGNAL_H__
#define __AGS_FX_NOTATION_AUDIO_SIGNAL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_recall_audio_signal.h>

G_BEGIN_DECLS

#define AGS_TYPE_FX_NOTATION_AUDIO_SIGNAL                (ags_fx_notation_audio_signal_get_type())
#define AGS_FX_NOTATION_AUDIO_SIGNAL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FX_NOTATION_AUDIO_SIGNAL, AgsFxNotationAudioSignal))
#define AGS_FX_NOTATION_AUDIO_SIGNAL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_FX_NOTATION_AUDIO_SIGNAL, AgsFxNotationAudioSignalClass))
#define AGS_IS_FX_NOTATION_AUDIO_SIGNAL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_FX_NOTATION_AUDIO_SIGNAL))
#define AGS_IS_FX_NOTATION_AUDIO_SIGNAL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_FX_NOTATION_AUDIO_SIGNAL))
#define AGS_FX_NOTATION_AUDIO_SIGNAL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_FX_NOTATION_AUDIO_SIGNAL, AgsFxNotationAudioSignalClass))

typedef struct _AgsFxNotationAudioSignal AgsFxNotationAudioSignal;
typedef struct _AgsFxNotationAudioSignalClass AgsFxNotationAudioSignalClass;

struct _AgsFxNotationAudioSignal
{
  AgsRecallAudioSignal recall_audio_signal;
};

struct _AgsFxNotationAudioSignalClass
{
  AgsRecallAudioSignalClass recall_audio_signal;

  void (*stream_feed)(AgsFxNotationAudioSignal *fx_notation_audio_signal,
		      AgsAudioSignal *source,
		      AgsNote *note,
		      gboolean pattern_mode,
		      guint x0, guint x1,
		      guint y,
		      gdouble delay_counter, guint64 offset_counter,
		      guint frame_count,
		      gdouble delay, guint buffer_size);

  void (*notify_remove)(AgsFxNotationAudioSignal *fx_notation_audio_signal,
			AgsAudioSignal *source,
			AgsNote *note,
			guint x0, guint x1,
			guint y);
};

GType ags_fx_notation_audio_signal_get_type();

void ags_fx_notation_audio_signal_stream_feed(AgsFxNotationAudioSignal *fx_notation_audio_signal,
					      AgsAudioSignal *source,
					      AgsNote *note,
					      gboolean pattern_mode,
					      guint x0, guint x1,
					      guint y,
					      gdouble delay_counter, guint64 offset_counter,
					      guint frame_count,
					      gdouble delay, guint buffer_size);

void ags_fx_notation_audio_signal_notify_remove(AgsFxNotationAudioSignal *fx_notation_audio_signal,
						AgsAudioSignal *source,
						AgsNote *note,
						guint x0, guint x1,
						guint y);

/*  */
AgsFxNotationAudioSignal* ags_fx_notation_audio_signal_new(AgsAudioSignal *audio_signal);

G_END_DECLS

#endif /*__AGS_FX_NOTATION_AUDIO_SIGNAL_H__*/
