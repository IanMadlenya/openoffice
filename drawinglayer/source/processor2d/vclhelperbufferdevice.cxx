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
#include "precompiled_drawinglayer.hxx"

#include <vclhelperbufferdevice.hxx>
#include <basegfx/range/b2drange.hxx>
#include <vcl/bitmapex.hxx>
#include <basegfx/matrix/b2dhommatrix.hxx>
#include <tools/stream.hxx>
#include <vcl/timer.hxx>
#include <comphelper/broadcasthelper.hxx>
#include <vcl/lazydelete.hxx>

//////////////////////////////////////////////////////////////////////////////
// buffered VDev usage

namespace
{
    typedef ::std::vector< VirtualDevice* > aBuffers;

    class VDevBuffer : public Timer, protected comphelper::OBaseMutex
    {
    private:
        aBuffers            maBuffers;

    public:
        VDevBuffer();
        virtual ~VDevBuffer();

        VirtualDevice* alloc(OutputDevice& rOutDev, const Size& rSizePixel, bool bClear, bool bMono);
        void free(VirtualDevice& rDevice);

        // Timer virtuals
        virtual void Timeout();
    };

    VDevBuffer::VDevBuffer()
    :   Timer(),
        maBuffers()
    {
        SetTimeout(10L * 1000L); // ten seconds
    }

    VDevBuffer::~VDevBuffer()
    {
        ::osl::MutexGuard aGuard(m_aMutex);
        Stop();

        while(!maBuffers.empty())
        {
            delete *(maBuffers.end() - 1);
            maBuffers.pop_back();
        }
    }

    VirtualDevice* VDevBuffer::alloc(OutputDevice& rOutDev, const Size& rSizePixel, bool bClear, bool bMono)
    {
        ::osl::MutexGuard aGuard(m_aMutex);
        VirtualDevice* pRetval = 0;

        if(!maBuffers.empty())
        {
            bool bOkay(false);
            aBuffers::iterator aFound(maBuffers.end());

            for(aBuffers::iterator a(maBuffers.begin()); a != maBuffers.end(); a++)
            {
                OSL_ENSURE(*a, "Empty pointer in VDevBuffer (!)");

                if((bMono && 1 == (*a)->GetBitCount()) || (!bMono && (*a)->GetBitCount() > 1))
                {
                    // candidate is valid due to bit depth
                    if(aFound != maBuffers.end())
                    {
                        // already found
                        if(bOkay)
                        {
                            // found is valid
                            const bool bCandidateOkay((*a)->GetOutputWidthPixel() >= rSizePixel.getWidth() && (*a)->GetOutputHeightPixel() >= rSizePixel.getHeight());

                            if(bCandidateOkay)
                            {
                                // found and candidate are valid
                                const sal_uLong aSquare((*aFound)->GetOutputWidthPixel() * (*aFound)->GetOutputHeightPixel());
                                const sal_uLong aCandidateSquare((*a)->GetOutputWidthPixel() * (*a)->GetOutputHeightPixel());

                                if(aCandidateSquare < aSquare)
                                {
                                    // candidate is valid and smaller, use it
                                    aFound = a;
                                }
                            }
                            else
                            {
                                // found is valid, candidate is not. Keep found
                            }
                        }
                        else
                        {
                            // found is invalid, use candidate
                            aFound = a;
                            bOkay = (*aFound)->GetOutputWidthPixel() >= rSizePixel.getWidth() && (*aFound)->GetOutputHeightPixel() >= rSizePixel.getHeight();
                        }
                    }
                    else
                    {
                        // none yet, use candidate
                        aFound = a;
                        bOkay = (*aFound)->GetOutputWidthPixel() >= rSizePixel.getWidth() && (*aFound)->GetOutputHeightPixel() >= rSizePixel.getHeight();
                    }
                }
            }

            if(aFound != maBuffers.end())
            {
                pRetval = *aFound;
                maBuffers.erase(aFound);

                if(bOkay)
                {
                    if(bClear)
                    {
                        pRetval->Erase(Rectangle(0, 0, rSizePixel.getWidth(), rSizePixel.getHeight()));
                    }
                }
                else
                {
                    pRetval->SetOutputSizePixel(rSizePixel, bClear);
                }
            }
        }

        // no success yet, create new buffer
        if(!pRetval)
        {
            pRetval = (bMono) ? new VirtualDevice(rOutDev, 1) : new VirtualDevice(rOutDev);
            pRetval->SetOutputSizePixel(rSizePixel, bClear);
        }
        else
        {
            // reused, reset some values
            pRetval->SetMapMode();
        }

        return pRetval;
    }

