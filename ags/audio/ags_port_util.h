/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#ifndef __AGS_PORT_UTIL_H__
#define __AGS_PORT_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/plugin/ags_plugin_port.h>

#include <ags/audio/ags_port.h>

G_BEGIN_DECLS

#define AGS_TYPE_PORT_UTIL         (ags_port_util_get_type())

typedef struct _AgsPortUtil AgsPortUtil;

struct _AgsPortUtil
{
  //empty
};

GType ags_port_util_get_type(void);

void ags_port_util_load_ladspa_conversion(AgsPort *port,
					  AgsPluginPort *plugin_port);

void ags_port_util_load_lv2_conversion(AgsPort *port,
				       AgsPluginPort *plugin_port);

void ags_port_util_load_vst3_conversion(AgsPort *port,
					AgsPluginPort *plugin_port);

G_END_DECLS

#endif /*__AGS_PORT_UTIL_H__*/
