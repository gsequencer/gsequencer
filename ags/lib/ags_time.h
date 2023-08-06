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

#ifndef __AGS_TIME__
#define __AGS_TIME__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define AGS_TIME_ZERO "0000:00.000"

#define AGS_USEC_PER_SEC    (1000000) /* The number of msecs per sec. */
#define AGS_NSEC_PER_SEC    (1000000000) /* The number of nsecs per sec. */

void ags_time_nanosleep(struct timespec *req);
gchar* ags_time_get_uptime_from_offset(guint offset,
				       gdouble bpm,
				       gdouble delay,
				       gdouble delay_factor);

gboolean ags_time_timeout_expired(struct timespec *start_time,
				  struct timespec *timeout_delay);

G_END_DECLS

#endif /*__AGS_TIME__*/
