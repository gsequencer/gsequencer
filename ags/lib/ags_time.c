/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/lib/ags_time.h>

#include <math.h>

/**
 * SECTION:ags_time
 * @short_description: utility functions of time
 * @title: AgsTime
 * @section_id:
 * @include: ags/lib/ags_time.h
 *
 * Functions to help you dealing with time.
 */

/**
 * ags_time_get_uptime_from_offset:
 * @offset: the offset in tics
 * @bpm: the beats per minute
 * @delay: the buffer time as delay
 * @delay_factor: tact segmentation
 *
 * Calculates uptime from @offset by applying factors @bpm, @delay and @delay_factor
 * giving you the result as string.
 *
 * Returns: the string containing the time, a minus sign is prepend if factors
 *   are invalid
 *
 * Since: 1.0.0
 */
gchar*
ags_time_get_uptime_from_offset(guint offset,
				gdouble bpm,
				gdouble delay,
				gdouble delay_factor)
{
  gchar *uptime;

  gdouble delay_min, delay_sec, delay_msec;
  gdouble tact_redux;
  guint min, sec, msec;
  
  if(bpm <= 0.0 ||
     delay <= 0.0 ||
     delay_factor <= 0.0){
    return(g_strdup_printf("-%s",
			   AGS_TIME_ZERO));
  }

  /* translate to time string */
  tact_redux = offset;

  delay_sec = 16.0 * delay_factor * bpm / 60.0;
  delay_min = delay_sec * 60.0;
  delay_msec = delay_sec / 1000.0;

  min = (guint) floor(tact_redux / delay_min);

  if(min > 0){
    tact_redux = tact_redux - (min * delay_min);
  }

  sec = (guint) floor(tact_redux / delay_sec);

  if(sec > 0){
    tact_redux = tact_redux - (sec * delay_sec);
  }

  msec = (guint) floor(tact_redux / delay_msec);

  uptime = g_strdup_printf("%.4d:%.2d.%.3d", min, sec, msec);

  return(uptime);
}
