/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2014 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __AGS_BUFFER_AUDIO_SIGNAL_H__
#define __AGS_BUFFER_AUDIO_SIGNAL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_recall_audio_signal.h>

#define AGS_TYPE_BUFFER_AUDIO_SIGNAL                (ags_buffer_audio_signal_get_type())
#define AGS_BUFFER_AUDIO_SIGNAL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_BUFFER_AUDIO_SIGNAL, AgsBufferAudioSignal))
#define AGS_BUFFER_AUDIO_SIGNAL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_BUFFER_AUDIO_SIGNAL, AgsBufferAudioSignalClass))
#define AGS_IS_BUFFER_AUDIO_SIGNAL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_BUFFER_AUDIO_SIGNAL))
#define AGS_IS_BUFFER_AUDIO_SIGNAL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_BUFFER_AUDIO_SIGNAL))
#define AGS_BUFFER_AUDIO_SIGNAL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_BUFFER_AUDIO_SIGNAL, AgsBufferAudioSignalClass))

typedef struct _AgsBufferAudioSignal AgsBufferAudioSignal;
typedef struct _AgsBufferAudioSignalClass AgsBufferAudioSignalClass;

struct _AgsBufferAudioSignal
{
  AgsRecallAudioSignal recall_audio_signal;
};

struct _AgsBufferAudioSignalClass
{
  AgsRecallAudioSignalClass recall_audio_signal;
};

GType ags_buffer_audio_signal_get_type();

AgsBufferAudioSignal* ags_buffer_audio_signal_new(AgsAudioSignal *audio_signal);

#endif /*__AGS_BUFFER_AUDIO_SIGNAL_H__*/
