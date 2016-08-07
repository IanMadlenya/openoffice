#**************************************************************
#  
#  Licensed to the Apache Software Foundation (ASF) under one
#  or more contributor license agreements.  See the NOTICE file
#  distributed with this work for additional information
#  regarding copyright ownership.  The ASF licenses this file
#  to you under the Apache License, Version 2.0 (the
#  "License"); you may not use this file except in compliance
#  with the License.  You may obtain a copy of the License at
#  
#    http://www.apache.org/licenses/LICENSE-2.0
#  
#  Unless required by applicable law or agreed to in writing,
#  software distributed under the License is distributed on an
#  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
#  KIND, either express or implied.  See the License for the
#  specific language governing permissions and limitations
#  under the License.
#  
#**************************************************************



$(eval $(call gb_JunitTest_JunitTest,sfx2_unoapi,SRCDIR))

$(eval $(call gb_JunitTest_set_defs,sfx2_unoapi,\
	$$(DEFS) \
	-Dorg.openoffice.test.arg.sce=$(SRCDIR)/sfx2/qa/unoapi/sfx.sce \
	-Dorg.openoffice.test.arg.xcl=$(SRCDIR)/sfx2/qa/unoapi/knownissues.xcl \
	-Dorg.openoffice.test.arg.tdoc=$(SRCDIR)/sfx2/qa/unoapi/testdocuments \
))

$(eval $(call gb_JunitTest_add_jars,sfx2_unoapi,\
	$(OUTDIR)/bin/OOoRunner.jar \
	$(OUTDIR)/bin/ridl.jar \
	$(OUTDIR)/bin/test.jar \
	$(OUTDIR)/bin/unoil.jar \
	$(OUTDIR)/bin/jurt.jar \
))

$(eval $(call gb_JunitTest_add_sourcefiles,sfx2_unoapi,\
	sfx2/qa/unoapi/Test \
))

$(eval $(call gb_JunitTest_add_classes,sfx2_unoapi,\
	org.openoffice.sfx2.qa.unoapi.Test \
))

# vim: set noet sw=4 ts=4:
