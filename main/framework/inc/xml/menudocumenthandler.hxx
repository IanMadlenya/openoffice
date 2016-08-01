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



#ifndef __FRAMEWORK_XML_MENUDOCUMENTHANDLER_HXX_
#define __FRAMEWORK_XML_MENUDOCUMENTHANDLER_HXX_

//_________________________________________________________________________________________________________________
//	interface includes
//_________________________________________________________________________________________________________________

#ifndef __COM_SUN_STAR_XML_SAX_XDOCUMENTHANDLER_HPP_
#include <com/sun/star/xml/sax/XDocumentHandler.hpp>
#endif
#include <com/sun/star/container/XIndexContainer.hpp>
#include <com/sun/star/lang/XSingleComponentFactory.hpp>
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/uno/Sequence.h>
#include <com/sun/star/uno/XComponentContext.hpp>

//_________________________________________________________________________________________________________________
//	other includes
//_________________________________________________________________________________________________________________
#include <threadhelp/threadhelpbase.hxx>
#include <rtl/ustring.hxx>
#include <cppuhelper/implbase1.hxx>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <framework/fwedllapi.h>

//_________________________________________________________________________________________________________________
//	namespace
//_________________________________________________________________________________________________________________

namespace framework{

class FWE_DLLPUBLIC ReadMenuDocumentHandlerBase : public ThreadHelpBase,	// Struct for right initialization of mutex member! Must be first of baseclasses.
									public ::cppu::WeakImplHelper1< ::com::sun::star::xml::sax::XDocumentHandler >
{
	public:
		ReadMenuDocumentHandlerBase();
		virtual ~ReadMenuDocumentHandlerBase();

		// XDocumentHandler
		virtual void SAL_CALL startDocument(void)
		throw (	::com::sun::star::xml::sax::SAXException,
				::com::sun::star::uno::RuntimeException ) = 0;

		virtual void SAL_CALL endDocument(void)
		throw(	::com::sun::star::xml::sax::SAXException,
				::com::sun::star::uno::RuntimeException ) = 0;

		virtual void SAL_CALL startElement(
			const rtl::OUString& aName,
			const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList > &xAttribs)
		throw(	::com::sun::star::xml::sax::SAXException,
				::com::sun::star::uno::RuntimeException ) = 0;

		virtual void SAL_CALL endElement(const rtl::OUString& aName)
		throw(	::com::sun::star::xml::sax::SAXException,
				::com::sun::star::uno::RuntimeException ) = 0;

		virtual void SAL_CALL characters(const rtl::OUString& aChars)
		throw(	::com::sun::star::xml::sax::SAXException,
				::com::sun::star::uno::RuntimeException ) = 0;

		virtual void SAL_CALL ignorableWhitespace(const rtl::OUString& aWhitespaces)
		throw(	::com::sun::star::xml::sax::SAXException,
				::com::sun::star::uno::RuntimeException );

		virtual void SAL_CALL processingInstruction(const rtl::OUString& aTarget,
													const rtl::OUString& aData)
		throw(	::com::sun::star::xml::sax::SAXException,
				::com::sun::star::uno::RuntimeException );

		virtual void SAL_CALL setDocumentLocator(
			const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XLocator > &xLocator)
		throw(	::com::sun::star::xml::sax::SAXException,
				::com::sun::star::uno::RuntimeException );

	protected:
		::rtl::OUString getErrorLineString();

		::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XLocator > m_xLocator;
		::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XDocumentHandler> m_xReader;
		void initPropertyCommon( com::sun::star::uno::Sequence< com::sun::star::beans::PropertyValue > &rProps,
								 const rtl::OUString &rCommandURL, const rtl::OUString &rHelpId,
                                 const rtl::OUString &rLabel, sal_Int16 nItemStyleBits );
	private:
		rtl::OUString m_aType;
		rtl::OUString m_aLabel;
		rtl::OUString m_aContainer;
		rtl::OUString m_aHelpURL;
		rtl::OUString m_aCommandURL;
        rtl::OUString m_aStyle;
		::com::sun::star::uno::Sequence< com::sun::star::beans::PropertyValue > m_aItemProp;
};


class FWE_DLLPUBLIC OReadMenuDocumentHandler : public ReadMenuDocumentHandlerBase
{
	public:
		// #110897#
        OReadMenuDocumentHandler(
			const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xServiceFactory,
			const com::sun::star::uno::Reference< com::sun::star::container::XIndexContainer >& rItemContainer );
		virtual ~OReadMenuDocumentHandler();

