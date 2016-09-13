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

#ifndef __AGS_MACROS_H__
#define __AGS_MACROS_H__

#include <glib.h>
#include <glib-object.h>

#define AGS_DECLARE_INTERFACE(ModuleObjName, module_obj_name, MODULE, OBJ_NAME, PrerequisiteName) \
  GType module_obj_name##_get_type (void);				\
  G_GNUC_BEGIN_IGNORE_DEPRECATIONS					\
  typedef struct _##ModuleObjName ModuleObjName;			\
  typedef struct _##ModuleObjName##Interface ModuleObjName##Interface;	\
									\
  _GLIB_DEFINE_AUTOPTR_CHAINUP (ModuleObjName, PrerequisiteName)	\
									\
  static inline ModuleObjName * MODULE##_##OBJ_NAME (gpointer ptr) {	\
    return G_TYPE_CHECK_INSTANCE_CAST (ptr, module_obj_name##_get_type (), ModuleObjName); } \
  static inline gboolean MODULE##_IS_##OBJ_NAME (gpointer ptr) {	\
    return G_TYPE_CHECK_INSTANCE_TYPE (ptr, module_obj_name##_get_type ()); } \
  static inline ModuleObjName##Interface * MODULE##_##OBJ_NAME##_GET_INTERFACE (gpointer ptr) { \
    return G_TYPE_INSTANCE_GET_INTERFACE (ptr, module_obj_name##_get_type (), ModuleObjName##Interface); } \
  G_GNUC_END_IGNORE_DEPRECATIONS


#endif /*__AGS_MACROS_H__*/
