/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#ifndef __AGS_WASAPI_DEVOUT_H__
#define __AGS_WASAPI_DEVOUT_H__

#include <glib.h>
#include <glib-object.h>

#include <pthread.h>

#include <ags/config.h>

#ifdef AGS_WITH_WASAPI
#include <windows.h>
#include <tchar.h>
#include <math.h>
#include <float.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <avrt.h>
#include <functiondiscoverykeys_devpkey.h>
#include <winerror.h>
#endif

#include <ags/libags.h>

#define AGS_TYPE_WASAPI_DEVOUT                (ags_wasapi_devout_get_type())
#define AGS_WASAPI_DEVOUT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_WASAPI_DEVOUT, AgsWasapiDevout))
#define AGS_WASAPI_DEVOUT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_WASAPI_DEVOUT, AgsWasapiDevout))
#define AGS_IS_WASAPI_DEVOUT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_WASAPI_DEVOUT))
#define AGS_IS_WASAPI_DEVOUT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_WASAPI_DEVOUT))
#define AGS_WASAPI_DEVOUT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_WASAPI_DEVOUT, AgsWasapiDevoutClass))

#define AGS_WASAPI_DEVOUT_GET_OBJ_MUTEX(obj) (((AgsWasapiDevout *) obj)->obj_mutex)

typedef struct _AgsWasapiDevout AgsWasapiDevout;
typedef struct _AgsWasapiDevoutClass AgsWasapiDevoutClass;

/**
 * AgsWasapiDevoutFlags:
 * @AGS_WASAPI_DEVOUT_ADDED_TO_REGISTRY: the core-audio devout was added to registry, see #AgsConnectable::add_to_registry()
 * @AGS_WASAPI_DEVOUT_CONNECTED: indicates the core-audio devout was connected by calling #AgsConnectable::connect()
 * @AGS_WASAPI_DEVOUT_BUFFER0: ring-buffer 0
 * @AGS_WASAPI_DEVOUT_BUFFER1: ring-buffer 1
 * @AGS_WASAPI_DEVOUT_BUFFER2: ring-buffer 2
 * @AGS_WASAPI_DEVOUT_BUFFER3: ring-buffer 3
 * @AGS_WASAPI_DEVOUT_BUFFER4: ring-buffer 4
 * @AGS_WASAPI_DEVOUT_BUFFER5: ring-buffer 5
 * @AGS_WASAPI_DEVOUT_BUFFER6: ring-buffer 6
 * @AGS_WASAPI_DEVOUT_BUFFER7: ring-buffer 7
 * @AGS_WASAPI_DEVOUT_ATTACK_FIRST: use first attack, instead of second one
 * @AGS_WASAPI_DEVOUT_PLAY: do playback
 * @AGS_WASAPI_DEVOUT_SHUTDOWN: stop playback
 * @AGS_WASAPI_DEVOUT_START_PLAY: playback starting
 * @AGS_WASAPI_DEVOUT_NONBLOCKING: do non-blocking calls
 * @AGS_WASAPI_DEVOUT_INITIALIZED: the soundcard was initialized
 *
 * Enum values to control the behavior or indicate internal state of #AgsWasapiDevout by
 * enable/disable as flags.
 */
