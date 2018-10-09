/*
 * Copyright (c) 2017 FlyLab
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
//  GBPropertyList.h
//  GroundBase
//
//  Created by Manuel Deneu on 01/10/2017.
//  Copyright © 2017 Manuel Deneu. All rights reserved.
//


/**
 * \file GBPropertyList.h
 * \brief See GBPropertyList.hpp for C++ API
 * A property list file is basically a XML document, with type checks (ie a 'type' attribute)
 * You can't add any random values to it, only Basic types ( string, numbers, containers)
 */

#ifndef GBPropertyList_h
#define GBPropertyList_h

#ifdef __cplusplus
extern "C" {
#endif
    


#include <GBString.h>
#include <GBDictionary.h>

#include <GBObject.h>

/*
 for XML format :
 - root node _must_ be named `Root`
 */

typedef enum
{
    GBPropertyListTypeXML       = 0,
    GBPropertyListTypeXMLTyped  = 1, // xml node must have a 'Type' property
    GBPropertyListTypeJSON      = 2,
} GBPropertyListType;

extern GBObjectClassRef GBPropertyListClass;

#define GBPropertyListClassName (const char*) "GBPropertyList"

/*!
 * @discussion An opaque GBPropertyList instance.
 */
typedef struct  _GBPropertyList GBPropertyList;

/*!
 * @discussion Initialize an empty GBPropertyList instance. You own the returned object. See GBObject ownership notes.
 * @return an empty GBPropertyList instance. Use `GBPropertyListWriteRootObject` to add content.
 */
GBPropertyList* GBPropertyListInit(void);

/*!
 * @discussion Initialize an  GBPropertyList instance from the content of a file. You own the returned object. See GBObject ownership notes.
 * @param filePath An accessible filePath to a valid PropertyList file. Will return NULL if NULL.
 * @return an initialized GBPropertyList instance, or NULL if the file/filePath is invalid. Use `GBPropertyListGetRoot` to access content.
 */
GBPropertyList* GBPropertyListInitWithFile( const GBString* filePath);
    
/*!
 * @discussion Add a root section to the propertyList. This section will be called `plist`.
 * @param plist A valid GBPropertyList instance. will return 0 if NULL. Will release previous root content, if any.
 * @param sequence a valid GBDictionary instance, that will be cloned. will return 0 if NULL
 * @return 1 on sucess, 0 on error.
 */
BOOLEAN_RETURN uint8_t GBPropertyListWriteRootObject( GBPropertyList* plist ,const GBDictionary* sequence);

/*!
 * @discussion Saves the property list to a file, given its filepath.
 * @param plist A valid GBPropertyList instance. will return 0 if NULL, or if no content was set.
 * @param filePath An accessible filePath to a writable file. Will return 0 if NULL
 * @return 1 on sucess, 0 on error.
 */
BOOLEAN_RETURN uint8_t GBPropertyListSaveFile( GBPropertyList* plist ,const GBString* filePath);

/*!
 * @discussion returns the content of a propertyList.
 * @param plist A valid GBPropertyList instance. will return NULL if NULL, or if no content was set.
 * @return the root content as a GBDictionary (might be NULL). You must retain it to keep-it around.
 */
const GBDictionary* GBPropertyListGetRoot(const GBPropertyList* plist);
    
#ifdef __cplusplus
}
#endif

#endif /* GBPropertyList_h */
