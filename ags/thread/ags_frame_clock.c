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

#include <ags/object/ags_soundcard.h>

#include <math.h>

#include <string.h>

void ags_frame_clock_class_init(AgsFrameClockClass *frame_clock);
void ags_frame_clock_init (AgsFrameClock *frame_clock);
void ags_frame_clock_finalize(GObject *gobject);

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
  guint i;
  
  frame_clock->flags = 0;

  /* add frame_clock mutex */
  g_rec_mutex_init(&(frame_clock->obj_mutex));

  /* common fields */
  frame_clock->buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  frame_clock->samplerate = (guint) AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  
  frame_clock->bpm = AGS_SOUNDCARD_DEFAULT_BPM;

  frame_clock->absolute_delay = (60.0 * frame_clock->samplerate) / (4.0 * frame_clock->bpm) / frame_clock->buffer_size;
  
  frame_clock->fixed_absolute_delay = ((ceil((AGS_FRAME_CLOCK_DEFAULT_PERIOD * frame_clock->absolute_delay * frame_clock->buffer_size) / frame_clock->buffer_size) * frame_clock->buffer_size) / (AGS_FRAME_CLOCK_DEFAULT_PERIOD * frame_clock->absolute_delay * frame_clock->buffer_size)) * frame_clock->absolute_delay;

  frame_clock->absolute_frame_offset = 0;

  frame_clock->frame_offset = 0;

  frame_clock->period_frame_offset = 0;

  frame_clock->do_loop = FALSE;
  
  frame_clock->loop_left = 0;
  frame_clock->loop_right = 64;

  frame_clock->has_16th_pulse = TRUE;

  frame_clock->absolute_note_offset = 0;

  frame_clock->note_offset = 0;

  frame_clock->has_256th_pulse = TRUE;

  memset(&(frame_clock->absolute_note_256th_offset[0]), 0, 16 * sizeof(guint));

  frame_clock->absolute_note_256th_offset_length = 0;

  memset(&(frame_clock->note_256th_offset[0]), 0, 16 * sizeof(guint));

  frame_clock->note_256th_offset_length = 0;

  memset(&(frame_clock->note_256th_frame_offset[0]), 0, 16 * sizeof(guint));
  
  frame_clock->note_256th_frame_offset_length = 0;
}

