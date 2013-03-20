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

#ifndef SW_SIDEBAR_PAGE_PROPERTY_PANEL_HXX
#define SW_SIDEBAR_PAGE_PROPERTY_PANEL_HXX

#include <svx/sidebar/Popup.hxx>

#include <sfx2/sidebar/ControllerItem.hxx>

namespace svx { namespace sidebar {
    class PopupControl;
} }

#include <i18npool/paper.hxx>

#include <svx/pageitem.hxx>
#include <svx/rulritem.hxx>
#include <editeng/sizeitem.hxx>

#include <vcl/ctrl.hxx>
#include <vcl/fixed.hxx>
#include <vcl/button.hxx>
#include <vcl/toolbox.hxx>
#include <vcl/lstbox.hxx>
#include <vcl/field.hxx>
#include <svl/intitem.hxx>

#include <boost/scoped_ptr.hpp>


namespace css = ::com::sun::star;
namespace cssu = ::com::sun::star::uno;

namespace sw { namespace sidebar {

    class PagePropertyPanel 
        : public Control,
          public ::sfx2::sidebar::ControllerItem::ItemUpdateReceiverInterface
    {
    public:
        static PagePropertyPanel* Create(
            Window* pParent,
            SfxBindings* pBindings );

        // interface of ::sfx2::sidebar::ControllerItem::ItemUpdateReceiverInterface
        virtual void NotifyItemUpdate(
            const sal_uInt16 nSId,
            const SfxItemState eState,
            const SfxPoolItem* pState );

        // action on 'More Options' button in panel title
        void ShowPageStyleFormatDialog( void );

        SfxBindings* GetBindings() const
        {
            return mpBindings;
        }


        ::svx::sidebar::PopupControl* CreatePageOrientationControl( ::svx::sidebar::PopupContainer* pParent );
        void ExecuteOrientationChange( const sal_Bool bLandscape );
        void ClosePageOrientationPopup();

        ::svx::sidebar::PopupControl* CreatePageMarginControl( ::svx::sidebar::PopupContainer* pParent );
        void ExecuteMarginLRChange( 
            const long nPageLeftMargin,
            const long nPageRightMargin );
        void ExecuteMarginULChange(
            const long nPageTopMargin,
            const long nPageBottomMargin );
        void ExecutePageLayoutChange( const bool bMirrored );
        void ClosePageMarginPopup();

        ::svx::sidebar::PopupControl* CreatePageSizeControl( ::svx::sidebar::PopupContainer* pParent );
        void ExecuteSizeChange( const Paper ePaper );
        void ClosePageSizePopup();

        ::svx::sidebar::PopupControl* CreatePageColumnControl( ::svx::sidebar::PopupContainer* pParent );
        void ExecuteColumnChange( const sal_uInt16 nColumnType );
        void ClosePageColumnPopup();

    private:
        PagePropertyPanel(
            Window* pParent,
            SfxBindings* pBindings );
        virtual ~PagePropertyPanel(void);

        SfxBindings* mpBindings;

        // toolboxes - on click open corresponding popup
        FixedText   maFtOrientation;
        ::boost::scoped_ptr<Window> mpToolBoxOrientationBackground;
        ::boost::scoped_ptr<ToolBox> mpToolBoxOrientation;
        FixedText				maFtMargin;
        ::boost::scoped_ptr<Window> mpToolBoxMarginBackground;
        ::boost::scoped_ptr<ToolBox> mpToolBoxMargin;
        FixedText				maFtSize;
        ::boost::scoped_ptr<Window> mpToolBoxSizeBackground;
        ::boost::scoped_ptr<ToolBox> mpToolBoxSize;
        FixedText				maFtColumn;
        ::boost::scoped_ptr<Window> mpToolBoxColumnBackground;
        ::boost::scoped_ptr<ToolBox> mpToolBoxColumn;

        Image*					maImgSize;
        Image*					maImgSize_L;
        Image					mImgPortrait;
        Image					mImgLandscape;
        Image					mImgNarrow;
        Image					mImgNormal;
        Image					mImgWide;
        Image					mImgMirrored;
        Image					mImgMarginCustom;
        Image					mImgNarrow_L;
        Image					mImgNormal_L;
        Image					mImgWide_L;
        Image					mImgMirrored_L;
        Image					mImgMarginCustom_L;
        Image					mImgA3;
        Image					mImgA4;
        Image					mImgA5;
        Image					mImgB4;
        Image					mImgB5;
        Image					mImgC5;
        Image					mImgLetter;
        Image					mImgLegal;
        Image					mImgSizeNone;
        Image					mImgA3_L;
        Image					mImgA4_L;
        Image					mImgA5_L;
        Image					mImgB4_L;
        Image					mImgB5_L;
        Image					mImgC5_L;
        Image					mImgLetter_L;
        Image					mImgLegal_L;
        Image					mImgSizeNone_L;
        Image					mImgColumn1;
        Image					mImgColumn2;
        Image					mImgColumn3;
        Image					mImgLeft;
        Image					mImgRight;
        Image					mImgColumnNone;
        Image					mImgColumn1_L;
        Image					mImgColumn2_L;
        Image					mImgColumn3_L;
        Image					mImgLeft_L;
        Image					mImgRight_L;
        Image					mImgColumnNone_L;

        // item keeping the following page style attributes:
        // - page orientation
        // - page usage - only left, only right, both, mirrored
        // item also hold the numbering type for the page style which should
        // be kept stable.
        ::boost::scoped_ptr<SvxPageItem> mpPageItem;

        // item keeping the page style's left and right margins
        ::boost::scoped_ptr<SvxLongLRSpaceItem> mpPageLRMarginItem;
        // item keeping the page style's top and bottom margins
        ::boost::scoped_ptr<SvxLongULSpaceItem> mpPageULMarginItem;

        // item keeping the page style's page size
        ::boost::scoped_ptr<SvxSizeItem> mpPageSizeItem;
        // Paper corresponding to the page style's page size
        Paper mePaper;

        // item keeping the page column type
        ::boost::scoped_ptr<SfxInt16Item> mpPageColumnTypeItem;

        FieldUnit 			meFUnit;
        SfxMapUnit			meUnit;

        // controller items
        ::sfx2::sidebar::ControllerItem m_aSwPagePgULControl;
        ::sfx2::sidebar::ControllerItem m_aSwPagePgLRControl;
        ::sfx2::sidebar::ControllerItem m_aSwPagePgSizeControl;
        ::sfx2::sidebar::ControllerItem m_aSwPagePgControl;
        ::sfx2::sidebar::ControllerItem m_aSwPageColControl;
        ::sfx2::sidebar::ControllerItem m_aSwPagePgMetricControl;
        
        // popups
        ::boost::scoped_ptr< ::svx::sidebar::Popup > mpOrientationPopup;
        ::boost::scoped_ptr< ::svx::sidebar::Popup > mpMarginPopup;
        ::boost::scoped_ptr< ::svx::sidebar::Popup > mpSizePopup;
        ::boost::scoped_ptr< ::svx::sidebar::Popup > mpColumnPopup;

        bool mbInvalidateSIDAttrPageOnSIDAttrPageSizeNotify;

        // handler for popup toolboxes to show the popups
        DECL_LINK(ClickOrientationHdl, ToolBox* );
        DECL_LINK(ClickMarginHdl, ToolBox* );
        DECL_LINK(ClickSizeHdl, ToolBox* );
        DECL_LINK(ClickColumnHdl, ToolBox* );

        void Initialize();

        void MetricState( SfxItemState eState, const SfxPoolItem* pState );

        // helper to adjust popup toolbox' images
        void ChangeMarginImage();
        void ChangeSizeImage();
        void ChangeColumnImage( const sal_uInt16 nColumnType );

    };

} } // end of namespace ::sw::sidebar

#endif
