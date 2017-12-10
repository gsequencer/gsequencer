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

#ifndef __LIBAGS_H__
#define __LIBAGS_H__

#include <ags/util/ags_destroy_util.h>
#include <ags/util/ags_list_util.h>
#include <ags/util/ags_id_generator.h>

/* library */
#include <ags/lib/ags_complex.h>
#include <ags/lib/ags_conversion.h>
#include <ags/lib/ags_endian.h>
#include <ags/lib/ags_function.h>
#include <ags/lib/ags_log.h>
#include <ags/lib/ags_parameter.h>
#include <ags/lib/ags_string_util.h>
#include <ags/lib/ags_time.h>
#include <ags/lib/ags_turtle.h>
#include <ags/lib/ags_turtle_manager.h>

/* object */
#include <ags/object/ags_applicable.h>
#include <ags/object/ags_application_context.h>
#include <ags/object/ags_async_queue.h>
#include <ags/object/ags_config.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_connection.h>
#include <ags/object/ags_connection_manager.h>
#include <ags/object/ags_countable.h>
#include <ags/object/ags_concurrent_tree.h>
#include <ags/object/ags_distributed_manager.h>
#include <ags/object/ags_dynamic_connectable.h>
#include <ags/object/ags_macros.h>
#include <ags/object/ags_main_loop.h>
#include <ags/object/ags_marshal.h>
#include <ags/object/ags_mutable.h>
#include <ags/object/ags_packable.h>
#include <ags/object/ags_plugin.h>
#include <ags/object/ags_portlet.h>
#include <ags/object/ags_seekable.h>
#include <ags/object/ags_sequencer.h>
#include <ags/object/ags_soundcard.h>
#include <ags/object/ags_tactable.h>
#include <ags/object/ags_tree_iterator.h>

/* file */
#include <ags/file/ags_file.h>
#include <ags/file/ags_file_id_ref.h>
#include <ags/file/ags_file_launch.h>
#include <ags/file/ags_file_link.h>
#include <ags/file/ags_file_lookup.h>
#include <ags/file/ags_file_stock.h>
#include <ags/file/ags_file_util.h>

/* thread */
#include <ags/thread/ags_autosave_thread.h>
#include <ags/thread/ags_concurrency_provider.h>
#include <ags/thread/ags_destroy_worker.h>
#include <ags/thread/ags_generic_main_loop.h>
#include <ags/thread/ags_history.h>
#include <ags/thread/ags_message_delivery.h>
#include <ags/thread/ags_message_queue.h>
#include <ags/thread/ags_mutex_manager.h>
#include <ags/thread/ags_condition_manager.h>
#include <ags/thread/ags_poll_fd.h>
#include <ags/thread/ags_polling_thread.h>
#include <ags/thread/ags_returnable_thread.h>
#include <ags/thread/ags_single_thread.h>
#include <ags/thread/ags_task_completion.h>
#include <ags/thread/ags_task.h>
#include <ags/thread/ags_task_thread.h>
#include <ags/thread/ags_thread_application_context.h>
#include <ags/thread/ags_thread_pool.h>
#include <ags/thread/ags_thread-posix.h>
#include <ags/thread/ags_timestamp_factory.h>
#include <ags/thread/ags_timestamp_thread.h>
#include <ags/thread/ags_timestamp.h>
#include <ags/thread/ags_worker_thread.h>

/* thread file */
#include <ags/thread/file/ags_thread_file_xml.h>

/* server */
#include <ags/server/ags_registry.h>
#include <ags/server/ags_server.h>
#include <ags/server/ags_server_status.h>
#include <ags/server/ags_server_application_context.h>
#include <ags/server/ags_service_provider.h>

#include <ags/server/security/ags_password_store_manager.h>
#include <ags/server/security/ags_business_group.h>
#include <ags/server/security/ags_password_store.h>
#include <ags/server/security/ags_certificate_manager.h>
#include <ags/server/security/ags_certificate.h>
#include <ags/server/security/ags_xml_certificate.h>
#include <ags/server/security/ags_security_context.h>
#include <ags/server/security/ags_authentication_manager.h>
#include <ags/server/security/ags_xml_authentication.h>
#include <ags/server/security/ags_authentication.h>
#include <ags/server/security/ags_xml_password_store.h>

#include <ags/server/thread/ags_remote_task.h>
#include <ags/server/thread/ags_remote_task_thread.h>

#include <ags/server/controller/ags_local_factory_controller.h>
#include <ags/server/controller/ags_front_controller.h>
#include <ags/server/controller/ags_controller.h>
#include <ags/server/controller/ags_local_task_controller.h>
#include <ags/server/controller/ags_local_serialization_controller.h>
#include <ags/server/controller/ags_local_registry_controller.h>

#endif /*__LIBAGS_H__*/