void
ags_frame_clock_finalize(GObject *gobject)
{
  AgsFrameClock *frame_clock;

  frame_clock = AGS_FRAME_CLOCK(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_frame_clock_parent_class)->finalize(gobject);
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
 * Since: 8.5.0
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
 * Since: 8.5.0
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
 * Since: 8.5.0
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
 * Since: 8.5.0
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
 * Since: 8.5.0
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
 * Since: 8.5.0
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
 * Since: 8.5.0
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
 * Since: 8.5.0
 */
gdouble
ags_frame_clock_get_bpm(AgsFrameClock *frame_clock)
{
  guint bpm;
  
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
 * Since: 8.5.0
 */
void
ags_frame_clock_set_bpm(AgsFrameClock *frame_clock,
			gdouble bpm)
{
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
 * Since: 8.5.0
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
 * Since: 8.5.0
 */
guint64
ags_frame_clock_get_frame_offset(AgsFrameClock *frame_clock)
{
  guint frame_offset;
  
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
 * Since: 8.5.0
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
 * Since: 8.5.0
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
 * ags_frame_clock_get_loop:
 * @frame_clock: the #AgsFrameClock
 * @loop_left: (out): return location of loop left
 * @loop_right: (out): return location of loop right
 *
 * Get loop of @frame_clock.
 * 
 * Returns: %TRUE if do loop, otherwise %FALSE
 * 
 * Since: 8.5.0
 */
gboolean
ags_frame_clock_get_loop(AgsFrameClock *frame_clock,
			 guint *loop_left,
			 guint *loop_right)
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
 * Since: 8.5.0
 */
void
ags_frame_clock_set_loop(AgsFrameClock *frame_clock,
			 gboolean do_loop,
			 guint loop_left,
			 guint loop_right)
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
 * ags_frame_clock_get_absolute_note_offset:
 * @frame_clock: the #AgsFrameClock
 *
 * Get absolute note offset of @frame_clock.
 * 
 * Returns: the absolute note offset
 * 
 * Since: 8.5.0
 */
guint
ags_frame_clock_get_absolute_note_offset(AgsFrameClock *frame_clock)
{
  guint absolute_note_offset;
  
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
 * Since: 8.5.0
 */
guint
ags_frame_clock_get_note_offset(AgsFrameClock *frame_clock)
{
  guint note_offset;
  
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
 * ags_frame_clock_get_absolute_note_256th_offset:
 * @frame_clock: the #AgsFrameClock
 *
 * Get absolute note 256th offset of @frame_clock.
 * 
 * Returns: the absolute note 256th offset
 * 
 * Since: 8.5.0
 */
void
ags_frame_clock_get_absolute_note_256th_offset(AgsFrameClock *frame_clock,
					       guint *absolute_note_256th_offset,
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
 *
 * Get note 256th offset of @frame_clock.
 * 
 * Returns: the note 256th offset
 * 
 * Since: 8.5.0
 */
void
ags_frame_clock_get_note_256th_offset(AgsFrameClock *frame_clock,
				      guint *note_256th_offset,
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
 * Since: 8.5.0
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
 * ags_frame_clock_start:
 * @frame_clock: the #AgsFrameClock
 *
 * Start @frame_clock.
 * 
 * Since: 8.5.0
 */
void
ags_frame_clock_start(AgsFrameClock *frame_clock)
{
  if(!AGS_IS_FRAME_CLOCK(frame_clock) ||
     ags_frame_clock_test_flags(frame_clock, AGS_FRAME_CLOCK_STARTED)){
    return;
  }

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
 * Since: 8.5.0
 */
void
ags_frame_clock_stop(AgsFrameClock *frame_clock)
{
  if(!AGS_IS_FRAME_CLOCK(frame_clock) ||
     !ags_frame_clock_test_flags(frame_clock, AGS_FRAME_CLOCK_STARTED)){
    return;
  }
  
  ags_frame_clock_unset_flags(frame_clock,
			      AGS_FRAME_CLOCK_RUNNING);

  ags_frame_clock_unset_flags(frame_clock,
			      AGS_FRAME_CLOCK_STARTED);
}

/**
 * ags_frame_clock_increment_counter:
 * @frame_clock: the #AgsFrameClock
 *
 * Increment counter of @frame_clock.
 * 
 * Since: 8.5.0
 */
void
ags_frame_clock_increment_counter(AgsFrameClock *frame_clock)
{
  guint64 note_256th_frame_offset[16] = {0,};
  guint note_256th_offset[16] = {0,};
  
  guint buffer_size;
  gdouble bpm;
  gdouble fixed_absolute_delay;
  guint64 period_frame_offset;
  guint64 total_period_frame_offset;
  guint64 note_frame_offset;
  guint64 last_note_256th_frame_offset;
  guint64 loop_left_note_256th_frame_offset;
  guint64 heading_note_256th_frame_offset;
  guint64 stop_note_256th_frame_offset;
  guint note_256th_frame_offset_length;
  guint note_256th_offset_length;

  guint i, i_stop;
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

  total_period_frame_offset = (guint64) ceil((double) total_period_frame_offset / (double) buffer_size) * buffer_size;

  /* note frame offset */
  note_frame_offset = ags_frame_clock_get_note_frame_offset(frame_clock);

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
  
  /* increment frame offset */
  frame_clock->absolute_frame_offset += frame_clock->buffer_size;

  frame_clock->frame_offset += frame_clock->buffer_size;

  frame_clock->period_frame_offset += frame_clock->buffer_size;

  do_loop = FALSE;
  
  if(frame_clock->do_loop &&
     (double) frame_clock->frame_offset + (double) buffer_size >= (double) frame_clock->loop_right * fixed_absolute_delay * (double) buffer_size){
    do_loop = TRUE;

    frame_clock->frame_offset = (guint64) floor(floor((double) frame_clock->loop_left * fixed_absolute_delay * (double) buffer_size) / (double) buffer_size) * buffer_size;

    frame_clock->period_frame_offset = frame_clock->frame_offset % total_period_frame_offset;
  }

  if(frame_clock->period_frame_offset >= AGS_FRAME_CLOCK_DEFAULT_PERIOD * buffer_size){
    frame_clock->period_frame_offset = frame_clock->period_frame_offset % ((guint64) AGS_FRAME_CLOCK_DEFAULT_PERIOD * buffer_size);
  }
  
  /* 16th pulse */
  has_16th_pulse = FALSE;

  if(do_loop){
    has_16th_pulse = TRUE;

    frame_clock->absolute_note_offset += 1;
    
    frame_clock->note_offset = frame_clock->loop_left;

    frame_clock->note_frame_offset = (frame_clock->loop_left % (guint) AGS_FRAME_CLOCK_DEFAULT_PERIOD) * buffer_size;
  }else{
    if(note_frame_offset + (fixed_absolute_delay * (double) buffer_size) < frame_clock->frame_offset + buffer_size){
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

    if(fixed_absolute_delay / 16.0 <= 1.0){
      if(frame_clock->frame_offset == 0){
	for(i = 0; (i * (guint64) floor(fixed_absolute_delay / 16.0 * (double) buffer_size)) < buffer_size && i < 16; i++){
	  frame_clock->note_256th_offset[i] = (guint) floor(((double) i * floor(fixed_absolute_delay / 16.0 * (double) buffer_size)) / (fixed_absolute_delay / 16.0 * (double) buffer_size));
	}
	
	frame_clock->note_256th_offset_length = i;
      }else{
	heading_note_256th_frame_offset = floor((double) frame_clock->loop_left * fixed_absolute_delay * (double) buffer_size);

	stop_note_256th_frame_offset = (guint64) floor(heading_note_256th_frame_offset / buffer_size) * (guint64) buffer_size;

	for(i = 0; heading_note_256th_frame_offset >= stop_note_256th_frame_offset && i < 16; i++){
	  heading_note_256th_frame_offset -= (guint64) floor(fixed_absolute_delay / 16.0 * (double) buffer_size);
	}
	
	for(i = 0; heading_note_256th_frame_offset + ((guint64) (i + 1) * (guint64) floor(fixed_absolute_delay / 16.0 * (double) buffer_size)) < frame_clock->frame_offset + buffer_size && i < 16; i++){
	  frame_clock->note_256th_offset[i] = (guint) floor(((double) heading_note_256th_frame_offset + (((double) i + 1.0) * floor(fixed_absolute_delay / 16.0 * (double) buffer_size))) / (fixed_absolute_delay / 16.0 * (double) buffer_size));
	}
	
	frame_clock->note_256th_offset_length = i;
      }
    }else{
      frame_clock->note_256th_offset[0] = 16 * frame_clock->loop_left;
      frame_clock->note_256th_offset_length = 1;
    }
  }else{
    if(fixed_absolute_delay / 16.0 <= 1.0){
      has_256th_pulse = TRUE;
      
      if(frame_clock->frame_offset == 0){
	for(i = 0; (i * (guint64) floor(fixed_absolute_delay / 16.0 * (double) buffer_size)) < buffer_size && i < 16; i++){
	  frame_clock->note_256th_offset[i] = (guint) floor(((double) i * floor(fixed_absolute_delay / 16.0 * (double) buffer_size)) / (fixed_absolute_delay / 16.0 * (double) buffer_size));
	}
	
	frame_clock->note_256th_offset_length = i;
      }else{
	heading_note_256th_frame_offset = last_note_256th_frame_offset;

	for(i = 0; heading_note_256th_frame_offset + ((guint64) i * (guint64) floor(fixed_absolute_delay / 16.0 * (double) buffer_size)) < frame_clock->frame_offset + buffer_size && i < 16; i++){
	  frame_clock->note_256th_offset[i] = (guint) floor(((double) heading_note_256th_frame_offset + (((double) i + 1.0) * floor(fixed_absolute_delay / 16.0 * (double) buffer_size))) / (fixed_absolute_delay / 16.0 * (double) buffer_size));
	}
	
	frame_clock->note_256th_offset_length = i;
      }
    }else{
      if(heading_note_256th_frame_offset + (guint64) floor(fixed_absolute_delay / 16.0 * (double) buffer_size) < frame_clock->frame_offset + buffer_size){
	has_256th_pulse = TRUE;
	
	heading_note_256th_frame_offset = last_note_256th_frame_offset;

	frame_clock->note_256th_offset[0] = (guint) floor(((double) heading_note_256th_frame_offset + floor(fixed_absolute_delay / 16.0 * (double) buffer_size)) / (fixed_absolute_delay / 16.0 * (double) buffer_size));
	
	frame_clock->note_256th_offset_length = 1;
      }
    }
  }

  /* set pulse */
  frame_clock->has_16th_pulse = has_16th_pulse;

  frame_clock->has_256th_pulse = has_256th_pulse;
  
  if(do_loop){
    if(has_256th_pulse){
      heading_note_256th_frame_offset = floor((double) frame_clock->loop_left * fixed_absolute_delay * (double) buffer_size);

      stop_note_256th_frame_offset = (guint64) floor(heading_note_256th_frame_offset / buffer_size) * (guint64) buffer_size;
	
      for(i = 0; heading_note_256th_frame_offset >= stop_note_256th_frame_offset && i < 16; i++){
	heading_note_256th_frame_offset -= (guint64) floor(fixed_absolute_delay / 16.0 * (double) buffer_size);
      }
      
      for(i = 0; heading_note_256th_frame_offset + ((guint64) (i + 1) * (guint64) floor(fixed_absolute_delay / 16.0 * (double) buffer_size)) < frame_clock->frame_offset + buffer_size && i < 16; i++){
	frame_clock->note_256th_frame_offset[i] = ((heading_note_256th_frame_offset % ((guint64) AGS_FRAME_CLOCK_DEFAULT_PERIOD_256TH * (guint64) floor(fixed_absolute_delay / 16.0 * (double) buffer_size))) / ((guint64) floor(fixed_absolute_delay / 16.0 * (double) buffer_size))) + (i + 1);
      }
      
      frame_clock->note_256th_frame_offset_length = i;
    }
  }else{
    if(has_256th_pulse){
      for(i = 0; i < frame_clock->note_256th_offset_length && i < 16; i++){
	frame_clock->note_256th_frame_offset[i] = note_256th_frame_offset[note_256th_frame_offset_length - 1] + (i + 1);
      }
      
      frame_clock->note_256th_frame_offset_length = i;
    }
  }
  
  g_rec_mutex_unlock(frame_clock_mutex);
}

void
ags_frame_clock_from_string(AgsFrameClock *frame_clock,
			    gchar *str)
{
  //TODO:JK: implement me
}

gchar*
ags_frame_clock_to_string(AgsFrameClock *frame_clock)
{
  //TODO:JK: implement me

  return(NULL);
}

gchar*
ags_frame_clock_to_time_string(AgsFrameClock *frame_clock,
			       gint number_of_digits)
{
  //TODO:JK: implement me

  return(NULL);
}

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
 * Since: 8.5.0
 */
AgsFrameClock*
ags_frame_clock_new()
{
  AgsFrameClock *frame_clock;

  frame_clock = (AgsFrameClock *) g_object_new(AGS_TYPE_FRAME_CLOCK,
					       NULL);

  return(frame_clock);
}
