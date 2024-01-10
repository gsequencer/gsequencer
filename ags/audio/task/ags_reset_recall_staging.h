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

#ifndef __AGS_RESET_RECALL_STAGING_H__
#define __AGS_RESET_RECALL_STAGING_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_recall.h>

G_BEGIN_DECLS

#define AGS_TYPE_RESET_RECALL_STAGING                (ags_reset_recall_staging_get_type())
#define AGS_RESET_RECALL_STAGING(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_RESET_RECALL_STAGING, AgsResetRecallStaging))
#define AGS_RESET_RECALL_STAGING_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_RESET_RECALL_STAGING, AgsResetRecallStagingClass))
#define AGS_IS_RESET_RECALL_STAGING(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_RESET_RECALL_STAGING))
#define AGS_IS_RESET_RECALL_STAGING_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_RESET_RECALL_STAGING))
#define AGS_RESET_RECALL_STAGING_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_RESET_RECALL_STAGING, AgsResetRecallStagingClass))

typedef struct _AgsResetRecallStaging AgsResetRecallStaging;
typedef struct _AgsResetRecallStagingClass AgsResetRecallStagingClass;

struct _AgsResetRecallStaging
{
  AgsTask task;

  AgsSoundStagingFlags staging_flags;
  
  GList *recall;
};

struct _AgsResetRecallStagingClass
{
  AgsTaskClass task;
};

GType ags_reset_recall_staging_get_type();

void ags_reset_recall_staging_add(AgsResetRecallStaging *reset_recall_staging,
				  AgsRecall *recall);
void ags_reset_recall_staging_remove(AgsResetRecallStaging *reset_recall_staging,
				     AgsRecall *recall);

AgsResetRecallStaging* ags_reset_recall_staging_get_instance();

AgsResetRecallStaging* ags_reset_recall_staging_new();

G_END_DECLS

#endif /*__AGS_RESET_RECALL_STAGING_H__*/
