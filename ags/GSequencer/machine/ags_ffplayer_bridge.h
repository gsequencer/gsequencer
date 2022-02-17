/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#ifndef __AGS_FFPLAYER_BRIDGE_H__
#define __AGS_FFPLAYER_BRIDGE_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

#include <ags/GSequencer/ags_effect_bridge.h>

G_BEGIN_DECLS

#define AGS_TYPE_FFPLAYER_BRIDGE                (ags_ffplayer_bridge_get_type())
#define AGS_FFPLAYER_BRIDGE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FFPLAYER_BRIDGE, AgsFFPlayerBridge))
#define AGS_FFPLAYER_BRIDGE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_FFPLAYER_BRIDGE, AgsFFPlayerBridgeClass))
#define AGS_IS_FFPLAYER_BRIDGE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_FFPLAYER_BRIDGE))
#define AGS_IS_FFPLAYER_BRIDGE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_FFPLAYER_BRIDGE))
#define AGS_FFPLAYER_BRIDGE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_FFPLAYER_BRIDGE, AgsFFPlayerBridgeClass))

typedef struct _AgsFFPlayerBridge AgsFFPlayerBridge;
typedef struct _AgsFFPlayerBridgeClass AgsFFPlayerBridgeClass;

struct _AgsFFPlayerBridge
{
  AgsEffectBridge effect_bridge;
};

struct _AgsFFPlayerBridgeClass
{
  AgsEffectBridgeClass effect_bridge;
};

GType ags_ffplayer_bridge_get_type(void);

AgsFFPlayerBridge* ags_ffplayer_bridge_new(AgsAudio *audio);

G_END_DECLS

#endif /*__AGS_FFPLAYER_BRIDGE_H__*/
