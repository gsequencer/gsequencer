/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#ifndef __HOST_CONTEXT_H__
#define __HOST_CONTEXT_H__

#pragma once

#include <pluginterfaces/base/conststringtable.h>
#include <pluginterfaces/base/funknown.h>
#include <public.sdk/source/vst/hosting/pluginterfacesupport.h>
#include <base/source/fstring.h>
#include <pluginterfaces/vst/ivsthostapplication.h>
#include <base/source/updatehandler.h>
#include <map>

using namespace Steinberg::Vst;

namespace Steinberg
{
  namespace Vst {
//------------------------------------------------------------------------
/** Implementation's example of IHostApplication.
\ingroup hostingBase
*/
class HostApplication : public Steinberg::Vst::IHostApplication
{
public:
	HostApplication ();
	virtual ~HostApplication () { FUNKNOWN_DTOR }

	//--- IHostApplication ---------------
	Steinberg::tresult PLUGIN_API getName (Steinberg::Vst::String128 name) SMTG_OVERRIDE;
	Steinberg::tresult PLUGIN_API createInstance (Steinberg::TUID cid, Steinberg::TUID _iid, void** obj) SMTG_OVERRIDE;

	DECLARE_FUNKNOWN_METHODS

	  Steinberg::Vst::PlugInterfaceSupport* getPlugInterfaceSupport () const { return mPlugInterfaceSupport; }

protected:
	Steinberg::IPtr<Steinberg::Vst::PlugInterfaceSupport> mPlugInterfaceSupport;
};

class HostAttribute;
//------------------------------------------------------------------------
/** Implementation's example of IAttributeList.
\ingroup hostingBase
*/
class HostAttributeList : public Steinberg::Vst::IAttributeList
{
public:
	HostAttributeList ();
	virtual ~HostAttributeList ();

	Steinberg::tresult PLUGIN_API setInt (AttrID aid, Steinberg::int64 value) SMTG_OVERRIDE;
	Steinberg::tresult PLUGIN_API getInt (AttrID aid, Steinberg::int64& value) SMTG_OVERRIDE;
	Steinberg::tresult PLUGIN_API setFloat (AttrID aid, double value) SMTG_OVERRIDE;
	Steinberg::tresult PLUGIN_API getFloat (AttrID aid, double& value) SMTG_OVERRIDE;
	Steinberg::tresult PLUGIN_API setString (AttrID aid, const Steinberg::Vst::TChar* string) SMTG_OVERRIDE;
	Steinberg::tresult PLUGIN_API getString (AttrID aid, Steinberg::Vst::TChar* string, Steinberg::uint32 sizeInBytes) SMTG_OVERRIDE;
	Steinberg::tresult PLUGIN_API setBinary (AttrID aid, const void* data, Steinberg::uint32 sizeInBytes) SMTG_OVERRIDE;
	Steinberg::tresult PLUGIN_API getBinary (AttrID aid, const void*& data, Steinberg::uint32& sizeInBytes) SMTG_OVERRIDE;

	DECLARE_FUNKNOWN_METHODS
protected:
	void removeAttrID (AttrID aid);
	std::map<Steinberg::String, HostAttribute*> list;
};

//------------------------------------------------------------------------
/** Implementation's example of IMessage.
\ingroup hostingBase
*/
class HostMessage : public Steinberg::Vst::IMessage
{
public:
	HostMessage ();
	virtual ~HostMessage ();

	const char* PLUGIN_API getMessageID () SMTG_OVERRIDE;
	void PLUGIN_API setMessageID (const char* messageID) SMTG_OVERRIDE;
	Steinberg::Vst::IAttributeList* PLUGIN_API getAttributes () SMTG_OVERRIDE;

	DECLARE_FUNKNOWN_METHODS
protected:
	char* messageId;
	HostAttributeList* attributeList;
};
  }
}

#endif /*__HOST_CONTEXT_H__*/
