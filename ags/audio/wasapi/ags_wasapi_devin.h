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

#ifndef __AGS_WASAPI_DEVIN_H__
#define __AGS_WASAPI_DEVIN_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/ags_api_config.h>

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

G_BEGIN_DECLS

#define AGS_TYPE_WASAPI_DEVIN                (ags_wasapi_devin_get_type())
#define AGS_WASAPI_DEVIN(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_WASAPI_DEVIN, AgsWasapiDevin))
#define AGS_WASAPI_DEVIN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_WASAPI_DEVIN, AgsWasapiDevin))
#define AGS_IS_WASAPI_DEVIN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_WASAPI_DEVIN))
#define AGS_IS_WASAPI_DEVIN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_WASAPI_DEVIN))
#define AGS_WASAPI_DEVIN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_WASAPI_DEVIN, AgsWasapiDevinClass))

#define AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(obj) (&(((AgsWasapiDevin *) obj)->obj_mutex))

#define AGS_WASAPI_DEVIN_DEFAULT_WASAPI_BUFFER_SIZE (8192)

typedef struct _AgsWasapiDevin AgsWasapiDevin;
typedef struct _AgsWasapiDevinClass AgsWasapiDevinClass;

/**
 * AgsWasapiDevinFlags:
 * @AGS_WASAPI_DEVIN_ADDED_TO_REGISTRY: the core-audio devin was added to registry, see #AgsConnectable::add_to_registry()
 * @AGS_WASAPI_DEVIN_CONNECTED: indicates the core-audio devin was connected by calling #AgsConnectable::connect()
 * @AGS_WASAPI_DEVIN_BUFFER0: ring-buffer 0
 * @AGS_WASAPI_DEVIN_BUFFER1: ring-buffer 1
 * @AGS_WASAPI_DEVIN_BUFFER2: ring-buffer 2
 * @AGS_WASAPI_DEVIN_BUFFER3: ring-buffer 3
 * @AGS_WASAPI_DEVIN_BUFFER4: ring-buffer 4
 * @AGS_WASAPI_DEVIN_BUFFER5: ring-buffer 5
 * @AGS_WASAPI_DEVIN_BUFFER6: ring-buffer 6
 * @AGS_WASAPI_DEVIN_BUFFER7: ring-buffer 7
 * @AGS_WASAPI_DEVIN_ATTACK_FIRST: use first attack, instead of second one
 * @AGS_WASAPI_DEVIN_RECORD: do capture
 * @AGS_WASAPI_DEVIN_SHUTDOWN: stop capture
 * @AGS_WASAPI_DEVIN_START_RECORD: capture starting
 * @AGS_WASAPI_DEVIN_NONBLOCKING: do non-blocking calls
 * @AGS_WASAPI_DEVIN_INITIALIZED: the soundcard was initialized
 * @AGS_WASAPI_DEVIN_SHARE_MODE_EXCLUSIVE: share mode exclusive
 *
 * Enum values to control the behavior or indicate internal state of #AgsWasapiDevin by
 * enable/disable as flags.
 */
typedef enum{
  AGS_WASAPI_DEVIN_ADDED_TO_REGISTRY              = 1,
  AGS_WASAPI_DEVIN_CONNECTED                      = 1 <<  1,

  AGS_WASAPI_DEVIN_BUFFER0                        = 1 <<  2,
  AGS_WASAPI_DEVIN_BUFFER1                        = 1 <<  3,
  AGS_WASAPI_DEVIN_BUFFER2                        = 1 <<  4,
  AGS_WASAPI_DEVIN_BUFFER3                        = 1 <<  5,
  AGS_WASAPI_DEVIN_BUFFER4                        = 1 <<  6,
  AGS_WASAPI_DEVIN_BUFFER5                        = 1 <<  7,
  AGS_WASAPI_DEVIN_BUFFER6                        = 1 <<  8,
  AGS_WASAPI_DEVIN_BUFFER7                        = 1 <<  9,

  AGS_WASAPI_DEVIN_ATTACK_FIRST                   = 1 << 10,

  AGS_WASAPI_DEVIN_RECORD                         = 1 << 11,
  AGS_WASAPI_DEVIN_SHUTDOWN                       = 1 << 12,
  AGS_WASAPI_DEVIN_START_RECORD                   = 1 << 13,

  AGS_WASAPI_DEVIN_NONBLOCKING                    = 1 << 14,
  AGS_WASAPI_DEVIN_INITIALIZED                    = 1 << 15,

  AGS_WASAPI_DEVIN_SHARE_MODE_EXCLUSIVE           = 1 << 16,
}AgsWasapiDevinFlags;

#define AGS_WASAPI_DEVIN_ERROR (ags_wasapi_devin_error_quark())

typedef enum{
  AGS_WASAPI_DEVIN_ERROR_LOCKED_SOUNDCARD,
  AGS_WASAPI_DEVIN_ERROR_BROKEN_CONFIGURATION,
}AgsWasapiDevinError;

struct _AgsWasapiDevin
{
  GObject gobject;

  guint flags;
  
  GRecMutex obj_mutex;

  AgsUUID *uuid;

  guint dsp_channels;
  guint pcm_channels;
  guint format;
  guint buffer_size;
  guint samplerate;

  GRecMutex **buffer_mutex;
  void** buffer;

  guint wasapi_buffer_size;

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
  
  GMutex callback_mutex;
  GCond callback_cond;

  GMutex callback_finish_mutex;
  GCond callback_finish_cond;

  GObject *notify_soundcard;
};

struct _AgsWasapiDevinClass
{
  GObjectClass gobject;
};

GType ags_wasapi_devin_get_type();

GQuark ags_wasapi_devin_error_quark();

gboolean ags_wasapi_devin_test_flags(AgsWasapiDevin *wasapi_devin, guint flags);
void ags_wasapi_devin_set_flags(AgsWasapiDevin *wasapi_devin, guint flags);
void ags_wasapi_devin_unset_flags(AgsWasapiDevin *wasapi_devin, guint flags);

void ags_wasapi_devin_switch_buffer_flag(AgsWasapiDevin *wasapi_devin);

void ags_wasapi_devin_adjust_delay_and_attack(AgsWasapiDevin *wasapi_devin);
void ags_wasapi_devin_realloc_buffer(AgsWasapiDevin *wasapi_devin);

AgsWasapiDevin* ags_wasapi_devin_new();

G_END_DECLS

#endif /*__AGS_WASAPI_DEVIN_H__*/
