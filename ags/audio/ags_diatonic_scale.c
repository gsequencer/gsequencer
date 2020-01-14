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

#include <ags/audio/ags_diatonic_scale.h>

#include <string.h>

/**
 * SECTION:ags_diatonic_scale
 * @short_description: diatonic scale util
 * @title: AgsDiatonicScale
 * @section_id:
 * @include: ags/audio/ags_diatonic_scale.h
 *
 * Utility functions to handle diatonic scale.
 */

/**
 * ags_diatonic_scale_note_to_midi_key:
 * @note: the note name
 * @key: return location of MIDI key
 * 
 * Convert @note to MIDI key and store in @key.
 * 
 * Returns: the number of chars read
 * 
 * Since: 3.0.0
 */
guint
ags_diatonic_scale_note_to_midi_key(gchar *note,
				    glong *key)
{
  guint char_count;
  gint position;
  glong x_key;
  int retval;
  gboolean success;
  
  if(note == NULL ||
     note[0] == '\0' ||
     key == NULL){
    return(0);
  }

  x_key = 49;
  
  char_count = 0;

  success = FALSE;
  
  switch(note[0]){
  case 'a':
  {
    success = TRUE;
    
    x_key = 49;
  }
  break;
  case 'h':
  case 'b':
  {
    success = TRUE;
    
    x_key = 51;
  }
  break;
  case 'c':
  {
    success = TRUE;
    
    x_key = 52;
  }
  break;
  case 'd':
  {
    success = TRUE;
    
    x_key = 54;
  }
  break;
  case 'e':
  {
    success = TRUE;
    
    x_key = 56;
  }
  break;
  case 'f':
  {
    success = TRUE;
    
    x_key = 57;
  }
  break;
  case 'g':
  {
    success = TRUE;
    
    x_key = 59;
  }
  break;
  }

  if(success){
    char_count = 1;
  }
  
  if(!success ||
     note[1] == '\0'){
    goto ags_diatonic_scale_note_to_midi_key_COMPLETED;
  }

  success = FALSE;
  
  switch(note[1]){
  case '#':
  {
    success = TRUE;
    
    x_key += 1;
  }
  break;
  case 'b':
  {
    success = TRUE;
    
    x_key -= 1;
  }
  break;
  }

  if(success){
    char_count = 2;
  }

  if(!success ||
     note[2] == '\0'){
    goto ags_diatonic_scale_note_to_midi_key_COMPLETED;
  }

  success = FALSE;

  position = -1;

  retval = sscanf(note + 2, "%d", &position);

  if(retval > 0){
    if(position > 49){
      x_key += ((position - 1) * 12) - 48;
    }else if(position < 49){
      x_key += ((position - 1) * 12) - 48;
    }

    char_count += retval;
  }

ags_diatonic_scale_note_to_midi_key_COMPLETED:
  key[0] = x_key;

  return(char_count);
}

/**
 * ags_diatonic_scale_midi_key_to_note:
 * @key: MIDI key
 * @note: return location of note name
 * 
 * Convert MIDI @key to note name and store in @note.
 * 
 * Returns: the number of chars written
 * 
 * Since: 3.0.0
 */
guint
ags_diatonic_scale_midi_key_to_note(glong key,
				    gchar **note)
{
  gchar *x_note;
  gchar *str;

  guint char_count;
  gint sharp_flats;
  gint position;

  static const gchar *sharp = "#";
  
  if(key > 128 ||
     note == NULL){
    return(0);
  }

  x_note = NULL;
  
  sharp_flats = -1;
  position = -1;

  switch(key % 12){
  case 0:
  {
    x_note = "a";
  }
  break;
  case 1:
  {
    x_note = "a";
    sharp_flats = 1;
  }
  break;
  case 2:
  {
    x_note = "h";
  }
  break;
  case 3:
  {
    x_note = "c";
  }
  break;
  case 4:
  {
    x_note = "c";
    sharp_flats = 1;
  }
  break;
  case 5:
  {
    x_note = "d";
  }
  break;
  case 6:
  {
    x_note = "d";
    sharp_flats = 1;
  }
  break;
  case 7:
  {
    x_note = "e";
  }
  break;
  case 8:
  {
    x_note = "f";
  }
  break;
  case 9:
  {
    x_note = "f";
    sharp_flats = 1;
  }
  break;
  case 10:
  {
    x_note = "g";
  }
  case 11:
  {
    x_note = "g";
    sharp_flats = 1;
  }
  break;
  break;
  }

  position = floor((gdouble) key / 12.0) + 1;
  
  if(sharp_flats != -1 &&
     position != -1){
    str = g_strdup_printf("%s%s%d",
			  x_note,
			  sharp,
			  position);
  }else if(position != -1){
    str = g_strdup_printf("%s%d",
			  x_note,
			  position);
  }else{
    str = g_strdup_printf("%s",
			  x_note);
  }

  char_count = strlen(str);
  note[0] = str;

  return(char_count);
}
