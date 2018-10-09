//
//  GBPropertyList.c
//  GroundBase
//
//  Created by Manuel Deneu on 01/10/2017.
//  Copyright © 2017 Manuel Deneu. All rights reserved.
//

#include "GBObject_Private.h"
#include <GBPropertyList.h>
#include <stdlib.h>

#include <GBXMLDocument.h>
#include <GBString.h>
#include <GBNumber.h>
#include <string.h>

#define ROOT_NODE_NAME (const char*) "plist"
#define PROP_TYPE_NAME (const char*) "Type"

#define STRING_TYPE_NAME (const char*) "String"
#define INT_TYPE_NAME (const char*) "Int"


static GBObject* getObjectFromXMLNode( const GBXMLNode* node);

static void * ctor(void * _self, va_list * app);
static void * dtor(void * _self);

struct  _GBPropertyList
{
    GBObjectBase base;
    GBDictionary* _dict;
    
};

static const GBObjectClass _GBPropertyListClass =
{
    sizeof(struct _GBPropertyList ),
    ctor,
    dtor,
    NULL, // clone
    NULL,//equals,
    NULL,//description,
    NULL, // initialize
    NULL, // deinit
    NULL, // retain
    NULL, // release
    (char*)GBPropertyListClassName
};

GBObjectClassRef GBPropertyListClass = & _GBPropertyListClass;

static void * ctor(void * _self, va_list * app)
{
    UNUSED_PARAMETER(app);
    GBPropertyList* self = _self;
    
    if( self)
    {
        self->_dict = NULL;
        return self;
    }
    return NULL;
}

static void * dtor(void * _self)
{
    GBPropertyList* self = _self;
    
    if( self)
    {
        if( self->_dict)
        {
            GBRelease(self->_dict);
        }
        return self;
    }
    return NULL;
}

GBPropertyList* GBPropertyListInit()
{
    return GBObjectAlloc(GBDefaultAllocator, GBPropertyListClass);
}



GBPropertyList* GBPropertyListInitWithFile( const GBString* filePath)
{
    if (filePath)
    {
        const GBXMLDocument* doc  = GBXMLDocumentInitWithFile(filePath);
        if( !doc)
        {
            return NULL;
        }

        const GBXMLNode* root = GBXMLDocumentGetRootNode(doc);
        if( !root || !GBStringEqualsCStr( GBXMLNodeGetName(root) , ROOT_NODE_NAME ) )
        {
            return NULL;
        }
        
        GBPropertyList* self = GBPropertyListInit();
        
        self->_dict = GBDictionaryInit();
        
        const GBXMLNode* next = GBXMLNodeGetFirstChildren(root);
        
        while (next)
        {
            GBObject* obj = getObjectFromXMLNode(next);
            
            if(obj)
            {
                GBDictionaryAddValueForKey(self->_dict, obj, GBXMLNodeGetName(next));
                GBRelease(obj);
            }
            
            next = GBXMLNodeGetNext(next);
        }
        
        
        GBRelease(doc);
        return self;
    }
    
    return NULL;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

static GBObject* getObjectFromXMLNode( const GBXMLNode* node)
{
    DEBUG_ASSERT(node);
    

    if( GBXMLNodeHasProperty(node, PROP_TYPE_NAME))
    {
        char* prop = GBXMLNodeGetProperty(node, PROP_TYPE_NAME);
        DEBUG_ASSERT(prop);
        
        GBObject* ret = NULL;
        if( strcmp(prop, GBStringClassName) == 0 || strcmp(prop, STRING_TYPE_NAME) == 0)
        {
            char* c = GBXMLNodeGetContent(node);
            ret = GBStringInitWithCStr(c);
            free(c);
            
        }
        else if( strcmp(prop, INT_TYPE_NAME) == 0 )
        {
            char* c = GBXMLNodeGetContent(node);
            const int v  = (int)strtol(c, NULL, 10);
            ret = GBNumberInitWithInt(v);
            free(c);
        }
        else if (strcmp(prop, GBDictionaryClassName) == 0)
        {

            
            ret = GBDictionaryInit();
            
            const GBXMLNode* next = GBXMLNodeGetFirstChildren(node);
            
            while (next)
            {
                GBObject* obj = getObjectFromXMLNode(next);
                
                if(obj )
                {
                    if( !GBStringEqualsCStr(GBXMLNodeGetName(next), "text") )
                    {
                        GBDictionaryAddValueForKey( ret, obj, GBXMLNodeGetName(next));
                    }
                    GBRelease(obj);
                }
                next = GBXMLNodeGetNext(next);
            }
            
        }
        else if (strcmp(prop, GBStringClassName) == 0)
        {
            char* c = GBXMLNodeGetContent(node);
            ret = GBStringInitWithCStr(c);
            free(c);
        }
        else if (strcmp(prop, GBNumberClassName) == 0)
        {
            char* c = GBXMLNodeGetContent(node);
            const double v  = strtod(c, NULL);
            ret = GBNumberInitWithDouble( v );
            free(c);
        }
        
        
        free(prop);
        return ret;
        
    }
    
    else if ( GBXMLNodeGetFirstChildren(node) && GBXMLNodeIsText( GBXMLNodeGetFirstChildren(node)))
    {
        char* c = GBXMLNodeGetContent(GBXMLNodeGetFirstChildren(node));
        GBObject* ret = GBStringInitWithCStr(c);
        free(c);
        
        return ret;
    }
    
    else
    {
        const GBXMLNode* next = GBXMLNodeGetFirstChildren(node);
        
        GBDictionary* dict = GBDictionaryInit();
        
        while (next)
        {
            GBObject* obj = getObjectFromXMLNode(next);
            
            if(obj )
            {
                if( !GBStringEqualsCStr(GBXMLNodeGetName(next), "text") )
                {
                    GBDictionaryAddValueForKey( dict, obj, GBXMLNodeGetName(next));
                }
                GBRelease(obj);
            }
            next = GBXMLNodeGetNext(next);
        }
        
        return dict;
    }
    
    return NULL;
}

const GBDictionary* GBPropertyListGetRoot(const GBPropertyList* plist)
{
    if( plist && plist->_dict)
    {
        return plist->_dict;
    }
    return NULL;
}

BOOLEAN_RETURN uint8_t GBPropertyListWriteRootObject( GBPropertyList* plist,const GBDictionary* sequence)
{
    if( plist && sequence)
    {
        if( plist->_dict)
        {
            GBRelease(plist->_dict);
        }
        
        plist->_dict =  GBObjectClone( sequence);
        
        return plist->_dict != NULL;
        
    }
    return 0;
}

BOOLEAN_RETURN uint8_t GBPropertyListSaveFile( GBPropertyList* plist ,const GBString* file)
{
    if( plist && file && plist->_dict)
    {
        GBXMLDocument* doc = GBXMLDocumentInit();
        
        GBXMLNode* root =  GBXMLDocumentCreateRootNode(doc, ROOT_NODE_NAME);
        DEBUG_ASSERT(root);
        
        const GBArray* keys  = GBDictionaryGetKeyList(plist->_dict);
        
        const GBString* k = NULL;
        GBArrayForEach(keys, k)
        {
            GBXMLNodeAddGBObjectForKeyWithAttribute(root, GBDictionaryGetValueForKey(plist->_dict, k), k);
            
        }
        GBRelease(keys);
        const uint8_t ret = GBXMLDocumentSaveCopy(doc, file);
        GBRelease(doc);
        return ret;
    }
    return 0;
}
