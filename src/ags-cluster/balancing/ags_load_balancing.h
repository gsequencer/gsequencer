/* AGS Client - Advanced GTK Sequencer Client
 * Copyright (C) 2013 Joël Krähemann
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

#ifndef __AGS_LOAD_BALANCING_H__
#define __AGS_LOAD_BALANCING_H__

#include <glib.h>
#include <glib-object.h>

#include <ags-cluster/cluster/ags_cluster.h>

#include <stdio.h>

#define AGS_TYPE_LOAD_BALANCING                (ags_load_balancing_get_type())
#define AGS_LOAD_BALANCING(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_LOAD_BALANCING, AgsLoadBalancing))
#define AGS_LOAD_BALANCING_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_LOAD_BALANCING, AgsLoadBalancingClass))
#define AGS_IS_LOAD_BALANCING(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_LOAD_BALANCING))
#define AGS_IS_LOAD_BALANCING_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_LOAD_BALANCING))
#define AGS_LOAD_BALANCING_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_LOAD_BALANCING, AgsLoadBalancingClass))

typedef struct _AgsLoadBalancing AgsLoadBalancing;
typedef struct _AgsLoadBalancingClass AgsLoadBalancingClass;

struct _AgsLoadBalancing
{
  GObject object;

  GList *activity_log;

  AgsCluster *cluster;
};

struct _AgsLoadBalancingClass
{
  GObjectClass object;
};

GType ags_load_balancing_get_type();

AgsLoadBalancing* ags_load_balancing_new();

#endif /*__AGS_LOAD_BALANCING_H__*/
