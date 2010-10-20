#ifndef __AGS_SYNTH_H__
#define __AGS_SYNTH_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "ags_devout.h"

void ags_synth_sin(AgsDevout *devout, short *buffer, guint offset,
		   guint freq, guint phase, guint length,
		   double volume);

void ags_synth_saw(AgsDevout *devout, short *buffer, guint offset,
		   guint freq, guint phase, guint length,
		   double volume);
void ags_synth_triangle(AgsDevout *devout, short *buffer, guint offset,
			guint freq, guint phase, guint length,
			double volume);
void ags_synth_square(AgsDevout *devout, short *buffer, guint offset,
		      guint freq, guint phase, guint length,
		      double volume);

#endif /*__AGS_SYNTH_H__*/
