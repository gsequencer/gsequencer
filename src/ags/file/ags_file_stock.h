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

#ifndef __AGS_FILE_STOCK_H__
#define __AGS_FILE_STOCK_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/widget/ags_led.h>
#include <ags/widget/ags_vindicator.h>
#include <ags/widget/ags_hindicator.h>
#include <ags/widget/ags_waveform.h>
#include <ags/widget/ags_dial.h>
#include <ags/widget/ags_cartesian.h>

#define AGS_FILE_DEFAULT_VERSION "0.4.2\0"
#define AGS_FILE_EFFECTS_DEFAULT_VERSION "0.4.2\0"
#define AGS_CLIPBOARD_DEFAULT_VERSION "0.4.2\0"

#define AGS_FILE_FALSE "FALSE\0"
#define AGS_FILE_TRUE "TRUE\0"

#define AGS_FILE_BOOLEAN_PROP "gboolean\0"
#define AGS_FILE_CHAR_PROP "gchar\0"
#define AGS_FILE_INT64_PROP "gint64\0"
#define AGS_FILE_UINT64_PROP "guint64\0"
#define AGS_FILE_FLOAT_PROP "gfloat\0"
#define AGS_FILE_DOUBLE_PROP "gdouble\0"

#define AGS_FILE_POINTER_PROP "gpointer\0"
#define AGS_FILE_OBJECT_PROP "GObject\0"

#define AGS_FILE_BOOLEAN_POINTER_PROP "gboolean-pointer\0"
#define AGS_FILE_CHAR_POINTER_PROP "gchar-pointer\0"
#define AGS_FILE_INT64_POINTER_PROP "gint64-pointer\0"
#define AGS_FILE_UINT64_POINTER_PROP "guint64-pointer\0"
#define AGS_FILE_FLOAT_POINTER_PROP "gfloat-pointer\0"
#define AGS_FILE_DOUBLE_POINTER_PROP "gdouble-pointer\0"

#define AGS_FILE_ID_PROP "id\0"
#define AGS_FILE_NAME_PROP "name\0"
#define AGS_FILE_TYPE_PROP "type\0"
#define AGS_FILE_SCOPE_PROP "scope\0"
#define AGS_FILE_VERSION_PROP "version\0"
#define AGS_FILE_BUILD_ID_PROP "build-id\0"
#define AGS_FILE_XPATH_PROP "xpath\0"
#define AGS_FILE_FLAGS_PROP "flags\0"

#define AGS_FILE_DYNAMIC_WIDGET_LED (g_type_name(AgsTypeLed))
#define AGS_FILE_DYNAMIC_WIDGET_VINDICATOR (g_type_name(AgsTypeVIndicator))
#define AGS_FILE_DYNAMIC_WIDGET_HINDICATOR (g_type_name(AgsTypeHIndicator))
#define AGS_FILE_DYNAMIC_WIDGET_LABEL (g_type_name(GtkTypeLabel))
#define AGS_FILE_DYNAMIC_WIDGET_WAVEFORM (g_type_name(AgsTypeWaveform))
#define AGS_FILE_DYNAMIC_WIDGET_BUTTON (g_type_name(GtkTypeButton))
#define AGS_FILE_DYNAMIC_WIDGET_TOGGLE_BUTTON (g_type_name(GtkTypeToggleButton))
#define AGS_FILE_DYNAMIC_WIDGET_CHECK_BUTTON (g_type_name(GtkTypeCheckButton))
#define AGS_FILE_DYNAMIC_WIDGET_SPIN_BUTTON (g_type_name(GtkTypeSpinButton))
#define AGS_FILE_DYNAMIC_WIDGET_VSCALE (g_type_name(GtkTypeVScale))
#define AGS_FILE_DYNAMIC_WIDGET_HSCALE (g_type_name(GtkTypeHScale))
#define AGS_FILE_DYNAMIC_WIDGET_DIAL (g_type_name(AgsTypeDial))
#define AGS_FILE_DYNAMIC_WIDGET_CARTESIAN (g_type_name(GtkTypeCartesian))

#endif /*__AGS_FILE_STOCK_H__*/
