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

#include <ags/plugin/ags_lv2_log_manager.h>

void ags_lv2_log_manager_class_init(AgsLv2LogManagerClass *lv2_log_manager);
void ags_lv2_log_manager_init(AgsLv2LogManager *lv2_log_manager);
void ags_lv2_log_manager_finalize(GObject *gobject);

/**
 * SECTION:ags_lv2_log_manager
 * @short_description: allow lv2 plugins to log
 * @title: AgsLv2LogManager
 * @section_id:
 * @include: ags/plugin/ags_lv2_log_manager.h
 *
 * The #AgsLv2LogManager handles console IO.
 */

static gpointer ags_lv2_log_manager_parent_class = NULL;

AgsLv2LogManager *ags_lv2_log_manager = NULL;

GType
ags_lv2_log_manager_get_type()
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_lv2_log_manager = 0;

    const GTypeInfo ags_lv2_log_manager_info = {
      sizeof (AgsLv2LogManagerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_lv2_log_manager_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsLv2LogManager),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_lv2_log_manager_init,
    };

    ags_type_lv2_log_manager = g_type_register_static(G_TYPE_OBJECT,
						      "AgsLv2LogManager",
						      &ags_lv2_log_manager_info,
						      0);

    g_once_init_leave(&g_define_type_id__static, ags_type_lv2_log_manager);
  }

  return g_define_type_id__static;
}

void
ags_lv2_log_manager_class_init(AgsLv2LogManagerClass *lv2_log_manager)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_lv2_log_manager_parent_class = g_type_class_peek_parent(lv2_log_manager);

  /* GObject */
  gobject = (GObjectClass *) lv2_log_manager;

  gobject->finalize = ags_lv2_log_manager_finalize;
}

void
ags_lv2_log_manager_init(AgsLv2LogManager *lv2_log_manager)
{
  /* empty */
}

void
ags_lv2_log_manager_finalize(GObject *gobject)
{
  /* empty */

  /* call parent */
  G_OBJECT_CLASS(ags_lv2_log_manager_parent_class)->finalize(gobject);
}

/**
 * ags_lv2_log_manager_printf:
 * @handle: (type gpointer) (transfer none): the log handle
 * @type: (type gpointer) (transfer none): LV2 URID
 * @fmt: format
 * @...: %NULL terminated variable arguments
 * 
 * Print formatted.
 * 
 * Returns: 0
 *
 * Since: 3.0.0
 */
int
ags_lv2_log_manager_printf(LV2_Log_Handle handle,
			   LV2_URID type,
			   const char* fmt,
			   ...)
{
  va_list ap;

  int retval;
  
  va_start(ap, fmt);
  retval = ags_lv2_log_manager_vprintf(handle, type, fmt, ap);
  va_end(ap);

  return(retval);
}

/**
 * ags_lv2_log_manager_vprintf:
 * @handle: (type gpointer) (transfer none): the log handle
 * @type: (type gpointer) (transfer none): LV2 URID
 * @fmt: format
 * @ap: argument pointer
 *
 * Variadict print formatted.
 *
 * Returns: 0
 *
 * Since: 3.0.0
 */
int
ags_lv2_log_manager_vprintf(LV2_Log_Handle handle,
			    LV2_URID type,
			    const char* fmt,
			    va_list ap)
{
  g_logv(G_LOG_DOMAIN,
	 0,
	 fmt,
	 ap);

  return(0);
}

/**
 * ags_lv2_log_manager_get_instance:
 * 
 * Singleton function to optain the id manager instance.
 *
 * Returns: (transfer none): an instance of #AgsLv2LogManager
 *
 * Since: 3.0.0
 */
AgsLv2LogManager*
ags_lv2_log_manager_get_instance()
{
  if(ags_lv2_log_manager == NULL){
    ags_lv2_log_manager = ags_lv2_log_manager_new();
    
    //    ags_lv2_log_manager_load_default(ags_lv2_log_manager);
  }

  return(ags_lv2_log_manager);
}

/**
 * ags_lv2_log_manager_new:
 *
 * Instantiate a id manager. 
 *
 * Returns: a new #AgsLv2LogManager
 *
 * Since: 3.0.0
 */
AgsLv2LogManager*
ags_lv2_log_manager_new()
{
  AgsLv2LogManager *lv2_log_manager;

  lv2_log_manager = (AgsLv2LogManager *) g_object_new(AGS_TYPE_LV2_LOG_MANAGER,
						      NULL);

  return(lv2_log_manager);
}
