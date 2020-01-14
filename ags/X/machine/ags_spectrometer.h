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

#ifndef __AGS_SPECTROMETER_H__
#define __AGS_SPECTROMETER_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/X/ags_machine.h>

G_BEGIN_DECLS

#define AGS_TYPE_SPECTROMETER                (ags_spectrometer_get_type())
#define AGS_SPECTROMETER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SPECTROMETER, AgsSpectrometer))
#define AGS_SPECTROMETER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SPECTROMETER, AgsSpectrometerClass))
#define AGS_IS_SPECTROMETER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_SPECTROMETER))
#define AGS_IS_SPECTROMETER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_SPECTROMETER))
#define AGS_SPECTROMETER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_SPECTROMETER, AgsSpectrometerClass))

#define AGS_SPECTROMETER_DEFAULT_X_START (AGS_CARTESIAN_DEFAULT_X_START)
#define AGS_SPECTROMETER_DEFAULT_X_END (859.0)

#define AGS_SPECTROMETER_DEFAULT_Y_START (AGS_CARTESIAN_DEFAULT_Y_START)
#define AGS_SPECTROMETER_DEFAULT_Y_END (239.0)

#define AGS_SPECTROMETER_PLOT_DEFAULT_POINT_COUNT (192)
#define AGS_SPECTROMETER_EXTRA_SCALE (5.0)

typedef struct _AgsSpectrometer AgsSpectrometer;
typedef struct _AgsSpectrometerClass AgsSpectrometerClass;

struct _AgsSpectrometer
{
  AgsMachine machine;

  gchar *name;
  gchar *xml_type;

  AgsCartesian *cartesian;
  GList *fg_plot;

  GList *frequency_buffer_play_port;
  GList *frequency_buffer_recall_port;

  GList *magnitude_buffer_play_port;
  GList *magnitude_buffer_recall_port;

  guint buffer_size;
  
  double *frequency_buffer;
  double *magnitude_buffer;
};

struct _AgsSpectrometerClass
{
  AgsMachineClass machine;
};

GType ags_spectrometer_get_type(void);

AgsPort* ags_spectrometer_find_specifier(GList *recall, gchar *specifier);

AgsPlot* ags_spectrometer_fg_plot_alloc(AgsSpectrometer *spectrometer,
					gdouble color_r, gdouble color_g, double color_b);

gboolean ags_spectrometer_cartesian_queue_draw_timeout(GtkWidget *widget);

AgsSpectrometer* ags_spectrometer_new(GObject *soundcard);

G_END_DECLS

#endif /*__AGS_SPECTROMETER_H__*/
