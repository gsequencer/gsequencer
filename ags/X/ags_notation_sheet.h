/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#ifndef __AGS_NOTATION_SHEET_H__
#define __AGS_NOTATION_SHEET_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/X/ags_machine.h>

#include <ags/X/editor/ags_notation_toolbar.h>
#include <ags/X/editor/ags_machine_selector.h>
//#include <ags/X/editor/ags_notation_page.h>

#define AGS_TYPE_NOTATION_SHEET                (ags_notation_sheet_get_type ())
#define AGS_NOTATION_SHEET(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_NOTATION_SHEET, AgsNotationSheet))
#define AGS_NOTATION_SHEET_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_NOTATION_SHEET, AgsNotationSheetClass))
#define AGS_IS_NOTATION_SHEET(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_NOTATION_SHEET))
#define AGS_IS_NOTATION_SHEET_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_NOTATION_SHEET))
#define AGS_NOTATION_SHEET_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_NOTATION_SHEET, AgsNotationSheetClass))
  
#define AGS_NOTATION_SHEET_DEFAULT_VERSION "2.0.0"
#define AGS_NOTATION_SHEET_DEFAULT_BUILD_ID "Sat Feb  3 16:15:50 UTC 2018"

#define AGS_NOTATION_SHEET_MAX_CONTROLS (16 * 16 * 1200)

typedef struct _AgsNotationSheet AgsNotationSheet;
typedef struct _AgsNotationSheetClass AgsNotationSheetClass;

typedef enum{
  AGS_NOTATION_SHEET_CONNECTED                    = 1,
  AGS_NOTATION_SHEET_PATTERN_MODE                 = 1 <<  1,
  AGS_NOTATION_SHEET_PASTE_MATCH_AUDIO_CHANNEL    = 1 <<  2,
  AGS_NOTATION_SHEET_PASTE_NO_DUPLICATES          = 1 <<  3,
}AgsNotationSheetFlags;

struct _AgsNotationSheet
{
  GtkVBox vbox;

  guint flags;

  gchar *version;
  gchar *build_id;

  guint tact_counter;
  gdouble current_tact;

  guint chunk_duration;

  GObject *soundcard;
  
  GtkHPaned *paned;

  AgsNotationToolbar *notation_toolbar;
  
  AgsMachineSelector *machine_selector;
  AgsMachine *selected_machine;
  
  AgsNotebook *notebook;

  AgsScrolledPiano *scrolled_piano;
  
  //  AgsNotationPage *notation_page;
};

struct _AgsNotationSheetClass
{
  GtkVBoxClass vbox;

  void (*machine_changed)(AgsNotationSheet *notation_sheet, AgsMachine *machine);
};

GType ags_notation_sheet_get_type(void);

void ags_notation_sheet_machine_changed(AgsNotationSheet *notation_sheet,
					AgsMachine *machine);

AgsNotationSheet* ags_notation_sheet_new();

#endif /*__AGS_NOTATION_SHEET_H__*/
