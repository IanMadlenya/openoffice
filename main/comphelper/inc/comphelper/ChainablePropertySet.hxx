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



#ifndef _COMPHELPER_CHAINABLEPROPERTYSET_HXX_
#define _COMPHELPER_CHAINABLEPROPERTYSET_HXX_

#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/beans/XPropertyState.hpp>
#include <com/sun/star/beans/XMultiPropertySet.hpp>
#include <comphelper/PropertyInfoHash.hxx>
#include "comphelper/comphelperdllapi.h"

namespace comphelper 
{ 
	class ChainablePropertySetInfo;
}
namespace vos
{
	class IMutex;
}
/*
 * A ChainablePropertySet has the following features:
 *
 * 1. It implements both the PropertySet and MultiPropertySet interfaces.
 * 2. It can be 'included' in a MasterPropertySet to seamlessly appear as if
 *    if it's properties were in the master.
 *
 * To be used as a base class for PropertySets, the subclass must implement
 * the 6 protected pure virtual functions. If a mutex is passed to the
 * constructor, this is locked before any call to _getSingleValue or
 * _setSingleValue and released after all processing has completed 
 * (including _postSetValues or _postGetValues )
 * 
 * The implementations of getPropertyValues/setPropertyValues call 
 * lockMutex and _preGetValues/_preSetValues once before calling
 * _getSingleValue/_setSingleValue for each property. After each
 * property has been dealt with, _postGetValues/_postSetValues
 * are called once.
 *
 * Any MasterPropertySet implementations that can include an 
 * implementation of a given ChainablePropertySet must be 
 * declared as a 'friend' in the implementation of the ChainablePropertySet.
 * 
 */

namespace comphelper
{
	class COMPHELPER_DLLPUBLIC ChainablePropertySet : public ::com::sun::star::beans::XPropertySet,
								 public ::com::sun::star::beans::XPropertyState,
								 public ::com::sun::star::beans::XMultiPropertySet
	{
		friend class MasterPropertySet;
	protected:
		ChainablePropertySetInfo *mpInfo;
		vos::IMutex *mpMutex;
		::com::sun::star::uno::Reference < com::sun::star::beans::XPropertySetInfo > mxInfo;
		void lockMutex();
		void unlockMutex();

		virtual void _preSetValues ()
			throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::beans::PropertyVetoException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::WrappedTargetException ) = 0;
		virtual void _setSingleValue( const comphelper::PropertyInfo & rInfo, const ::com::sun::star::uno::Any &rValue ) 
			throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::beans::PropertyVetoException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::WrappedTargetException ) = 0;
		virtual void _postSetValues ()
			throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::beans::PropertyVetoException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::WrappedTargetException ) = 0;

		virtual void _preGetValues ()
			throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::beans::PropertyVetoException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::WrappedTargetException ) = 0;
		virtual void _getSingleValue( const comphelper::PropertyInfo & rInfo, ::com::sun::star::uno::Any & rValue ) 
			throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException ) = 0;
		virtual void _postGetValues ()
			throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::beans::PropertyVetoException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::WrappedTargetException ) = 0;

		virtual void _preGetPropertyState ()
			throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::beans::PropertyVetoException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::WrappedTargetException );
		virtual void _getPropertyState( const comphelper::PropertyInfo& rInfo, ::com::sun::star::beans::PropertyState& rState ) 
			throw(::com::sun::star::beans::UnknownPropertyException );
		virtual void _postGetPropertyState ()
			throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::beans::PropertyVetoException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::WrappedTargetException );

		virtual void _setPropertyToDefault( const comphelper::PropertyInfo& rEntry )  
			throw(::com::sun::star::beans::UnknownPropertyException );
		virtual ::com::sun::star::uno::Any _getPropertyDefault( const comphelper::PropertyInfo& rEntry ) 
			throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException );

	public:
		ChainablePropertySet( comphelper::ChainablePropertySetInfo* pInfo, vos::IMutex *pMutex = NULL )
			throw();
		virtual ~ChainablePropertySet() 
			throw();

		// XPropertySet
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  ) 
			throw(::com::sun::star::uno::RuntimeException);
		virtual void SAL_CALL setPropertyValue( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Any& aValue ) 
			throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::beans::PropertyVetoException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
		virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue( const ::rtl::OUString& PropertyName ) 
			throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
		virtual void SAL_CALL addPropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& xListener ) 
			throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
		virtual void SAL_CALL removePropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& aListener ) 
			throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
		virtual void SAL_CALL addVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener ) 
			throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
		virtual void SAL_CALL removeVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener ) 
			throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);

		// XMultiPropertySet
		virtual void SAL_CALL setPropertyValues( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames, const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aValues ) 
			throw(::com::sun::star::beans::PropertyVetoException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
		virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any > SAL_CALL getPropertyValues( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames ) 
			throw(::com::sun::star::uno::RuntimeException);
		virtual void SAL_CALL addPropertiesChangeListener( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertiesChangeListener >& xListener ) 
			throw(::com::sun::star::uno::RuntimeException);
		virtual void SAL_CALL removePropertiesChangeListener( const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertiesChangeListener >& xListener ) 
			throw(::com::sun::star::uno::RuntimeException);
		virtual void SAL_CALL firePropertiesChangeEvent( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertiesChangeListener >& xListener ) 
			throw(::com::sun::star::uno::RuntimeException);

		// XPropertyState
		virtual ::com::sun::star::beans::PropertyState SAL_CALL getPropertyState( const ::rtl::OUString& PropertyName ) 
			throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::uno::RuntimeException);
		virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyState > SAL_CALL getPropertyStates( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyName ) 
			throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::uno::RuntimeException);
		virtual void SAL_CALL setPropertyToDefault( const ::rtl::OUString& PropertyName ) 
			throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::uno::RuntimeException);
		virtual ::com::sun::star::uno::Any SAL_CALL getPropertyDefault( const ::rtl::OUString& aPropertyName ) 
			throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
	};
}
#endif