		// #110897#
		const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& getServiceFactory();

		// XDocumentHandler
		virtual void SAL_CALL startDocument(void)
		throw (	::com::sun::star::xml::sax::SAXException,
				::com::sun::star::uno::RuntimeException );

		virtual void SAL_CALL endDocument(void)
		throw(	::com::sun::star::xml::sax::SAXException,
				::com::sun::star::uno::RuntimeException );

		virtual void SAL_CALL startElement(
			const rtl::OUString& aName,
			const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList > &xAttribs)
		throw(	::com::sun::star::xml::sax::SAXException,
				::com::sun::star::uno::RuntimeException );

		virtual void SAL_CALL endElement(const rtl::OUString& aName)
		throw(	::com::sun::star::xml::sax::SAXException,
				::com::sun::star::uno::RuntimeException );

		virtual void SAL_CALL characters(const rtl::OUString& aChars)
		throw(	::com::sun::star::xml::sax::SAXException,
				::com::sun::star::uno::RuntimeException );

	private:
          int		m_nElementDepth;
          sal_Bool	m_bMenuBarMode;
          com::sun::star::uno::Reference< ::com::sun::star::container::XIndexContainer > m_xMenuBarContainer;
          com::sun::star::uno::Reference< ::com::sun::star::lang::XSingleComponentFactory > m_xContainerFactory;
          const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& mxServiceFactory;
};	// OReadMenuDocumentHandler


class FWE_DLLPUBLIC OReadMenuBarHandler : public ReadMenuDocumentHandlerBase
{
	public:
		// #110897#
		OReadMenuBarHandler(
			const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xServiceFactory,
			const com::sun::star::uno::Reference< ::com::sun::star::container::XIndexContainer >& rMenuBarContainer,
            const com::sun::star::uno::Reference< ::com::sun::star::lang::XSingleComponentFactory >& rContainerFactory );
		virtual ~OReadMenuBarHandler();

		// #110897#
		const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& getServiceFactory();

		// XDocumentHandler
		virtual void SAL_CALL startDocument(void)
		throw (	::com::sun::star::xml::sax::SAXException,
				::com::sun::star::uno::RuntimeException );

		virtual void SAL_CALL endDocument(void)
		throw(	::com::sun::star::xml::sax::SAXException,
				::com::sun::star::uno::RuntimeException );

		virtual void SAL_CALL startElement(
			const rtl::OUString& aName,
			const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList > &xAttribs)
		throw(	::com::sun::star::xml::sax::SAXException,
				::com::sun::star::uno::RuntimeException );

		virtual void SAL_CALL endElement(const rtl::OUString& aName)
		throw(	::com::sun::star::xml::sax::SAXException,
				::com::sun::star::uno::RuntimeException );

		virtual void SAL_CALL characters(const rtl::OUString& aChars)
		throw(	::com::sun::star::xml::sax::SAXException,
				::com::sun::star::uno::RuntimeException );

	private:
		int			m_nElementDepth;
		sal_Bool	m_bMenuMode;
        com::sun::star::uno::Reference< ::com::sun::star::container::XIndexContainer > m_xMenuBarContainer;
        com::sun::star::uno::Reference< ::com::sun::star::lang::XSingleComponentFactory > m_xContainerFactory;

		// #110897#
		const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& mxServiceFactory;
};	// OReadMenuBarHandler


class FWE_DLLPUBLIC OReadMenuHandler : public ReadMenuDocumentHandlerBase
{
	public:
		OReadMenuHandler( const com::sun::star::uno::Reference< ::com::sun::star::container::XIndexContainer >& rMenuContainer,
                          const com::sun::star::uno::Reference< ::com::sun::star::lang::XSingleComponentFactory >& rContainerFactory );
		virtual ~OReadMenuHandler();

		// XDocumentHandler
		virtual void SAL_CALL startDocument(void)
		throw (	::com::sun::star::xml::sax::SAXException,
				::com::sun::star::uno::RuntimeException );

		virtual void SAL_CALL endDocument(void)
		throw(	::com::sun::star::xml::sax::SAXException,
				::com::sun::star::uno::RuntimeException );

