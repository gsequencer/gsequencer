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

#ifndef __AGS_PROGRAM_H__
#define __AGS_PROGRAM_H__

#include <glib.h>
#include <glib-object.h>

#include <libxml/tree.h>

#include <ags/libags.h>

#include <ags/audio/ags_marker.h>

G_BEGIN_DECLS

#define AGS_TYPE_PROGRAM                (ags_program_get_type())
#define AGS_PROGRAM(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PROGRAM, AgsProgram))
#define AGS_PROGRAM_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PROGRAM, AgsProgramClass))
#define AGS_IS_PROGRAM(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_PROGRAM))
#define AGS_IS_PROGRAM_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_PROGRAM))
#define AGS_PROGRAM_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_PROGRAM, AgsProgramClass))

#define AGS_PROGRAM_GET_OBJ_MUTEX(obj) (&(((AgsProgram *) obj)->obj_mutex))

#define AGS_PROGRAM_DEFAULT_BPM (120.0)

#define AGS_PROGRAM_TICS_PER_BEAT (1.0)
#define AGS_PROGRAM_MINIMUM_MARKER_LENGTH (1.0 / 16.0 / 64.0)
#define AGS_PROGRAM_MAXIMUM_MARKER_LENGTH (16.0)

#define AGS_PROGRAM_DEFAULT_LENGTH (64 * 16 * 16 * 1200 / AGS_PROGRAM_TICS_PER_BEAT)
#define AGS_PROGRAM_DEFAULT_JIFFIE (60.0 / AGS_PROGRAM_DEFAULT_BPM / AGS_PROGRAM_TICS_PER_BEAT)
#define AGS_PROGRAM_DEFAULT_DURATION (AGS_PROGRAM_DEFAULT_LENGTH * AGS_PROGRAM_DEFAULT_JIFFIE * AGS_USEC_PER_SEC)
#define AGS_PROGRAM_DEFAULT_OFFSET (64 * (1 / AGS_PROGRAM_MINIMUM_MARKER_LENGTH))

#define AGS_PROGRAM_DEFAULT_PRECISION (8)
#define AGS_PROGRAM_MAXIMUM_STEPS (128)

#define AGS_PROGRAM_CLIPBOARD_VERSION "5.1.0"
#define AGS_PROGRAM_CLIPBOARD_TYPE "AgsProgramClipboardXml"
#define AGS_PROGRAM_CLIPBOARD_FORMAT "AgsProgramNativeScale"
#define AGS_PROGRAM_CLIPBOARD_LEGACY_FORMAT "AgsProgramNativePiano"

typedef struct _AgsProgram AgsProgram;
typedef struct _AgsProgramClass AgsProgramClass;

/**
 * AgsProgramFlags:
 * @AGS_PROGRAM_BYPASS: ignore any program data
 * 
 * Enum values to control the behavior or indicate internal state of #AgsProgram by
 * enable/disable as flags.
 */
typedef enum{
  AGS_PROGRAM_BYPASS               = 1,
}AgsProgramFlags;

struct _AgsProgram
{
  GObject gobject;

  AgsProgramFlags flags;

  GRecMutex obj_mutex;

  AgsTimestamp *timestamp;

  gchar *control_name;

  GObject *port;
  
  GList *marker;  
  GList *selection;
};

struct _AgsProgramClass
{
  GObjectClass gobject;
};

GType ags_program_get_type(void);
GType ags_program_flags_get_type(void);

GRecMutex* ags_program_get_obj_mutex(AgsProgram *program);

gboolean ags_program_test_flags(AgsProgram *program, AgsProgramFlags flags);
void ags_program_set_flags(AgsProgram *program, AgsProgramFlags flags);
void ags_program_unset_flags(AgsProgram *program, AgsProgramFlags flags);

GList* ags_program_find_port(GList *program,
			     GObject *port);

GList* ags_program_find_near_timestamp(GList *program,
				       AgsTimestamp *timestamp);
GList* ags_program_find_near_timestamp_extended(GList *program,
						gchar *control_name,
						AgsTimestamp *timestamp);

gint ags_program_sort_func(gconstpointer a,
			   gconstpointer b);

GList* ags_program_add(GList *program,
		       AgsProgram *new_program);

GObject* ags_program_get_audio(AgsProgram *program);
void ags_program_set_audio(AgsProgram *program,
			   GObject *audio);

gchar* ags_program_get_control_name(AgsProgram *program);
void ags_program_set_control_name(AgsProgram *program,
				  gchar *control_name);

GObject* ags_program_get_port(AgsProgram *program);
void ags_program_set_port(AgsProgram *program,
			  GObject *port);

GList* ags_program_get_marker(AgsProgram *program);
void ags_program_set_marker(AgsProgram *program,
			    GList *marker);

void ags_program_add_marker(AgsProgram *program,
			    AgsMarker *marker,
			    gboolean use_selection_list);
void ags_program_remove_marker(AgsProgram *program,
			       AgsMarker *marker,
			       gboolean use_selection_list);

gboolean ags_program_remove_marker_at_position(AgsProgram *program,
					       guint x);

GList* ags_program_get_selection(AgsProgram *program);

gboolean ags_program_is_marker_selected(AgsProgram *program, AgsMarker *marker);

AgsMarker* ags_program_find_point(AgsProgram *program,
				  guint x,
				  gboolean use_selection_list);
GList* ags_program_find_region(AgsProgram *program,
			       guint x0,
			       guint x1,
			       gboolean use_selection_list);

void ags_program_free_selection(AgsProgram *program);
void ags_program_free_all_selection(GList *program);

void ags_program_add_point_to_selection(AgsProgram *program,
					guint x,
					gboolean replace_current_selection);
void ags_program_remove_point_from_selection(AgsProgram *program,
					     guint x);

void ags_program_add_region_to_selection(AgsProgram *program,
					 guint x0,
					 guint x1,
					 gboolean replace_current_selection);
void ags_program_remove_region_from_selection(AgsProgram *program,
					      guint x0,
					      guint x1);

void ags_program_add_all_to_selection(AgsProgram *program);

xmlNode* ags_program_copy_selection(AgsProgram *program);
xmlNode* ags_program_cut_selection(AgsProgram *program);

void ags_program_insert_from_clipboard(AgsProgram *program,
				       xmlNode *program_node,
				       gboolean reset_x_offset, guint x_offset);

void ags_program_insert_from_clipboard_extended(AgsProgram *program,
						xmlNode *program_node,
						gboolean reset_x_offset, guint x_offset,
						gboolean no_duplicates);

gchar** ags_program_get_specifier_unique(GList *program);
GList* ags_program_find_specifier(GList *program,
				  gchar *specifier);

guint ags_program_get_value(AgsProgram *program,
			    guint x, guint x_end,
			    gboolean use_prev_on_failure,
			    GValue *value);

AgsProgram* ags_program_new(gchar *control_name);

G_END_DECLS

#endif /*__AGS_PROGRAM_H__*/
