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

/*
 * Created on 14.11.2003
 *
 * To change the template for this generated file go to
 * Window>Preferences>Java>Code Generation>Code and Comments
 */
package com.sun.star.wizards.ui;

/**
 * @author rpiterman
 *
 * To change the template for this generated type comment go to
 * Window>Preferences>Java>Code Generation>Code and Comments
 */
public interface UIConsts
{

    public static final int RID_COMMON = 500;
    public static final int RID_DB_COMMON = 1000;
    public static final int RID_FORM = 2200;
    public static final int RID_QUERY = 2300;
    public static final int RID_REPORT = 2400;
    public static final int RID_TABLE = 2500;
    public static final int RID_IMG_REPORT = 1000;
    public static final int RID_IMG_FORM = 1100;
    public static final int RID_IMG_WEB = 1200;
    public static final Integer INVISIBLESTEP = 99;
    public static final String INFOIMAGEURL = "private:resource/dbu/image/19205";
    public static final String INFOIMAGEURL_HC = "private:resource/dbu/image/19230";
    /**
     * The tabindex of the navigation buttons in a wizard must be assigned a very
     * high tabindex because on every step their taborder must appear at the end
     */
    public static final short SOFIRSTWIZARDNAVITABINDEX = 30000;
    public static final Integer INTEGER_8 = 8;
    public static final Integer INTEGER_12 = 12;
    public static final Integer INTEGER_14 = 14;
    public static final Integer INTEGER_16 = 16;
    public static final Integer INTEGER_40 = 40;
    public static final Integer INTEGER_50 = 50;
    /**Steps of the QueryWizard
     *
     */
    public static final int SOFIELDSELECTIONPAGE = 1;
    public static final int SOSORTINGPAGE = 2;
    public static final int SOFILTERPAGE = 3;
    public static final int SOAGGREGATEPAGE = 4;
    public static final int SOGROUPSELECTIONPAGE = 5;
    public static final int SOGROUPFILTERPAGE = 6;
    public static final int SOTITLESPAGE = 7;
    public static final int SOSUMMARYPAGE = 8;
    public static final Integer[] INTEGERS = new Integer[]
    {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10
    };

    class CONTROLTYPE
    {

        public static final int BUTTON = 1;
        public static final int IMAGECONTROL = 2;
        public static final int LISTBOX = 3;
        public static final int COMBOBOX = 4;
        public static final int CHECKBOX = 5;
        public static final int RADIOBUTTON = 6;
        public static final int DATEFIELD = 7;
        public static final int EDITCONTROL = 8;
        public static final int FILECONTROL = 9;
        public static final int FIXEDLINE = 10;
        public static final int FIXEDTEXT = 11;
        public static final int FORMATTEDFIELD = 12;
        public static final int GROUPBOX = 13;
        public static final int HYPERTEXT = 14;
        public static final int NUMERICFIELD = 15;
        public static final int PATTERNFIELD = 16;
        public static final int PROGRESSBAR = 17;
        public static final int ROADMAP = 18;
        public static final int SCROLLBAR = 19;
        public static final int TIMEFIELD = 20;
        public static final int CURRENCYFIELD = 21;
        public static final int UNKNOWN = -1;
    }
}
