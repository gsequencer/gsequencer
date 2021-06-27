/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#include <ags/audio/osc/ags_osc_util.h>

#include <math.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#ifdef __APPLE__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

gpointer ags_osc_util_copy(gpointer ptr);
void ags_osc_util_free(gpointer ptr);

/**
 * SECTION:ags_osc_util
 * @short_description: OSC util
 * @title: AgsOscUtil
 * @section_id:
 * @include: ags/audio/osc/ags_osc_util.h
 *
 * Utility functions for OSC.
 */

GType
ags_osc_util_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_osc_util = 0;

    ags_type_osc_util =
      g_boxed_type_register_static("AgsOscUtil",
				   (GBoxedCopyFunc) ags_osc_util_copy,
				   (GBoxedFreeFunc) ags_osc_util_free);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_osc_util);
  }

  return g_define_type_id__volatile;
}

gpointer
ags_osc_util_copy(gpointer ptr)
{
  gpointer retval;

  retval = g_memdup(ptr, sizeof(AgsOscUtil));
 
  return(retval);
}

void
ags_osc_util_free(gpointer ptr)
{
  g_free(ptr);
}

/**
 * ags_osc_util_type_tag_string_count_type:
 * @type_tag: the type tag string
 * 
 * Count types in @type_tag.
 * 
 * Returns: the count of types specified
 * 
 * Since: 3.0.0
 */
guint
ags_osc_util_type_tag_string_count_type(gchar *type_tag)
{
  gsize count;

  if(type_tag == NULL ||
     type_tag[0] != ','){
    return(0);
  }
  
  count = strlen(type_tag);

  if(count <= 0){
    return(0);
  }

  count -= 1;
  
  return(count);
}

/**
 * ags_osc_util_meta_data:
 * @uri: the URI
 * @meta_data_length: the return location of meta data's length
 * 
 * Get meta data.
 * 
 * Returns: the byte array containing meta data
 * 
 * Since: 3.0.0
 */
unsigned char*
ags_osc_util_meta_data(gchar *uri,
		       guint *meta_data_length)
{
  unsigned char *meta_data;
  
  meta_data = g_strdup_printf("framing=slip\nversion=1.0 | 1.1\nuri=%s\ntypes=%s\n",
			      uri,
			      AGS_OSC_UTIL_TYPE_TAG_STRING_ALL);

  if(meta_data_length != NULL){
    *meta_data_length = strlen(meta_data);
  }
  
  return(meta_data);
}

/**
 * ags_osc_util_mime_header:
 * @uri: the URI
 * @mime_header_length: the return location of MIME header's length
 * 
 * Get MIME header.
 * 
 * Returns: the byte array containing MIME header
 * 
 * Since: 3.0.0
 */
unsigned char*
ags_osc_util_mime_header(gchar *uri,
			 guint *mime_header_length)
{
  unsigned char *mime_header;
  
  mime_header = g_strdup_printf("MIME-Version: 1.0\nContent-type: application/osc;\nframing=slip\nversion=1.0 | 1.1\nuri=%s\ntypes=%s\n",
				uri,
				AGS_OSC_UTIL_TYPE_TAG_STRING_ALL);

  if(mime_header_length != NULL){
    *mime_header_length = strlen(mime_header);
  }
  
  return(mime_header);
}

/**
 * ags_osc_util_slip_encode:
 * @osc_buffer: the OSC buffer
 * @buffer_length: the OSC buffer's length
 * @returned_buffer_length: the returned buffer's length
 * 
 * Encode @osc_buffer to be suitable for network transmission.
 * See SLIP (RFC1055).
 * 
 * Returns: the encoded byte array
 * 
 * Since: 3.0.0
 */
