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

#ifndef __AGS_EXPORT_WINDOW_H__
#define __AGS_EXPORT_WINDOW_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

#include <ags/app/ags_export_soundcard.h>

G_BEGIN_DECLS

#define AGS_TYPE_EXPORT_WINDOW                (ags_export_window_get_type())
#define AGS_EXPORT_WINDOW(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_EXPORT_WINDOW, AgsExportWindow))
#define AGS_EXPORT_WINDOW_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_EXPORT_WINDOW, AgsExportWindowClass))
#define AGS_IS_EXPORT_WINDOW(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_EXPORT_WINDOW))
#define AGS_IS_EXPORT_WINDOW_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_EXPORT_WINDOW))
#define AGS_EXPORT_WINDOW_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_EXPORT_WINDOW, AgsExportWindowClass))

typedef struct _AgsExportWindow AgsExportWindow;
typedef struct _AgsExportWindowClass AgsExportWindowClass;

typedef enum{
  AGS_EXPORT_WINDOW_LIVE_EXPORT        = 1,
  AGS_EXPORT_WINDOW_HAS_STOP_TIMEOUT   = 1 <<  1,
}AgsExportWindowFlags;

struct _AgsExportWindow
{
  GtkWindow window;

  guint flags;
  guint connectable_flags;

  gboolean do_stop;
  
  GtkCheckButton *live_export;
  GtkCheckButton *exclude_sequencer;

  GtkComboBoxText *mode;
  GtkSpinButton *tact;
  GtkLabel *duration;

  GList *export_soundcard;
  
  GtkBox *export_soundcard_box;
  GtkButton *add;

  GList *remove_filename;
  
  GtkToggleButton *export;
};

struct _AgsExportWindowClass
{
  GtkWindowClass window;
};

GType ags_export_window_get_type(void);

gboolean ags_export_window_test_flags(AgsExportWindow *export_window,
				      guint flags);
void ags_export_window_set_flags(AgsExportWindow *export_window,
				 guint flags);
void ags_export_window_unset_flags(AgsExportWindow *export_window,
				   guint flags);

GList* ags_export_window_get_export_soundcard(AgsExportWindow *export_window);
void ags_export_window_add_export_soundcard(AgsExportWindow *export_window,
					    AgsExportSoundcard *export_soundcard);
void ags_export_window_remove_export_soundcard(AgsExportWindow *export_window,
					       AgsExportSoundcard *export_soundcard);

void ags_export_window_reload_soundcard_editor(AgsExportWindow *export_window);

void ags_export_window_start_export(AgsExportWindow *export_window);
void ags_export_window_stop_export(AgsExportWindow *export_window);

AgsExportWindow* ags_export_window_new(GtkWindow *transient_for);

G_END_DECLS

#endif /*__AGS_EXPORT_WINDOW_H__*/
