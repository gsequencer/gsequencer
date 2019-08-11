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

#include <ags/audio/ags_filter_util.h>

#include <ags/audio/ags_audio_buffer_util.h>

#include <math.h>

void
ags_filter_util_lowpass_s8(gint8 *buffer,
			   guint buffer_size,
			   guint samplerate)
{
}

void
ags_filter_util_lowpass_s16(gint16 *buffer,
			    guint buffer_size,
			    guint samplerate)
{
}

void
ags_filter_util_lowpass_s24(gint32 *buffer,
			    guint buffer_size,
			    guint samplerate)
{
}

void
ags_filter_util_lowpass_s32(gint32 *buffer,
			    guint buffer_size,
			    guint samplerate)
{
}

void
ags_filter_util_lowpass_s64(gint64 *buffer,
			    guint buffer_size,
			    guint samplerate)
{
}

void
ags_filter_util_lowpass_float(gfloat *buffer,
			      guint buffer_size,
			      guint samplerate)
{
}

void
ags_filter_util_lowpass_double(gdouble *buffer,
			       guint buffer_size,
			       guint samplerate)
{
}

void
ags_filter_util_pitch_s8(gint8 *buffer,
			 guint buffer_size,
			 guint samplerate,
			 gdouble base_key,
			 gdouble tuning)
{
}

void
ags_filter_util_pitch_s16(gint16 *buffer,
			  guint buffer_size,
			  guint samplerate,
			  gdouble base_key,
			  gdouble tuning)
{
  gdouble base_freq, new_freq;
  gdouble offset_factor;
  gdouble t;
  guint i, j;

  auto gint16 ags_filter_util_pitch_s16_cubic_interpolate(gint16 x0, gint16 x1, gint16 x2, gint16 x3,
							  gdouble t);
  
  gint16 ags_filter_util_pitch_s16_cubic_interpolate(gint16 x0, gint16 x1, gint16 x2, gint16 x3,
						     gdouble t)
  {
    gint32 a0, a1, a2, a3;
    gint16 retval;

    a0 = x3 - x2 - x0 + x1;
    a1 = x0 - x1 - a0;
    a2 = x2 - x0;
    a3 = x1;

    retval = a0 * (t * t * t) + a1 * (t * t) + (a2 * t) + a3;

    return(retval);
  }
  
  base_freq = 27.5 * exp2((base_key) / 12.0);
  new_freq = 27.5 * exp2((base_key + (tuning / 100.0)) / 12.0);

  offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / new_freq);  
  
  if(new_freq > base_freq){
    for(i = 0; i < buffer_size; i++){
      t = offset_factor;
      j = i;
      
      buffer[i] = ags_filter_util_pitch_s16_cubic_interpolate(buffer[j],
							      buffer[j + 1],
							      buffer[j + 2],
							      buffer[j + 3],
							      t);
    }
  }else{
    for(i = 0; i < buffer_size; i++){
      t = 1.0 / offset_factor;
      j = i;
      
      buffer[i] = ags_filter_util_pitch_s16_cubic_interpolate(buffer[j],
							      buffer[j + 1],
							      buffer[j + 2],
							      buffer[j + 3],
							      t);
    }
  }
}

void
ags_filter_util_pitch_s24(gint32 *buffer,
			  guint buffer_size,
			  guint samplerate,
			  gdouble base_key,
			  gdouble tuning)
{
}

void
ags_filter_util_pitch_s32(gint32 *buffer,
			  guint buffer_size,
			  guint samplerate,
			  gdouble base_key,
			  gdouble tuning)
{
}

void
ags_filter_util_pitch_s64(gint64 *buffer,
			  guint buffer_size,
			  guint samplerate,
			  gdouble base_key,
			  gdouble tuning)
{
}

void
ags_filter_util_pitch_float(gfloat *buffer,
			    guint buffer_size,
			    guint samplerate,
			    gdouble base_key,
			    gdouble tuning)
{
}

void
ags_filter_util_pitch_double(gdouble *buffer,
			     guint buffer_size,
			     guint samplerate,
			     gdouble base_key,
			     gdouble tuning)
{
}
