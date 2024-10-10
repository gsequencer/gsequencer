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

#ifndef __AGS_VECTOR_256_MANAGER_H__
#define __AGS_VECTOR_256_MANAGER_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_sound_types.h>

G_BEGIN_DECLS

#define AGS_TYPE_VECTOR_256_MANAGER                (ags_vector_256_manager_get_type())
#define AGS_VECTOR_256_MANAGER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_VECTOR_256_MANAGER, AgsVector256Manager))
#define AGS_VECTOR_256_MANAGER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_VECTOR_256_MANAGER, AgsVector256ManagerClass))
#define AGS_IS_VECTOR_256_MANAGER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_VECTOR_256_MANAGER))
#define AGS_IS_VECTOR_256_MANAGER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_VECTOR_256_MANAGER))
#define AGS_VECTOR_256_MANAGER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_VECTOR_256_MANAGER, AgsVector256ManagerClass))

#define AGS_VECTOR_ARR(ptr) ((AgsVectorArr*)(ptr))

#define AGS_VECTOR_256_MANAGER_GET_OBJ_MUTEX(obj) (&(((AgsVector256Manager *) obj)->obj_mutex))

#define AGS_VECTOR_256_MANAGER_DEFAULT_VECTOR_COUNT (32)

typedef struct _AgsVector256Manager AgsVector256Manager;
typedef struct _AgsVector256ManagerClass AgsVector256ManagerClass;

typedef struct _AgsVectorArr AgsVectorArr;

typedef struct _AgsVectorMemS8 AgsVectorMemS8;
typedef struct _AgsVectorMemS16 AgsVectorMemS16;
typedef struct _AgsVectorMemS32 AgsVectorMemS32;
typedef struct _AgsVectorMemS64 AgsVectorMemS64;
typedef struct _AgsVectorMemFloat AgsVectorMemFloat;
typedef struct _AgsVectorMemDouble AgsVectorMemDouble;

typedef enum{
  AGS_VECTOR_256_SIGNED_8_BIT,
  AGS_VECTOR_256_SIGNED_16_BIT,
  AGS_VECTOR_256_SIGNED_32_BIT,
  AGS_VECTOR_256_SIGNED_64_BIT,
  AGS_VECTOR_256_FLOAT,
  AGS_VECTOR_256_DOUBLE,
}AgsVector256Types;

struct _AgsVector256Manager
{
  GObject gobject;

  GRecMutex obj_mutex;

  guint vector_count;
  
  GList *v8s8_arr;
  GList *v8s16_arr;
  GList *v8s32_arr;
  GList *v8s64_arr;
  GList *v8float_arr;
  GList *v8double_arr;
};

struct _AgsVector256ManagerClass
{
  GObjectClass gobject;
};

struct _AgsVectorArr
{
  AgsVector256Types vector_type;

  volatile gboolean locked;
  
  union{
    AgsVectorMemS8 *vec_s8;
    AgsVectorMemS16 *vec_s16;
    AgsVectorMemS32 *vec_s32;
    AgsVectorMemS64 *vec_s64;
    AgsVectorMemFloat *vec_float;
    AgsVectorMemDouble *vec_double;
  }vector;
};

struct _AgsVectorMemS8
{
  ags_v8s8 mem_s8[32];
};

struct _AgsVectorMemS16
{
  ags_v8s16 mem_s16[32];
};

struct _AgsVectorMemS32
{
  ags_v8s32 mem_s32[32];
};

struct _AgsVectorMemS64
{
  ags_v8s64 mem_s64[32];
};

struct _AgsVectorMemFloat
{
  ags_v8float mem_float[32];
};

struct _AgsVectorMemDouble
{
  ags_v8double mem_double[32];
};

GType ags_vector_256_manager_get_type(void);

AgsVectorArr* ags_vector_arr_alloc(AgsVector256Types vector_type);
void ags_vector_arr_free(AgsVectorArr *vector_arr);

void ags_vector_256_manager_reserve_all(AgsVector256Manager *vector_256_manager);

AgsVectorArr* ags_vector_256_manager_try_acquire(AgsVector256Manager *vector_256_manager,
						 AgsVector256Types vector_type);
void ags_vector_256_manager_release(AgsVector256Manager *vector_256_manager,
				    AgsVectorArr *vector_arr);

/*  */
AgsVector256Manager* ags_vector_256_manager_get_instance();

AgsVector256Manager* ags_vector_256_manager_new();

G_END_DECLS

#endif /*__AGS_VECTOR_256_MANAGER_H__*/
