/* This file is part of GSequencer.
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

#ifndef __AGS_ADD_LINE_MEMBER_H__
#define __AGS_ADD_LINE_MEMBER_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_task.h>
#include <ags/X/ags_line.h>
#include <ags/X/ags_line_member.h>

#define AGS_TYPE_ADD_LINE_MEMBER                (ags_add_line_member_get_type())
#define AGS_ADD_LINE_MEMBER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_ADD_LINE_MEMBER, AgsAddLineMember))
#define AGS_ADD_LINE_MEMBER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_ADD_LINE_MEMBER, AgsAddLineMemberClass))
#define AGS_IS_ADD_LINE_MEMBER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_ADD_LINE_MEMBER))
#define AGS_IS_ADD_LINE_MEMBER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_ADD_LINE_MEMBER))
#define AGS_ADD_LINE_MEMBER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_ADD_LINE_MEMBER, AgsAddLineMemberClass))

typedef struct _AgsAddLineMember AgsAddLineMember;
typedef struct _AgsAddLineMemberClass AgsAddLineMemberClass;

struct _AgsAddLineMember
{
  AgsTask task;

  AgsLine *line;
  AgsLineMember *line_member;

  guint x;
  guint y;
  guint width;
  guint height;
};

struct _AgsAddLineMemberClass
{
  AgsTaskClass task;
};

GType ags_add_line_member_get_type();

AgsAddLineMember* ags_add_line_member_new(AgsLine *line,
					  AgsLineMember *line_member,
					  guint x, guint y,
					  guint width, guint height);

#endif /*__AGS_ADD_LINE_MEMBER_H__*/
