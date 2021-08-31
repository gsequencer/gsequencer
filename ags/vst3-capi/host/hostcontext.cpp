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

#include <ags/vst3-capi/host/hostcontext.h>

using namespace Steinberg::Vst;

namespace Steinberg {
  namespace Vst {

//-----------------------------------------------------------------------------
HostApplication::HostApplication()
{
  FUNKNOWN_CTOR

    mPlugInterfaceSupport = owned (NEW PlugInterfaceSupport);
}

//-----------------------------------------------------------------------------
Steinberg::tresult PLUGIN_API HostApplication::getName (Steinberg::Vst::String128 name)
{
  Steinberg::String str("Advanced Gtk+ Sequencer VST3 HostApplication");

  str.copyTo16(name, 0, 127);
  
  return Steinberg::kResultTrue;
}

//-----------------------------------------------------------------------------
Steinberg::tresult PLUGIN_API HostApplication::createInstance (Steinberg::TUID cid, Steinberg::TUID _iid, void** obj)
{
  Steinberg::FUID classID (Steinberg::FUID::fromTUID (cid));
  Steinberg::FUID interfaceID (Steinberg::FUID::fromTUID (_iid));
  
  if(classID == IMessage::iid && interfaceID == IMessage::iid){
    *obj = new HostMessage;

    return Steinberg::kResultTrue;
  }else if (classID == IAttributeList::iid && interfaceID == IAttributeList::iid){
    *obj = new HostAttributeList;

    return Steinberg::kResultTrue;
  }

  *obj = nullptr;

  return Steinberg::kResultFalse;
}
  
//-----------------------------------------------------------------------------
Steinberg::tresult PLUGIN_API HostApplication::queryInterface (const char* _iid, void** obj)
{
  QUERY_INTERFACE (_iid, obj, FUnknown::iid, IHostApplication)
    QUERY_INTERFACE (_iid, obj, IHostApplication::iid, IHostApplication)

    if(mPlugInterfaceSupport && mPlugInterfaceSupport->queryInterface (iid, obj) == Steinberg::kResultTrue){
      return Steinberg::kResultOk;
    }

  *obj = nullptr;

  return Steinberg::kResultFalse;
}

//-----------------------------------------------------------------------------
Steinberg::uint32 PLUGIN_API HostApplication::addRef ()
{
  return 1;
}

//-----------------------------------------------------------------------------
Steinberg::uint32 PLUGIN_API HostApplication::release ()
{
  return 1;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
IMPLEMENT_FUNKNOWN_METHODS (HostMessage, IMessage, IMessage::iid)
//-----------------------------------------------------------------------------
HostMessage::HostMessage () : messageId (nullptr), attributeList (nullptr)
{
	FUNKNOWN_CTOR
}

//-----------------------------------------------------------------------------
HostMessage::~HostMessage ()
{
	setMessageID (nullptr);
	if (attributeList)
		attributeList->release ();
	FUNKNOWN_DTOR
}

//-----------------------------------------------------------------------------
const char* PLUGIN_API HostMessage::getMessageID ()
{
	return messageId;
}

//-----------------------------------------------------------------------------
void PLUGIN_API HostMessage::setMessageID (const char* mid)
{
	if (messageId)
		delete[] messageId;
	messageId = nullptr;
	if (mid)
	{
		size_t len = strlen (mid) + 1;
		messageId = new char[len];
		strcpy (messageId, mid);
	}
}

//-----------------------------------------------------------------------------
IAttributeList* PLUGIN_API HostMessage::getAttributes ()
{
	if (!attributeList)
		attributeList = new HostAttributeList;
	return attributeList;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class HostAttribute
{
public:
	enum Type
	{
		kInteger,
		kFloat,
		kString,
		kBinary
	};

	HostAttribute (Steinberg::int64 value) : size (0), type (kInteger) { v.intValue = value; }
	HostAttribute (double value) : size (0), type (kFloat) { v.floatValue = value; }
	/** size is in code unit (count of TChar) */
	HostAttribute (const TChar* value, Steinberg::uint32 sizeInCodeUnit) : size (sizeInCodeUnit), type (kString)
	{
		v.stringValue = new TChar[sizeInCodeUnit];
		memcpy (v.stringValue, value, sizeInCodeUnit * sizeof (TChar));
	}
	HostAttribute (const void* value, Steinberg::uint32 sizeInBytes) : size (sizeInBytes), type (kBinary)
	{
		v.binaryValue = new char[sizeInBytes];
		memcpy (v.binaryValue, value, sizeInBytes);
	}
	~HostAttribute ()
	{
		if (size)
			delete[] v.binaryValue;
	}

	Steinberg::int64 intValue () const { return v.intValue; }
	double floatValue () const { return v.floatValue; }
	/** sizeInCodeUnit is in code unit (count of TChar) */
	const TChar* stringValue (Steinberg::uint32& sizeInCodeUnit)
	{
		sizeInCodeUnit = size;
		return v.stringValue;
	}
	const void* binaryValue (Steinberg::uint32& sizeInBytes)
	{
		sizeInBytes = size;
		return v.binaryValue;
	}

	Type getType () const { return type; }

protected:
	union v
	{
		Steinberg::int64 intValue;
		double floatValue;
		TChar* stringValue;
		char* binaryValue;
	} v;
	Steinberg::uint32 size;
	Type type;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
IMPLEMENT_FUNKNOWN_METHODS (HostAttributeList, IAttributeList, IAttributeList::iid)
//-----------------------------------------------------------------------------
HostAttributeList::HostAttributeList ()
{
	FUNKNOWN_CTOR
}

//-----------------------------------------------------------------------------
HostAttributeList::~HostAttributeList ()
{
	auto it = list.rbegin ();
	while (it != list.rend ())
	{
		delete it->second;
		it++;
	}
	FUNKNOWN_DTOR
}

//-----------------------------------------------------------------------------
void HostAttributeList::removeAttrID (AttrID aid)
{
	auto it = list.find (aid);
	if (it != list.end ())
	{
		delete it->second;
		list.erase (it);
	}
}

//-----------------------------------------------------------------------------
Steinberg::tresult PLUGIN_API HostAttributeList::setInt (AttrID aid, Steinberg::int64 value)
{
	removeAttrID (aid);
	list[aid] = new HostAttribute (value);
	return Steinberg::kResultTrue;
}

//-----------------------------------------------------------------------------
Steinberg::tresult PLUGIN_API HostAttributeList::getInt (AttrID aid, Steinberg::int64& value)
{
	auto it = list.find (aid);
	if (it != list.end () && it->second)
	{
		value = it->second->intValue ();
		return Steinberg::kResultTrue;
	}
	return Steinberg::kResultFalse;
}

//-----------------------------------------------------------------------------
Steinberg::tresult PLUGIN_API HostAttributeList::setFloat (AttrID aid, double value)
{
	removeAttrID (aid);
	list[aid] = new HostAttribute (value);
	return Steinberg::kResultTrue;
}

//-----------------------------------------------------------------------------
Steinberg::tresult PLUGIN_API HostAttributeList::getFloat (AttrID aid, double& value)
{
	auto it = list.find (aid);
	if (it != list.end () && it->second)
	{
		value = it->second->floatValue ();
		return Steinberg::kResultTrue;
	}
	return Steinberg::kResultFalse;
}

//-----------------------------------------------------------------------------
Steinberg::tresult PLUGIN_API HostAttributeList::setString (AttrID aid, const TChar* string)
{
	removeAttrID (aid);
	// + 1 for the null-terminate
	list[aid] = new HostAttribute (string, Steinberg::String (string).length () + 1);
	return Steinberg::kResultTrue;
}

//-----------------------------------------------------------------------------
Steinberg::tresult PLUGIN_API HostAttributeList::getString (AttrID aid, TChar* string, Steinberg::uint32 sizeInBytes)
{
	auto it = list.find (aid);
	if (it != list.end () && it->second)
	{
		Steinberg::uint32 sizeInCodeUnit = 0;
		const TChar* _string = it->second->stringValue (sizeInCodeUnit);
		memcpy (string, _string, std::min<Steinberg::uint32> (sizeInCodeUnit * sizeof (TChar), sizeInBytes));
		return Steinberg::kResultTrue;
	}
	return Steinberg::kResultFalse;
}

//-----------------------------------------------------------------------------
Steinberg::tresult PLUGIN_API HostAttributeList::setBinary (AttrID aid, const void* data, Steinberg::uint32 sizeInBytes)
{
	removeAttrID (aid);
	list[aid] = new HostAttribute (data, sizeInBytes);
	return Steinberg::kResultTrue;
}

//-----------------------------------------------------------------------------
Steinberg::tresult PLUGIN_API HostAttributeList::getBinary (AttrID aid, const void*& data, Steinberg::uint32& sizeInBytes)
{
	auto it = list.find (aid);
	if (it != list.end () && it->second)
	{
		data = it->second->binaryValue (sizeInBytes);
		return Steinberg::kResultTrue;
	}
	sizeInBytes = 0;
	return Steinberg::kResultFalse;
}
  }
}
