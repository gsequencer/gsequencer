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

#include <ags/X/machine/ags_fm_syncsynth.h>
#include <ags/X/machine/ags_fm_syncsynth_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/X/ags_ui_provider.h>

#include <math.h>

#include <ags/i18n.h>

void ags_fm_syncsynth_class_init(AgsFMSyncsynthClass *fm_syncsynth);
void ags_fm_syncsynth_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_fm_syncsynth_plugin_interface_init(AgsPluginInterface *plugin);
void ags_fm_syncsynth_init(AgsFMSyncsynth *fm_syncsynth);
void ags_fm_syncsynth_finalize(GObject *gobject);

void ags_fm_syncsynth_connect(AgsConnectable *connectable);
void ags_fm_syncsynth_disconnect(AgsConnectable *connectable);

