/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/



// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_dbaccess.hxx"

#ifndef _DBA_CORE_BOOKMARKCONTAINER_HXX_
#include "bookmarkcontainer.hxx"
#endif
#ifndef DBACCESS_SHARED_DBASTRINGS_HRC
#include "dbastrings.hrc"
#endif
#ifndef _DBASHARED_APITOOLS_HXX_
#include "apitools.hxx"
#endif
#ifndef _DBA_CORE_RESOURCE_HXX_
#include "core_resource.hxx"
#endif
#ifndef _DBA_CORE_RESOURCE_HRC_
#include "core_resource.hrc"
#endif

#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif
#ifndef _COMPHELPER_SEQUENCE_HXX_
#include <comphelper/sequence.hxx>
#endif
#ifndef _COMPHELPER_ENUMHELPER_HXX_
#include <comphelper/enumhelper.hxx>
#endif
#ifndef _COMPHELPER_EXTRACT_HXX_
#include <comphelper/extract.hxx>
#endif
#ifndef _COM_SUN_STAR_LANG_XCOMPONENT_HPP_
#include <com/sun/star/lang/XComponent.hpp>
#endif
#ifndef _COMPHELPER_TYPES_HXX_
#include <comphelper/types.hxx>
#endif

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::container;
using namespace ::osl;
using namespace ::comphelper;
using namespace ::cppu;

