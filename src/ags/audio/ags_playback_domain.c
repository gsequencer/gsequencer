/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2015 Joël Krähemann
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

/**
 * SECTION:ags_playback_domain
 * @short_description: Outputting to soundcard domain
 * @title: AgsPlaybackDomain
 * @section_id:
 * @include: ags/audio/ags_playback_domain.h
 *
 * #AgsPlaybackDomain represents a domain to output.
 */

void ags_playback_domain_class_init(AgsPlaybackDomainClass *playback_domain);
void ags_playback_domain_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_playback_domain_init(AgsPlaybackDomain *playback_domain);
void ags_playback_domain_disconnect(AgsConnectable *connectable);
void ags_playback_domain_connect(AgsConnectable *connectable);
void ags_playback_domain_finalize(GObject *gobject);

static gpointer ags_playback_domain_parent_class = NULL;

GType
ags_playback_domain_get_type (void)
{
  static GType ags_type_playback_domain = 0;

  if(!ags_type_playback_domain){
    static const GTypeInfo ags_playback_domain_info = {
      sizeof (AgsPlaybackDomainClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_playback_domain_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPlaybackDomain),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_playback_domain_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_playback_domain_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_playback_domain = g_type_register_static(G_TYPE_OBJECT,
						      "AgsPlaybackDomain\0",
						      &ags_playback_domain_info,
						      0);

    g_type_add_interface_static(ags_type_playback_domain,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_playback_domain);
}

void
ags_playback_domain_class_init(AgsPlaybackDomainClass *playback_domain)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_playback_domain_parent_class = g_type_class_peek_parent(playback_domain);

  /* GObjectClass */
  gobject = (GObjectClass *) playback_domain;

  gobject->finalize = ags_playback_domain_finalize;
}

void
ags_playback_domain_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_playback_domain_connect;
  connectable->disconnect = ags_playback_domain_disconnect;
}

void
ags_playback_domain_init(AgsPlaybackDomain *playback_domain)
{
  playback_domain->domain = NULL;

  playback_domain->playback_domain = FALSE;
  playback_domain->sequencer = FALSE;
  playback_domain->notation = FALSE;

  playback_domain->playback_domain = NULL;
}

void
ags_playback_domain_finalize(GObject *gobject)
{
  AgsPlaybackDomain *playback_domain;

  playback_domain = AGS_PLAYBACK_DOMAIN(gobject);

  g_list_free_full(playback_domain->playback_domain,
		   g_object_unref);

  /* call parent */
  G_OBJECT_CLASS(ags_playback_domain_parent_class)->finalize(gobject);
}

void
ags_playback_domain_connect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

void
ags_playback_domain_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

/**
 * ags_playback_domain_new:
 *
 * Creates an #AgsPlaybackDomain, refering to @application_context.
 *
 * Returns: a new #AgsPlaybackDomain
 *
 * Since: 0.4
 */
AgsPlaybackDomain*
ags_playback_domain_new()
{
  AgsPlaybackDomain *playback_domain;

  playback_domain = (AgsPlaybackDomain *) g_object_new(AGS_TYPE_PLAYBACK_DOMAIN,
						       NULL);
  
  return(playback_domain);
}
