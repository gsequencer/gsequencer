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
  GList *start_list, *list;

  guint *sync_point_arr;
  
  guint i;
  
  frame_clock->flags = 0;

  /* add frame_clock mutex */
  g_rec_mutex_init(&(frame_clock->obj_mutex));

  /* common fields */
  frame_clock->buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  frame_clock->samplerate = (guint) AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  
  frame_clock->bpm = AGS_SOUNDCARD_DEFAULT_BPM;

  frame_clock->absolute_frame_offset = 0;

  frame_clock->frame_offset = 0;

  frame_clock->note_attack_position = 0;

  frame_clock->note_256th_attack_position = 0;

  frame_clock->has_16th_pulse = TRUE;

  frame_clock->do_loop = FALSE;
  
  frame_clock->loop_left = 0;
  frame_clock->loop_right = 64;

  frame_clock->absolute_note_offset = 0;

  frame_clock->note_offset = 0;

  start_list = NULL;

  for(i = 0; i < AGS_FRAME_CLOCK_DEFAULT_SYNC_POINT_ARRAY_LIST_LENGTH; i++){
    sync_point_arr = g_malloc(AGS_FRAME_CLOCK_DEFAULT_SYNC_POINT_ARRAY_LENGTH * sizeof(guint));

    memset(sync_point_arr, 0, AGS_FRAME_CLOCK_DEFAULT_SYNC_POINT_ARRAY_LENGTH * sizeof(guint));
    
    start_list = g_list_prepend(start_list,
				sync_point_arr);
  }

  frame_clock->note_attack = start_list;

  memset(&(frame_clock->absolute_note_256th_offset[0]), 0, 16 * sizeof(guint));

  frame_clock->absolute_note_256th_offset_length = 0;

  memset(&(frame_clock->note_256th_offset[0]), 0, 16 * sizeof(guint));

  frame_clock->note_256th_offset_length = 0;
  
  start_list = NULL;

  for(i = 0; i < 16 * AGS_FRAME_CLOCK_DEFAULT_SYNC_POINT_ARRAY_LIST_LENGTH; i++){
    sync_point_arr = g_malloc(AGS_FRAME_CLOCK_DEFAULT_SYNC_POINT_ARRAY_LENGTH * sizeof(guint));

    memset(sync_point_arr, 0, AGS_FRAME_CLOCK_DEFAULT_SYNC_POINT_ARRAY_LENGTH * sizeof(guint));
    
    start_list = g_list_prepend(start_list,
				sync_point_arr);
  }

  frame_clock->note_256th_attack = start_list;
}

