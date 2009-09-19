// WDXTagLib is a content plugin for Total Commander that allows you to show audio
// file tags in columns, edit tags and search in tags.
// Copyright (C) 2008 Dmitrie Murzaikin (murzzz@gmail.com)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#include "WDXBase.h"
#include "utils.h"
#include <stdexcept>

namespace WDX_API
{
WDXBase::WDXBase()
:	m_PluginInterfaceVerionHi(0),
	m_PluginInterfaceVerionLow(0)
{
	utils::DbgStr(__PRETTY_FUNCTION__);

	ClearAbortedFlag();
}

WDXBase::~WDXBase()
{
	utils::DbgStr(__PRETTY_FUNCTION__);
}

std::string WDXBase::GetDetectString() const
{
	utils::DbgStr(__PRETTY_FUNCTION__);

	return OnGetDetectString();
}

std::string WDXBase::OnGetDetectString() const
{
	utils::DbgStr(__PRETTY_FUNCTION__);

	return "";
}

void WDXBase::SetIniName( const std::string& sIniName )
{
	utils::DbgStr(__PRETTY_FUNCTION__);

	if (sIniName == m_IniName)
		return;
	m_IniName = sIniName;
	OnSetIniName( m_IniName );
}

void WDXBase::OnSetIniName( const std::string& sIniName )
{
	utils::DbgStr(__PRETTY_FUNCTION__);
}

std::string WDXBase::GetIniName() const
{
	utils::DbgStr(__PRETTY_FUNCTION__);

	return m_IniName;
}

void WDXBase::SetPluginInterfaceVersion( const DWORD dwHi, const DWORD dwLow )
{
	utils::DbgStr(__PRETTY_FUNCTION__);

	m_PluginInterfaceVerionHi = dwHi;
	m_PluginInterfaceVerionLow = dwLow;

	OnSetPluginInterfaceVersion( m_PluginInterfaceVerionHi, m_PluginInterfaceVerionLow );
}

void WDXBase::OnSetPluginInterfaceVersion( const DWORD dwHi, const DWORD dwLow )
{
	utils::DbgStr(__PRETTY_FUNCTION__);
}

EFieldType WDXBase::GetSupportedField( const int iFieldIndex, char* pszFieldName,
								char* pszUnits, int iMaxLen )
{
	utils::DbgStr(__PRETTY_FUNCTION__);

	if ( iFieldIndex < 0 || iFieldIndex >= static_cast<int>(m_Fields.Count()) )
		return ftNoMoreFields;

	const Field& f = m_Fields.Find( iFieldIndex );
	utils::strlcpy( pszFieldName, f.m_Name.c_str(), iMaxLen - 1 );
	utils::strlcpy( pszUnits, f.m_MultChoice.c_str(), iMaxLen - 1 );
	return f.m_Type;
}

EFieldType WDXBase::GetValue( const WCHAR* pszFileName, const int iFieldIndex,
						const int iUnitIndex, void* pFieldValue,
						const int iMaxLen, const int iFlags )
{
	utils::DbgStr(__PRETTY_FUNCTION__);

	///@todo do parameter checking properly
	if (iUnitIndex < 0)
		utils::ShowError(utils::Int2Str(iUnitIndex));

	///@todo encapsulate this into FieldList, or throw NoSuchField exception
	if ( iFieldIndex < 0 || iFieldIndex >= static_cast<int>(m_Fields.Count()) )
		return ftNoSuchField;

	// abort flag down
	ClearAbortedFlag( );

	///@todo make interface easier, hide field value in Field class and so on
	return OnGetValue(pszFileName, iFieldIndex, iUnitIndex, pFieldValue, iMaxLen, iFlags);
}

EFieldType WDXBase::SetValue( const WCHAR* FileName, const int FieldIndex,
						const int UnitIndex, const int FieldType,
						const void* FieldValue, const int flags )
{
	utils::DbgStr(__PRETTY_FUNCTION__);

	///@todo here it should be &&, not ||
	if ( !FileName || (-1 == FieldIndex) ) // this indicates end of changing attributes
	{
		OnEndOfSetValue();
		return ftSetSuccess;
	}

	if ( FieldIndex < 0 || FieldIndex >= static_cast<int>(m_Fields.Count()) )
		return ftNoSuchField;

	return OnSetValue(FileName, FieldIndex, UnitIndex, FieldType, FieldValue, flags);
}

EFieldType WDXBase::OnSetValue(const std::wstring& sFileName, const int iFieldIndex,
				const int iUnitIndex, const int iFieldType,
				const void* pFieldValue, const int iFlags) const
{
	utils::DbgStr(__PRETTY_FUNCTION__);

	return ftNoSuchField;
}

int WDXBase::GetSupportedFieldFlags(const int iFieldIndex)
{
	utils::DbgStr(__PRETTY_FUNCTION__);

	if (-1 == iFieldIndex) // we should return a combination of all supported flags here
	{
		return m_Fields.GetAllFlags();
	}

	if ( iFieldIndex < 0 || iFieldIndex >= static_cast<int>(m_Fields.Count()) )
	{
		return ftNoMoreFields;
	}

	return m_Fields.Find( iFieldIndex ).m_Flag;
}

void WDXBase::OnEndOfSetValue() const
{
	utils::DbgStr(__PRETTY_FUNCTION__);
}

void WDXBase::StopGetValue(const std::wstring& sFileName)
{
	utils::DbgStr(__PRETTY_FUNCTION__);

	// abort flag up

	SetAbortedFilename( sFileName );
	SetAbortedFlag( );
}

void WDXBase::SetAbortedFlag( )
{
	utils::DbgStr(__PRETTY_FUNCTION__);

	// aborted!
	m_bIsAborted = true;
}

void WDXBase::ClearAbortedFlag( )
{
	utils::DbgStr(__PRETTY_FUNCTION__);

	// clear filename if previous state was ABORTED
	if (m_bIsAborted)
	{
		SetAbortedFilename(TEXT(""));
	}

	// clear flag itself
	m_bIsAborted = false;
}

bool WDXBase::IsAborted() const
{
	utils::DbgStr(__PRETTY_FUNCTION__);

	return m_bIsAborted;
}

void WDXBase::SetAbortedFilename(const std::wstring& sValue)
{
	utils::DbgStr(__PRETTY_FUNCTION__);

	m_sAbortedFilename = sValue;
}

std::wstring WDXBase::GetAbortedFilename() const
{
	utils::DbgStr(__PRETTY_FUNCTION__);

	return m_sAbortedFilename;
}

void WDXBase::PluginUnloading()
{
	utils::DbgStr(__PRETTY_FUNCTION__);

	OnPluginUnloading();
}

void WDXBase::OnPluginUnloading()
{
	utils::DbgStr(__PRETTY_FUNCTION__);
}

FieldList::FieldList()
{

}

FieldList::~FieldList()
{

}

size_t FieldList::Count() const
{
	return m_Fields.size();
}

void FieldList::Add(int nIdx, const Field& fld)
{
	std::pair<MapOfFields::iterator, bool> ResultPair =
		m_Fields.insert(MapOfFields::value_type(nIdx, fld));

	if (!ResultPair.second)
	{
		throw std::runtime_error("Failed to add field");
	}
}

Field& FieldList::Find(const int Idx)
{
	MapOfFields::iterator iter = m_Fields.find(Idx);
	if (m_Fields.end() == iter)
	{
		throw std::runtime_error("Failed to find field by index");
	}

	return (*iter).second;
}

int FieldList::GetAllFlags()
{
	int iTotalFlags = 0;
	for (MapOfFields::const_iterator iter = m_Fields.begin();
		iter != m_Fields.end();
		++iter)
	{
		const Field& f = (*iter).second;
		if (f.m_Flag)
			iTotalFlags |= f.m_Flag;
	}
	return iTotalFlags;
}

} // namespace
