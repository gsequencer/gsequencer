/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#ifndef __AGS_FX_TREMOLO_AUDIO_SIGNAL_H__
#define __AGS_FX_TREMOLO_AUDIO_SIGNAL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_recall_audio_signal.h>
#include <ags/audio/ags_tremolo_util.h>

G_BEGIN_DECLS

#define AGS_TYPE_FX_TREMOLO_AUDIO_SIGNAL                (ags_fx_tremolo_audio_signal_get_type())
#define AGS_FX_TREMOLO_AUDIO_SIGNAL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FX_TREMOLO_AUDIO_SIGNAL, AgsFxTremoloAudioSignal))
#define AGS_FX_TREMOLO_AUDIO_SIGNAL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_FX_TREMOLO_AUDIO_SIGNAL, AgsFxTremoloAudioSignalClass))
#define AGS_IS_FX_TREMOLO_AUDIO_SIGNAL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_FX_TREMOLO_AUDIO_SIGNAL))
#define AGS_IS_FX_TREMOLO_AUDIO_SIGNAL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_FX_TREMOLO_AUDIO_SIGNAL))
#define AGS_FX_TREMOLO_AUDIO_SIGNAL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_FX_TREMOLO_AUDIO_SIGNAL, AgsFxTremoloAudioSignalClass))

typedef struct _AgsFxTremoloAudioSignal AgsFxTremoloAudioSignal;
typedef struct _AgsFxTremoloAudioSignalClass AgsFxTremoloAudioSignalClass;

struct _AgsFxTremoloAudioSignal
{
  AgsRecallAudioSignal recall_audio_signal;

  AgsTremoloUtil tremolo_util;
};

struct _AgsFxTremoloAudioSignalClass
{
  AgsRecallAudioSignalClass recall_audio_signal;
};

GType ags_fx_tremolo_audio_signal_get_type();

/*  */
AgsFxTremoloAudioSignal* ags_fx_tremolo_audio_signal_new(AgsAudioSignal *audio_signal);

G_END_DECLS

#endif /*__AGS_FX_TREMOLO_AUDIO_SIGNAL_H__*/
