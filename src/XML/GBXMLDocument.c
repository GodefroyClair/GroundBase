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
//  GBXMLDocument.c
//  GroundBase
//
//  Created by Manuel Deneu on 14/07/2016.
//
#include <stdio.h>
#include <string.h> // memcpy
#include <GBXMLDocument.h>

#include "GBXML_Private.h"
#include "../GBObject_Private.h"

static void * GBXMLDocumentCtor(void * _self, va_list * app);
static void * GBXMLDocumentDtor (void * _self);
static void * GBXMLDocumentClone (const void * _self);
static uint8_t GBXMLDocumentEquals (const void * _self, const void * _b);
static GBRef GBXMLDocumentDescription (const void * self);


#define XML_VERSION_STR (const char*) "1.0"


static const GBObjectClass _XmlDocClass =
{
    sizeof(struct _GBXMLDocument),
    GBXMLDocumentCtor,
    GBXMLDocumentDtor,
    GBXMLDocumentClone,
    GBXMLDocumentEquals,
    GBXMLDocumentDescription,
    GBXmlInitClasses,
    NULL, // deinit
    NULL,/*RetainCallback,*/
    NULL,/*ReleaseCallback,*/
    (char*)GBXMLDocumentClassName
};


GBObjectClassRef GBXMLDocumentClass = & _XmlDocClass;


static void * GBXMLDocumentCtor(void * _self, va_list * app)
{
    UNUSED_PARAMETER( app );
    
    GBXMLDocument *self = _self;
    
    self->super.node = NULL;

    return self;
    
    return NULL;
}

static void * GBXMLDocumentDtor (void * _self)
{
    GBXMLDocument *self = _self;
        
    xmlFreeDoc( self->super.node );
    
    return self;
}

static void * GBXMLDocumentClone (const void * _self)
{
    UNUSED_PARAMETER( _self );
    
    return NULL;
}

static uint8_t GBXMLDocumentEquals (const void * _self, const void * _b)
{
    UNUSED_PARAMETER( _self );
    UNUSED_PARAMETER( _b );
    
    return 1;
}

static GBRef GBXMLDocumentDescription (const void * _self)
{
    const GBXMLDocument* self = _self;
    
    if( self)
    {
        if( self->super.node && ((xmlDocPtr) self->super.node)->URL)
        {
            return GBStringInitWithCStr( (const char*) ((xmlDocPtr) self->super.node)->URL);
        }
        return GBStringInitWithCStr("-NO NAME-");
    }
    
    return NULL;
}

GBXMLDocument* GBXMLDocumentInit()
{
    GBXMLDocument* doc = GBObjectAlloc( GBDefaultAllocator,GBXMLDocumentClass);
    
    if(doc)
    {
        
        xmlDocPtr root = xmlNewDoc((const xmlChar*)XML_VERSION_STR);
        doc->super.node = root;
        
        DEBUG_ASSERT( ( (xmlNodePtr) doc->super.node )->type ==XML_DOCUMENT_NODE);

        
        return doc;
 
    }
    
    return NULL;
}

GBXMLDocument* GBXMLDocumentInitWithFile( const  GBString* filepath )
{
    GBXMLDocument* document = GBObjectAlloc( GBDefaultAllocator, GBXMLDocumentClass);
    
    if(document == NULL)
        return NULL;
    
    if(  GBXMLDocumentParseFile(document, filepath) == 0 )
    {
        GBRelease(document);
        return  NULL;
    }
    
    return document;
}

GBXMLDocument* GBXMLDocumentInitWithBuffer( const char* buffer , GBSize bufferSize)
{
    GBXMLDocument* document = GBObjectAlloc( GBDefaultAllocator, GBXMLDocumentClass);
    
    if(document == NULL)
        return NULL;
    
    if(  GBXMLDocumentParseBuffer(document, buffer, bufferSize) == 0)
    {
        GBRelease(document);
        return  NULL;
    }
    
    return document;
    
    
}

BOOLEAN_RETURN uint8_t GBXMLDocumentParseFile( GBXMLDocument* document , const GBString* filepath)
{
    xmlParserCtxtPtr ctx = xmlNewParserCtxt();
    
    document->super.node = xmlCtxtReadFile( ctx, GBStringGetCStr( filepath ), NULL, XML_PARSE_DTDATTR/*XML_PARSE_DTDVALID*/);
    xmlFreeParserCtxt( ctx );
    if ( document->super.node == NULL )
    {
        return 0;
    }
    if ( ctx->valid == 0 )
    {
        return 0;
    }
    
    
    
    return 1;
}

