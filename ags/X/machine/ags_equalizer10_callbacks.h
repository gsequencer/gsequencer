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

#ifndef __AGS_EQUALIZER10_CALLBACKS_H__
#define __AGS_EQUALIZER10_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/X/machine/ags_equalizer10.h>

G_BEGIN_DECLS

void ags_equalizer10_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsEqualizer10 *equalizer10);

void ags_equalizer10_resize_audio_channels_callback(AgsEqualizer10 *equalizer10,
						    guint audio_channels, guint audio_channels_old,
						    gpointer data);
void ags_equalizer10_resize_pads_callback(AgsEqualizer10 *equalizer10,
					  GType channel_type,
					  guint pads, guint pads_old,
					  gpointer data);

void ags_equalizer10_peak_28hz_callback(GtkRange *range,
					AgsEqualizer10 *equalizer10);
void ags_equalizer10_peak_56hz_callback(GtkRange *range,
					AgsEqualizer10 *equalizer10);
void ags_equalizer10_peak_112hz_callback(GtkRange *range,
					 AgsEqualizer10 *equalizer10);
void ags_equalizer10_peak_224hz_callback(GtkRange *range,
					 AgsEqualizer10 *equalizer10);
void ags_equalizer10_peak_448hz_callback(GtkRange *range,
					 AgsEqualizer10 *equalizer10);
void ags_equalizer10_peak_896hz_callback(GtkRange *range,
					 AgsEqualizer10 *equalizer10);
void ags_equalizer10_peak_1792hz_callback(GtkRange *range,
					  AgsEqualizer10 *equalizer10);
void ags_equalizer10_peak_3584hz_callback(GtkRange *range,
					  AgsEqualizer10 *equalizer10);
void ags_equalizer10_peak_7168hz_callback(GtkRange *range,
					  AgsEqualizer10 *equalizer10);
void ags_equalizer10_peak_14336hz_callback(GtkRange *range,
					   AgsEqualizer10 *equalizer10);

void ags_equalizer10_pressure_callback(GtkRange *range,
				       AgsEqualizer10 *equalizer10);

G_END_DECLS

#endif /*__AGS_EQUALIZER10_CALLBACKS_H__*/