//........................................................................
namespace dbaccess
{
//........................................................................

//==========================================================================
//= OBookmarkContainer
//==========================================================================
DBG_NAME(OBookmarkContainer)
//--------------------------------------------------------------------------
OBookmarkContainer::OBookmarkContainer(OWeakObject& _rParent, Mutex& _rMutex)
	:m_rParent(_rParent)
	,m_aContainerListeners(_rMutex)
	,m_rMutex(_rMutex)
{
	DBG_CTOR(OBookmarkContainer, NULL);
}

//--------------------------------------------------------------------------
void OBookmarkContainer::dispose()
{
	MutexGuard aGuard(m_rMutex);

	// say our listeners goobye
	EventObject aEvt(*this);
	m_aContainerListeners.disposeAndClear(aEvt);

	// remove our elements
	m_aBookmarksIndexed.clear();
	m_aBookmarks.clear();
}

//--------------------------------------------------------------------------
void SAL_CALL OBookmarkContainer::acquire(  ) throw()
{
	m_rParent.acquire();
}

//--------------------------------------------------------------------------
void SAL_CALL OBookmarkContainer::release(  ) throw()
{
	m_rParent.release();
}

//--------------------------------------------------------------------------
OBookmarkContainer::~OBookmarkContainer()
{
	DBG_DTOR(OBookmarkContainer, NULL);
}

// XServiceInfo
//--------------------------------------------------------------------------
::rtl::OUString SAL_CALL OBookmarkContainer::getImplementationName(  ) throw(RuntimeException)
{
	return ::rtl::OUString::createFromAscii("com.sun.star.comp.dba.OBookmarkContainer");
}

//--------------------------------------------------------------------------
sal_Bool SAL_CALL OBookmarkContainer::supportsService( const ::rtl::OUString& _rServiceName ) throw (RuntimeException)
{
	MutexGuard aGuard(m_rMutex);
	checkValid(sal_False);
	return findValue(getSupportedServiceNames(), _rServiceName, sal_True).getLength() != 0;
}

//--------------------------------------------------------------------------
Sequence< ::rtl::OUString > SAL_CALL OBookmarkContainer::getSupportedServiceNames(  ) throw(RuntimeException)
{
	Sequence< ::rtl::OUString > aReturn(1);
	aReturn.getArray()[0] = ::rtl::OUString::createFromAscii("com.sun.star.sdb.DefinitionContainer");
	return aReturn;
}

// XNameContainer
//--------------------------------------------------------------------------
void SAL_CALL OBookmarkContainer::insertByName( const ::rtl::OUString& _rName, const Any& aElement ) throw(IllegalArgumentException, ElementExistException, WrappedTargetException, RuntimeException)
{
	MutexGuard aGuard(m_rMutex);
	checkValid(sal_True);

	if (checkExistence(_rName))
		throw ElementExistException();

	if (0 == _rName.getLength())
		throw IllegalArgumentException();

	// approve the new object
	::rtl::OUString sNewLink;
	if (!(aElement >>= sNewLink))
		throw IllegalArgumentException();


	implAppend(_rName, sNewLink);

	// notify the listeners
	if (m_aContainerListeners.getLength())
	{
		ContainerEvent aEvent(*this, makeAny(_rName), makeAny(sNewLink), Any());
		OInterfaceIteratorHelper aListenerIterator(m_aContainerListeners);
		while (aListenerIterator.hasMoreElements())
			static_cast< XContainerListener* >(aListenerIterator.next())->elementInserted(aEvent);
	}
}

//--------------------------------------------------------------------------
void SAL_CALL OBookmarkContainer::removeByName( const ::rtl::OUString& _rName ) throw(NoSuchElementException, WrappedTargetException, RuntimeException)
{
	::rtl::OUString sOldBookmark;
	{
		MutexGuard aGuard(m_rMutex);
		checkValid(sal_True);

		// check the arguments
		if (!_rName.getLength())
			throw IllegalArgumentException();

		if (!checkExistence(_rName))
			throw NoSuchElementException();

		// the old element (for the notifications)
		sOldBookmark = m_aBookmarks[_rName];

		// do the removal
		implRemove(_rName);
	}

	// notify the listeners
	if (m_aContainerListeners.getLength())
	{
		ContainerEvent aEvent(*this, makeAny(_rName), makeAny(sOldBookmark), Any());
		OInterfaceIteratorHelper aListenerIterator(m_aContainerListeners);
		while (aListenerIterator.hasMoreElements())
			static_cast< XContainerListener* >(aListenerIterator.next())->elementRemoved(aEvent);
	}
}

// XNameReplace
//--------------------------------------------------------------------------
void SAL_CALL OBookmarkContainer::replaceByName( const ::rtl::OUString& _rName, const Any& aElement ) throw(IllegalArgumentException, NoSuchElementException, WrappedTargetException, RuntimeException)
{
	ClearableMutexGuard aGuard(m_rMutex);
	checkValid(sal_True);

	// check the arguments
	if (!_rName.getLength())
		throw IllegalArgumentException();

	// do we have such an element?
	if (!checkExistence(_rName))
		throw NoSuchElementException();

	// approve the new object
	::rtl::OUString sNewLink;
	if (!(aElement >>= sNewLink))
		throw IllegalArgumentException();

	// the old element (for the notifications)
	::rtl::OUString sOldLink = m_aBookmarks[_rName];

	// do the replace
	implReplace(_rName, sNewLink);

	// notify the listeners
	aGuard.clear();
	if (m_aContainerListeners.getLength())
	{
		ContainerEvent aEvent(*this, makeAny(_rName), makeAny(sNewLink), makeAny(sOldLink));
		OInterfaceIteratorHelper aListenerIterator(m_aContainerListeners);
		while (aListenerIterator.hasMoreElements())
			static_cast< XContainerListener* >(aListenerIterator.next())->elementReplaced(aEvent);
	}
}

//--------------------------------------------------------------------------
void SAL_CALL OBookmarkContainer::addContainerListener( const Reference< XContainerListener >& _rxListener ) throw(RuntimeException)
{
	MutexGuard aGuard(m_rMutex);
	if (_rxListener.is())
		m_aContainerListeners.addInterface(_rxListener);
}

//--------------------------------------------------------------------------
void SAL_CALL OBookmarkContainer::removeContainerListener( const Reference< XContainerListener >& _rxListener ) throw(RuntimeException)
{
	MutexGuard aGuard(m_rMutex);
	if (_rxListener.is())
		m_aContainerListeners.removeInterface(_rxListener);
}

// XElementAccess
//--------------------------------------------------------------------------
Type SAL_CALL OBookmarkContainer::getElementType( ) throw (RuntimeException)
{
	MutexGuard aGuard(m_rMutex);
	checkValid(sal_False);
	return ::getCppuType( static_cast< ::rtl::OUString* >(NULL) );
}

//--------------------------------------------------------------------------
sal_Bool SAL_CALL OBookmarkContainer::hasElements( ) throw (RuntimeException)
{
	MutexGuard aGuard(m_rMutex);
	checkValid(sal_False);
	return !m_aBookmarks.empty();
}

// XEnumerationAccess
//--------------------------------------------------------------------------
Reference< XEnumeration > SAL_CALL OBookmarkContainer::createEnumeration(  ) throw(RuntimeException)
{
	MutexGuard aGuard(m_rMutex);
	checkValid(sal_False);
	return new ::comphelper::OEnumerationByIndex(static_cast<XIndexAccess*>(this));
}

//--------------------------------------------------------------------------
// XIndexAccess
sal_Int32 SAL_CALL OBookmarkContainer::getCount(  ) throw(RuntimeException)
{
	MutexGuard aGuard(m_rMutex);
	checkValid(sal_False);
	return m_aBookmarks.size();
}

//--------------------------------------------------------------------------
Any SAL_CALL OBookmarkContainer::getByIndex( sal_Int32 _nIndex ) throw(IndexOutOfBoundsException, WrappedTargetException, RuntimeException)
{
	MutexGuard aGuard(m_rMutex);
	checkValid(sal_False);

	if ((_nIndex < 0) || (_nIndex >= (sal_Int32)m_aBookmarksIndexed.size()))
		throw IndexOutOfBoundsException();

	return makeAny(m_aBookmarksIndexed[_nIndex]->second);
}

//--------------------------------------------------------------------------
Any SAL_CALL OBookmarkContainer::getByName( const ::rtl::OUString& _rName ) throw(NoSuchElementException, WrappedTargetException, RuntimeException)
{
	MutexGuard aGuard(m_rMutex);
	checkValid(sal_False);

	if (!checkExistence(_rName))
		throw NoSuchElementException();

	return makeAny(m_aBookmarks[_rName]);
}

//--------------------------------------------------------------------------
Sequence< ::rtl::OUString > SAL_CALL OBookmarkContainer::getElementNames(  ) throw(RuntimeException)
{
	MutexGuard aGuard(m_rMutex);
	checkValid(sal_False);

	Sequence< ::rtl::OUString > aNames(m_aBookmarks.size());
	::rtl::OUString* pNames = aNames.getArray();
	;
	for	(	ConstMapIteratorVectorIterator aNameIter = m_aBookmarksIndexed.begin();
			aNameIter != m_aBookmarksIndexed.end();
			++pNames, ++aNameIter
		)
	{
		*pNames = (*aNameIter)->first;
	}

	return aNames;
}

//--------------------------------------------------------------------------
sal_Bool SAL_CALL OBookmarkContainer::hasByName( const ::rtl::OUString& _rName ) throw(RuntimeException)
{
	MutexGuard aGuard(m_rMutex);
	checkValid(sal_False);

	return checkExistence(_rName);
}

//--------------------------------------------------------------------------
void OBookmarkContainer::implRemove(const ::rtl::OUString& _rName)
{
	MutexGuard aGuard(m_rMutex);

	// look for the name in the index access vector
	MapString2StringIterator aMapPos = m_aBookmarks.end();
	for (	MapIteratorVectorIterator aSearch = m_aBookmarksIndexed.begin();
			aSearch != m_aBookmarksIndexed.end();
			++aSearch
		)
	{
#ifdef DBG_UTIL
		::rtl::OUString sName = (*aSearch)->first;
#endif
		if ((*aSearch)->first == _rName)
		{
			aMapPos = *aSearch;
			m_aBookmarksIndexed.erase(aSearch);
			break;
		}
	}

	if (m_aBookmarks.end() == aMapPos)
	{
		DBG_ERROR("OBookmarkContainer::implRemove: inconsistence!");
		return;
	}

	// remove the map entries
	m_aBookmarks.erase(aMapPos);
}

//--------------------------------------------------------------------------
void OBookmarkContainer::implAppend(const ::rtl::OUString& _rName, const ::rtl::OUString& _rDocumentLocation)
{
	MutexGuard aGuard(m_rMutex);

	OSL_ENSURE(m_aBookmarks.find(_rName) == m_aBookmarks.end(),"Bookmark already known!");
	m_aBookmarksIndexed.push_back(m_aBookmarks.insert(	MapString2String::value_type(_rName,_rDocumentLocation)).first);
}

//--------------------------------------------------------------------------
void OBookmarkContainer::implReplace(const ::rtl::OUString& _rName, const ::rtl::OUString& _rNewLink)
{
	MutexGuard aGuard(m_rMutex);
	DBG_ASSERT(checkExistence(_rName), "OBookmarkContainer::implReplace : invalid name !");

	m_aBookmarks[_rName] = _rNewLink;
}

//--------------------------------------------------------------------------
void OBookmarkContainer::checkValid(sal_Bool /*_bIntendWriteAccess*/) const throw (RuntimeException, DisposedException)
{
}

//--------------------------------------------------------------------------
Reference< XInterface > SAL_CALL OBookmarkContainer::getParent(  ) throw (RuntimeException)
{
	return m_rParent;
}

//--------------------------------------------------------------------------
void SAL_CALL OBookmarkContainer::setParent( const Reference< XInterface >& /*Parent*/ ) throw (NoSupportException, RuntimeException)
{
	throw NoSupportException();
}

//........................................................................
}	// namespace dbaccess
//........................................................................