		virtual void SAL_CALL startElement(
			const rtl::OUString& aName,
			const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList > &xAttribs)
		throw(	::com::sun::star::xml::sax::SAXException,
				::com::sun::star::uno::RuntimeException );

		virtual void SAL_CALL endElement(const rtl::OUString& aName)
		throw(	::com::sun::star::xml::sax::SAXException,
				::com::sun::star::uno::RuntimeException );

		virtual void SAL_CALL characters(const rtl::OUString& aChars)
		throw(	::com::sun::star::xml::sax::SAXException,
				::com::sun::star::uno::RuntimeException );

	private:
		int					m_nElementDepth;
		sal_Bool			m_bMenuPopupMode;
        com::sun::star::uno::Reference< ::com::sun::star::container::XIndexContainer > m_xMenuContainer;
        com::sun::star::uno::Reference< ::com::sun::star::lang::XSingleComponentFactory > m_xContainerFactory;
}; // OReadMenuHandler


class FWE_DLLPUBLIC OReadMenuPopupHandler : public ReadMenuDocumentHandlerBase
{
	public:
		OReadMenuPopupHandler( const com::sun::star::uno::Reference< ::com::sun::star::container::XIndexContainer >& rMenuContainer,
                               const com::sun::star::uno::Reference< ::com::sun::star::lang::XSingleComponentFactory >& rContainerFactory );
		virtual ~OReadMenuPopupHandler();

		// XDocumentHandler
		virtual void SAL_CALL startDocument(void)
			throw (	::com::sun::star::xml::sax::SAXException,
					::com::sun::star::uno::RuntimeException );

		virtual void SAL_CALL endDocument(void)
			throw (	::com::sun::star::xml::sax::SAXException,
					::com::sun::star::uno::RuntimeException );

		virtual void SAL_CALL startElement(
			const rtl::OUString& aName,
			const ::com::sun::star::uno::Reference<
				::com::sun::star::xml::sax::XAttributeList > &xAttribs)
			throw (	::com::sun::star::xml::sax::SAXException,
					::com::sun::star::uno::RuntimeException );

		virtual void SAL_CALL endElement(const rtl::OUString& aName)
			throw (	::com::sun::star::xml::sax::SAXException,
					::com::sun::star::uno::RuntimeException );

		virtual void SAL_CALL characters(const rtl::OUString& aChars)
			throw (	::com::sun::star::xml::sax::SAXException,
					::com::sun::star::uno::RuntimeException );

	private:
		enum NextElementClose { ELEM_CLOSE_NONE, ELEM_CLOSE_MENUITEM, ELEM_CLOSE_MENUSEPARATOR };

		int					m_nElementDepth;
		sal_Bool			m_bMenuMode;
        com::sun::star::uno::Reference< ::com::sun::star::container::XIndexContainer > m_xMenuContainer;
        com::sun::star::uno::Reference< ::com::sun::star::lang::XSingleComponentFactory > m_xContainerFactory;
        com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >     m_xComponentContext;
		NextElementClose	m_nNextElementExpected;
}; // OReadMenuPopupHandler


class FWE_DLLPUBLIC OWriteMenuDocumentHandler
{
	public:
		OWriteMenuDocumentHandler(
            const ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess >& rMenuBarContainer,
			const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XDocumentHandler >& rDocumentHandler );
		virtual ~OWriteMenuDocumentHandler();

		void WriteMenuDocument() throw
			( ::com::sun::star::xml::sax::SAXException, ::com::sun::star::uno::RuntimeException );
	protected:
		virtual void WriteMenu( const ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess >& rSubMenuContainer ) throw
			( ::com::sun::star::xml::sax::SAXException, ::com::sun::star::uno::RuntimeException );

        virtual void WriteMenuItem( const rtl::OUString& aCommandURL, const rtl::OUString& aLabel, const rtl::OUString& aHelpURL, sal_Int16 nStyle = 0 );
		virtual void WriteMenuSeparator();

        com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess > m_xMenuBarContainer;
		::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XDocumentHandler > m_xWriteDocumentHandler;
		::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList > m_xEmptyList;
		::rtl::OUString m_aAttributeType;
};

} // namespace framework

#endif	// #ifndef __FRAMEWORK_XML_MENUDOCUMENTHANDLER_HXX_
