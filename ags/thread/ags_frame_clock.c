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

#include <ags/thread/ags_frame_clock.h>

#include <ags/util/ags_soundcard_helper.h>

#include <ags/lib/ags_time.h>

#include <ags/object/ags_config.h>
#include <ags/object/ags_soundcard.h>

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>

#include <libxml/xlink.h>
#include <libxml/valid.h>
#include <libxml/xmlIO.h>
#include <libxml/xmlmemory.h>
#include <libxml/xmlsave.h>

#include <math.h>

#include <string.h>

void ags_frame_clock_class_init(AgsFrameClockClass *frame_clock);
void ags_frame_clock_init(AgsFrameClock *frame_clock);
void ags_frame_clock_finalize(GObject *gobject);

void ags_frame_clock_counter_init(AgsFrameClock *frame_clock);
void ags_frame_clock_counter_reset(AgsFrameClock *frame_clock);

/**
 * SECTION:ags_frame_clock
 * @short_description: frame_clock unix and alike
 * @title: AgsFrameClock
 * @section_id:
 * @include: ags/thread/ags_frame_clock.h
 *
 * #AgsFrameClock measure of time.
 */

static gpointer ags_frame_clock_parent_class = NULL;

GType
ags_frame_clock_get_type (void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_frame_clock = 0;

    static const GTypeInfo ags_frame_clock_info = {
      sizeof(AgsFrameClockClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_frame_clock_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsFrameClock),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_frame_clock_init,
    };

    ags_type_frame_clock = g_type_register_static(G_TYPE_OBJECT,
						  "AgsFrameClock",
						  &ags_frame_clock_info,
						  0);

    g_once_init_leave(&g_define_type_id__static, ags_type_frame_clock);
  }

  return(g_define_type_id__static);
}

