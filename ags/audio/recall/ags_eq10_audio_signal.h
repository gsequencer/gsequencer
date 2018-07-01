/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#ifndef __AGS_EQ10_AUDIO_SIGNAL_H__
#define __AGS_EQ10_AUDIO_SIGNAL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_recall_audio_signal.h>

#define AGS_TYPE_EQ10_AUDIO_SIGNAL                (ags_eq10_audio_signal_get_type())
#define AGS_EQ10_AUDIO_SIGNAL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_EQ10_AUDIO_SIGNAL, AgsEq10AudioSignal))
#define AGS_EQ10_AUDIO_SIGNAL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_EQ10_AUDIO_SIGNAL, AgsEq10AudioSignalClass))
#define AGS_IS_EQ10_AUDIO_SIGNAL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_EQ10_AUDIO_SIGNAL))
#define AGS_IS_EQ10_AUDIO_SIGNAL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_EQ10_AUDIO_SIGNAL))
#define AGS_EQ10_AUDIO_SIGNAL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_EQ10_AUDIO_SIGNAL, AgsEq10AudioSignalClass))

#define AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE (8)

typedef struct _AgsEq10AudioSignal AgsEq10AudioSignal;
typedef struct _AgsEq10AudioSignalClass AgsEq10AudioSignalClass;

struct _AgsEq10AudioSignal
{
  AgsRecallAudioSignal recall_audio_signal;

  double cache_28hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE];
  double cache_56hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE];
  double cache_112hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE];
  double cache_224hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE];
  double cache_448hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE];
  double cache_896hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE];
  double cache_1792hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE];
  double cache_3584hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE];
  double cache_7168hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE];
  double cache_14336hz[AGS_EQ10_AUDIO_SIGNAL_CACHE_SIZE];

  double *output_buffer;
  double *input_buffer;
};

struct _AgsEq10AudioSignalClass
{
  AgsRecallAudioSignalClass recall_audio_signal;
};

GType ags_eq10_audio_signal_get_type();

AgsEq10AudioSignal* ags_eq10_audio_signal_new(AgsAudioSignal *audio_signal);

#endif /*__AGS_EQ10_AUDIO_SIGNAL_H__*/