    void VDevBuffer::free(VirtualDevice& rDevice)
    {
        ::osl::MutexGuard aGuard(m_aMutex);
        maBuffers.push_back(&rDevice);
        Start();
    }

    void VDevBuffer::Timeout()
    {
        ::osl::MutexGuard aGuard(m_aMutex);

        while(!maBuffers.empty())
        {
            delete *(maBuffers.end() - 1);
            maBuffers.pop_back();
        }
    }
}

//////////////////////////////////////////////////////////////////////////////
// support for rendering Bitmap and BitmapEx contents

namespace drawinglayer
{
    // static global VDev buffer for the VclProcessor2D's (VclMetafileProcessor2D and VclPixelProcessor2D)
    VDevBuffer& getVDevBuffer()
    {
        // secure global instance with Vcl's safe desroyer of external (seen by
        // library base) stuff, the remembered VDevs need to be deleted before 
        // Vcl's deinit
        static vcl::DeleteOnDeinit< VDevBuffer > aVDevBuffer(new VDevBuffer());
        return *aVDevBuffer.get();
    }

    impBufferDevice::impBufferDevice(
        OutputDevice& rOutDev, 
        const basegfx::B2DRange& rRange,
        bool bAddOffsetToMapping)
    :   mrOutDev(rOutDev),
        mpContent(0),
        mpMask(0),
        mpAlpha(0)
    {
        basegfx::B2DRange aRangePixel(rRange);
        aRangePixel.transform(mrOutDev.GetViewTransformation());
        const Rectangle aRectPixel(
            (sal_Int32)floor(aRangePixel.getMinX()), (sal_Int32)floor(aRangePixel.getMinY()), 
            (sal_Int32)ceil(aRangePixel.getMaxX()), (sal_Int32)ceil(aRangePixel.getMaxY()));
        const Point aEmptyPoint;
        maDestPixel = Rectangle(aEmptyPoint, mrOutDev.GetOutputSizePixel());
        maDestPixel.Intersection(aRectPixel);

        if(isVisible())
        {
            mpContent = getVDevBuffer().alloc(mrOutDev, maDestPixel.GetSize(), false, false);

            // #i93485# assert when copying from window to VDev is used
            OSL_ENSURE(mrOutDev.GetOutDevType() != OUTDEV_WINDOW, 
                "impBufferDevice render helper: Copying from Window to VDev, this should be avoided (!)");

            const bool bWasEnabledSrc(mrOutDev.IsMapModeEnabled());
            mrOutDev.EnableMapMode(false);
            mpContent->DrawOutDev(aEmptyPoint, maDestPixel.GetSize(), maDestPixel.TopLeft(), maDestPixel.GetSize(), mrOutDev);
            mrOutDev.EnableMapMode(bWasEnabledSrc);

            MapMode aNewMapMode(mrOutDev.GetMapMode());

            if(bAddOffsetToMapping)
            {
                const Point aLogicTopLeft(mrOutDev.PixelToLogic(maDestPixel.TopLeft()));
                aNewMapMode.SetOrigin(Point(-aLogicTopLeft.X(), -aLogicTopLeft.Y()));
            }

            mpContent->SetMapMode(aNewMapMode);

            // copy AA flag for new target
            mpContent->SetAntialiasing(mrOutDev.GetAntialiasing());
        }
    }

    impBufferDevice::~impBufferDevice()
    {
        if(mpContent)
        {
            getVDevBuffer().free(*mpContent);
        }

        if(mpMask)
        {
            getVDevBuffer().free(*mpMask);
        }

        if(mpAlpha)
        {
            getVDevBuffer().free(*mpAlpha);
        }
    }

