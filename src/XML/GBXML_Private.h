/*
 * Copyright (c) 2016 FlyLab
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
//
//  GBXML_Private.h
//  GroundBase
//
//  Created by Manuel Deneu on 15/07/2016.
/*
 
 This is a private header! 
 
 */

#ifndef GBXML_Private_h
#define GBXML_Private_h



//#include <libxml/parser.h>
#include <libxml/tree.h>

#include "../GBObject_Private.h"
#include <GBCommons.h>

struct _GBXMLNode
{
    GBObjectBase base;
    void* node; // either xmlNodePtr or xmlNodePtr
};

struct _GBXMLDocument
{
    struct _GBXMLNode super;
    
};

struct _GBXMLNode* GBXMLNodeCreateWithContent(xmlNodePtr node ,  const GBObject* parent); // private ctor

// shared with GBXMLNode GBXMLDocument GBPList
void GBXmlInitClasses( void );

#endif /* GBXML_Private_h */
