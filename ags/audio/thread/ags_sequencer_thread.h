/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
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

#ifndef __AGS_SEQUENCER_THREAD_H__
#define __AGS_SEQUENCER_THREAD_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/object/ags_soundcard.h>

#ifdef AGS_USE_LINUX_THREADS
#include <ags/thread/ags_thread-kthreads.h>
#else
#include <ags/thread/ags_thread-posix.h>
#endif 

#define AGS_TYPE_SEQUENCER_THREAD                (ags_sequencer_thread_get_type())
#define AGS_SEQUENCER_THREAD(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SEQUENCER_THREAD, AgsSequencerThread))
#define AGS_SEQUENCER_THREAD_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_SEQUENCER_THREAD, AgsSequencerThreadClass))
#define AGS_IS_SEQUENCER_THREAD(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_SEQUENCER_THREAD))
#define AGS_IS_SEQUENCER_THREAD_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_SEQUENCER_THREAD))
#define AGS_SEQUENCER_THREAD_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_SEQUENCER_THREAD, AgsSequencerThreadClass))

#define AGS_SEQUENCER_THREAD_DEFAULT_JIFFIE (ceil(AGS_SOUNDCARD_DEFAULT_SAMPLERATE / AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE) + AGS_SOUNDCARD_DEFAULT_OVERCLOCK)

typedef struct _AgsSequencerThread AgsSequencerThread;
typedef struct _AgsSequencerThreadClass AgsSequencerThreadClass;

struct _AgsSequencerThread
{
  AgsThread thread;

  time_t time_val;

  GObject *sequencer;
  
  AgsThread *timestamp_thread;

  GError *error;
};

struct _AgsSequencerThreadClass
{
  AgsThreadClass thread;

  void (*interval_timeout)(AgsSequencerThread *sequencer_thread);
};

GType ags_sequencer_thread_get_type();

void ags_sequencer_thread_interval_timeout(AgsSequencerThread *sequencer_thread);

AgsSequencerThread* ags_sequencer_thread_new(GObject *sequencer);

#endif /*__AGS_SEQUENCER_THREAD_H__*/