unsigned char*
ags_osc_util_slip_encode(unsigned char *osc_buffer,
			 guint buffer_length,
			 guint *returned_buffer_length)
{
  unsigned char *slip_buffer;

  guint slip_buffer_length;
  guint i, j;
  
  slip_buffer_length = (guint) AGS_OSC_UTIL_SLIP_CHUNK_LENGTH + 2;
  
  slip_buffer = (unsigned char *) malloc((slip_buffer_length + 2) * sizeof(unsigned char));

  slip_buffer[0] = AGS_OSC_UTIL_SLIP_END;
  
  for(i = 0, j = 1; i < buffer_length; i++, j++){
    if(j + 2 >= slip_buffer_length){
      slip_buffer_length = slip_buffer_length + AGS_OSC_UTIL_SLIP_CHUNK_LENGTH;
      slip_buffer = (unsigned char *) realloc(slip_buffer,
					      (slip_buffer_length + AGS_OSC_UTIL_SLIP_CHUNK_LENGTH) * sizeof(unsigned char));
    }
    
    switch(osc_buffer[i]){
    case AGS_OSC_UTIL_SLIP_END:
      {
	slip_buffer[j] = AGS_OSC_UTIL_SLIP_ESC;
	j++;
	
	slip_buffer[j] = AGS_OSC_UTIL_SLIP_ESC_END;
      }
      break;
    case AGS_OSC_UTIL_SLIP_ESC:
      {
	slip_buffer[j] = AGS_OSC_UTIL_SLIP_ESC;
	j++;
	
	slip_buffer[j] = AGS_OSC_UTIL_SLIP_ESC_ESC;
      }
      break;
    default:
      {
	slip_buffer[j] = osc_buffer[i];
      }
    }
  }

  slip_buffer[j] = AGS_OSC_UTIL_SLIP_END;

  if(returned_buffer_length != NULL){
    *returned_buffer_length = j + 1;
  }
  
  return(slip_buffer);
}

/**
 * ags_osc_util_slip_decode:
 * @slip_buffer: the SLIP encoded OSC buffer
 * @slip_buffer_length: the buffer length of SLIP encoded OSC buffer
 * @returned_buffer_length: the returned buffer's length
 * 
 * Decode @slip_buffer from SLIP encoded format.
 * See SLIP (RFC1055).
 * 
 * Returns: the OSC buffer as byte array
 * 
 * Since: 3.0.0
 */
unsigned char*
ags_osc_util_slip_decode(unsigned char *slip_buffer,
			 guint slip_buffer_length,
			 guint *returned_buffer_length)
{
  unsigned char *osc_buffer;

  guint buffer_length;
  guint i, j;

  buffer_length = slip_buffer_length;

  osc_buffer = (unsigned char *) malloc(buffer_length * sizeof(unsigned char));

  for(i = 0, j = 1; j < slip_buffer_length; i++, j++){
    switch(slip_buffer[j]){
    case AGS_OSC_UTIL_SLIP_ESC:
      {
	j++;

	if(slip_buffer[j] == AGS_OSC_UTIL_SLIP_ESC_END){
	  osc_buffer[i] = AGS_OSC_UTIL_SLIP_END;
	}else if(slip_buffer[j] == AGS_OSC_UTIL_SLIP_ESC_ESC){
	  osc_buffer[i] = AGS_OSC_UTIL_SLIP_ESC;
	}
      }
      break;
    default:
      {
	osc_buffer[i] = slip_buffer[j];
      }
    }
  }

  if(returned_buffer_length != NULL){
    *returned_buffer_length = i - 1;
  }
  
  return(osc_buffer);
}

/**
 * ags_osc_util_timetag_now:
 * @tv_sec: the return location of number of seconds since midnight on January 1, 1900
 * @tv_fraction: the return location of fraction of seconds to a precision of about 200 picoseconds
 * 
 * Get current time.
 * 
 * Since: 3.0.0
 */
void
ags_osc_util_timetag_now(gint32 *tv_sec, gint32 *tv_fraction)
{
  struct timespec time_now;

#ifdef __APPLE__
  clock_serv_t cclock;
  mach_timespec_t mts;
#endif
  
  static const guint sec_since_1900_to_1970 = 2208988800;
  
#ifdef __APPLE__
  host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
      
  clock_get_time(cclock, &mts);
  mach_port_deallocate(mach_task_self(), cclock);
      
  time_now.tv_sec = mts.tv_sec;
  time_now.tv_nsec = mts.tv_nsec;
#else
  clock_gettime(CLOCK_MONOTONIC, &time_now);
#endif

  if(tv_sec != NULL){
    tv_sec[0] = time_now.tv_sec + 2208988800;
  }

  if(tv_fraction != NULL){
    tv_fraction[0] = 0xfffffffe & (guint) (time_now.tv_nsec * 4.294967296);
  }
}