typedef enum{
	     AGS_WASAPI_DEVOUT_ADDED_TO_REGISTRY              = 1,
	     AGS_WASAPI_DEVOUT_CONNECTED                      = 1 <<  1,

	     AGS_WASAPI_DEVOUT_BUFFER0                        = 1 <<  2,
	     AGS_WASAPI_DEVOUT_BUFFER1                        = 1 <<  3,
	     AGS_WASAPI_DEVOUT_BUFFER2                        = 1 <<  4,
	     AGS_WASAPI_DEVOUT_BUFFER3                        = 1 <<  5,
	     AGS_WASAPI_DEVOUT_BUFFER4                        = 1 <<  6,
	     AGS_WASAPI_DEVOUT_BUFFER5                        = 1 <<  7,
	     AGS_WASAPI_DEVOUT_BUFFER6                        = 1 <<  8,
	     AGS_WASAPI_DEVOUT_BUFFER7                        = 1 <<  9,

	     AGS_WASAPI_DEVOUT_ATTACK_FIRST                   = 1 << 10,

	     AGS_WASAPI_DEVOUT_PLAY                           = 1 << 11,
	     AGS_WASAPI_DEVOUT_SHUTDOWN                       = 1 << 12,
	     AGS_WASAPI_DEVOUT_START_PLAY                     = 1 << 13,

	     AGS_WASAPI_DEVOUT_NONBLOCKING                    = 1 << 14,
	     AGS_WASAPI_DEVOUT_INITIALIZED                    = 1 << 15,
}AgsWasapiDevoutFlags;

#define AGS_WASAPI_DEVOUT_ERROR (ags_wasapi_devout_error_quark())

typedef enum{
	     AGS_WASAPI_DEVOUT_ERROR_LOCKED_SOUNDCARD,
	     AGS_WASAPI_DEVOUT_ERROR_BROKEN_CONFIGURATION,
}AgsWasapiDevoutError;

struct _AgsWasapiDevout
{
  GObject gobject;

  guint flags;
  
  pthread_mutex_t *obj_mutex;
  pthread_mutexattr_t *obj_mutexattr;

  AgsApplicationContext *application_context;

  AgsUUID *uuid;

  guint dsp_channels;
  guint pcm_channels;
  guint format;
  guint buffer_size;
  guint samplerate;

  pthread_mutex_t **buffer_mutex;

  guint sub_block_count;
  pthread_mutex_t **sub_block_mutex;

  void** buffer;

  gboolean use_cache;
  guint cache_buffer_size;

  guint current_cache;
  guint completed_cache;
  guint cache_offset;
  void **cache;

  double bpm; // beats per minute
  gdouble delay_factor;
  
  gdouble *delay; // count of tics within buffer size
  guint *attack; // where currently tic resides in the stream's offset, measured in 1/64 of bpm

  gdouble tact_counter;
  gdouble delay_counter; // next time attack changeing when delay_counter == delay
  guint tic_counter; // in the range of default period

  guint start_note_offset;
  guint note_offset;
  guint note_offset_absolute;
  
  guint loop_left;
  guint loop_right;
  gboolean do_loop;
  
  guint loop_offset;

  gchar *device;

#ifdef AGS_WITH_WASAPI
  IMMDevice *mm_device;
  IAudioClient *audio_client;
#else
  gpointer mm_device;
  gpointer audio_client;
#endif
  
  pthread_mutex_t *callback_mutex;
  pthread_cond_t *callback_cond;

  pthread_mutex_t *callback_finish_mutex;
  pthread_cond_t *callback_finish_cond;

  GObject *notify_soundcard;
};

struct _AgsWasapiDevoutClass
{
  GObjectClass gobject;
};

GType ags_wasapi_devout_get_type();

GQuark ags_wasapi_devout_error_quark();

pthread_mutex_t* ags_wasapi_devout_get_class_mutex();

gboolean ags_wasapi_devout_test_flags(AgsWasapiDevout *wasapi_devout, guint flags);
void ags_wasapi_devout_set_flags(AgsWasapiDevout *wasapi_devout, guint flags);
void ags_wasapi_devout_unset_flags(AgsWasapiDevout *wasapi_devout, guint flags);

void ags_wasapi_devout_switch_buffer_flag(AgsWasapiDevout *wasapi_devout);

void ags_wasapi_devout_adjust_delay_and_attack(AgsWasapiDevout *wasapi_devout);
void ags_wasapi_devout_realloc_buffer(AgsWasapiDevout *wasapi_devout);

AgsWasapiDevout* ags_wasapi_devout_new(AgsApplicationContext *application_context);

#endif /*__AGS_WASAPI_DEVOUT_H__*/
