/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
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
#include <ags/widget/ags_cartestian.h>

#define AGS_FILE_DEFAULT_VERSION "0.4.0"
#define AGS_EFFECTS_DEFAULT_VERSION "0.4.0"
#define AGS_CLIPBOARD_DEFAULT_VERSION "0.4.0"

#define AGS_FILE_DEFAULT_ENCODING "UTF-8" //"ISO-8859-15"
#define AGS_FILE_DEFAULT_DTD  "~/ags/file/ags_file.dtd"

#define AGS_FILE_FALSE "FALSE"
#define AGS_FILE_TRUE "TRUE"

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
