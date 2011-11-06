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



#ifndef INCLUDED_CONFIGMGR_SOURCE_DATA_HXX
#define INCLUDED_CONFIGMGR_SOURCE_DATA_HXX

#include "sal/config.h"

#include <climits>
#include <map>
#include <vector>

#include "boost/noncopyable.hpp"
#include "rtl/ref.hxx"
#include "rtl/ustring.hxx"
#include "sal/types.h"
#include "salhelper/simplereferenceobject.hxx"

#include "additions.hxx"
#include "modifications.hxx"
#include "nodemap.hxx"
#include "path.hxx"

namespace configmgr {

class Node;

struct Data: private boost::noncopyable {
    enum { NO_LAYER = INT_MAX };

    struct ExtensionXcu: public salhelper::SimpleReferenceObject {
        int layer;
        Additions additions;
    };

    NodeMap templates;

    NodeMap components;

    Modifications modifications;

    static rtl::OUString createSegment(
        rtl::OUString const & templateName, rtl::OUString const & name);

    static sal_Int32 parseSegment(
        rtl::OUString const & path, sal_Int32 index, rtl::OUString * name,
        bool * setElement, rtl::OUString * templateName);

    static rtl::OUString fullTemplateName(
        rtl::OUString const & component, rtl::OUString const & name);

    //TODO: better rules under which circumstances a short template name matches
    static bool equalTemplateNames(
        rtl::OUString const & shortName, rtl::OUString const & longName);

    static rtl::Reference< Node > findNode(
        int layer, NodeMap const & map, rtl::OUString const & name);

    rtl::Reference< Node > resolvePathRepresentation(
        rtl::OUString const & pathRepresentation,
        rtl::OUString * canonicRepresenation, Path * path, int * finalizedLayer)
        const;

    rtl::Reference< Node > getTemplate(
        int layer, rtl::OUString const & fullName) const;

    Additions * addExtensionXcuAdditions(
        rtl::OUString const & url, int layer);

    rtl::Reference< ExtensionXcu > removeExtensionXcuAdditions(
        rtl::OUString const & url);

private:
    typedef std::map< rtl::OUString, rtl::Reference< ExtensionXcu > >
        ExtensionXcuAdditions;

    ExtensionXcuAdditions extensionXcuAdditions_;
};

}

#endif
