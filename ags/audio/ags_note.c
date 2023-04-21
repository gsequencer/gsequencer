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

#include <ags/audio/ags_note.h>

#include <ags/audio/midi/ags_midi_buffer_util.h>

#include <stdlib.h>
#include <complex.h>

#include <ags/i18n.h>

void ags_note_class_init(AgsNoteClass *note);
void ags_note_init(AgsNote *note);
void ags_note_set_property(GObject *gobject,
			   guint prop_id,
			   const GValue *value,
			   GParamSpec *param_spec);
void ags_note_get_property(GObject *gobject,
			   guint prop_id,
			   GValue *value,
			   GParamSpec *param_spec);
void ags_note_finalize(GObject *gobject);

/**
 * SECTION:ags_note
 * @short_description: Note class
 * @title: AgsNote
 * @section_id:
 * @include: ags/audio/ags_note.h
 *
 * #AgsNote represents a tone. The note object contains envelope information by
 * specifying `attack`, `decay`, `sustain`, `release` and `ratio` properties as
 * #AgsComplex data type.
 *
 * The real field of AgsComplex is the magnitude of the envelope and the imag
 * field the relative offset. Raised by `ratio` real field amount.
 */

enum{
  PROP_0,
  PROP_X0,
  PROP_X1,
  PROP_Y,
  PROP_RT_OFFSET,
  PROP_RT_ATTACK,
  PROP_STREAM_DELAY,
  PROP_STREAM_ATTACK,
  PROP_STREAM_FRAME_COUNT,
  PROP_ATTACK,
  PROP_DECAY,
  PROP_SUSTAIN,
  PROP_RELEASE,
  PROP_RATIO,
  PROP_NOTE_NAME,
  PROP_FREQUENCY,
};

static gpointer ags_note_parent_class = NULL;

GType
ags_note_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_note = 0;

    static const GTypeInfo ags_note_info = {
      sizeof(AgsNoteClass),
      NULL,
      NULL,
      (GClassInitFunc) ags_note_class_init,
      NULL,
      NULL,
      sizeof(AgsNote),
      0,
      (GInstanceInitFunc) ags_note_init,
    };

    ags_type_note = g_type_register_static(G_TYPE_OBJECT,
					   "AgsNote",
					   &ags_note_info,
					   0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_note);
  }

  return g_define_type_id__volatile;
}

void 
ags_note_class_init(AgsNoteClass *note)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_note_parent_class = g_type_class_peek_parent(note);

  gobject = (GObjectClass *) note;

  gobject->set_property = ags_note_set_property;
  gobject->get_property = ags_note_get_property;

  gobject->finalize = ags_note_finalize;

  /* properties */
  /**
   * AgsNote:x0:
   *
   * Note offset x0.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("x0",
				 i18n_pspec("offset x0"),
				 i18n_pspec("The first x offset"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_X0,
				  param_spec);

  /**
   * AgsNote:x1:
   *
   * Note offset x1.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("x1",
				 i18n_pspec("offset x1"),
				 i18n_pspec("The last x offset"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_X1,
				  param_spec);

  /**
   * AgsNote:y:
   *
   * Note offset y.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("y",
				 i18n_pspec("offset y"),
				 i18n_pspec("The y offset"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_Y,
				  param_spec);

  /**
   * AgsNote:rt-offset:
   *
   * Note realtime offset.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint64("rt-offset",
				   i18n_pspec("realtime offset"),
				   i18n_pspec("The realtime offset"),
				   0,
				   G_MAXUINT64,
				   0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RT_OFFSET,
				  param_spec);
  
  /**
   * AgsNote:rt-attack:
   *
   * Note realtime attack.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("rt-attack",
				 i18n_pspec("realtime attack"),
				 i18n_pspec("The realtime attack"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RT_ATTACK,
				  param_spec);

  /**
   * AgsNote:stream-delay:
   *
   * The stream's delay.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_double("stream-delay",
				   i18n_pspec("delay of stream"),
				   i18n_pspec("The delay of the stream"),
				   0.0,
				   G_MAXDOUBLE,
				   0.0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_STREAM_DELAY,
				  param_spec);

  /**
   * AgsNote:stream-attack:
   *
   * The stream's attack.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_double("stream-attack",
				   i18n_pspec("stream attack offset"),
				   i18n_pspec("The first x offset"),
				   0.0,
				   G_MAXDOUBLE,
				   0.0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_STREAM_ATTACK,
				  param_spec);

  /**
   * AgsNote:stream-frame-count:
   *
   * The stream's frame count.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint64("stream-frame-count",
				   i18n_pspec("stream frame count"),
				   i18n_pspec("The stream frame count"),
				   0,
				   G_MAXUINT64,
				   0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_STREAM_FRAME_COUNT,
				  param_spec);
  
  /**
   * AgsNote:attack:
   *
   * Envelope attack.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_boxed("attack",
				  i18n_pspec("envelope's attack"),
				  i18n_pspec("The envelope's attack"),
				  AGS_TYPE_COMPLEX,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_ATTACK,
				  param_spec);

  /**
   * AgsNote:decay:
   *
   * Envelope decay.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_boxed("decay",
				  i18n_pspec("envelope's decay"),
				  i18n_pspec("The envelope's decay"),
				  AGS_TYPE_COMPLEX,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DECAY,
				  param_spec);

  /**
   * AgsNote:sustain:
   *
   * Envelope sustain.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_boxed("sustain",
				  i18n_pspec("envelope's sustain"),
				  i18n_pspec("The envelope's sustain"),
				  AGS_TYPE_COMPLEX,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SUSTAIN,
				  param_spec);

  /**
   * AgsNote:release:
   *
   * Envelope release.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_boxed("release",
				  i18n_pspec("envelope's release"),
				  i18n_pspec("The envelope's release"),
				  AGS_TYPE_COMPLEX,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RELEASE,
				  param_spec);

  /**
   * AgsNote:ratio:
   *
   * Envelope ratio.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_boxed("ratio",
				  i18n_pspec("envelope's ratio"),
				  i18n_pspec("The envelope's ratio"),
				  AGS_TYPE_COMPLEX,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RATIO,
				  param_spec);

  /**
   * AgsNote:note-name:
   *
   * The note's name.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("note-name",
				   i18n_pspec("note name"),
				   i18n_pspec("The note's name"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_NOTE_NAME,
				  param_spec);

  /**
   * AgsNote:frequency:
   *
   * The note's frequency.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_double("frequency",
				   i18n_pspec("frequency"),
				   i18n_pspec("The note's frequency"),
				   0.0,
				   G_MAXDOUBLE,
				   0.0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FREQUENCY,
				  param_spec);
}

void
ags_note_init(AgsNote *note)
{
  double _Complex z;
  
  note->flags = 0;

  /* note mutex */
  g_rec_mutex_init(&(note->obj_mutex));

  /* fields */
  note->x[0] = 0;
  note->x[1] = 1;
  note->y = 0;

  note->rt_offset = 0;
  note->rt_attack = 0;
  
  note->stream_delay = 0.0;
  note->stream_attack = 0;

  z = 0.25 + I * 1.0;
  ags_complex_set(&(note->attack),
		  z);

  z = 0.25 + I * 1.0;
  ags_complex_set(&(note->decay),
		  z);

  z = 0.25 + I * 1.0;
  ags_complex_set(&(note->sustain),
		  z);

  z = 0.25 + I * 1.0;
  ags_complex_set(&(note->release),
		  z);

  z = I * 1.0;
  ags_complex_set(&(note->ratio),
		  z);
  
  note->note_name = NULL;
  note->frequency = 440.0;
}

