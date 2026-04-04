/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2026 Joël Krähemann
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

#ifndef __AGS_FRAME_CLOCK_H__
#define __AGS_FRAME_CLOCK_H__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define AGS_TYPE_FRAME_CLOCK                (ags_frame_clock_get_type())
#define AGS_TYPE_FRAME_CLOCK_FLAGS          (ags_frame_clock_flags_get_type())
#define AGS_FRAME_CLOCK(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FRAME_CLOCK, AgsFrameClock))
#define AGS_FRAME_CLOCK_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_FRAME_CLOCK, AgsFrameClockClass))
#define AGS_IS_FRAME_CLOCK(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_FRAME_CLOCK))
#define AGS_IS_FRAME_CLOCK_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_FRAME_CLOCK))
#define AGS_FRAME_CLOCK_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_FRAME_CLOCK, AgsFrameClockClass))

#define AGS_FRAME_CLOCK_GET_OBJ_MUTEX(obj) (&(((AgsFrameClock *) obj)->obj_mutex))

#define AGS_FRAME_CLOCK_DEFAULT_SYNC_POINT_ARRAY_LENGTH (2048.0)
#define AGS_FRAME_CLOCK_DEFAULT_SYNC_POINT_ARRAY_LIST_LENGTH (4.0)
#define AGS_FRAME_CLOCK_DEFAULT_PERIOD (AGS_FRAME_CLOCK_DEFAULT_SYNC_POINT_ARRAY_LIST_LENGTH * AGS_FRAME_CLOCK_DEFAULT_SYNC_POINT_ARRAY_LENGTH)
#define AGS_FRAME_CLOCK_DEFAULT_PERIOD_256TH (16.0 * AGS_FRAME_CLOCK_DEFAULT_SYNC_POINT_ARRAY_LIST_LENGTH * AGS_FRAME_CLOCK_DEFAULT_SYNC_POINT_ARRAY_LENGTH)

typedef struct _AgsFrameClock AgsFrameClock;
typedef struct _AgsFrameClockClass AgsFrameClockClass;

/**
 * AgsFrameClockFlags:
 * @AGS_FRAME_CLOCK_STARTED: frame clock has started
 * @AGS_FRAME_CLOCK_RUNNING: frame clock is running
 * 
 * Enum values to control the behavior or indicate internal state of #AgsFrameClock by
 * enable/disable as sync_flags.
 */
typedef enum{
  AGS_FRAME_CLOCK_STARTED      = 1,
  AGS_FRAME_CLOCK_RUNNING      = 1 <<  1,
}AgsFrameClockFlags;

struct _AgsFrameClock
{
  GObject gobject;

  AgsFrameClockFlags flags;

  GRecMutex obj_mutex;

  guint buffer_size;
  guint samplerate;
  
  gdouble bpm;

  long double absolute_delay;
  long double fixed_absolute_delay;

  guint64 absolute_frame_offset;
  
  guint64 frame_offset;

  guint64 period_frame_offset;
  
  gboolean do_loop;
  
  guint64 loop_left;
  guint64 loop_right;

  gboolean has_16th_pulse;
  
  guint64 absolute_note_offset;
  
  guint64 note_offset;

  guint64 note_frame_offset;
  
  gboolean has_256th_pulse;
  
  guint64 absolute_note_256th_offset[16];
  guint absolute_note_256th_offset_length;

  guint64 note_256th_offset[16];
  guint note_256th_offset_length;

  guint64 note_256th_frame_offset[16];
  guint note_256th_frame_offset_length;
};

struct _AgsFrameClockClass
{
  GObjectClass gobject;
};

GType ags_frame_clock_get_type(void);
GType ags_frame_clock_flags_get_type(void);

/* flags */
gboolean ags_frame_clock_test_flags(AgsFrameClock *frame_clock,
				    AgsFrameClockFlags flags);
void ags_frame_clock_set_flags(AgsFrameClock *frame_clock,
			       AgsFrameClockFlags flags);
void ags_frame_clock_unset_flags(AgsFrameClock *frame_clock,
				 AgsFrameClockFlags flags);

/* getter/setter */
guint ags_frame_clock_get_buffer_size(AgsFrameClock *frame_clock);
void ags_frame_clock_set_buffer_size(AgsFrameClock *frame_clock,
				     guint buffer_size);

guint ags_frame_clock_get_samplerate(AgsFrameClock *frame_clock);
void ags_frame_clock_set_samplerate(AgsFrameClock *frame_clock,
				    guint samplerate);

gdouble ags_frame_clock_get_bpm(AgsFrameClock *frame_clock);
void ags_frame_clock_set_bpm(AgsFrameClock *frame_clock,
			     gdouble bpm);

guint64 ags_frame_clock_get_absolute_frame_offset(AgsFrameClock *frame_clock);

guint64 ags_frame_clock_get_frame_offset(AgsFrameClock *frame_clock);

gboolean ags_frame_clock_get_has_16th_pulse(AgsFrameClock *frame_clock);

gboolean ags_frame_clock_get_loop(AgsFrameClock *frame_clock,
				  guint64 *loop_left,
				  guint64 *loop_right);
void ags_frame_clock_set_loop(AgsFrameClock *frame_clock,
			      gboolean do_loop,
			      guint64 loop_left,
			      guint64 loop_right);

guint64 ags_frame_clock_get_absolute_note_offset(AgsFrameClock *frame_clock);

guint64 ags_frame_clock_get_note_offset(AgsFrameClock *frame_clock);

guint64 ags_frame_clock_get_note_frame_offset(AgsFrameClock *frame_clock);

void ags_frame_clock_get_absolute_note_256th_offset(AgsFrameClock *frame_clock,
						    guint64 *absolute_note_256th_offset,
						    guint *length);

void ags_frame_clock_get_note_256th_offset(AgsFrameClock *frame_clock,
					   guint64 *note_256th_offset,
					   guint *length);

void ags_frame_clock_get_note_256th_frame_offset(AgsFrameClock *frame_clock,
						 guint64 *note_256th_frame_offset,
						 guint *length);

/*  */
void ags_frame_clock_start(AgsFrameClock *frame_clock);
void ags_frame_clock_stop(AgsFrameClock *frame_clock);
  
void ags_frame_clock_increment_counter(AgsFrameClock *frame_clock);

/* from/to string */
void ags_frame_clock_from_string(AgsFrameClock *frame_clock,
				 gchar *str);
gchar* ags_frame_clock_to_string(AgsFrameClock *frame_clock);

/*  */
gchar* ags_frame_clock_to_time_string(AgsFrameClock *frame_clock,
				      gint number_of_digits);

/* singleton */
AgsFrameClock* ags_frame_clock_get_instance();

/* instantiate */
AgsFrameClock* ags_frame_clock_new();

G_END_DECLS

#endif /*__AGS_FRAME_CLOCK_H__*/
