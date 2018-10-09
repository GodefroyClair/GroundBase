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
//  GBXMLDocument.h
//  GroundBase
//
//  Created by Manuel Deneu on 14/07/2016.
//


/**
 * \file GBXMLDocument.h
 * \brief XML reading/Writing/parsing API.
 */

#ifndef GBXMLDocument_h
#define GBXMLDocument_h

#ifdef __cplusplus
extern "C" {
#endif

#include <GBXMLNode.h>

extern GBObjectClassRef GBXMLDocumentClass;
#define GBXMLDocumentClassName (const char*) "GBXMLDocument"

/*!
 * @discussion a XML Document that can be created from a file ( GBXMLDocumentInitWithFile), a string buffer ( GBXMLDocumentInitWithBuffer ) or empty ( GBXMLDocumentInit ).
 */
typedef struct _GBXMLDocument GBXMLDocument;

/*!
 * @discussion Initialize an empty GBXMLDocument instance. You own the returned object. See GBObject ownership notes.
 * @return an empty GBXMLDocument instance
 */
GBXMLDocument* GBXMLDocumentInit(void);
    
/*!
 * @discussion Initialize an  GBXMLDocument instance from the content of a file. You own the returned object. See GBObject ownership notes.
 * @param filePath An accessible filePath to a valid XML file. Will return NULL if NULL.
 * @return an GBXMLDocument instance containing the XML data from the file, NULL on any error.
 */
GBXMLDocument* GBXMLDocumentInitWithFile( const  GBString* filepath );

/*!
 * @discussion Initialize an  GBXMLDocument instance from the content of a buffer. You own the returned object. See GBObject ownership notes.
 * @param buffer a valid buffer containing XML text.
 * @param bufferSize the size of the buffer 
 * @return an GBXMLDocument instance containing the XML data from the buffer, NULL on any error.
 */
GBXMLDocument* GBXMLDocumentInitWithBuffer( const char* buffer , GBSize bufferSize);

BOOLEAN_RETURN uint8_t GBXMLDocumentParseFile( GBXMLDocument* document , const GBString* filepath);
BOOLEAN_RETURN uint8_t GBXMLDocumentParseBuffer( GBXMLDocument* document , const void* buffer , GBSize bufferSize);

BOOLEAN_RETURN uint8_t GBXMLDocumentAddValue( GBXMLDocument* document, GBRef value ,  const GBString* key);
BOOLEAN_RETURN uint8_t GBXMLDocumentContains(const GBXMLDocument* document , const GBString* key);
BOOLEAN_RETURN uint8_t GBXMLDocumentRemove( GBXMLDocument* document , const GBString* key);

GBSize GBXMLDocumentGetSize(const GBXMLDocument* document);


BOOLEAN_RETURN uint8_t GBXMLDocumentClear( GBXMLDocument* document);


BOOLEAN_RETURN uint8_t GBXMLDocumentSave( GBXMLDocument* document );
BOOLEAN_RETURN uint8_t GBXMLDocumentSaveCopy( const GBXMLDocument* document , const GBString* filename);

GBSize GBXMLDocumentGetBufferSize(const GBXMLDocument* document);
BOOLEAN_RETURN uint8_t GBXMLDocumentWriteBuffer( const GBXMLDocument* document , char* bufferOut);


GBXMLNode* GBXMLDocumentGetRootNode( const GBXMLDocument* document);

/* No need to release returned GBXMLNode */
GBXMLNode* GBXMLDocumentCreateRootNode( GBXMLDocument* document , const char *name);

#ifdef __cplusplus
}
#endif

#endif /* GBXMLDocument_h */
