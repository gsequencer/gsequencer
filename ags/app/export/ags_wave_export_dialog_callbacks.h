/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#ifndef __AGS_WAVE_EXPORT_DIALOG_CALLBACKS_H__
#define __AGS_WAVE_EXPORT_DIALOG_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

#include <ags/app/export/ags_wave_export_dialog.h>

G_BEGIN_DECLS

void ags_wave_export_dialog_file_open_button_callback(GtkWidget *file_open_button,
						      AgsWaveExportDialog *wave_export_dialog);

void ags_wave_export_dialog_start_tact_callback(GtkSpinButton *spin_button, AgsWaveExportDialog *wave_export_dialog);
void ags_wave_export_dialog_end_tact_callback(GtkSpinButton *spin_button, AgsWaveExportDialog *wave_export_dialog);

void ags_wave_export_dialog_save_as_callback(GtkWidget *widget, AgsWaveExportDialog *wave_export_dialog);

G_END_DECLS

#endif /*__AGS_WAVE_EXPORT_DIALOG_CALLBACKS_H__*/
