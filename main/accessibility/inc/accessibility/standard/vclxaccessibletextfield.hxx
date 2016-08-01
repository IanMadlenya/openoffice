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



#ifndef ACCESSIBILITY_STANDARD_VCLXACCESSIBLETEXTFIELD_HXX
#define ACCESSIBILITY_STANDARD_VCLXACCESSIBLETEXTFIELD_HXX

#include <accessibility/standard/vclxaccessibletextcomponent.hxx>

#ifndef _CPPUHELPER_IMPLBASE1_HXX
#include <cppuhelper/implbase1.hxx>
#endif

typedef ::cppu::ImplHelper1<
    ::com::sun::star::accessibility::XAccessible
    > VCLXAccessible_BASE;


/** This class represents non editable text fields.  The object passed to
    the constructor is expected to be a list (a <type>ListBox</type> to be
    more specific).  From this always the selected item is token to be made
    accessible by this class.  When the selected item changes then also the
    exported text changes.
*/
class VCLXAccessibleTextField :
    public VCLXAccessibleTextComponent,
    public VCLXAccessible_BASE
{
public:
	VCLXAccessibleTextField (VCLXWindow* pVCLXindow,
							 const ::com::sun::star::uno::Reference<
							 	::com::sun::star::accessibility::XAccessible >& _xParent);

    // XInterface
	DECLARE_XINTERFACE()

	// XTypeProvider
	DECLARE_XTYPEPROVIDER()

	// XAccessible
    ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleContext> SAL_CALL
        getAccessibleContext (void)
        throw (::com::sun::star::uno::RuntimeException);

    // XAccessibleContext
    sal_Int32 SAL_CALL getAccessibleChildCount (void)
        throw (::com::sun::star::uno::RuntimeException);
    ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible> SAL_CALL
        getAccessibleChild (sal_Int32 i)
        throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::uno::RuntimeException);
	sal_Int16 SAL_CALL getAccessibleRole (void)
        throw (::com::sun::star::uno::RuntimeException);
    ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL
		getAccessibleParent(  )
		throw (::com::sun::star::uno::RuntimeException);

	// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName (void)
        throw (::com::sun::star::uno::RuntimeException);
    // Return text field specific services.
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL
        getSupportedServiceNames (void)
        throw (::com::sun::star::uno::RuntimeException);

protected:
	virtual ~VCLXAccessibleTextField (void);

    /** With this method the text of the currently selected item is made
        available to the <type>VCLXAccessibleTextComponent</type> base class.
    */
    ::rtl::OUString implGetText (void);

private:
    /** We need to save the accessible parent to return it in <type>getAccessibleParent()</type>,
		because this method of the base class returns the wrong parent.
    */
	::com::sun::star::uno::Reference<
		::com::sun::star::accessibility::XAccessible >	m_xParent;
};

#endif