GType
ags_frame_clock_flags_get_type()
{
  static gsize g_flags_type_id__static;

  if(g_once_init_enter(&g_flags_type_id__static)){
    static const GFlagsValue values[] = {
      { AGS_FRAME_CLOCK_STARTED, "AGS_FRAME_CLOCK_STARTED", "frame-clock-started" },
      { AGS_FRAME_CLOCK_RUNNING, "AGS_FRAME_CLOCK_RUNNING", "frame-clock-running" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsFrameClockFlags"), values);

    g_once_init_leave(&g_flags_type_id__static, g_flags_type_id);
  }
  
  return(g_flags_type_id__static);
}

void
ags_frame_clock_class_init(AgsFrameClockClass *frame_clock)
{
  GObjectClass *gobject;

  ags_frame_clock_parent_class = g_type_class_peek_parent(frame_clock);

  /* GObjectClass */
  gobject = (GObjectClass *) frame_clock;

  gobject->finalize = ags_frame_clock_finalize;
}

void
ags_frame_clock_init(AgsFrameClock *frame_clock)
{ 
  AgsConfig *config;
  
  config = ags_config_get_instance();
  
  frame_clock->flags = 0;

  /* add frame_clock mutex */
  g_rec_mutex_init(&(frame_clock->obj_mutex));

  /* common fields */
  frame_clock->buffer_size = ags_soundcard_helper_config_get_buffer_size(config);
  frame_clock->samplerate = ags_soundcard_helper_config_get_samplerate(config);
  
  frame_clock->bpm = AGS_SOUNDCARD_DEFAULT_BPM;

  frame_clock->absolute_delay = (60.0 * frame_clock->samplerate) / (4.0 * frame_clock->bpm) / frame_clock->buffer_size;
  
  frame_clock->fixed_absolute_delay = ((ceil((AGS_FRAME_CLOCK_DEFAULT_PERIOD * frame_clock->absolute_delay * frame_clock->buffer_size) / frame_clock->buffer_size) * frame_clock->buffer_size) / (AGS_FRAME_CLOCK_DEFAULT_PERIOD * frame_clock->absolute_delay * frame_clock->buffer_size)) * frame_clock->absolute_delay;

  /* loop */
  frame_clock->do_loop = FALSE;
  
  frame_clock->loop_left = 0;
  frame_clock->loop_right = 64;

  /* counter init */
  frame_clock->start_note_offset = 0;

  ags_frame_clock_counter_init(frame_clock);
}

void
ags_frame_clock_finalize(GObject *gobject)
{
  AgsFrameClock *frame_clock;

  frame_clock = AGS_FRAME_CLOCK(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_frame_clock_parent_class)->finalize(gobject);
}

void
ags_frame_clock_counter_init(AgsFrameClock *frame_clock)
{
  guint i;
  
  if(!AGS_IS_FRAME_CLOCK(frame_clock)){
    return;
  }

  frame_clock->delay_counter = 0.0;

  /* frame offset */
  frame_clock->absolute_frame_offset = 0;

  frame_clock->frame_offset = 0;

  frame_clock->period_frame_offset = 0;
  
  /* 16th pulse */
  frame_clock->has_16th_pulse = TRUE;

  frame_clock->absolute_note_offset = 0;

  frame_clock->note_offset = 0;

  frame_clock->note_frame_offset = 0;

  /* 256th pulse */
  frame_clock->has_256th_pulse = TRUE;

  /* absolute note 256th offset */
  memset(&(frame_clock->absolute_note_256th_offset[0]), 0, 16 * sizeof(guint64));

  frame_clock->absolute_note_256th_offset_length = (guint) floorl((long double) frame_clock->buffer_size / (frame_clock->absolute_delay / 16.0 * (long double) frame_clock->buffer_size)) + 1;

  for(i = 0; i < frame_clock->absolute_note_256th_offset_length && i < 16; i++){
    frame_clock->absolute_note_256th_offset[i] = (guint64) i;
  }
  
  /* note 256th offset */
  memset(&(frame_clock->note_256th_offset[0]), 0, 16 * sizeof(guint64));

  frame_clock->note_256th_offset_length = (guint) floorl((long double) frame_clock->buffer_size / (frame_clock->absolute_delay / 16.0 * (long double) frame_clock->buffer_size)) + 1;

  for(i = 0; i < frame_clock->note_256th_offset_length && i < 16; i++){
    frame_clock->note_256th_offset[i] = (guint64) i;
  }

  /* note 256th frame offset */
  memset(&(frame_clock->note_256th_frame_offset[0]), 0, 16 * sizeof(guint64));
 
  frame_clock->note_256th_frame_offset_length = (guint) floorl((long double) frame_clock->buffer_size / (frame_clock->absolute_delay / 16.0 * (long double) frame_clock->buffer_size)) + 1;

  for(i = 0; i < frame_clock->note_256th_frame_offset_length && i < 16; i++){
    frame_clock->note_256th_frame_offset[i] = (guint64) floorl((long double) i * (frame_clock->absolute_delay / 16.0 * (long double) frame_clock->buffer_size));
  }
}

void
ags_frame_clock_counter_reset(AgsFrameClock *frame_clock)
{
  guint i;
  
  if(!AGS_IS_FRAME_CLOCK(frame_clock)){
    return;
  }

  frame_clock->absolute_frame_offset = (guint64) ((floorl(((long double) frame_clock->absolute_note_offset * (frame_clock->fixed_absolute_delay * (long double) frame_clock->buffer_size)) / (long double) frame_clock->buffer_size) * (long double) frame_clock->buffer_size) + (frame_clock->delay_counter * (long double) frame_clock->buffer_size));

  frame_clock->frame_offset = (guint64) ((floorl(((long double) frame_clock->note_offset * (frame_clock->fixed_absolute_delay * (long double) frame_clock->buffer_size)) / (long double) frame_clock->buffer_size) * (long double) frame_clock->buffer_size) + (frame_clock->delay_counter * (long double) frame_clock->buffer_size));
  
  frame_clock->period_frame_offset = frame_clock->frame_offset % ((guint64) AGS_FRAME_CLOCK_DEFAULT_PERIOD * frame_clock->buffer_size);

  /* absolute note 256th offset */
  memset(&(frame_clock->note_256th_offset[0]), 0, 16 * sizeof(guint64));

  frame_clock->absolute_note_256th_offset_length = (guint) floorl((long double) frame_clock->buffer_size / (frame_clock->absolute_delay / 16.0 * (long double) frame_clock->buffer_size)) + 1;

  for(i = 0; i < frame_clock->absolute_note_256th_offset_length && i < 16; i++){
    frame_clock->absolute_note_256th_offset[i] = (guint64) floorl(frame_clock->absolute_frame_offset / (frame_clock->absolute_delay / 16.0 * (long double) frame_clock->buffer_size)) + i;
  }
  
  /* note 256th offset */
  memset(&(frame_clock->note_256th_offset[0]), 0, 16 * sizeof(guint64));

  frame_clock->note_256th_offset_length = (guint) floorl((long double) frame_clock->buffer_size / (frame_clock->absolute_delay / 16.0 * (long double) frame_clock->buffer_size)) + 1;

  for(i = 0; i < frame_clock->note_256th_offset_length && i < 16; i++){
    frame_clock->note_256th_offset[i] = (guint64) floorl(frame_clock->frame_offset / (frame_clock->absolute_delay / 16.0 * (long double) frame_clock->buffer_size)) + i;
  }

  /* note 256th frame offset */
  memset(&(frame_clock->note_256th_frame_offset[0]), 0, 16 * sizeof(guint64));
 
  frame_clock->note_256th_frame_offset_length = (guint) floorl((long double) frame_clock->buffer_size / (frame_clock->absolute_delay / 16.0 * (long double) frame_clock->buffer_size)) + 1;

  for(i = 0; i < frame_clock->note_256th_frame_offset_length && i < 16; i++){
    frame_clock->note_256th_frame_offset[i] = ((guint64) floorl(frame_clock->frame_offset / frame_clock->buffer_size) * (guint64) frame_clock->buffer_size) + (guint64) floorl((long double) i * (frame_clock->absolute_delay / 16.0 * (long double) frame_clock->buffer_size));
  }
}

/**
 * ags_frame_clock_test_flags:
 * @frame_clock: the #AgsFrameClock
 * @flags: the flags
 * 
 * Test @flags to be set.
 * 
 * Returns: if @flags set returning %TRUE otherwise %FALSE
 * 
 * Since: 9.0.0
 */
gboolean
ags_frame_clock_test_flags(AgsFrameClock *frame_clock,
			   AgsFrameClockFlags flags)
{
  gboolean success;
  
  GRecMutex *frame_clock_mutex;

  if(!AGS_IS_FRAME_CLOCK(frame_clock)){
    return(FALSE);
  }
  
  /* get frame clock mutex */
  frame_clock_mutex = AGS_FRAME_CLOCK_GET_OBJ_MUTEX(frame_clock);

  /* test flags */
  g_rec_mutex_lock(frame_clock_mutex);

  success = ((flags & (frame_clock->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(frame_clock_mutex);

  return(success);
}

/**
 * ags_frame_clock_set_flags:
 * @frame_clock: the #AgsFrameClock
 * @flags: the flags
 * 
 * Set @flags of @frame_clock.
 * 
 * Since: 9.0.0
 */
void
ags_frame_clock_set_flags(AgsFrameClock *frame_clock,
			  AgsFrameClockFlags flags)
{
  GRecMutex *frame_clock_mutex;

  if(!AGS_IS_FRAME_CLOCK(frame_clock)){
    return;
  }
  
  /* get frame clock mutex */
  frame_clock_mutex = AGS_FRAME_CLOCK_GET_OBJ_MUTEX(frame_clock);

  /* set flags */
  g_rec_mutex_lock(frame_clock_mutex);

  frame_clock->flags |= flags;

  g_rec_mutex_unlock(frame_clock_mutex);
}

/**
 * ags_frame_clock_unset_flags:
 * @frame_clock: the #AgsFrameClock
 * @flags: the flags
 * 
 * Unset @flags of @frame_clock.
 * 
 * Since: 9.0.0
 */
void
ags_frame_clock_unset_flags(AgsFrameClock *frame_clock,
			    AgsFrameClockFlags flags)
{
  GRecMutex *frame_clock_mutex;

  if(!AGS_IS_FRAME_CLOCK(frame_clock)){
    return;
  }
  
  /* get frame clock mutex */
  frame_clock_mutex = AGS_FRAME_CLOCK_GET_OBJ_MUTEX(frame_clock);

  /* unset flags */
  g_rec_mutex_lock(frame_clock_mutex);

  frame_clock->flags &= (~flags);

  g_rec_mutex_unlock(frame_clock_mutex);
}

/**
 * ags_frame_clock_get_buffer_size:
 * @frame_clock: the #AgsFrameClock
 *
 * Get buffer size of @frame_clock.
 * 
 * Returns: the buffer size
 * 
 * Since: 9.0.0
 */
guint
ags_frame_clock_get_buffer_size(AgsFrameClock *frame_clock)
{
  guint buffer_size;
  
  GRecMutex *frame_clock_mutex;

  if(!AGS_IS_FRAME_CLOCK(frame_clock)){
    return(0);
  }
  
  /* get frame clock mutex */
  frame_clock_mutex = AGS_FRAME_CLOCK_GET_OBJ_MUTEX(frame_clock);

  /* get buffer size */
  g_rec_mutex_lock(frame_clock_mutex);

  buffer_size = frame_clock->buffer_size;

  g_rec_mutex_unlock(frame_clock_mutex);

  return(buffer_size);
}

/**
 * ags_frame_clock_set_buffer_size:
 * @frame_clock: the #AgsFrameClock
 * @buffer_size: the buffer size
 *
 * Set buffer size of @frame_clock.
 * 
 * Since: 9.0.0
 */
void
ags_frame_clock_set_buffer_size(AgsFrameClock *frame_clock,
				guint buffer_size)
{
  GRecMutex *frame_clock_mutex;

  if(!AGS_IS_FRAME_CLOCK(frame_clock)){
    return;
  }
  
  /* get frame clock mutex */
  frame_clock_mutex = AGS_FRAME_CLOCK_GET_OBJ_MUTEX(frame_clock);

  /* set buffer size */
  g_rec_mutex_lock(frame_clock_mutex);

  frame_clock->buffer_size = buffer_size;

  frame_clock->absolute_delay = (60.0 * frame_clock->samplerate) / (4.0 * frame_clock->bpm) / frame_clock->buffer_size;
  
  frame_clock->fixed_absolute_delay = ((ceil((AGS_FRAME_CLOCK_DEFAULT_PERIOD * frame_clock->absolute_delay * frame_clock->buffer_size) / frame_clock->buffer_size) * frame_clock->buffer_size) / (AGS_FRAME_CLOCK_DEFAULT_PERIOD * frame_clock->absolute_delay * frame_clock->buffer_size)) * frame_clock->absolute_delay;
  
  ags_frame_clock_counter_reset(frame_clock);
  
  g_rec_mutex_unlock(frame_clock_mutex);
}

/**
 * ags_frame_clock_get_samplerate:
 * @frame_clock: the #AgsFrameClock
 *
 * Get samplerate of @frame_clock.
 * 
 * Returns: the samplerate
 * 
 * Since: 9.0.0
 */
guint
ags_frame_clock_get_samplerate(AgsFrameClock *frame_clock)
{
  guint samplerate;
  
  GRecMutex *frame_clock_mutex;

  if(!AGS_IS_FRAME_CLOCK(frame_clock)){
    return(0);
  }
  
  /* get frame clock mutex */
  frame_clock_mutex = AGS_FRAME_CLOCK_GET_OBJ_MUTEX(frame_clock);

  /* get samplerate */
  g_rec_mutex_lock(frame_clock_mutex);

  samplerate = frame_clock->samplerate;

  g_rec_mutex_unlock(frame_clock_mutex);

  return(samplerate);
}

/**
 * ags_frame_clock_set_samplerate:
 * @frame_clock: the #AgsFrameClock
 * @samplerate: the samplerate
 *
 * Set samplerate of @frame_clock.
 * 
 * Since: 9.0.0
 */
void
ags_frame_clock_set_samplerate(AgsFrameClock *frame_clock,
			       guint samplerate)
{ 
  GRecMutex *frame_clock_mutex;

  if(!AGS_IS_FRAME_CLOCK(frame_clock)){
    return;
  }
  
  /* get frame clock mutex */
  frame_clock_mutex = AGS_FRAME_CLOCK_GET_OBJ_MUTEX(frame_clock);

  /* set samplerate */
  g_rec_mutex_lock(frame_clock_mutex);

  frame_clock->samplerate = samplerate;

  frame_clock->absolute_delay = (60.0 * frame_clock->samplerate) / (4.0 * frame_clock->bpm) / frame_clock->buffer_size;

  frame_clock->fixed_absolute_delay = ((ceil((AGS_FRAME_CLOCK_DEFAULT_PERIOD * frame_clock->absolute_delay * frame_clock->buffer_size) / frame_clock->buffer_size) * frame_clock->buffer_size) / (AGS_FRAME_CLOCK_DEFAULT_PERIOD * frame_clock->absolute_delay * frame_clock->buffer_size)) * frame_clock->absolute_delay;

  ags_frame_clock_counter_reset(frame_clock);
  
  g_rec_mutex_unlock(frame_clock_mutex);
}

/**
 * ags_frame_clock_get_bpm:
 * @frame_clock: the #AgsFrameClock
 *
 * Get bpm of @frame_clock.
 * 
 * Returns: the bpm
 * 
 * Since: 9.0.0
 */
gdouble
ags_frame_clock_get_bpm(AgsFrameClock *frame_clock)
{
  gdouble bpm;
  
  GRecMutex *frame_clock_mutex;

  if(!AGS_IS_FRAME_CLOCK(frame_clock)){
    return(0.0);
  }
  
  /* get frame clock mutex */
  frame_clock_mutex = AGS_FRAME_CLOCK_GET_OBJ_MUTEX(frame_clock);

  /* get bpm */
  g_rec_mutex_lock(frame_clock_mutex);

  bpm = frame_clock->bpm;

  g_rec_mutex_unlock(frame_clock_mutex);

  return(bpm);
}

/**
 * ags_frame_clock_set_bpm:
 * @frame_clock: the #AgsFrameClock
 * @bpm: the bpm
 *
 * Set bpm of @frame_clock.
 * 
 * Since: 9.0.0
 */
void
ags_frame_clock_set_bpm(AgsFrameClock *frame_clock,
			gdouble bpm)
{
  guint i;
  
  GRecMutex *frame_clock_mutex;

  if(!AGS_IS_FRAME_CLOCK(frame_clock)){
    return;
  }
  
  /* get frame clock mutex */
  frame_clock_mutex = AGS_FRAME_CLOCK_GET_OBJ_MUTEX(frame_clock);

  /* set bpm */
  g_rec_mutex_lock(frame_clock_mutex);

  frame_clock->bpm = bpm;

  frame_clock->absolute_delay = (60.0 * frame_clock->samplerate) / (4.0 * frame_clock->bpm) / frame_clock->buffer_size;

  frame_clock->fixed_absolute_delay = ((ceil((AGS_FRAME_CLOCK_DEFAULT_PERIOD * frame_clock->absolute_delay * frame_clock->buffer_size) / frame_clock->buffer_size) * frame_clock->buffer_size) / (AGS_FRAME_CLOCK_DEFAULT_PERIOD * frame_clock->absolute_delay * frame_clock->buffer_size)) * frame_clock->absolute_delay;

  if(frame_clock->delay_counter >= frame_clock->absolute_delay){
    frame_clock->delay_counter = 0.0;
  }
  
  ags_frame_clock_counter_reset(frame_clock);
  
  g_rec_mutex_unlock(frame_clock_mutex);
}

/**
 * ags_frame_clock_get_absolute_frame_offset:
 * @frame_clock: the #AgsFrameClock
 *
 * Get absolute frame offset of @frame_clock.
 * 
 * Returns: the absolute frame offset
 * 
 * Since: 9.0.0
 */
guint64
ags_frame_clock_get_absolute_frame_offset(AgsFrameClock *frame_clock)
{
  guint absolute_frame_offset;
  
  GRecMutex *frame_clock_mutex;

  if(!AGS_IS_FRAME_CLOCK(frame_clock)){
    return(0);
  }
  
  /* get frame clock mutex */
  frame_clock_mutex = AGS_FRAME_CLOCK_GET_OBJ_MUTEX(frame_clock);

  /* get absolute frame offset */
  g_rec_mutex_lock(frame_clock_mutex);

  absolute_frame_offset = frame_clock->absolute_frame_offset;

  g_rec_mutex_unlock(frame_clock_mutex);

  return(absolute_frame_offset);
}

/**
 * ags_frame_clock_get_frame_offset:
 * @frame_clock: the #AgsFrameClock
 *
 * Get frame offset of @frame_clock.
 * 
 * Returns: the frame offset
 * 
 * Since: 9.0.0
 */
guint64
ags_frame_clock_get_frame_offset(AgsFrameClock *frame_clock)
{
  guint64 frame_offset;
  
  GRecMutex *frame_clock_mutex;

  if(!AGS_IS_FRAME_CLOCK(frame_clock)){
    return(0);
  }
  
  /* get frame clock mutex */
  frame_clock_mutex = AGS_FRAME_CLOCK_GET_OBJ_MUTEX(frame_clock);

  /* get frame offset */
  g_rec_mutex_lock(frame_clock_mutex);

  frame_offset = frame_clock->frame_offset;

  g_rec_mutex_unlock(frame_clock_mutex);

  return(frame_offset);
}

/**
 * ags_frame_clock_get_note_frame_offset:
 * @frame_clock: the #AgsFrameClock
 *
 * Get note frame offset of @frame_clock.
 * 
 * Returns: the note frame offset
 * 
 * Since: 9.0.0
 */
guint64
ags_frame_clock_get_note_frame_offset(AgsFrameClock *frame_clock)
{
  guint64 note_frame_offset;
  
  GRecMutex *frame_clock_mutex;

  if(!AGS_IS_FRAME_CLOCK(frame_clock)){
    return(0);
  }
  
  /* get frame clock mutex */
  frame_clock_mutex = AGS_FRAME_CLOCK_GET_OBJ_MUTEX(frame_clock);

  /* get note frame offset */
  g_rec_mutex_lock(frame_clock_mutex);

  note_frame_offset = frame_clock->note_frame_offset;

  g_rec_mutex_unlock(frame_clock_mutex);

  return(note_frame_offset);
}

/**
 * ags_frame_clock_get_has_16th_pulse:
 * @frame_clock: the #AgsFrameClock
 *
 * Get has 16th pulse of @frame_clock.
 * 
 * Returns: %TRUE if has 16th pulse, otherwise %FALSE
 * 
 * Since: 9.0.0
 */
gboolean
ags_frame_clock_get_has_16th_pulse(AgsFrameClock *frame_clock)
{
  gboolean has_16th_pulse;
  
  GRecMutex *frame_clock_mutex;

  if(!AGS_IS_FRAME_CLOCK(frame_clock)){
    return(FALSE);
  }
  
  /* get frame clock mutex */
  frame_clock_mutex = AGS_FRAME_CLOCK_GET_OBJ_MUTEX(frame_clock);

  /* get has 16th pulse */
  g_rec_mutex_lock(frame_clock_mutex);

  has_16th_pulse = frame_clock->has_16th_pulse;

  g_rec_mutex_unlock(frame_clock_mutex);

  return(has_16th_pulse);
}

/**
 * ags_frame_clock_get_has_256th_pulse:
 * @frame_clock: the #AgsFrameClock
 *
 * Get has 256th pulse of @frame_clock.
 * 
 * Returns: %TRUE if has 256th pulse, otherwise %FALSE
 * 
 * Since: 9.0.0
 */
gboolean
ags_frame_clock_get_has_256th_pulse(AgsFrameClock *frame_clock)
{
  gboolean has_256th_pulse;
  
  GRecMutex *frame_clock_mutex;

  if(!AGS_IS_FRAME_CLOCK(frame_clock)){
    return(FALSE);
  }
  
  /* get frame clock mutex */
  frame_clock_mutex = AGS_FRAME_CLOCK_GET_OBJ_MUTEX(frame_clock);

  /* get has 256th pulse */
  g_rec_mutex_lock(frame_clock_mutex);

  has_256th_pulse = frame_clock->has_256th_pulse;

  g_rec_mutex_unlock(frame_clock_mutex);

  return(has_256th_pulse);
}

/**
 * ags_frame_clock_get_loop:
 * @frame_clock: the #AgsFrameClock
 * @loop_left: (out): return location of loop left
 * @loop_right: (out): return location of loop right
 *
 * Get loop of @frame_clock.
 * 
 * Returns: %TRUE if do loop, otherwise %FALSE
 * 
 * Since: 9.0.0
 */
gboolean
ags_frame_clock_get_loop(AgsFrameClock *frame_clock,
			 guint64 *loop_left,
			 guint64 *loop_right)
{
  gboolean do_loop;
  
  GRecMutex *frame_clock_mutex;

  if(!AGS_IS_FRAME_CLOCK(frame_clock)){
    return(FALSE);
  }
  
  /* get frame clock mutex */
  frame_clock_mutex = AGS_FRAME_CLOCK_GET_OBJ_MUTEX(frame_clock);

  /* get loop */
  g_rec_mutex_lock(frame_clock_mutex);

  do_loop = frame_clock->do_loop;

  if(loop_left != NULL){
    loop_left[0] = frame_clock->loop_left;
  }

  if(loop_right != NULL){
    loop_right[0] = frame_clock->loop_right;
  }

  g_rec_mutex_unlock(frame_clock_mutex);

  return(do_loop);
}

/**
 * ags_frame_clock_set_loop:
 * @frame_clock: the #AgsFrameClock
 * @do_loop: do loop if %TRUE, otherwise %FALSE
 * @loop_left: loop left
 * @loop_right: loop right
 *
 * Get loop of @frame_clock.
 * 
 * Returns: %TRUE if do loop, otherwise %FALSE
 * 
 * Since: 9.0.0
 */
void
ags_frame_clock_set_loop(AgsFrameClock *frame_clock,
			 gboolean do_loop,
			 guint64 loop_left,
			 guint64 loop_right)
{
  GRecMutex *frame_clock_mutex;

  if(!AGS_IS_FRAME_CLOCK(frame_clock)){
    return;
  }
  
  /* get frame clock mutex */
  frame_clock_mutex = AGS_FRAME_CLOCK_GET_OBJ_MUTEX(frame_clock);

  /* set loop */
  g_rec_mutex_lock(frame_clock_mutex);
  
  frame_clock->do_loop = do_loop;

  frame_clock->loop_left = loop_left;
  frame_clock->loop_right = loop_right;
    
  g_rec_mutex_unlock(frame_clock_mutex);
}

/**
 * ags_frame_clock_get_start_note_offset:
 * @frame_clock: the #AgsFrameClock
 *
 * Get start note offset of @frame_clock.
 * 
 * Returns: the start note offset
 * 
 * Since: 9.0.0
 */
guint64
ags_frame_clock_get_start_note_offset(AgsFrameClock *frame_clock)
{
  guint64 start_note_offset;
  
  GRecMutex *frame_clock_mutex;

  if(!AGS_IS_FRAME_CLOCK(frame_clock)){
    return(0);
  }
  
  /* get frame clock mutex */
  frame_clock_mutex = AGS_FRAME_CLOCK_GET_OBJ_MUTEX(frame_clock);

  /* get note offset */
  g_rec_mutex_lock(frame_clock_mutex);

  start_note_offset = frame_clock->start_note_offset;

  g_rec_mutex_unlock(frame_clock_mutex);

  return(start_note_offset);
}

/**
 * ags_frame_clock_set_start_note_offset:
 * @frame_clock: the #AgsFrameClock
 * @start_note_offset: the start note offset
 *
 * Set start note offset of @frame_clock.
 * 
 * Since: 9.0.0
 */
void
ags_frame_clock_set_start_note_offset(AgsFrameClock *frame_clock,
				      guint64 start_note_offset)
{
  GRecMutex *frame_clock_mutex;

  if(!AGS_IS_FRAME_CLOCK(frame_clock)){
    return;
  }
  
  /* get frame clock mutex */
  frame_clock_mutex = AGS_FRAME_CLOCK_GET_OBJ_MUTEX(frame_clock);

  /* set note offset */
  g_rec_mutex_lock(frame_clock_mutex);

  frame_clock->start_note_offset = start_note_offset;

  if(frame_clock->note_offset < start_note_offset){
    ags_frame_clock_set_note_offset(frame_clock,
				    start_note_offset);
  }

  g_rec_mutex_unlock(frame_clock_mutex);
}

/**
 * ags_frame_clock_get_absolute_note_offset:
 * @frame_clock: the #AgsFrameClock
 *
 * Get absolute note offset of @frame_clock.
 * 
 * Returns: the absolute note offset
 * 
 * Since: 9.0.0
 */
guint64
ags_frame_clock_get_absolute_note_offset(AgsFrameClock *frame_clock)
{
  guint64 absolute_note_offset;
  
  GRecMutex *frame_clock_mutex;

  if(!AGS_IS_FRAME_CLOCK(frame_clock)){
    return(0);
  }
  
  /* get frame clock mutex */
  frame_clock_mutex = AGS_FRAME_CLOCK_GET_OBJ_MUTEX(frame_clock);

  /* get absolute note offset */
  g_rec_mutex_lock(frame_clock_mutex);

  absolute_note_offset = frame_clock->absolute_note_offset;

  g_rec_mutex_unlock(frame_clock_mutex);

  return(absolute_note_offset);
}

/**
 * ags_frame_clock_get_note_offset:
 * @frame_clock: the #AgsFrameClock
 *
 * Get note offset of @frame_clock.
 * 
 * Returns: the note offset
 * 
 * Since: 9.0.0
 */
guint64
ags_frame_clock_get_note_offset(AgsFrameClock *frame_clock)
{
  guint64 note_offset;
  
  GRecMutex *frame_clock_mutex;

  if(!AGS_IS_FRAME_CLOCK(frame_clock)){
    return(0);
  }
  
  /* get frame clock mutex */
  frame_clock_mutex = AGS_FRAME_CLOCK_GET_OBJ_MUTEX(frame_clock);

  /* get note offset */
  g_rec_mutex_lock(frame_clock_mutex);

  note_offset = frame_clock->note_offset;

  g_rec_mutex_unlock(frame_clock_mutex);

  return(note_offset);
}

/**
 * ags_frame_clock_set_note_offset:
 * @frame_clock: the #AgsFrameClock
 * @note_offset: the note offset
 *
 * Set note offset of @frame_clock.
 * 
 * Since: 9.0.0
 */
void
ags_frame_clock_set_note_offset(AgsFrameClock *frame_clock,
				guint64 note_offset)
{
  GRecMutex *frame_clock_mutex;

  if(!AGS_IS_FRAME_CLOCK(frame_clock)){
    return;
  }
  
  /* get frame clock mutex */
  frame_clock_mutex = AGS_FRAME_CLOCK_GET_OBJ_MUTEX(frame_clock);

  /* set note offset */
  g_rec_mutex_lock(frame_clock_mutex);

  frame_clock->delay_counter = 0.0;
  
  frame_clock->absolute_note_offset = note_offset;
  
  frame_clock->note_offset = note_offset;

  frame_clock->note_frame_offset = (guint64) floorl((long double) note_offset * (frame_clock->fixed_absolute_delay * (long double) frame_clock->buffer_size));
  
  ags_frame_clock_counter_reset(frame_clock);
  
  g_rec_mutex_unlock(frame_clock_mutex);
}

/**
 * ags_frame_clock_get_absolute_note_256th_offset:
 * @frame_clock: the #AgsFrameClock
 * @absolute_note_256th_offset: (out): return location of note 256th offset, maximum length 16
 * @length: (out): return location of length
 *
 * Get absolute note 256th offset of @frame_clock.
 * 
 * Since: 9.0.0
 */
void
ags_frame_clock_get_absolute_note_256th_offset(AgsFrameClock *frame_clock,
					       guint64 *absolute_note_256th_offset,
					       guint *length)
{
  guint i;
  
  GRecMutex *frame_clock_mutex;

  if(!AGS_IS_FRAME_CLOCK(frame_clock)){
    return;
  }
  
  /* get frame clock mutex */
  frame_clock_mutex = AGS_FRAME_CLOCK_GET_OBJ_MUTEX(frame_clock);

  /* get absolute note 256th offset */
  g_rec_mutex_lock(frame_clock_mutex);

  if(absolute_note_256th_offset != NULL){
    for(i = 0; i < frame_clock->absolute_note_256th_offset_length && i < 16; i++){
      absolute_note_256th_offset[i] = frame_clock->absolute_note_256th_offset[i];
    }
  }

  if(length != NULL){
    length[0] = frame_clock->absolute_note_256th_offset_length;
  }
  
  g_rec_mutex_unlock(frame_clock_mutex);
}

/**
 * ags_frame_clock_get_note_256th_offset:
 * @frame_clock: the #AgsFrameClock
 * @note_256th_offset: (out): return location of note 256th offset, maximum length 16
 * @length: (out): return location of length
 *
 * Get note 256th offset of @frame_clock.
 * 
 * Since: 9.0.0
 */
void
ags_frame_clock_get_note_256th_offset(AgsFrameClock *frame_clock,
				      guint64 *note_256th_offset,
				      guint *length)
{
  guint i;
  
  GRecMutex *frame_clock_mutex;

  if(!AGS_IS_FRAME_CLOCK(frame_clock)){
    return;
  }
  
  /* get frame clock mutex */
  frame_clock_mutex = AGS_FRAME_CLOCK_GET_OBJ_MUTEX(frame_clock);

  /* get absolute note 256th offset */
  g_rec_mutex_lock(frame_clock_mutex);

  if(note_256th_offset != NULL){
    for(i = 0; i < frame_clock->note_256th_offset_length && i < 16; i++){
      note_256th_offset[i] = frame_clock->note_256th_offset[i];
    }
  }

  if(length != NULL){
    length[0] = frame_clock->note_256th_offset_length;
  }
  
  g_rec_mutex_unlock(frame_clock_mutex);
}

/**
 * ags_frame_clock_get_note_256th_frame_offset:
 * @frame_clock: the #AgsFrameClock
 * @note_256th_frame_offset: (out): return location of note 256th frame offset
 * @length: (out): return location of array length, maximum 16 items
 *
 * Get note 256th frame offset of @frame_clock.
 * 
 * Since: 9.0.0
 */
void
ags_frame_clock_get_note_256th_frame_offset(AgsFrameClock *frame_clock,
					    guint64 *note_256th_frame_offset,
					    guint *length)
{
  guint i;
  
  GRecMutex *frame_clock_mutex;

  if(!AGS_IS_FRAME_CLOCK(frame_clock)){
    return;
  }
  
  /* get frame clock mutex */
  frame_clock_mutex = AGS_FRAME_CLOCK_GET_OBJ_MUTEX(frame_clock);

  /* get note 256th frame_offset */
  g_rec_mutex_lock(frame_clock_mutex);

  if(note_256th_frame_offset != NULL){
    for(i = 0; i < frame_clock->note_256th_frame_offset_length && i < 16; i++){
      note_256th_frame_offset[i] = frame_clock->note_256th_frame_offset[i];
    }
  }

  if(length != NULL){
    length[0] = frame_clock->note_256th_frame_offset_length;
  }
  
  g_rec_mutex_unlock(frame_clock_mutex);
}

/**
 * ags_frame_clock_copy_time:
 * @destination: the destination #AgsFrameClock
 * @source: the source #AgsFrameClock
 *
 * Copy time of @source to @destination.
 * 
 * Since: 9.0.0
 */
void
ags_frame_clock_copy_time(AgsFrameClock *destination,
			  AgsFrameClock *source)
{
  long double delay_counter;

  guint64 absolute_frame_offset;
  
  guint64 frame_offset;

  guint64 period_frame_offset;

  gboolean has_16th_pulse;

  guint64 start_note_offset;
  
  guint64 absolute_note_offset;
  
  guint64 note_offset;

  guint64 note_frame_offset;
  
  gboolean has_256th_pulse;
  
  guint64 absolute_note_256th_offset[16] = {0,};
  guint absolute_note_256th_offset_length;

  guint64 note_256th_offset[16] = {0,};
  guint note_256th_offset_length;

  guint64 note_256th_frame_offset[16] = {0,};
  guint note_256th_frame_offset_length;
  
  GRecMutex *destination_mutex;
  GRecMutex *source_mutex;
  
  if(!AGS_IS_FRAME_CLOCK(destination) ||
     !AGS_IS_FRAME_CLOCK(source)){
    return;
  }
  
  /* get frame clock mutex */
  destination_mutex = AGS_FRAME_CLOCK_GET_OBJ_MUTEX(destination);

  source_mutex = AGS_FRAME_CLOCK_GET_OBJ_MUTEX(source);

  /* copy local */
  g_rec_mutex_lock(source_mutex);
  
  delay_counter = source->delay_counter;
  
  absolute_frame_offset = source->absolute_frame_offset;
  
  frame_offset = source->frame_offset;
  
  period_frame_offset = source->period_frame_offset;

  has_16th_pulse = source->has_16th_pulse;
  
  start_note_offset = source->start_note_offset;
  
  absolute_note_offset = source->absolute_note_offset;
  
  note_offset = source->note_offset;
  
  note_frame_offset = source->note_frame_offset;
  
  has_256th_pulse = source->has_256th_pulse;
  
  memcpy(&(absolute_note_256th_offset[0]), &(source->absolute_note_256th_offset[0]), 16 * sizeof(guint64));
  
  absolute_note_256th_offset_length = source->absolute_note_256th_offset_length;
  
  memcpy(&(note_256th_offset[0]), &(source->note_256th_offset[0]), 16 * sizeof(guint64));
  
  note_256th_offset_length = source->note_256th_offset_length;
  
  memcpy(&(note_256th_frame_offset[0]), &(source->note_256th_frame_offset[0]), 16 * sizeof(guint64));
  
  note_256th_frame_offset_length = source->note_256th_frame_offset_length;
  
  g_rec_mutex_unlock(source_mutex);
  
  /* copy destination */
  g_rec_mutex_lock(destination_mutex);
  
  destination->delay_counter = delay_counter;
  
  destination->absolute_frame_offset = absolute_frame_offset;
  
  destination->frame_offset = frame_offset;
  
  destination->period_frame_offset = period_frame_offset;

  destination->has_16th_pulse = has_16th_pulse;
  
  destination->start_note_offset = start_note_offset;
  
  destination->absolute_note_offset = absolute_note_offset;
  
  destination->note_offset = note_offset;
  
  destination->note_frame_offset = note_frame_offset;
  
  destination->has_256th_pulse = has_256th_pulse;
  
  memcpy(&(destination->absolute_note_256th_offset[0]), &(absolute_note_256th_offset[0]), 16 * sizeof(guint64));
  
  destination->absolute_note_256th_offset_length = absolute_note_256th_offset_length;
  
  memcpy(&(destination->note_256th_offset[0]), &(note_256th_offset[0]), 16 * sizeof(guint64));
  
  destination->note_256th_offset_length = note_256th_offset_length;
  
  memcpy(&(destination->note_256th_frame_offset[0]), &(note_256th_frame_offset[0]), 16 * sizeof(guint64));
  
  destination->note_256th_frame_offset_length = note_256th_frame_offset_length;  

  g_rec_mutex_unlock(destination_mutex);
}

/**
 * ags_frame_clock_start:
 * @frame_clock: the #AgsFrameClock
 *
 * Start @frame_clock.
 * 
 * Since: 9.0.0
 */
void
ags_frame_clock_start(AgsFrameClock *frame_clock)
{
  guint64 start_note_offset;
  
  if(!AGS_IS_FRAME_CLOCK(frame_clock) ||
     ags_frame_clock_test_flags(frame_clock, AGS_FRAME_CLOCK_STARTED)){
    return;
  }

  /* start note offset */
  start_note_offset = ags_frame_clock_get_start_note_offset(frame_clock);
  
  ags_frame_clock_set_note_offset(frame_clock,
				  start_note_offset);
  
  ags_frame_clock_set_flags(frame_clock,
			    AGS_FRAME_CLOCK_STARTED);

  
  ags_frame_clock_set_flags(frame_clock,
			    AGS_FRAME_CLOCK_RUNNING);
}

/**
 * ags_frame_clock_stop:
 * @frame_clock: the #AgsFrameClock
 *
 * Stop @frame_clock.
 * 
 * Since: 9.0.0
 */
void
ags_frame_clock_stop(AgsFrameClock *frame_clock)
{
  guint64 start_note_offset;
  
  guint i;
  
  if(!AGS_IS_FRAME_CLOCK(frame_clock) ||
     !ags_frame_clock_test_flags(frame_clock, AGS_FRAME_CLOCK_STARTED)){
    return;
  }

  /* counter init */
  ags_frame_clock_counter_init(frame_clock);

  /* unset running */
  ags_frame_clock_unset_flags(frame_clock,
			      AGS_FRAME_CLOCK_RUNNING);
  
  ags_frame_clock_unset_flags(frame_clock,
			      AGS_FRAME_CLOCK_STARTED);
  
  /* start note offset */
  start_note_offset = ags_frame_clock_get_start_note_offset(frame_clock);
  
  ags_frame_clock_set_note_offset(frame_clock,
				  start_note_offset);
}

/**
 * ags_frame_clock_increment_counter:
 * @frame_clock: the #AgsFrameClock
 *
 * Increment counter of @frame_clock.
 * 
 * Since: 9.0.0
 */
void
ags_frame_clock_increment_counter(AgsFrameClock *frame_clock)
{
  guint64 absolute_note_256th_offset[16] = {0,};
  guint64 note_256th_frame_offset[16] = {0,};
  guint64 note_256th_offset[16] = {0,};
  
  guint buffer_size;
  gdouble bpm;
  long double fixed_absolute_delay;
  guint64 period_frame_offset;
  guint64 total_period_frame_offset;
  guint64 note_frame_offset;
  guint absolute_note_256th_offset_length;
  guint note_256th_frame_offset_length;
  guint note_256th_offset_length;
  guint64 last_absolute_note_256th_offset;
  guint64 last_note_256th_frame_offset;
  guint64 last_note_256th_offset;
  guint64 loop_left_note_256th_frame_offset;
  guint64 heading_note_256th_frame_offset;
  guint64 stop_note_256th_frame_offset;

  guint i, i_stop;
  guint j;
  gboolean do_loop;
  gboolean has_16th_pulse;
  gboolean has_256th_pulse;
  
  GRecMutex *frame_clock_mutex;
  
  if(!AGS_IS_FRAME_CLOCK(frame_clock)){
    return;
  }
  
  if(!ags_frame_clock_test_flags(frame_clock, AGS_FRAME_CLOCK_RUNNING)){
    g_message("frame clock not running");
    
    return;
  }
  
  /* get frame clock mutex */
  frame_clock_mutex = AGS_FRAME_CLOCK_GET_OBJ_MUTEX(frame_clock);

  /* increment counter */
  g_rec_mutex_lock(frame_clock_mutex);

  buffer_size = frame_clock->buffer_size;

  bpm = frame_clock->bpm;

  fixed_absolute_delay = frame_clock->fixed_absolute_delay;
  
  period_frame_offset = frame_clock->period_frame_offset;

  total_period_frame_offset = AGS_FRAME_CLOCK_DEFAULT_PERIOD * fixed_absolute_delay * buffer_size;

  total_period_frame_offset = (guint64) ceill((long double) total_period_frame_offset / (long double) buffer_size) * buffer_size;

  /* note frame offset */
  note_frame_offset = ags_frame_clock_get_note_frame_offset(frame_clock);

  /* absolute note 256th offset */
  ags_frame_clock_get_absolute_note_256th_offset(frame_clock,
						 &(absolute_note_256th_offset[0]),
						 &absolute_note_256th_offset_length);
  
  last_absolute_note_256th_offset = 0;
  
  if(absolute_note_256th_offset_length > 0){
    last_absolute_note_256th_offset = absolute_note_256th_offset[absolute_note_256th_offset_length - 1];
  }
  
  /* note 256th frame offset */
  note_256th_frame_offset_length = 0;
  
  ags_frame_clock_get_note_256th_frame_offset(frame_clock,
					      &(note_256th_frame_offset[0]),
					      &note_256th_frame_offset_length);
  
  last_note_256th_frame_offset = 0;
  
  if(note_256th_frame_offset_length > 0){
    last_note_256th_frame_offset = note_256th_frame_offset[note_256th_frame_offset_length - 1];
  }
  
  /* note 256th offset */
  note_256th_offset_length = 0;

  ags_frame_clock_get_note_256th_offset(frame_clock,
					&(note_256th_offset[0]),
					&note_256th_offset_length);
  
  last_note_256th_offset = 0;
  
  if(note_256th_offset_length > 0){
    last_note_256th_offset = note_256th_offset[note_256th_offset_length - 1];
  }

  /* increment frame offset */
  frame_clock->absolute_frame_offset += frame_clock->buffer_size;

  frame_clock->frame_offset += frame_clock->buffer_size;

  frame_clock->period_frame_offset += frame_clock->buffer_size;

  do_loop = FALSE;
  
  if(frame_clock->do_loop &&
     (long double) frame_clock->frame_offset + (long double) buffer_size >= (long double) frame_clock->loop_right * fixed_absolute_delay * (long double) buffer_size){
    do_loop = TRUE;

    frame_clock->frame_offset = (guint64) floorl(floorl((long double) frame_clock->loop_left * fixed_absolute_delay * (long double) buffer_size) / (long double) buffer_size) * buffer_size;

    frame_clock->period_frame_offset = frame_clock->frame_offset % total_period_frame_offset;
  }

  if(frame_clock->period_frame_offset >= total_period_frame_offset){
    frame_clock->period_frame_offset = frame_clock->period_frame_offset % total_period_frame_offset;
  }
  
  /* 16th pulse */
  has_16th_pulse = FALSE;

  if(do_loop){
    has_16th_pulse = TRUE;

    frame_clock->absolute_note_offset += 1;
    
    frame_clock->note_offset = frame_clock->loop_left;

    frame_clock->note_frame_offset = (frame_clock->loop_left % (guint64) AGS_FRAME_CLOCK_DEFAULT_PERIOD) * buffer_size;
  }else{
    if(((guint64) floor((double) note_frame_offset / (double) buffer_size) * buffer_size) + (guint64) (fixed_absolute_delay * (long double) buffer_size) < frame_clock->frame_offset + buffer_size){
      has_16th_pulse = TRUE;

      frame_clock->absolute_note_offset += 1;
      
      frame_clock->note_offset += 1;
    
      frame_clock->note_frame_offset += (fixed_absolute_delay * buffer_size);
    }
  }
  
  /* 256th pulse */
  has_256th_pulse = FALSE;
  
  if(do_loop){
    has_256th_pulse = TRUE;

    heading_note_256th_frame_offset = floorl((long double) frame_clock->loop_left * fixed_absolute_delay * (long double) buffer_size);

    if(fixed_absolute_delay / 16.0 <= 1.0){
      for(i = 0; ((i + 1) * (guint64) floorl(fixed_absolute_delay / 16.0 * (long double) buffer_size)) < buffer_size && i < 16; i++){
	frame_clock->absolute_note_256th_offset[i] = (16 * frame_clock->absolute_note_offset) + (guint64) (i + 1);
    
	frame_clock->note_256th_frame_offset[i] = (((16 * frame_clock->loop_left) + i) * (guint64) floorl(fixed_absolute_delay / 16.0 * (long double) buffer_size));
	  
	frame_clock->note_256th_offset[i] = (16 * frame_clock->loop_left) + (guint64) i;
      }

      frame_clock->absolute_note_256th_offset_length = i;

      frame_clock->note_256th_frame_offset_length = i;
	
      frame_clock->note_256th_offset_length = i;
    }else{
      frame_clock->note_256th_frame_offset[0] = (16 * frame_clock->absolute_note_offset);
      frame_clock->note_256th_frame_offset_length = 1;
      
      frame_clock->absolute_note_256th_offset[0] = 16 * frame_clock->loop_left;
      frame_clock->absolute_note_256th_offset_length = 1;
    
      frame_clock->note_256th_offset[0] = 16 * frame_clock->loop_left;
      frame_clock->note_256th_offset_length = 1;
    }
  }else{
    heading_note_256th_frame_offset = last_note_256th_frame_offset;
    
    if(fixed_absolute_delay / 16.0 <= 1.0){
      has_256th_pulse = TRUE;
    
      if(frame_clock->frame_offset == 0){
	for(i = 0, j = 1; (j * (guint64) floorl(fixed_absolute_delay / 16.0 * (long double) buffer_size)) < frame_clock->frame_offset + buffer_size && i < 16; i++, j++){
	  frame_clock->absolute_note_256th_offset[i] = last_absolute_note_256th_offset + (guint64) j;
    
	  frame_clock->note_256th_frame_offset[i] = heading_note_256th_frame_offset + (j * (guint64) floorl(fixed_absolute_delay / 16.0 * (long double) buffer_size));
	
	  frame_clock->note_256th_offset[i] = last_note_256th_offset + (guint64) j;
	}

	frame_clock->absolute_note_256th_offset_length = i;

	frame_clock->note_256th_frame_offset_length = i;
	
	frame_clock->note_256th_offset_length = i;
      }else{
	heading_note_256th_frame_offset = last_note_256th_frame_offset;

	for(i = 0, j = 1; heading_note_256th_frame_offset + (j * (guint64) floorl(fixed_absolute_delay / 16.0 * (long double) buffer_size)) < frame_clock->frame_offset + buffer_size && i < 16; i++, j++){
	  frame_clock->absolute_note_256th_offset[i] = last_absolute_note_256th_offset + j;
    
	  frame_clock->note_256th_frame_offset[i] = heading_note_256th_frame_offset + (j * (guint64) floorl(fixed_absolute_delay / 16.0 * (long double) buffer_size));
	  
	  frame_clock->note_256th_offset[i] = last_note_256th_offset + (guint64) j;
	}

	frame_clock->absolute_note_256th_offset_length = i;

	frame_clock->note_256th_frame_offset_length = i;
	
	frame_clock->note_256th_offset_length = i;
      }
    }else{
      if(heading_note_256th_frame_offset + (guint64) floorl(fixed_absolute_delay / 16.0 * (long double) buffer_size) < frame_clock->frame_offset + buffer_size){
	has_256th_pulse = TRUE;

	frame_clock->absolute_note_256th_offset[0] = last_absolute_note_256th_offset + 1;
    
	frame_clock->absolute_note_256th_offset_length = 1;

	frame_clock->note_256th_frame_offset[0] = heading_note_256th_frame_offset + ((guint64) floorl(fixed_absolute_delay / 16.0 * (long double) buffer_size));
	
	frame_clock->note_256th_frame_offset_length = 1;
	
	frame_clock->note_256th_offset[0] = (guint64) floorl(((long double) heading_note_256th_frame_offset + (floorl(fixed_absolute_delay / 16.0 * (long double) buffer_size))) / (fixed_absolute_delay / 16.0 * (long double) buffer_size));
	
	frame_clock->note_256th_offset_length = 1;
      }
    }
  }

  /* set pulse */
  frame_clock->has_16th_pulse = has_16th_pulse;

  if(has_16th_pulse){
    frame_clock->delay_counter = 0.0;
  }else{
    frame_clock->delay_counter += 1.0;
  }
  
  frame_clock->has_256th_pulse = has_256th_pulse;
  
  g_rec_mutex_unlock(frame_clock_mutex);
}

/**
 * ags_frame_clock_from_string:
 * @frame_clock: the #AgsFrameClock
 * @str: the string
 * 
 * Frame clock from string.
 * 
 * Since: 9.0.0
 */
void
ags_frame_clock_from_string(AgsFrameClock *frame_clock,
			    gchar *str)
{
  xmlDoc *doc;
  xmlNode *root_node;

  xmlChar *prop_name;
  xmlChar *prop_value;
  xmlChar *iter;
  xmlChar *endptr;

  AgsFrameClockFlags flags;
  guint buffer_size;
  guint samplerate;
  gdouble bpm;
  guint i;
  
  xmlInitParser();

  doc = xmlReadMemory(str, strlen(str), NULL, NULL, 0);

  root_node = xmlDocGetRootElement(doc);

  /* flags */
  prop_name = BAD_CAST "flags";

  prop_value = xmlGetProp(root_node,
			  prop_name);

  flags = (AgsFrameClockFlags) g_ascii_strtoull(prop_value,
						NULL,
						16);

#if 0
  if((AGS_FRAME_CLOCK_STARTED & flags) != 0 &&
     !ags_frame_clock_test_flags(frame_clock, AGS_FRAME_CLOCK_STARTED)){
    ags_frame_clock_start(frame_clock);
  }
#endif

  /* buffer size */
  prop_name = BAD_CAST "buffer-size";

  prop_value = xmlGetProp(root_node,
			  prop_name);

  buffer_size = (guint) g_ascii_strtoull(prop_value,
					 NULL,
					 10);

  ags_frame_clock_set_buffer_size(frame_clock,
				  buffer_size);
  
  /* samplerate */
  prop_name = BAD_CAST "samplerate";

  prop_value = xmlGetProp(root_node,
			  prop_name);
  
  samplerate = (guint) g_ascii_strtoull(prop_value,
					NULL,
					10);

  ags_frame_clock_set_samplerate(frame_clock,
				 samplerate);
  
  /* bpm */
  prop_name = BAD_CAST "bpm";

  prop_value = xmlGetProp(root_node,
			  prop_name);
  
  bpm = g_ascii_strtod(prop_value,
		       NULL);

  ags_frame_clock_set_bpm(frame_clock,
			  bpm);

  /* delay counter */
  prop_name = BAD_CAST "delay-counter";

  prop_value = xmlGetProp(root_node,
			  prop_name);
  
  frame_clock->delay_counter = strtold(prop_value,
					      NULL);
  
  /* frame offset */
  prop_name = BAD_CAST "frame-offset";

  prop_value = xmlGetProp(root_node,
			  prop_name);
  
  frame_clock->frame_offset = g_ascii_strtoull(prop_value,
					       NULL,
					       10);
  
  /* period frame offset */
  prop_name = BAD_CAST "period-frame-offset";

  prop_value = xmlGetProp(root_node,
			  prop_name);
  
  frame_clock->period_frame_offset = g_ascii_strtoull(prop_value,
						      NULL,
						      10);
  
  /* do loop */
  prop_name = BAD_CAST "do-loop";

  prop_value = xmlGetProp(root_node,
			  prop_name);
  
  frame_clock->do_loop = (!g_ascii_strncasecmp(prop_value, "true", 5)) ? TRUE: FALSE;
  
  /* loop left */
  prop_name = BAD_CAST "loop-left";

  prop_value = xmlGetProp(root_node,
			  prop_name);
  
  frame_clock->loop_left = g_ascii_strtoull(prop_value,
					    NULL,
					    10);
  
  /* loop right */
  prop_name = BAD_CAST "loop-right";

  prop_value = xmlGetProp(root_node,
			  prop_name);
  
  frame_clock->loop_right = g_ascii_strtoull(prop_value,
					     NULL,
					     10);
  
  /* has 16th pulse */
  prop_name = BAD_CAST "has-16th-pulse";

  prop_value = xmlGetProp(root_node,
			  prop_name);
  
  frame_clock->has_16th_pulse = (!g_ascii_strncasecmp(prop_value, "true", 5)) ? TRUE: FALSE;
  
  /* absolute note offset */
  prop_name = BAD_CAST "absolute-note-offset";

  prop_value = xmlGetProp(root_node,
			  prop_name);
  
  frame_clock->absolute_note_offset = g_ascii_strtoull(prop_value,
						       NULL,
						       10);
  
  /* note offset */
  prop_name = BAD_CAST "note-offset";

  prop_value = xmlGetProp(root_node,
			  prop_name);
  
  frame_clock->note_offset = g_ascii_strtoull(prop_value,
					      NULL,
					      10);
  
  /* note frame offset */
  prop_name = BAD_CAST "note-frame-offset";

  prop_value = xmlGetProp(root_node,
			  prop_name);
  
  frame_clock->note_frame_offset = g_ascii_strtoull(prop_value,
						    NULL,
						    10);
  
  /* has 256th pulse */
  prop_name = BAD_CAST "has-256th-pulse";

  prop_value = xmlGetProp(root_node,
			  prop_name);
  
  frame_clock->has_256th_pulse = (!g_ascii_strncasecmp(prop_value, "true", 5)) ? TRUE: FALSE;

  /* absolute note 256th offset */
  prop_name = BAD_CAST "absolute-note-256th-offset-length";

  prop_value = xmlGetProp(root_node,
			  prop_name);
  
  frame_clock->absolute_note_256th_offset_length = g_ascii_strtoull(prop_value,
								    NULL,
								    10);
  
  prop_name = BAD_CAST "absolute-note-256th-offset";

  prop_value = xmlGetProp(root_node,
			  prop_name);

  iter = prop_value;
  
  for(i = 0; i < frame_clock->absolute_note_256th_offset_length && i < 16; i++){
    endptr = NULL;
    
    frame_clock->absolute_note_256th_offset[i] = g_ascii_strtoull(iter,
								  (gchar **) &endptr,
								  10);

    if(endptr == NULL){
      g_warning("frame clock from string - premature end");
      
      break;
    }
    
    iter = (endptr++);
  }
  
  /* note 256th offset */
  prop_name = BAD_CAST "note-256th-offset-length";

  prop_value = xmlGetProp(root_node,
			  prop_name);
  
  frame_clock->note_256th_offset_length = g_ascii_strtoull(prop_value,
							   NULL,
							   10);
  
  prop_name = BAD_CAST "note-256th-offset";

  prop_value = xmlGetProp(root_node,
			  prop_name);

  iter = prop_value;
  
  for(i = 0; i < frame_clock->note_256th_offset_length && i < 16; i++){
    endptr = NULL;
    
    frame_clock->note_256th_offset[i] = g_ascii_strtoull(iter,
							 (gchar **) &endptr,
							 10);

    if(endptr == NULL){
      g_warning("frame clock from string - premature end");
      
      break;
    }

    iter = (endptr++);
  }
  
  /* note 256th frame offset */
  prop_name = BAD_CAST "note-256th-frame-offset-length";

  prop_value = xmlGetProp(root_node,
			  prop_name);
  
  frame_clock->note_256th_frame_offset_length = g_ascii_strtoull(prop_value,
								 NULL,
								 10);
  
  prop_name = BAD_CAST "note-256th-frame-offset";

  prop_value = xmlGetProp(root_node,
			  prop_name);

  iter = prop_value;
  
  for(i = 0; i < frame_clock->note_256th_frame_offset_length && i < 16; i++){
    endptr = NULL;
    
    frame_clock->note_256th_frame_offset[i] = g_ascii_strtoull(iter,
							       (gchar **) &endptr,
							       10);

    if(endptr == NULL){
      g_warning("frame clock from string - premature end");
      
      break;
    }

    iter = (endptr++);
  }
}

/**
 * ags_frame_clock_to_string:
 * @frame_clock: the #AgsFrameClock
 * 
 * Frame clock to string.
 *
 * Returns: (transfer full): the serialized string
 * 
 * Since: 9.0.0
 */
gchar*
ags_frame_clock_to_string(AgsFrameClock *frame_clock)
{
  xmlDoc *doc;
  xmlNode *root_node;

  xmlChar *buffer;
  xmlChar *encoding;
  xmlChar *version;
  xmlChar *node_name;
  xmlChar *prop_name;
  xmlChar *prop_value;
  xmlChar *iter;
  gchar *retval;
  
  int size;
  gint out_count;
  guint i;
  
  version = BAD_CAST "1.0";
  
  doc = xmlNewDoc(version);

  node_name = BAD_CAST "ags-frame-clock";

  root_node = xmlNewNode(NULL,
			 node_name);
  
  xmlDocSetRootElement(doc,
		       root_node);

  /* flags */
  prop_name = BAD_CAST "flags";

  prop_value = BAD_CAST g_strdup_printf("0x%x", frame_clock->flags);
  
  xmlNewProp(root_node,
	     prop_name,
	     prop_value);
  
  /* buffer size */
  prop_name = BAD_CAST "buffer-size";

  prop_value = BAD_CAST g_strdup_printf("%u", frame_clock->buffer_size);
  
  xmlNewProp(root_node,
	     prop_name,
	     prop_value);

  /* samplerate */
  prop_name = BAD_CAST "samplerate";

  prop_value = BAD_CAST g_strdup_printf("%u", frame_clock->samplerate);
  
  xmlNewProp(root_node,
	     prop_name,
	     prop_value);
  
  /* bpm */
  prop_name = BAD_CAST "bpm";

  prop_value = BAD_CAST g_strdup_printf("%f", frame_clock->bpm);
  
  xmlNewProp(root_node,
	     prop_name,
	     prop_value);
  
  /* absolute delay */
  prop_name = BAD_CAST "absolute-delay";

  prop_value = BAD_CAST g_strdup_printf("%Lf", frame_clock->absolute_delay);
  
  xmlNewProp(root_node,
	     prop_name,
	     prop_value);
  
  /* fixed absolute delay */
  prop_name = BAD_CAST "fixed-absolute-delay";

  prop_value = BAD_CAST g_strdup_printf("%Lf", frame_clock->fixed_absolute_delay);
  
  xmlNewProp(root_node,
	     prop_name,
	     prop_value);
    
  /* absolute delay */
  prop_name = BAD_CAST "delay-counter";

  prop_value = BAD_CAST g_strdup_printf("%Lf", frame_clock->delay_counter);
  
  xmlNewProp(root_node,
	     prop_name,
	     prop_value);
  
  /* frame offset */
  prop_name = BAD_CAST "frame-offset";

  prop_value = BAD_CAST g_strdup_printf("%" G_GUINT64_FORMAT, frame_clock->frame_offset);
  
  xmlNewProp(root_node,
	     prop_name,
	     prop_value);

  /* period frame offset */
  prop_name = BAD_CAST "period-frame-offset";

  prop_value = BAD_CAST g_strdup_printf("%" G_GUINT64_FORMAT, frame_clock->period_frame_offset);
  
  xmlNewProp(root_node,
	     prop_name,
	     prop_value);

  /* do loop */
  prop_name = BAD_CAST "do-loop";

  prop_value = BAD_CAST g_strdup_printf("%s", ((frame_clock->do_loop) ? "true": "false"));
  
  xmlNewProp(root_node,
	     prop_name,
	     prop_value);
  
  /* loop left */
  prop_name = BAD_CAST "loop-left";

  prop_value = BAD_CAST g_strdup_printf("%" G_GUINT64_FORMAT, frame_clock->loop_left);
  
  xmlNewProp(root_node,
	     prop_name,
	     prop_value);
  
  /* loop right */
  prop_name = BAD_CAST "loop-right";

  prop_value = BAD_CAST g_strdup_printf("%" G_GUINT64_FORMAT, frame_clock->loop_right);
  
  xmlNewProp(root_node,
	     prop_name,
	     prop_value);
  
  /* has 16th pulse */
  prop_name = BAD_CAST "has-16th-pulse";

  prop_value = BAD_CAST g_strdup_printf("%s", (frame_clock->has_16th_pulse ? "true": "false"));
  
  xmlNewProp(root_node,
	     prop_name,
	     prop_value);
  
  /* absolute frame offset */
  prop_name = BAD_CAST "absolute-frame-offset";

  prop_value = BAD_CAST g_strdup_printf("%" G_GUINT64_FORMAT, frame_clock->absolute_frame_offset);
  
  xmlNewProp(root_node,
	     prop_name,
	     prop_value);

  /* absolute note offset */
  prop_name = BAD_CAST "absolute-note-offset";

  prop_value = BAD_CAST g_strdup_printf("%" G_GUINT64_FORMAT, frame_clock->absolute_note_offset);
  
  xmlNewProp(root_node,
	     prop_name,
	     prop_value);

  /* note offset */
  prop_name = BAD_CAST "note-offset";

  prop_value = BAD_CAST g_strdup_printf("%" G_GUINT64_FORMAT, frame_clock->note_offset);
  
  xmlNewProp(root_node,
	     prop_name,
	     prop_value);
  
  /* note frame offset */
  prop_name = BAD_CAST "note-frame-offset";

  prop_value = BAD_CAST g_strdup_printf("%" G_GUINT64_FORMAT, frame_clock->note_frame_offset);
  
  xmlNewProp(root_node,
	     prop_name,
	     prop_value);

  /* has 256th pulse */
  prop_name = BAD_CAST "has-256th-pulse";

  prop_value = BAD_CAST g_strdup_printf("%s", (frame_clock->has_256th_pulse ? "true": "false"));
  
  xmlNewProp(root_node,
	     prop_name,
	     prop_value);

  /* alloc prop value */
  prop_value = g_malloc((16 * (21 + 1)) * sizeof(xmlChar));

  /* absolute note 256th offset */
  prop_name = "absolute-note-256th-offset";

  memset(prop_value, 0, (16 * (21 + 1)) * sizeof(xmlChar));

  iter = prop_value;
  
  for(i = 0; i < frame_clock->absolute_note_256th_offset_length && i < 16; i++){
    if(i + 1 < frame_clock->absolute_note_256th_offset_length){
      out_count = g_snprintf(iter, prop_value + (16 * (21 + 1)) - iter, "%" G_GUINT64_FORMAT " ", frame_clock->absolute_note_256th_offset[i]);
    }else{
      out_count = g_snprintf(iter, prop_value + (16 * (21 + 1)) - iter, "%" G_GUINT64_FORMAT, frame_clock->absolute_note_256th_offset[i]);
    }

    iter += (out_count);

    iter[0] = '\0';
  }
  
  xmlNewProp(root_node,
	     prop_name,
	     prop_value);
  
  /* note 256th offset */
  prop_name = "note-256th-offset";

  memset(prop_value, 0, (16 * (21 + 1)) * sizeof(xmlChar));

  iter = prop_value;
  
  for(i = 0; i < frame_clock->note_256th_offset_length && i < 16; i++){
    if(i + 1 < frame_clock->note_256th_offset_length){
      out_count = g_snprintf(iter, prop_value + (16 * (21 + 1)) - iter, "%" G_GUINT64_FORMAT " ", frame_clock->note_256th_offset[i]);
    }else{
      out_count = g_snprintf(iter, prop_value + (16 * (21 + 1)) - iter, "%" G_GUINT64_FORMAT, frame_clock->note_256th_offset[i]);
    }

    iter += (out_count);

    iter[0] = '\0';
  }
  
  xmlNewProp(root_node,
	     prop_name,
	     prop_value);
  
  /* note 256th frame offset */
  prop_name = "note-256th-frame-offset";

  memset(prop_value, 0, (16 * (21 + 1)) * sizeof(xmlChar));

  iter = prop_value;
  
  for(i = 0; i < frame_clock->note_256th_frame_offset_length && i < 16; i++){
    if(i + 1 < frame_clock->note_256th_frame_offset_length){
      out_count = g_snprintf(iter, prop_value + (16 * (21 + 1)) - iter, "%" G_GUINT64_FORMAT " ", frame_clock->note_256th_frame_offset[i]);
    }else{
      out_count = g_snprintf(iter, prop_value + (16 * (21 + 1)) - iter, "%" G_GUINT64_FORMAT, frame_clock->note_256th_frame_offset[i]);
    }

    iter += (out_count);

    iter[0] = '\0';
  }
  
  xmlNewProp(root_node,
	     prop_name,
	     prop_value);

  /* free prop value */
  g_free(prop_value);

  /* to string */
  buffer = NULL;

  encoding = BAD_CAST "UTF-8";

  size = 0;
  
  xmlDocDumpFormatMemoryEnc(doc, &buffer, &size, encoding, TRUE);

  xmlFreeDoc(doc);

  retval = g_strdup(buffer);

  xmlFree(buffer);
  
  return(retval);
}

/**
 * ags_frame_clock_to_time_string:
 * @frame_clock: the #AgsFrameClock
 * 
 * Frame clock to time string.
 *
 * Returns: (transfer full): the time string
 * 
 * Since: 9.0.0
 */
gchar*
ags_frame_clock_to_time_string(AgsFrameClock *frame_clock)
{
  gchar *uptime_str;
  
  guint64 secs_count;
  guint64 nsecs_count;
  guint min, sec, msec;

  secs_count = (guint64) floor((double) frame_clock->frame_offset / (double) frame_clock->samplerate);

  nsecs_count = (guint64) (((double) (frame_clock->frame_offset % frame_clock->samplerate) / (double) frame_clock->samplerate) * (double) AGS_NSEC_PER_SEC);

  min = (guint) floor((double) secs_count / 60.0);

  sec = secs_count % 60;
  
  msec = (guint) floor((double) nsecs_count / 1000000.0);
  
  uptime_str = g_strdup_printf("%.4d:%.2d.%.3d",
			   min,
			   sec,
			   msec);

  return(uptime_str);
}

/**
 * ags_frame_clock_get_instance:
 * 
 * Singleton frame clock.
 *
 * Returns: (transfer none): the singleton #AgsFrameClock
 * 
 * Since: 9.0.0
 */
AgsFrameClock*
ags_frame_clock_get_instance()
{
  static AgsFrameClock *frame_clock = NULL;

  static GMutex mutex = {0,};
  
  g_mutex_lock(&mutex);
  
  if(frame_clock == NULL){
    frame_clock = ags_frame_clock_new();
  }

  g_mutex_unlock(&mutex);
  
  return(frame_clock);
}

/**
 * ags_frame_clock_new:
 *
 * Creates an #AgsFrameClock
 *
 * Returns: a new #AgsFrameClock
 *
 * Since: 9.0.0
 */
AgsFrameClock*
ags_frame_clock_new()
{
  AgsFrameClock *frame_clock;

  frame_clock = (AgsFrameClock *) g_object_new(AGS_TYPE_FRAME_CLOCK,
					       NULL);

  return(frame_clock);
}