BOOLEAN_RETURN uint8_t GBXMLDocumentParseBuffer( GBXMLDocument* document , const void* buffer , GBSize bufferSize)
{
    xmlParserCtxtPtr ctx = xmlNewParserCtxt();
    
    document->super.node = xmlCtxtReadMemory(ctx, (const char*)buffer, (int) bufferSize, NULL, NULL, 0);
    xmlFreeParserCtxt(ctx);
    
    if ( document->super.node == NULL )
    {
        return 0;
    }
    if ( ctx->valid == 0 )
    {
        return 0;
    }
    
    
    return 1;
}

GBXMLNode* GBXMLDocumentGetRootNode( const GBXMLDocument* document)
{
    if( document == NULL)
        return NULL;
    
    return GBXMLNodeCreateWithContent( xmlDocGetRootElement( document->super.node ) , document );
}


GBXMLNode* GBXMLDocumentCreateRootNode( GBXMLDocument* document , const char *name)
{
    if( document == NULL)
        return NULL;
    
    if( xmlDocGetRootElement( document->super.node ) != NULL)
        return NULL; // should return root node from super?

    xmlNodePtr root = xmlNewNode(NULL, (const xmlChar*) name);
    xmlDocSetRootElement(document->super.node, root);
    
    DEBUG_ASSERT( xmlDocGetRootElement(document->super.node) == root );
    
    return GBXMLNodeCreateWithContent( xmlDocGetRootElement( document->super.node ) , document );
}

/*
GBDictionary* GBXMLDocumentGetDictionary( const GBXMLDocument* document)
{
    return GBXMLNodeGetDictionary(GBXMLDocumentGetRootNode(document));
}
*/


BOOLEAN_RETURN uint8_t GBXMLDocumentSave( GBXMLDocument* doc )
{
    const GBString* file = GBStringInitWithCStr( (const char*)((xmlDocPtr) doc->super.node)->URL );
    
    const uint8_t ret = GBXMLDocumentSaveCopy(doc , file);
    
    GBRelease(file);
    
    return ret;
}

BOOLEAN_RETURN uint8_t GBXMLDocumentSaveCopy( const GBXMLDocument* document , const GBString* filename)
{
    if( document == NULL)
        return 0;
    
    xmlIndentTreeOutput = 1;
    xmlKeepBlanksDefault(0);

    return /*xmlSaveFileEnc*/xmlSaveFormatFile( GBStringGetCStr( filename ), document->super.node, 1) != -1;
    
}

GBSize GBXMLDocumentGetBufferSize(const GBXMLDocument* document)
{
    if( document == NULL)
        return GBSizeInvalid;
    
    xmlChar *buffer = NULL;
    int size = -1;
    xmlDocDumpMemory(document->super.node, &buffer, &size);
    xmlFree( buffer );
    
    if( size == -1)
        return GBSizeInvalid;
    
    return (GBSize) size;
    
}
BOOLEAN_RETURN uint8_t GBXMLDocumentWriteBuffer( const GBXMLDocument* document , char* bufferOut)
{
    if( document == NULL)
        return 0;
    
    xmlChar *s = NULL;
    int size = -1;
    xmlDocDumpMemory( document->super.node, &s, &size);
    
    
    uint8_t ret = 0;
    if(size > 0)
    {
        ret = memcpy(bufferOut, s, size) == bufferOut;
    }
    
    xmlFree(s);
    
    return ret;
}

GBSize GBXMLDocumentGetSize(const GBXMLDocument* document)
{
    return GBXMLNodeGetNumChildren( GBXMLDocumentGetRootNode(document));
}


BOOLEAN_RETURN uint8_t GBXMLDocumentClear( GBXMLDocument* document)
{
    UNUSED_PARAMETER(document);
    
    DEBUG_ASSERT(0); // IMPLEMENT ME :)
    return 0;
}

BOOLEAN_RETURN uint8_t GBXMLDocumentAddValue( GBXMLDocument* document, GBRef value ,  const GBString* key)
{
    return GBXMLNodeAddGBObjectForKey( GBXMLDocumentGetRootNode(document), value, key);
}
BOOLEAN_RETURN uint8_t GBXMLDocumentContains(const GBXMLDocument* document , const GBString* key)
{
    return GBXMLNodeGetChildrenByName(GBXMLDocumentGetRootNode(document), key) != NULL;
}
BOOLEAN_RETURN uint8_t GBXMLDocumentRemove( GBXMLDocument* document , const GBString* key)
{
    return GBXMLNodeRemoveChildren( GBXMLDocumentGetRootNode(document) , key);
}


void GBXmlInitClasses()
{
    static uint8_t run = 1;
    
    if( run )
    {
        LIBXML_TEST_VERSION
        run = 0;
    }
}