void
ags_note_set_property(GObject *gobject,
		      guint prop_id,
		      const GValue *value,
		      GParamSpec *param_spec)
{
  AgsNote *note;

  GRecMutex *note_mutex;

  note = AGS_NOTE(gobject);

  /* get note mutex */
  note_mutex = AGS_NOTE_GET_OBJ_MUTEX(note);

  switch(prop_id){
  case PROP_X0:
  {
    g_rec_mutex_lock(note_mutex);

    note->x[0] = g_value_get_uint(value);

    g_rec_mutex_unlock(note_mutex);
  }
  break;
  case PROP_X1:
  {
    g_rec_mutex_lock(note_mutex);

    note->x[1] = g_value_get_uint(value);

    g_rec_mutex_unlock(note_mutex);
  }
  break;
  case PROP_Y:
  {
    g_rec_mutex_lock(note_mutex);

    note->y = g_value_get_uint(value);

    g_rec_mutex_unlock(note_mutex);
  }
  break;
  case PROP_RT_OFFSET:
  {
    g_rec_mutex_lock(note_mutex);

    note->rt_offset = g_value_get_uint64(value);

    g_rec_mutex_unlock(note_mutex);
  }
  break;
  case PROP_RT_ATTACK:
  {
    g_rec_mutex_lock(note_mutex);

    note->rt_attack = g_value_get_uint(value);

    g_rec_mutex_unlock(note_mutex);
  }
  break;
  case PROP_STREAM_DELAY:
  {
    g_rec_mutex_lock(note_mutex);

    note->stream_delay = g_value_get_double(value);

    g_rec_mutex_unlock(note_mutex);
  }
  break;
  case PROP_STREAM_ATTACK:
  {
    g_rec_mutex_lock(note_mutex);

    note->stream_attack = g_value_get_double(value);

    g_rec_mutex_unlock(note_mutex);
  }
  break;
  case PROP_STREAM_FRAME_COUNT:
  {
    g_rec_mutex_lock(note_mutex);

    note->stream_frame_count = g_value_get_uint64(value);

    g_rec_mutex_unlock(note_mutex);
  }
  break;
  case PROP_ATTACK:
  {
    AgsComplex *attack;

    attack = (AgsComplex *) g_value_get_boxed(value);

    g_rec_mutex_lock(note_mutex);

    ags_complex_set(&(note->attack),
		    ags_complex_get(attack));

    g_rec_mutex_unlock(note_mutex);
  }
  break;
  case PROP_SUSTAIN:
  {
    AgsComplex *sustain;

    sustain = (AgsComplex *) g_value_get_boxed(value);

    g_rec_mutex_lock(note_mutex);

    ags_complex_set(&(note->sustain),
		    ags_complex_get(sustain));

    g_rec_mutex_unlock(note_mutex);
  }
  break;
  case PROP_DECAY:
  {
    AgsComplex *decay;

    decay = (AgsComplex *) g_value_get_boxed(value);

    g_rec_mutex_lock(note_mutex);

    ags_complex_set(&(note->decay),
		    ags_complex_get(decay));

    g_rec_mutex_unlock(note_mutex);
  }
  break;
  case PROP_RELEASE:
  {
    AgsComplex *release;

    release = (AgsComplex *) g_value_get_boxed(value);

    g_rec_mutex_lock(note_mutex);

    ags_complex_set(&(note->release),
		    ags_complex_get(release));

    g_rec_mutex_unlock(note_mutex);
  }
  break;
  case PROP_RATIO:
  {
    AgsComplex *ratio;

    ratio = (AgsComplex *) g_value_get_boxed(value);

    g_rec_mutex_lock(note_mutex);

    ags_complex_set(&(note->ratio),
		    ags_complex_get(ratio));

    g_rec_mutex_unlock(note_mutex);
  }
  break;
  case PROP_NOTE_NAME:
  {
    gchar *note_name;
      
    note_name = g_value_get_string(value);
      
    g_rec_mutex_lock(note_mutex);

    if(note_name == note->note_name){
      g_rec_mutex_unlock(note_mutex);

      return;
    }

    if(note->note_name != NULL){
      g_free(note->note_name);
    }

    note->note_name = g_strdup(note_name);

    g_rec_mutex_unlock(note_mutex);
  }
  break;
  case PROP_FREQUENCY:
  {
    g_rec_mutex_lock(note_mutex);

    note->frequency = g_value_get_double(value);

    g_rec_mutex_unlock(note_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_note_get_property(GObject *gobject,
		      guint prop_id,
		      GValue *value,
		      GParamSpec *param_spec)
{
  AgsNote *note;

  GRecMutex *note_mutex;

  note = AGS_NOTE(gobject);

  /* get note mutex */
  note_mutex = AGS_NOTE_GET_OBJ_MUTEX(note);

  switch(prop_id){
  case PROP_X0:
  {
    g_rec_mutex_lock(note_mutex);

    g_value_set_uint(value, note->x[0]);

    g_rec_mutex_unlock(note_mutex);
  }
  break;
  case PROP_X1:
  {
    g_rec_mutex_lock(note_mutex);

    g_value_set_uint(value, note->x[1]);

    g_rec_mutex_unlock(note_mutex);
  }
  break;
  case PROP_Y:
  {
    g_rec_mutex_lock(note_mutex);

    g_value_set_uint(value, note->y);

    g_rec_mutex_unlock(note_mutex);
  }
  break;
  case PROP_RT_OFFSET:
  {
    g_rec_mutex_lock(note_mutex);

    g_value_set_uint64(value, note->rt_offset);

    g_rec_mutex_unlock(note_mutex);
  }
  break;
  case PROP_RT_ATTACK:
  {
    g_rec_mutex_lock(note_mutex);

    g_value_set_uint(value, note->rt_attack);

    g_rec_mutex_unlock(note_mutex);
  }
  break;
  case PROP_STREAM_DELAY:
  {
    g_rec_mutex_lock(note_mutex);

    g_value_set_double(value, note->stream_delay);

    g_rec_mutex_unlock(note_mutex);
  }
  break;
  case PROP_STREAM_ATTACK:
  {
    g_rec_mutex_lock(note_mutex);

    g_value_set_double(value, note->stream_attack);

    g_rec_mutex_unlock(note_mutex);
  }
  break;
  case PROP_STREAM_FRAME_COUNT:
  {
    g_rec_mutex_lock(note_mutex);

    g_value_set_uint64(value, note->stream_frame_count);

    g_rec_mutex_unlock(note_mutex);
  }
  break;
  case PROP_ATTACK:
  {
    g_rec_mutex_lock(note_mutex);

    g_value_set_boxed(value, &(note->attack));

    g_rec_mutex_unlock(note_mutex);
  }
  break;
  case PROP_SUSTAIN:
  {
    g_rec_mutex_lock(note_mutex);

    g_value_set_boxed(value, &(note->sustain));

    g_rec_mutex_unlock(note_mutex);
  }
  break;
  case PROP_DECAY:
  {
    g_rec_mutex_lock(note_mutex);

    g_value_set_boxed(value, &(note->decay));

    g_rec_mutex_unlock(note_mutex);
  }
  break;
  case PROP_RELEASE:
  {
    g_rec_mutex_lock(note_mutex);

    g_value_set_boxed(value, &(note->release));

    g_rec_mutex_unlock(note_mutex);
  }
  break;
  case PROP_RATIO:
  {
    g_rec_mutex_lock(note_mutex);

    g_value_set_boxed(value, &(note->ratio));

    g_rec_mutex_unlock(note_mutex);
  }
  break;
  case PROP_NOTE_NAME:
  {
    g_rec_mutex_lock(note_mutex);

    g_value_set_string(value, note->note_name);

    g_rec_mutex_unlock(note_mutex);
  }
  break;
  case PROP_FREQUENCY:
  {
    g_rec_mutex_lock(note_mutex);

    g_value_set_double(value, note->frequency);

    g_rec_mutex_unlock(note_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_note_finalize(GObject *gobject)
{
  AgsNote *note;

  note = AGS_NOTE(gobject);
  
  /* note name */
  if(note->note_name != NULL){
    free(note->note_name);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_note_parent_class)->finalize(gobject);
}

/**
 * ags_note_get_obj_mutex:
 * @note: the #AgsNote
 * 
 * Get object mutex.
 * 
 * Returns: the #GRecMutex to lock @note
 * 
 * Since: 3.1.0
 */
GRecMutex*
ags_note_get_obj_mutex(AgsNote *note)
{
  if(!AGS_IS_NOTE(note)){
    return(NULL);
  }

  return(AGS_NOTE_GET_OBJ_MUTEX(note));
}

/**
 * ags_note_test_flags:
 * @note: the #AgsNote
 * @flags: the flags
 * 
 * Test @flags to be set on @note.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 * 
 * Since: 3.0.0
 */
gboolean
ags_note_test_flags(AgsNote *note, AgsNoteFlags flags)
{
  gboolean retval;
  
  GRecMutex *note_mutex;

  if(!AGS_IS_NOTE(note)){
    return(FALSE);
  }
      
  /* get note mutex */
  note_mutex = AGS_NOTE_GET_OBJ_MUTEX(note);

  /* test */
  g_rec_mutex_lock(note_mutex);

  retval = (flags & (note->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(note_mutex);

  return(retval);
}

/**
 * ags_note_set_flags:
 * @note: the #AgsNote
 * @flags: the flags
 * 
 * Set @flags on @note.
 * 
 * Since: 3.0.0
 */
void
ags_note_set_flags(AgsNote *note, AgsNoteFlags flags)
{
  GRecMutex *note_mutex;

  if(!AGS_IS_NOTE(note)){
    return;
  }
      
  /* get note mutex */
  note_mutex = AGS_NOTE_GET_OBJ_MUTEX(note);

  /* set */
  g_rec_mutex_lock(note_mutex);

  note->flags |= flags;
  
  g_rec_mutex_unlock(note_mutex);
}

/**
 * ags_note_unset_flags:
 * @note: the #AgsNote
 * @flags: the flags
 * 
 * Unset @flags on @note.
 * 
 * Since: 3.0.0
 */
void
ags_note_unset_flags(AgsNote *note, AgsNoteFlags flags)
{
  GRecMutex *note_mutex;

  if(!AGS_IS_NOTE(note)){
    return;
  }
      
  /* get note mutex */
  note_mutex = AGS_NOTE_GET_OBJ_MUTEX(note);

  /* unset */
  g_rec_mutex_lock(note_mutex);

  note->flags &= (~flags);
  
  g_rec_mutex_unlock(note_mutex);
}

/**
 * ags_note_sort_func:
 * @a: the #AgsNote
 * @b: another #AgsNote
 * 
 * Sort notes.
 * 
 * Returns: 0 if equal, -1 if smaller and 1 if bigger offset
 *
 * Since: 3.0.0
 */
gint
ags_note_sort_func(gconstpointer a,
		   gconstpointer b)
{
  guint a_x0, b_x0;
  guint a_y, b_y;
  
  if(a == NULL || b == NULL){
    return(0);
  }

  g_object_get(a,
	       "x0", &a_x0,
	       "y", &a_y,
	       NULL);
    
  g_object_get(b,
	       "x0", &b_x0,
	       "y", &b_y,
	       NULL);

  if(a_x0 == b_x0){
    if(a_y == b_y){
      return(0);
    }

    if(a_y < b_y){
      return(-1);
    }else{
      return(1);
    }
  }

  if(a_x0 < b_x0){
    return(-1);
  }else{
    return(1);
  }  
}

/**
 * ags_note_get_is_minor:
 * @note: the #AgsNote
 *
 * Gets is minor.
 * 
 * Returns: is minor
 * 
 * Since: 3.1.0
 */
gboolean
ags_note_get_is_minor(AgsNote *note)
{
  gboolean is_minor;
  
  if(!AGS_IS_NOTE(note)){
    return(FALSE);
  }

  g_object_get(note,
	       "is-minor", &is_minor,
	       NULL);

  return(is_minor);
}

/**
 * ags_note_set_is_minor:
 * @note: the #AgsNote
 * @is_minor: is minor
 *
 * Sets is minor.
 * 
 * Since: 3.1.0
 */
void
ags_note_set_is_minor(AgsNote *note, gboolean is_minor)
{
  if(!AGS_IS_NOTE(note)){
    return;
  }

  g_object_set(note,
	       "is-minor", is_minor,
	       NULL);
}

/**
 * ags_note_get_sharp_flats:
 * @note: the #AgsNote
 *
 * Gets sharp flats.
 * 
 * Returns: the sharp flats
 * 
 * Since: 3.1.0
 */
guint
ags_note_get_sharp_flats(AgsNote *note)
{
  guint sharp_flats;
  
  if(!AGS_IS_NOTE(note)){
    return(0);
  }

  g_object_get(note,
	       "sharp-flats", &sharp_flats,
	       NULL);

  return(sharp_flats);
}

/**
 * ags_note_set_sharp_flats:
 * @note: the #AgsNote
 * @sharp_flats: the sharp flats
 *
 * Sets sharp flats.
 * 
 * Since: 3.1.0
 */
void
ags_note_set_sharp_flats(AgsNote *note, guint sharp_flats)
{
  if(!AGS_IS_NOTE(note)){
    return;
  }

  g_object_set(note,
	       "sharp-flats", sharp_flats,
	       NULL);
}

/**
 * ags_note_get_x0:
 * @note: the #AgsNote
 *
 * Gets x0.
 * 
 * Returns: the sharp flats
 * 
 * Since: 3.1.0
 */
guint
ags_note_get_x0(AgsNote *note)
{
  guint x0;
  
  if(!AGS_IS_NOTE(note)){
    return(0);
  }

  g_object_get(note,
	       "x0", &x0,
	       NULL);

  return(x0);
}

/**
 * ags_note_set_x0:
 * @note: the #AgsNote
 * @x0: the x0
 *
 * Sets x0.
 * 
 * Since: 3.1.0
 */
void
ags_note_set_x0(AgsNote *note, guint x0)
{
  if(!AGS_IS_NOTE(note)){
    return;
  }

  g_object_set(note,
	       "x0", x0,
	       NULL);
}

/**
 * ags_note_get_x1:
 * @note: the #AgsNote
 *
 * Gets x1.
 * 
 * Returns: the x1
 * 
 * Since: 3.1.0
 */
guint
ags_note_get_x1(AgsNote *note)
{
  guint x1;
  
  if(!AGS_IS_NOTE(note)){
    return(0);
  }

  g_object_get(note,
	       "x1", &x1,
	       NULL);

  return(x1);
}

/**
 * ags_note_set_x1:
 * @note: the #AgsNote
 * @x1: the x1
 *
 * Sets x1.
 * 
 * Since: 3.1.0
 */
void
ags_note_set_x1(AgsNote *note, guint x1)
{
  if(!AGS_IS_NOTE(note)){
    return;
  }

  g_object_set(note,
	       "x1", x1,
	       NULL);
}

/**
 * ags_note_get_y:
 * @note: the #AgsNote
 *
 * Gets y.
 * 
 * Returns: the y
 * 
 * Since: 3.1.0
 */
guint
ags_note_get_y(AgsNote *note)
{
  guint y;
  
  if(!AGS_IS_NOTE(note)){
    return(0);
  }

  g_object_get(note,
	       "y", &y,
	       NULL);

  return(y);
}

/**
 * ags_note_set_y:
 * @note: the #AgsNote
 * @y: the y
 *
 * Sets y.
 * 
 * Since: 3.1.0
 */
void
ags_note_set_y(AgsNote *note, guint y)
{
  if(!AGS_IS_NOTE(note)){
    return;
  }

  g_object_set(note,
	       "y", y,
	       NULL);
}

/**
 * ags_note_get_rt_offset:
 * @note: the #AgsNote
 *
 * Gets rt-offset.
 * 
 * Returns: the rt-offset
 * 
 * Since: 3.1.0
 */
guint64
ags_note_get_rt_offset(AgsNote *note)
{
  guint64 rt_offset;
  
  if(!AGS_IS_NOTE(note)){
    return(0);
  }

  g_object_get(note,
	       "rt-offset", &rt_offset,
	       NULL);

  return(rt_offset);
}

/**
 * ags_note_set_rt_offset:
 * @note: the #AgsNote
 * @rt_offset: the rt-offset
 *
 * Sets rt-offset.
 * 
 * Since: 3.1.0
 */
void
ags_note_set_rt_offset(AgsNote *note, guint64 rt_offset)
{
  if(!AGS_IS_NOTE(note)){
    return;
  }

  g_object_set(note,
	       "rt-offset", rt_offset,
	       NULL);
}

/**
 * ags_note_get_rt_attack:
 * @note: the #AgsNote
 *
 * Gets rt-attack.
 * 
 * Returns: the rt-attack
 * 
 * Since: 3.1.0
 */
guint
ags_note_get_rt_attack(AgsNote *note)
{
  guint rt_attack;
  
  if(!AGS_IS_NOTE(note)){
    return(0);
  }

  g_object_get(note,
	       "rt-attack", &rt_attack,
	       NULL);

  return(rt_attack);
}

/**
 * ags_note_set_rt_attack:
 * @note: the #AgsNote
 * @rt_attack: the rt-attack
 *
 * Sets rt-attack.
 * 
 * Since: 3.1.0
 */
void
ags_note_set_rt_attack(AgsNote *note, guint rt_attack)
{
  if(!AGS_IS_NOTE(note)){
    return;
  }

  g_object_set(note,
	       "rt-attack", rt_attack,
	       NULL);
}

/**
 * ags_note_get_attack:
 * @note: the #AgsNote
 *
 * Gets attack.
 * 
 * Returns: the attack
 * 
 * Since: 3.1.0
 */
AgsComplex*
ags_note_get_attack(AgsNote *note)
{
  AgsComplex *attack;
  
  if(!AGS_IS_NOTE(note)){
    return(NULL);
  }

  g_object_get(note,
	       "attack", &attack,
	       NULL);

  return(attack);
}

/**
 * ags_note_set_attack:
 * @note: the #AgsNote
 * @attack: the attack
 *
 * Sets attack.
 * 
 * Since: 3.1.0
 */
void
ags_note_set_attack(AgsNote *note, AgsComplex *attack)
{
  if(!AGS_IS_NOTE(note)){
    return;
  }

  g_object_set(note,
	       "attack", attack,
	       NULL);
}

/**
 * ags_note_get_sustain:
 * @note: the #AgsNote
 *
 * Gets sustain.
 * 
 * Returns: the sustain
 * 
 * Since: 3.1.0
 */
AgsComplex*
ags_note_get_sustain(AgsNote *note)
{
  AgsComplex *sustain;
  
  if(!AGS_IS_NOTE(note)){
    return(NULL);
  }

  g_object_get(note,
	       "sustain", &sustain,
	       NULL);

  return(sustain);
}

/**
 * ags_note_set_sustain:
 * @note: the #AgsNote
 * @sustain: the sustain
 *
 * Sets sustain.
 * 
 * Since: 3.1.0
 */
void
ags_note_set_sustain(AgsNote *note, AgsComplex *sustain)
{
  if(!AGS_IS_NOTE(note)){
    return;
  }

  g_object_set(note,
	       "sustain", sustain,
	       NULL);
}

/**
 * ags_note_get_decay:
 * @note: the #AgsNote
 *
 * Gets decay.
 * 
 * Returns: the decay
 * 
 * Since: 3.1.0
 */
AgsComplex*
ags_note_get_decay(AgsNote *note)
{
  AgsComplex *decay;
  
  if(!AGS_IS_NOTE(note)){
    return(NULL);
  }

  g_object_get(note,
	       "decay", &decay,
	       NULL);

  return(decay);
}

/**
 * ags_note_set_decay:
 * @note: the #AgsNote
 * @decay: the decay
 *
 * Sets decay.
 * 
 * Since: 3.1.0
 */
void
ags_note_set_decay(AgsNote *note, AgsComplex *decay)
{
  if(!AGS_IS_NOTE(note)){
    return;
  }

  g_object_set(note,
	       "decay", decay,
	       NULL);
}

/**
 * ags_note_get_release:
 * @note: the #AgsNote
 *
 * Gets release.
 * 
 * Returns: the release
 * 
 * Since: 3.1.0
 */
AgsComplex*
ags_note_get_release(AgsNote *note)
{
  AgsComplex *release;
  
  if(!AGS_IS_NOTE(note)){
    return(NULL);
  }

  g_object_get(note,
	       "release", &release,
	       NULL);

  return(release);
}

/**
 * ags_note_set_release:
 * @note: the #AgsNote
 * @release: the release
 *
 * Sets release.
 * 
 * Since: 3.1.0
 */
void
ags_note_set_release(AgsNote *note, AgsComplex *release)
{
  if(!AGS_IS_NOTE(note)){
    return;
  }

  g_object_set(note,
	       "release", release,
	       NULL);
}

/**
 * ags_note_get_ratio:
 * @note: the #AgsNote
 *
 * Gets ratio.
 * 
 * Returns: the ratio
 * 
 * Since: 3.1.0
 */
AgsComplex*
ags_note_get_ratio(AgsNote *note)
{
  AgsComplex *ratio;
  
  if(!AGS_IS_NOTE(note)){
    return(NULL);
  }

  g_object_get(note,
	       "ratio", &ratio,
	       NULL);

  return(ratio);
}

/**
 * ags_note_set_ratio:
 * @note: the #AgsNote
 * @ratio: the ratio
 *
 * Sets ratio.
 * 
 * Since: 3.1.0
 */
void
ags_note_set_ratio(AgsNote *note, AgsComplex *ratio)
{
  if(!AGS_IS_NOTE(note)){
    return;
  }

  g_object_set(note,
	       "ratio", ratio,
	       NULL);
}

/**
 * ags_note_find_prev:
 * @note: (element-type AgsAudio.Note) (transfer none): the #GList-struct containing #AgsNote
 * @x0: x offset
 * @y:  y offset
 * 
 * Find prev note having the same y offset.
 *
 * Returns: (element-type AgsAudio.Note) (transfer none): the matching entry as #GList-struct if first entry's x offset bigger than @x0, else %NULL
 *
 * Since: 3.0.0
 */
GList*
ags_note_find_prev(GList *note,
		   guint x0, guint y)
{
  GList *current_match;

  guint current_x0, current_y;
  
  if(note == NULL){
    return(NULL);
  }

  g_object_get(note->data,
	       "x0", &current_x0,
	       NULL);
  
  if(current_x0 > x0){
    return(NULL);
  }

  current_match = NULL;
  
  while(note != NULL){
    g_object_get(note->data,
		 "y", &current_y,
		 NULL);
    
    if(current_y == y){
      current_match = note;

      note = note->next;
      
      continue;
    }

    g_object_get(note->data,
		 "x0", &current_x0,
		 NULL);

    if(current_x0 > x0){
      return(current_match);
    }

    note = note->next;
  }

  return(current_match);
}

/**
 * ags_note_find_next:
 * @note: (element-type AgsAudio.Note) (transfer none): the #GList-struct containing #AgsNote
 * @x0: x offset
 * @y:  y offset
 * 
 * Find next note having the same y offset.
 *
 * Returns: (element-type AgsAudio.Note) (transfer none): the matching entry as #GList-struct if last entry's x offset smaller than @x0, else %NULL
 *
 * Since: 3.0.0
 */
GList*
ags_note_find_next(GList *note,
		   guint x0, guint y)
{
  GList *current_match;

  guint current_x0, current_y;

  if(note == NULL){
    return(NULL);
  }
  
  g_object_get(note->data,
	       "x0", &current_x0,
	       NULL);
  
  if(current_x0 < x0){
    return(NULL);
  }

  current_match = NULL;

  while(note != NULL){
    g_object_get(note->data,
		 "y", &current_y,
		 NULL);
    
    if(current_y == y){
      current_match = note;

      note = note->prev;
      
      continue;
    }

    g_object_get(note->data,
		 "x0", &current_x0,
		 NULL);

    if(current_x0 < x0){
      return(current_match);
    }

    note = note->prev;
  }

  return(current_match);
}

/**
 * ags_note_length_to_smf_delta_time:
 * @note_length: the note length to convert
 * @bpm: the source bpm
 * @delay_factor: the source delay factor
 * @nn: numerator
 * @dd: denominator
 * @cc: clocks
 * @bb: beats
 * @tempo: tempo
 * 
 * Convert note length to SMF delta-time.
 * 
 * Returns: the delta-time
 * 
 * Since: 3.0.0
 */
glong
ags_note_length_to_smf_delta_time(guint note_length,
				  gdouble bpm, gdouble delay_factor,
				  glong nn, glong dd, glong cc, glong bb,
				  glong tempo)
{
  //TODO:JK: implement me

  return(0);
}

/**
 * ags_note_smf_delta_time_to_length:
 * @delta_time: delta-time
 * @nn: numerator
 * @dd: denominator
 * @cc: clocks
 * @bb: beats
 * @tempo: tempo
 * @bpm: the target bpm
 * @delay_factor: the target delay factor
 * 
 * Convert SMF delta-time to note length.
 * 
 * Returns: the note length
 * 
 * Since: 3.0.0
 */
guint
ags_note_smf_delta_time_to_length(glong delta_time,
				  glong nn, glong dd, glong cc, glong bb,
				  glong tempo,
				  gdouble bpm, gdouble delay_factor)
{
  //TODO:JK: implement me

  return(0);
}

/**
 * ags_note_to_raw_midi:
 * @note: the #AgsNote
 * @bpm: the bpm to use
 * @delay_factor: the segmentation delay factor
 * @buffer_length: the length of the returned buffer
 * 
 * Convert @note to raw MIDI and set the buffer length of returned bytes
 * in the array as @buffer_length.
 *
 * Returns: The sequencer raw midi as array.
 *
 * Since: 3.0.0
 */
guchar*
ags_note_to_raw_midi(AgsNote *note,
		     gdouble bpm, gdouble delay_factor,
		     guint *buffer_length)
{
  guchar *raw_midi;
  guint length;
  guint current_length;
  long delta_time;
  guint delta_time_length;
  guchar status;
  int channel;
  int key;
  int velocity;
  int pressure;
  gdouble ticks_per_beat;
  guint i, i_stop;
  guint j;
  guint k;
  
  if(note == NULL){
    if(buffer_length != NULL){
      *buffer_length = 0;
    }
    
    return(NULL);
  }

  length = 0;

  /* key-on */
  k = 0;
  
  /* delta-time */
  delta_time = note->x[0] / 16.0 / bpm * 60.0 / ((AGS_USEC_PER_SEC * bpm / 4.0) / (4.0 * bpm) / AGS_USEC_PER_SEC);
  delta_time_length = 
    current_length = ags_midi_buffer_util_get_varlength_size(delta_time);

  /* status and channel */
  channel = 0;
  status = (0x90 | (0x7f & channel));
  current_length++;

  /* note / key */
  key = (0x7f & (note->y));
  current_length++;

  /* velocity */
  velocity = (0x7f & (guchar) (128 * (ags_complex_get(&(note->attack)))));
  current_length++;

  /* prepare buffer */
  raw_midi = (guchar *) malloc(current_length * sizeof(guchar));
  length += current_length;

  ags_midi_buffer_util_put_varlength(raw_midi,
				     delta_time);
  k += delta_time_length;
  
  raw_midi[k] = status;
  raw_midi[k + 1] = key;
  raw_midi[k + 2] = velocity;

  k += 3;

  /* key-pressure */
  ticks_per_beat = AGS_NOTE_DEFAULT_TICKS_PER_QUARTER_NOTE / 4.0 / delay_factor;

  if(ticks_per_beat > 2.0){
    i_stop = (note->x[1] - note->x[0]) * (ticks_per_beat - 2.0);

    for(i = 1; i <= i_stop; i++){
      /* delta-time */
      delta_time = (note->x[0] + i + 1)  / 16.0 / bpm * 60.0 / ((AGS_USEC_PER_SEC * bpm / 4.0) / (4.0 * bpm) / AGS_USEC_PER_SEC);
      delta_time_length = 
	current_length = ags_midi_buffer_util_get_varlength_size(delta_time);

      /* status and channel */
      channel = 0;
      status = (0x90 | (0x7f & channel));
      current_length++;

      /* note / key */
      key = (0x7f & (note->y));
      current_length++;

      /* pressure */
      //TODO:JK: verify
      pressure = (0x7f & (guchar) (128 * (((ags_complex_get(&(note->decay)) / i) - (i * ags_complex_get(&(note->sustain)))))));
      current_length++;

      /* prepare buffer */
      raw_midi = (guchar *) realloc(raw_midi,
				    current_length * sizeof(guchar));
      length += current_length;

      ags_midi_buffer_util_put_varlength(raw_midi,
					 delta_time);
      k += delta_time_length;
  
      raw_midi[k] = status;
      raw_midi[k + 1] = key;
      raw_midi[k + 2] = pressure;

      k += 3;
    }
  }

  /* key-off */
  /* delta-time */
  delta_time = note->x[1] / 16.0 / bpm * 60.0 / ((AGS_USEC_PER_SEC * bpm / 4.0) / (4.0 * bpm) / AGS_USEC_PER_SEC);
  delta_time_length = 
    current_length = ags_midi_buffer_util_get_varlength_size(delta_time);

  /* status and channel */
  channel = 0;
  status = (0x90 | (0x7f & channel));
  current_length++;

  /* note / key */
  key = (0x7f & (note->y));
  current_length++;

  /* velocity */
  velocity = (0x7f & (guchar) (128 * (ags_complex_get(&(note->attack)))));
  current_length++;

  /* prepare buffer */
  raw_midi = (guchar *) realloc(raw_midi,
				current_length * sizeof(guchar));
  length += current_length;

  ags_midi_buffer_util_put_varlength(raw_midi,
				     delta_time);
  k += delta_time_length;
  
  raw_midi[k] = status;
  raw_midi[k + 1] = key;
  raw_midi[k + 2] = velocity;

  /* return value */
  if(buffer_length != NULL){
    *buffer_length = length;
  }
  
  return(raw_midi);
}

/**
 * ags_note_to_raw_midi_extended:
 * @note: the #AgsNote
 * @bpm: the source bpm
 * @delay_factor: the source delay factor
 * @nn: numerator
 * @dd: denominator
 * @cc: clocks
 * @bb: beats
 * @tempo: tempo
 * @buffer_length: the return location of buffer length
 * 
 * Convert @note to raw-midi.
 * 
 * Returns: the raw-midi buffer
 * 
 * Since: 3.0.0
 */
guchar*
ags_note_to_raw_midi_extended(AgsNote *note,
			      gdouble bpm, gdouble delay_factor,
			      glong nn, glong dd, glong cc, glong bb,
			      glong tempo,
			      guint *buffer_length)
{
  //TODO:JK: implement me

  return(NULL);
}

/**
 * ags_note_to_seq_event:
 * @note: the #AgsNote
 * @bpm: the bpm to use
 * @delay_factor: the segmentation delay factor
 * @n_events: the count of events
 * 
 * Convert @note to ALSA sequencer events and set the number of events
 * in the array as @n_events.
 *
 * Returns: (type gpointer) (transfer none): The sequencer events as array.
 *
 * Since: 3.0.0
 */
snd_seq_event_t*
ags_note_to_seq_event(AgsNote *note,
		      gdouble bpm, gdouble delay_factor,
		      guint *n_events)
{
  snd_seq_event_t *event;
  
  event = NULL;
  
  //TODO:JK: implement me

  return(event);
}

/**
 * ags_note_to_seq_event_extended:
 * @note: the #AgsNote
 * @bpm: the source bpm
 * @delay_factor: the source delay factor
 * @nn: numerator
 * @dd: denominator
 * @cc: clocks
 * @bb: beats
 * @tempo: tempo
 * @n_events: the return location of event count
 * 
 * Convert @note to raw-midi.
 * 
 * Returns: (type gpointer) (transfer none): an array of snd_seq_event_t structs
 * 
 * Since: 3.0.0
 */
snd_seq_event_t*
ags_note_to_seq_event_extended(AgsNote *note,
			       gdouble bpm, gdouble delay_factor,
			       glong nn, glong dd, glong cc, glong bb,
			       glong tempo,
			       guint *n_events)
{
  snd_seq_event_t *event;
  
  event = NULL;
  
  //TODO:JK: implement me

  return(event);
}

/**
 * ags_note_from_raw_midi:
 * @raw_midi: the data array
 * @bpm: the bpm to use
 * @delay_factor: the segmentation delay factor
 * @length: the length of the array
 *
 * Parse @raw_midi data and convert to #AgsNote.
 *
 * Returns: (element-type AgsAudio.Note) (transfer full): a #GList-struct containing the notes
 *
 * Since: 3.0.0
 */
GList*
ags_note_from_raw_midi(guchar *raw_midi,
		       gdouble bpm, gdouble delay_factor,
		       guint length)
{
  GList *list;

  list = NULL;

  //TODO:JK: implement me
  
  return(list);
}

/**
 * ags_note_from_raw_midi_extended:
 * @raw_midi: the data array
 * @nn: numerator
 * @dd: denominator
 * @cc: clocks
 * @bb: beats
 * @tempo: tempo
 * @bpm: the bpm to use
 * @delay_factor: the segmentation delay factor
 * @length: the length of the array
 *
 * Parse @raw_midi data and convert to #AgsNote.
 *
 * Returns: (element-type AgsAudio.Note) (transfer full): a #GList-struct containing the notes
 *
 * Since: 3.0.0
 */
GList*
ags_note_from_raw_midi_extended(guchar *raw_midi,
				glong nn, glong dd, glong cc, glong bb,
				glong tempo,
				gdouble bpm, gdouble delay_factor,
				guint length)
{
  GList *list;

  list = NULL;

  //TODO:JK: implement me
  
  return(list);
}

/**
 * ags_note_from_seq_event:
 * @event: (type gpointer) (transfer none): ALSA sequencer events as array
 * @bpm: the bpm to use
 * @delay_factor: the segmentation delay factor
 * @n_events: the arrays length
 *
 * Convert ALSA sequencer data @event to #AgsNote.
 *
 * Returns: (element-type AgsAudio.Note) (transfer full): a #GList-struct containing the notes
 *
 * Since: 3.0.0
 */
GList*
ags_note_from_seq_event(snd_seq_event_t *event,
			gdouble bpm, gdouble delay_factor,
			guint n_events)
{
  GList *list;

  list = NULL;

  //TODO:JK: implement me
  
  return(list);
}

/**
 * ags_note_from_seq_event_extended:
 * @event: (type gpointer) (transfer none): the snd_seq_event_t struct array
 * @nn: numerator
 * @dd: denominator
 * @cc: clocks
 * @bb: beats
 * @tempo: tempo
 * @bpm: the bpm to use
 * @delay_factor: the segmentation delay factor
 * @n_events: the count snd_seq_event_t structs
 *
 * Parse @raw_midi data and convert to #AgsNote.
 *
 * Returns: (element-type AgsAudio.Note) (transfer full): a #GList-struct containing the notes
 *
 * Since: 3.0.0
 */
GList*
ags_note_from_seq_event_extended(snd_seq_event_t *event,
				 glong nn, glong dd, glong cc, glong bb,
				 glong tempo,
				 gdouble bpm, gdouble delay_factor,
				 guint n_events)
{
  GList *list;

  list = NULL;

  //TODO:JK: implement me
  
  return(list);
}

/**
 * ags_note_duplicate:
 * @note: the #AgsNote
 * 
 * Duplicate a note.
 *
 * Returns: (transfer full): the duplicated #AgsNote.
 *
 * Since: 3.0.0
 */
AgsNote*
ags_note_duplicate(AgsNote *note)
{
  AgsNote *note_copy;

  GRecMutex *note_mutex;

  if(!AGS_IS_NOTE(note)){
    return(NULL);
  }
  
  /* get note mutex */
  note_mutex = AGS_NOTE_GET_OBJ_MUTEX(note);

  /* instantiate note */  
  note_copy = ags_note_new();

  note_copy->flags = 0;

  g_rec_mutex_lock(note_mutex);

  note_copy->note_name = g_strdup(note->note_name);
  
  if(note->x[0] < note->x[1]){
    note_copy->x[0] = note->x[0];
    note_copy->x[1] = note->x[1];
  }else{
    note_copy->x[0] = note->x[1];
    note_copy->x[1] = note->x[0];
  }
  
  note_copy->y = note->y;

  note_copy->stream_delay = note->stream_delay;
  note_copy->stream_attack = note->stream_attack;
  
  note_copy->attack.real = note->attack.real;
  note_copy->attack.imag = note->attack.imag;

  note_copy->decay.real = note->decay.real;
  note_copy->decay.imag = note->decay.imag;

  note_copy->sustain.real = note->sustain.real;
  note_copy->sustain.imag = note->sustain.imag;

  note_copy->release.real = note->release.real;
  note_copy->release.imag = note->release.imag;

  note_copy->ratio.real = note->ratio.real;
  note_copy->ratio.imag = note->ratio.imag;

  g_rec_mutex_unlock(note_mutex);

  return(note_copy);
}

/**
 * ags_note_new:
 *
 * Creates a new instance of #AgsNote
 *
 * Returns: the new #AgsNote
 *
 * Since: 3.0.0
 */
AgsNote*
ags_note_new()
{
  AgsNote *note;

  note = (AgsNote *) g_object_new(AGS_TYPE_NOTE,
				  NULL);

  return(note);
}

/**
 * ags_note_new_with_offset:
 * @x0: x0
 * @x1: x1
 * @y: y
 * @stream_delay: delay
 * @stream_attack: attack
 *
 * Creates a new instance of #AgsNote
 *
 * Returns: the new #AgsNote
 *
 * Since: 3.0.0
 */
AgsNote*
ags_note_new_with_offset(guint x0, guint x1,
			 guint y,
			 gdouble stream_delay, gdouble stream_attack)
{
  AgsNote *note;

  note = (AgsNote *) g_object_new(AGS_TYPE_NOTE,
				  "x0", x0,
				  "x1", x1,
				  "y", y,
				  "stream-delay", stream_delay,
				  "stream-attack", stream_attack,
				  NULL);

  return(note);
}
