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

#ifndef __AGS_RESET_FX_ANALYSE_H__
#define __AGS_RESET_FX_ANALYSE_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/fx/ags_fx_analyse_channel.h>

G_BEGIN_DECLS

#define AGS_TYPE_RESET_FX_ANALYSE                (ags_reset_fx_analyse_get_type())
#define AGS_RESET_FX_ANALYSE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_RESET_FX_ANALYSE, AgsResetFxAnalyse))
#define AGS_RESET_FX_ANALYSE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_RESET_FX_ANALYSE, AgsResetFxAnalyseClass))
#define AGS_IS_RESET_FX_ANALYSE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_RESET_FX_ANALYSE))
#define AGS_IS_RESET_FX_ANALYSE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_RESET_FX_ANALYSE))
#define AGS_RESET_FX_ANALYSE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_RESET_FX_ANALYSE, AgsResetFxAnalyseClass))

typedef struct _AgsResetFxAnalyse AgsResetFxAnalyse;
typedef struct _AgsResetFxAnalyseClass AgsResetFxAnalyseClass;

struct _AgsResetFxAnalyse
{
  AgsTask task;

  GList *fx_analyse_channel;
};

struct _AgsResetFxAnalyseClass
{
  AgsTaskClass task;
};

GType ags_reset_fx_analyse_get_type();

void ags_reset_fx_analyse_add(AgsResetFxAnalyse *reset_fx_analyse,
			      AgsFxAnalyseChannel *fx_analyse_channel);
void ags_reset_fx_analyse_remove(AgsResetFxAnalyse *reset_fx_analyse,
				 AgsFxAnalyseChannel *fx_analyse_channel);

AgsResetFxAnalyse* ags_reset_fx_analyse_get_instance();

AgsResetFxAnalyse* ags_reset_fx_analyse_new();

G_END_DECLS

#endif /*__AGS_RESET_FX_ANALYSE_H__*/
