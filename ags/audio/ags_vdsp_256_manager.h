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

#ifndef __AGS_VDSP_256_MANAGER_H__
#define __AGS_VDSP_256_MANAGER_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_VDSP_256_MANAGER                (ags_vdsp_256_manager_get_type())
#define AGS_VDSP_256_MANAGER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_VDSP_256_MANAGER, AgsVDSP256Manager))
#define AGS_VDSP_256_MANAGER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_VDSP_256_MANAGER, AgsVDSP256ManagerClass))
#define AGS_IS_VDSP_256_MANAGER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_VDSP_256_MANAGER))
#define AGS_IS_VDSP_256_MANAGER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_VDSP_256_MANAGER))
#define AGS_VDSP_256_MANAGER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_VDSP_256_MANAGER, AgsVDSP256ManagerClass))

#define AGS_VDSP_ARR(ptr) ((AgsVDSPArr*)(ptr))

#define AGS_VDSP_256_MANAGER_GET_OBJ_MUTEX(obj) (&(((AgsVDSP256Manager *) obj)->obj_mutex))

#define AGS_VDSP_256_MANAGER_DEFAULT_VDSP_COUNT (96)

typedef struct _AgsVDSP256Manager AgsVDSP256Manager;
typedef struct _AgsVDSP256ManagerClass AgsVDSP256ManagerClass;

typedef struct _AgsVDSPArr AgsVDSPArr;

typedef struct _AgsVDSPMemInt AgsVDSPMemInt;
typedef struct _AgsVDSPMemFloat AgsVDSPMemFloat;
typedef struct _AgsVDSPMemDouble AgsVDSPMemDouble;

typedef enum{
  AGS_VDSP_256_INT,
  AGS_VDSP_256_FLOAT,
  AGS_VDSP_256_DOUBLE,
}AgsVDSP256Types;

struct _AgsVDSP256Manager
{
  GObject gobject;

  GRecMutex obj_mutex;

  guint vdsp_count;
  
  GList *int_arr;
  GList *float_arr;
  GList *double_arr;
};

struct _AgsVDSP256ManagerClass
{
  GObjectClass gobject;
};

struct _AgsVDSPArr
{
  AgsVDSP256Types vdsp_type;

  gboolean locked;
  
  union{
    AgsVDSPMemInt *vec_int;
    AgsVDSPMemFloat *vec_float;
    AgsVDSPMemDouble *vec_double;
  }vdsp;
};

struct _AgsVDSPMemInt
{
  int mem_int[256];
};

struct _AgsVDSPMemFloat
{
  float mem_float[256];
};

struct _AgsVDSPMemDouble
{
  double mem_double[256];
};

GType ags_vdsp_256_manager_get_type(void);

AgsVDSPArr* ags_vdsp_arr_alloc(AgsVDSP256Types vdsp_type);
void ags_vdsp_arr_free(AgsVDSPArr *vdsp_arr);

void ags_vdsp_256_manager_reserve_all(AgsVDSP256Manager *vdsp_256_manager);

AgsVDSPArr* ags_vdsp_256_manager_try_acquire(AgsVDSP256Manager *vdsp_256_manager,
					     AgsVDSP256Types vdsp_type);
gboolean ags_vdsp_256_manager_try_acquire_dual(AgsVDSP256Manager *vdsp_256_manager,
					       AgsVDSP256Types vdsp_type_a, AgsVDSP256Types vdsp_type_b,
					       AgsVDSPArr **vdsp_arr_a, AgsVDSPArr **vdsp_arr_b);
gboolean ags_vdsp_256_manager_try_acquire_triple(AgsVDSP256Manager *vdsp_256_manager,
						 AgsVDSP256Types vdsp_type_a, AgsVDSP256Types vdsp_type_b, AgsVDSP256Types vdsp_type_c,
						 AgsVDSPArr **vdsp_arr_a, AgsVDSPArr **vdsp_arr_b, AgsVDSPArr **vdsp_arr_c);
gboolean ags_vdsp_256_manager_try_acquire_quad(AgsVDSP256Manager *vdsp_256_manager,
					       AgsVDSP256Types vdsp_type_a, AgsVDSP256Types vdsp_type_b, AgsVDSP256Types vdsp_type_c, AgsVDSP256Types vdsp_type_d,
					       AgsVDSPArr **vdsp_arr_a, AgsVDSPArr **vdsp_arr_b, AgsVDSPArr **vdsp_arr_c, AgsVDSPArr **vdsp_arr_d);

void ags_vdsp_256_manager_release(AgsVDSP256Manager *vdsp_256_manager,
				  AgsVDSPArr *vdsp_arr);

/*  */
AgsVDSP256Manager* ags_vdsp_256_manager_get_instance();

AgsVDSP256Manager* ags_vdsp_256_manager_new();

G_END_DECLS

#endif /*__AGS_VDSP_256_MANAGER_H__*/