    void impBufferDevice::paint(double fTrans)
    {
        if(isVisible())
        {
            const Point aEmptyPoint;
            const Size aSizePixel(maDestPixel.GetSize());
            const bool bWasEnabledDst(mrOutDev.IsMapModeEnabled());
            static bool bDoSaveForVisualControl(false);
        
            mrOutDev.EnableMapMode(false);
            mpContent->EnableMapMode(false);
            Bitmap aContent(mpContent->GetBitmap(aEmptyPoint, aSizePixel)); 
        
            if(bDoSaveForVisualControl)
            {
                SvFileStream aNew((const String&)String(ByteString( "c:\\content.bmp" ), RTL_TEXTENCODING_UTF8), STREAM_WRITE|STREAM_TRUNC);
                aNew << aContent;
            }
        
            if(mpAlpha)
            {
                mpAlpha->EnableMapMode(false);
                const AlphaMask aAlphaMask(mpAlpha->GetBitmap(aEmptyPoint, aSizePixel));

                if(bDoSaveForVisualControl)
                {
                    SvFileStream aNew((const String&)String(ByteString( "c:\\transparence.bmp" ), RTL_TEXTENCODING_UTF8), STREAM_WRITE|STREAM_TRUNC);
                    aNew << aAlphaMask.GetBitmap();
                }

                mrOutDev.DrawBitmapEx(maDestPixel.TopLeft(), BitmapEx(aContent, aAlphaMask));
            }
            else if(mpMask)
            {
                mpMask->EnableMapMode(false);
                const Bitmap aMask(mpMask->GetBitmap(aEmptyPoint, aSizePixel)); 

                if(bDoSaveForVisualControl)
                {
                    SvFileStream aNew((const String&)String(ByteString( "c:\\mask.bmp" ), RTL_TEXTENCODING_UTF8), STREAM_WRITE|STREAM_TRUNC);
                    aNew << aMask;
                }

                mrOutDev.DrawBitmapEx(maDestPixel.TopLeft(), BitmapEx(aContent, aMask));
            }
            else if(0.0 != fTrans)
            {
                sal_uInt8 nMaskValue((sal_uInt8)basegfx::fround(fTrans * 255.0));
                const AlphaMask aAlphaMask(aSizePixel, &nMaskValue);
                mrOutDev.DrawBitmapEx(maDestPixel.TopLeft(), BitmapEx(aContent, aAlphaMask));
            }
            else
            {
                mrOutDev.DrawBitmap(maDestPixel.TopLeft(), aContent);
            }
        
            mrOutDev.EnableMapMode(bWasEnabledDst);
        }
    }

    VirtualDevice& impBufferDevice::getContent() 
    { 
        OSL_ENSURE(mpContent, "impBufferDevice: No content, check isVisible() before accessing (!)");
        return *mpContent; 
    }

    VirtualDevice& impBufferDevice::getMask()
    { 
        OSL_ENSURE(mpContent, "impBufferDevice: No content, check isVisible() before accessing (!)");
        if(!mpMask)
        {
            mpMask = getVDevBuffer().alloc(mrOutDev, maDestPixel.GetSize(), true, true);
            mpMask->SetMapMode(mpContent->GetMapMode());

            // do NOT copy AA flag for mask!
        }
        
        return *mpMask;
    }
    
    VirtualDevice& impBufferDevice::getTransparence() 
    { 
        OSL_ENSURE(mpContent, "impBufferDevice: No content, check isVisible() before accessing (!)");
        if(!mpAlpha)
        {
            mpAlpha = getVDevBuffer().alloc(mrOutDev, maDestPixel.GetSize(), true, false);
            mpAlpha->SetMapMode(mpContent->GetMapMode());

            // copy AA flag for new target; masking needs to be smooth
            mpAlpha->SetAntialiasing(mpContent->GetAntialiasing());
        }
        
        return *mpAlpha;
    }
} // end of namespace drawinglayer

//////////////////////////////////////////////////////////////////////////////
// eof