void
ags_frame_clock_finalize(GObject *gobject)
{
  AgsFrameClock *frame_clock;

  frame_clock = AGS_FRAME_CLOCK(gobject);

  g_list_free_full(frame_clock->note_attack,
		   (GDestroyNotify) g_free);

  g_list_free_full(frame_clock->note_256th_attack,
		   (GDestroyNotify) g_free);
  
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
 * ags_frame_clock_get_note_attack_position:
 * @frame_clock: the #AgsFrameClock
 *
 * Get note attack position of @frame_clock.
 * 
 * Returns: the note attack position
 * 
 * Since: 8.5.0
 */
guint
ags_frame_clock_get_note_attack_position(AgsFrameClock *frame_clock)
{
  guint note_attack_position;
  
  GRecMutex *frame_clock_mutex;

  if(!AGS_IS_FRAME_CLOCK(frame_clock)){
    return(0);
  }
  
  /* get frame clock mutex */
  frame_clock_mutex = AGS_FRAME_CLOCK_GET_OBJ_MUTEX(frame_clock);

  /* get note attack position */
  g_rec_mutex_lock(frame_clock_mutex);

  note_attack_position = frame_clock->note_attack_position;

  g_rec_mutex_unlock(frame_clock_mutex);

  return(note_attack_position);
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
ags_frame_clock_get_note_attack(AgsFrameClock *frame_clock)
{
  GList *list;
  
  guint note_attack;
  
  GRecMutex *frame_clock_mutex;

  if(!AGS_IS_FRAME_CLOCK(frame_clock)){
    return(0);
  }
  
  /* get frame clock mutex */
  frame_clock_mutex = AGS_FRAME_CLOCK_GET_OBJ_MUTEX(frame_clock);

  /* get note attack */
  g_rec_mutex_lock(frame_clock_mutex);

  list = g_list_nth(frame_clock->note_attack,
		    (guint) floor((double) frame_clock->note_attack_position / AGS_FRAME_CLOCK_DEFAULT_SYNC_POINT_ARRAY_LENGTH));
  
  note_attack = 0;

  if(list != NULL){
    note_attack = ((guint *) list->data)[frame_clock->note_attack_position % (guint) AGS_FRAME_CLOCK_DEFAULT_SYNC_POINT_ARRAY_LENGTH];
  }
  
  g_rec_mutex_unlock(frame_clock_mutex);

  return(note_attack);
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
 * ags_frame_clock_get_note_256th_attack_position:
 * @frame_clock: the #AgsFrameClock
 * @note_256th_attack_position: (out): return location of note 256th attack position
 * @length: (out): return location of array length, maximum 16 items
 *
 * Get note 256th attack position of @frame_clock.
 * 
 * Since: 8.5.0
 */
void
ags_frame_clock_get_note_256th_attack_position(AgsFrameClock *frame_clock,
					       guint *note_256th_attack_position,
					       guint *length)
{
  guint i;
  
  GRecMutex *frame_clock_mutex;

  if(!AGS_IS_FRAME_CLOCK(frame_clock)){
    return;
  }
  
  /* get frame clock mutex */
  frame_clock_mutex = AGS_FRAME_CLOCK_GET_OBJ_MUTEX(frame_clock);

  /* get note 256th attack position */
  g_rec_mutex_lock(frame_clock_mutex);

  if(note_256th_attack_position != NULL){
    for(i = 0; i < frame_clock->note_256th_attack_position_length && i < 16; i++){
      note_256th_attack_position[i] = frame_clock->note_256th_attack_position[i];
    }
  }

  if(length != NULL){
    length[0] = frame_clock->note_256th_attack_position_length;
  }
  
  g_rec_mutex_unlock(frame_clock_mutex);
}

/**
 * ags_frame_clock_get_note_256th_attack:
 * @frame_clock: the #AgsFrameClock
 * @note_256th_attack: (out): return location of note 256th attack
 * @length: (out): return location of array length, maximum 16 items
 *
 * Get note 256th attack of @frame_clock.
 * 
 * Since: 8.5.0
 */
void
ags_frame_clock_get_note_256th_attack(AgsFrameClock *frame_clock,
				      guint *note_256th_attack,
				      guint *length)
{
  GList *list;
  
  guint i;
  
  GRecMutex *frame_clock_mutex;

  if(!AGS_IS_FRAME_CLOCK(frame_clock)){
    return(0);
  }
  
  /* get frame clock mutex */
  frame_clock_mutex = AGS_FRAME_CLOCK_GET_OBJ_MUTEX(frame_clock);

  /* get note attack */
  g_rec_mutex_lock(frame_clock_mutex);
  
  if(note_256th_attack != NULL){
    for(i = 0; i < frame_clock->note_256th_attack_position_length && i < 16; i++){
      list = g_list_nth(frame_clock->note_256th_attack,
			(guint) floor((double) (frame_clock->note_256th_attack_position + i) / AGS_FRAME_CLOCK_DEFAULT_SYNC_POINT_ARRAY_LENGTH));

      if(list != NULL){
	note_256th_attack[i] = ((guint *) list->data)[(frame_clock->note_256th_attack_position + i) % (guint) AGS_FRAME_CLOCK_DEFAULT_SYNC_POINT_ARRAY_LENGTH];
      }
    }
  }

  if(length != NULL){
    length[0] = frame_clock->note_256th_attack_position_length;
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
