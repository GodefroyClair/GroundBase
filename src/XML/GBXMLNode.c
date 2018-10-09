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
//  GBXMLNode.c
//  GroundBase
//
//  Created by Manuel Deneu on 14/07/2016.
//

#include <stdio.h>
#include <string.h> // strcmp

#include <GBList.h>
#include <GBArray.h>
#include <GBXMLNode.h>
#include <GBNumber.h>
#include <GBDictionary.h>
#include <GBContainer.h>

#include "GBXML_Private.h"
#include "../GBObject_Private.h"

GBXMLNode* GBXMLNodeCreate(void);

static void * ctor(void * _self, va_list * app);
static void * dtor (void * _self);

static uint8_t  equals (const void * _self, const void * _b);
static GBRef description (const void * self);

static const GBObjectClass _XmlNodeClass =
{
    sizeof(struct _GBXMLNode),
    ctor,
    dtor,
    NULL, // clone
    equals,
    description,
    GBXmlInitClasses,
    NULL,
    NULL, // deinit
    NULL,
    (char*)GBXMLNodeClassName
};


GBObjectClassRef GBXMLNodeClass = & _XmlNodeClass;

static void * ctor(void * _self, va_list * app)
{
    UNUSED_PARAMETER(app);
    GBXMLNode* self = _self;
    
    self->node = NULL;
    //self->owned = 0;
    
    return self;
}

static void * dtor (void * _self)
{
    GBXMLNode* self = _self;
    
//    if( self->owned == 0)
        //xmlFreeNode( self->node );
    
    return self;
}

static uint8_t  equals (const void * _self, const void * _b)
{
    UNUSED_PARAMETER( _self );
    UNUSED_PARAMETER( _b );
    
    const GBXMLNode* a = _self;
    const GBXMLNode* b = _b;
    
    
    if( GBXMLNodeNameEquals(a, (const char*) ((xmlNodePtr)b->node)->name) == 0)
        return 0;
    
    if( GBXMLNodeGetType(a) !=  GBXMLNodeGetType(b))
        return 0;
    
    xmlChar* bufA = xmlNodeGetContent(a->node);
    xmlChar* bufB = xmlNodeGetContent(b->node);
    
    const uint8_t match = xmlStrcmp(bufA, bufB) == 0;
    
    xmlFree(bufB);
    xmlFree(bufA);
    
    
    return match;
}
static GBRef description (const void * _self)
{
    
    const GBXMLNode* self = _self;
    
    if( self)
    {
        if( ((xmlNodePtr)self->node)->name )
        {
            if(strlen((const char*)((xmlNodePtr) self->node)->name)==0 )
            {
                return GBStringInitWithCStr("Empty Name");
            }
            return GBStringInitWithCStr((const char*)((xmlNodePtr)self->node)->name);
        }
        return GBStringInitWithCStr("No Name");
    }
    return NULL;
}



GBXMLNode* GBXMLNodeCreateWithContent(xmlNodePtr node /*, uint8_t owned*/, const GBObject* parent)
{
    GBXMLNode* n = GBXMLNodeCreate();
    n->node = node;

    DEBUG_ASSERT(GBObjectIsValid(parent));
    
    GBObjectAddStrongReference(parent, n);
    GBRelease(n);
    return n;
}

GBXMLNode* GBXMLNodeCreate(void )
{
    GBXMLNode* p = GBObjectAlloc( GBDefaultAllocator, GBXMLNodeClass);
    
    return p;
}

GBXMLNodeType GBXMLNodeGetType( const GBXMLNode* node)
{
    return (GBXMLNodeType) ((xmlNodePtr)node->node)->type;
}
const GBString* GBXMLNodeGetName(const GBXMLNode* node)
{
    if (node == NULL)
        return NULL;
    
    const GBString* ret = GBStringInitWithCStr((const char*) ((xmlNodePtr)node->node)->name);
    
    GBObjectAddStrongReference(node, ret);
    
    GBRelease(ret); // release because ref count if incremened by GBObjectAddChild
    return ret;
    
}

uint8_t GBXMLNodeNameEquals( const GBXMLNode* node , const char* name)
{
    if (node == NULL)
        return 0;
    
    return strcmp( (const char*)((xmlNodePtr)node->node)->name , name) == 0;
}

uint8_t GBXMLNodeHasProperty( const GBXMLNode* node , const char* propertyName)
{
    return xmlHasProp(node->node, (const xmlChar *) propertyName) != NULL ;
}

char* GBXMLNodeGetProperty( const GBXMLNode* node , const char* propertyName)
{
    return (char*) xmlGetProp(node->node, (const xmlChar *) propertyName);
}


// need to call free on returned pointer
char* GBXMLNodeGetContent( const GBXMLNode* node)
{
    if( !node)
        return NULL;
    
    return (char*) xmlNodeGetContent(node->node);
}

BOOLEAN_RETURN uint8_t GBXMLNodeContentIsEqualTo( const GBXMLNode* node , const char* content)
{
    if( node == NULL)
        return 0;
    
    char *c = GBXMLNodeGetContent(node);
    
    uint8_t ret = strcmp(c, content ) == 0;
    
    xmlFree(c);
    
    return ret;
}

uint8_t GBXMLNodeHasChildren(const GBXMLNode* node)
{
    if( node == NULL || ((xmlNodePtr)node->node)->children == NULL)
        return 0;
    
    return 1;
}

GBSize GBXMLNodeGetNumChildren(const GBXMLNode* node)
{
    if( node == NULL)
        return 0;
    
    return (GBSize) xmlChildElementCount(node->node);
}

GBXMLNode* GBXMLNodeGetFirstChildren(const GBXMLNode* node)
{
    if( node == NULL || ((xmlNodePtr)node->node)->children == NULL)
        return NULL;

    GBXMLNode* c = GBXMLNodeCreateWithContent( ((xmlNodePtr)node->node)->children , node );
    
    GBObjectAddStrongReference(node, c);
    
    return c;
}

GBXMLNode* GBXMLNodeGetChildrenForIndex( const GBXMLNode* node, GBIndex index)
{
    xmlNode *curNode = NULL;
    GBIndex accum = 0;
    for (curNode = ((xmlNodePtr)node->node)->children; curNode; curNode = curNode->next)
    {
        if( curNode->type == XML_ELEMENT_NODE)
        {
            if( accum == index)
            {
                GBXMLNode* c= GBXMLNodeCreateWithContent(curNode ,node);
                
                return c;
            }
            accum++;
        }
    }
    
    return NULL;
}

BOOLEAN_RETURN uint8_t GBXMLNodeRemoveChildren(GBXMLNode* node ,  const GBString* key)
{
    if( node == NULL || ((xmlNodePtr)node->node)->children == NULL)
        return 0;
    
    GBXMLNode* child = GBXMLNodeGetChildrenByName(node, key) ;
    if( child == NULL)
        return 0;
    
    xmlUnlinkNode(child->node);

    GBRelease(child);
    
    return 1;
}

GBXMLNode* GBXMLNodeGetChildrenByName( const GBXMLNode* node, const GBString* name)
{
    if( node == NULL || ((xmlNodePtr)node->node)->children == NULL)
        return NULL;

    xmlNode *curNode = NULL;
    
    GBSize childrenNum =  ArrayGetSize(node->base.strongReferences);
    
    for (GBIndex i = 0 ; i<childrenNum    ; i++)
    {
        
        GBRef item  = ArrayGetValueAtIndex(node->base.strongReferences, i);
    
        if( IsKindOfClass(item, GBXMLNodeClass))
        {
            
            GBXMLNode* childNode = CONST_CAST(GBXMLNode*) item; // Const cast because of ArrayGetValueAtIndex returning a const ref.
            
            if( strcmp(GBStringGetCStr( GBXMLNodeGetName(childNode)) , GBStringGetCStr(name) )==0)
            {
                return childNode;
            }
        }
    }
    for (curNode = ((xmlNodePtr)node->node)->children; curNode; curNode = curNode->next)
    {
        if( xmlStrcmp((const xmlChar*) GBStringGetCStr(name), curNode->name) == 0)
        {
            
            GBXMLNode* c= GBXMLNodeCreateWithContent(curNode ,node);

            return c;
        }
    }

    return NULL;
}

GBXMLNode* GBXMLNodeGetNext(const GBXMLNode* node)
{
    if( node == NULL || ((xmlNodePtr)node->node)->next == NULL)
        return NULL;
    
    GBXMLNode* c = GBXMLNodeCreateWithContent( ((xmlNodePtr)node->node)->next  , node);
    
    return c;
}


GBXMLNode* GBXMLNodeAddNewChild( GBXMLNode* node , const char* name , const char* typename)
{
    if (node == NULL)
        return NULL;
    
    DEBUG_ASSERT(name);
    xmlNodePtr newChild = xmlNewChild(node->node , NULL /*namespace*/,(const xmlChar*)name /*name*/ , NULL/*content*/);
    
    if( newChild == NULL)
        return NULL;

    GBXMLNode *child = GBXMLNodeCreateWithContent(newChild,   node  );
    
    if( typename && GBXMLNodeAddProperty(child, TypeNameAttribute, typename ) == 0)
    {
        GBRelease(child);
        return NULL;
    }

    return child;
}


BOOLEAN_RETURN uint8_t GBXMLNodeAddProperty(GBXMLNode* node , const char* propName , const char* value)
{
    if( node == NULL)
        return 0;
    
    return xmlNewProp(node->node,(const xmlChar*) propName, (const xmlChar*) value) != NULL;
    
}
BOOLEAN_RETURN uint8_t GBXMLNodeSetContent(GBXMLNode* node , const char* content)
{
    if( node == NULL)
        return 0;
    
    xmlNodeSetContent(node->node, (const xmlChar*) content);
    
    return 1;
}

BOOLEAN_RETURN uint8_t GBXMLNodeSetName(GBXMLNode* node , const char* name)
{
    if( node == NULL)
        return 0;
    
        
    xmlNodeSetName(node->node, (const xmlChar *)name);
    return 1;
}

BOOLEAN_RETURN uint8_t GBXMLNodeAddGBObjectForKeyWithAttribute(GBXMLNode* node , GBRef object , const GBString* key)
{
    if( node == NULL)
        return 0;
    if( xmlNodeIsText( ((xmlNodePtr)node->node)->children))
        return 0;
    
    if( IsKindOfClass(object, GBNumberClass))
        return GBXMLNodeAddGBNumberForKey(node , object , key , 1);
    
    else if( IsKindOfClass(object, GBListClass))
    {
        return GBXMLNodeAddGBListForKey(node, object, key , 1);
    }
    
    else if( IsKindOfClass(object, GBDictionaryClass))
    {
        return GBXMLNodeAddGBDictionaryForKey(node, object, key , 1);
    }
    else if( isGBContainter(object) )
    {
        return GBXMLNodeAddGBCollectionForKey(node, object, key , 1);
    }
    else if( IsKindOfClass(object, GBStringClass))
    {
        return GBXMLNodeAddGBStringForKey(node , object , key , 1);
    }
    
    return 0;
}

BOOLEAN_RETURN uint8_t GBXMLNodeAddGBObjectForKey(GBXMLNode* node , GBRef object , const GBString* key)
{
    if( node == NULL)
        return 0;

    if( xmlNodeIsText( ((xmlNodePtr)node->node)->children))
        return 0;
    
    if( IsKindOfClass(object, GBNumberClass))
        return GBXMLNodeAddGBNumberForKey(node , object , key , 0);
    
    else if( IsKindOfClass(object, GBListClass))
    {
        return GBXMLNodeAddGBListForKey(node, object, key , 0);
    }
    
    else if( IsKindOfClass(object, GBDictionaryClass))
    {
        return GBXMLNodeAddGBDictionaryForKey(node, object, key , 0);
    }
    else if( isGBContainter(object) )
    {
        return GBXMLNodeAddGBCollectionForKey(node, object, key , 0);
    }
    else if( IsKindOfClass(object, GBStringClass))
    {
        return GBXMLNodeAddGBStringForKey(node , object , key , 0);
    }
    return 0;
    
}

BOOLEAN_RETURN uint8_t GBXMLNodeIsText( const GBXMLNode* node)
{
    return xmlNodeIsText( node->node ) == 1;
}
BOOLEAN_RETURN uint8_t GBXMLNodeAddGBNumberForKey(GBXMLNode* node , GBRef number , const GBString* key , uint8_t writeTypeAttr)
{
    
    if( node == NULL)
        return 0;
    
    if( xmlNodeIsText(((xmlNodePtr)node->node)->children))
        return 0;
    
    GBXMLNode* newNode = GBXMLNodeAddNewChild(node, GBStringGetCStr(key) ,writeTypeAttr? GBObjectGetClassNameC(number) : NULL);
    
    
    
    if( newNode)
    {
        
        char * tmp = GBNumberGetString(number);
        
        if( tmp)
        {
            const uint8_t r = GBXMLNodeSetContent(newNode , tmp);
            
            free(tmp);
            return r;
        }
        
    }
    return 0;
    /*
    if( node == NULL)
        return 0;
    
    if( xmlNodeIsText(((xmlNodePtr)node->node)->children))
        return 0;

    GBXMLNode* newNode = GBXMLNodeAddNewChild(node, GBStringGetCStr(key));
    
    if( newNode)
    {
        char * tmp = GBNumberGetString(number); // NULL
        
        switch (GBNumberGetType(number))
        {
            case GBNumberTypeInt:
                GBXMLNodeAddProperty(newNode , TYPE_ATTRIBUTE_NAME , GBNumberTypeName_Int);
                break;
                
            case GBNumberTypeFloat:
                GBXMLNodeAddProperty(newNode , TYPE_ATTRIBUTE_NAME , GBNumberTypeName_Float);
                break;
                
            case GBNumberTypeDouble:
                GBXMLNodeAddProperty(newNode , TYPE_ATTRIBUTE_NAME , GBNumberTypeName_Double);
                break;
                
            case GBNumberTypeLong:
                GBXMLNodeAddProperty(newNode , TYPE_ATTRIBUTE_NAME , GBNumberTypeName_Long);
                break;
                
            default:
                break;
        }

        if( tmp)
        {
            const uint8_t r = GBXMLNodeSetContent(newNode , tmp);
            
            free(tmp);
            return r;
        }

    }
    
    return 0;
    */
}
static int dictionaryIterateAddToNode( GBRef dict, const GBString* key , GBRef value , void* context);


/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */


typedef struct
{
    GBXMLNode* newNode;
    uint8_t writeTypeAttr;
    
}DictAddNodeContext;

static int dictionaryIterateAddToNode( GBRef dict, const GBString* key , GBRef value , void* context)
{
    UNUSED_PARAMETER(dict);
    DEBUG_ASSERT(context && key);
    
    DictAddNodeContext *ctx = context;

    if( ctx->writeTypeAttr)
    {
        GBXMLNodeAddGBObjectForKeyWithAttribute(ctx->newNode, value, key);
    }
    else
    {
        GBXMLNodeAddGBObjectForKey( ctx->newNode, value, key);
    }
    
    return 1;
}

BOOLEAN_RETURN uint8_t GBXMLNodeAddGBDictionaryForKey(GBXMLNode* node , GBRef dict , const GBString* key, uint8_t writeTypeAttr)
{
    if( node == NULL)
        return 0;
    
    //GBXMLNode *newNode = GBXMLNodeAddNewChild(node , GBStringGetCStr( key ) );
    
    GBXMLNode* newNode = GBXMLNodeAddNewChild(node, GBStringGetCStr(key) ,writeTypeAttr? GBObjectGetClassNameC(dict) : NULL);
    
    DEBUG_ASSERT(newNode);
    
    if( newNode)
    {
        DictAddNodeContext ctx;
        ctx.newNode = newNode;
        ctx.writeTypeAttr = writeTypeAttr;
        
        GBSequenceIterateValues(dict, dictionaryIterateAddToNode, &ctx);
//        GBDictionaryIterateValues(dict, dictionaryIterateAddToNode, newNode);
        return 1;
    }
    return 0;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

BOOLEAN_RETURN uint8_t GBXMLNodeAddGBArrayForKey(GBXMLNode* node , GBRef array , const GBString* key , uint8_t writeTypeAttr)
{
    return GBXMLNodeAddGBCollectionForKey(node , array , key , writeTypeAttr);
}
BOOLEAN_RETURN uint8_t GBXMLNodeAddGBCollectionForKey(GBXMLNode* node , GBRef collection , const GBString* key , uint8_t writeTypeAttr)
{
    
    UNUSED_PARAMETER(node);
    UNUSED_PARAMETER(collection);
    UNUSED_PARAMETER(key);
    UNUSED_PARAMETER(writeTypeAttr);
    
    DEBUG_ASSERT(0);
    return 0;
    /*
    if( node == NULL)
        return 0;
    
    if( xmlNodeIsText( ((xmlNodePtr)node->node)->children))
        return 0;
    
    GBXMLNode *newNode = GBXMLNodeAddNewChild(node , GBStringGetCStr( key ) );
    DEBUG_ASSERT(newNode);
    
    const GBString* className = GBObjectGetClassName(collection);
    GBXMLNodeAddProperty(newNode, TYPE_ATTRIBUTE_NAME, GBStringGetCStr( className));
    GBRelease(className);
    
    //const GBString* keyVal = GBStringInitWithCStr( VALUE_NAME_TOKEN );
    
    GBSize accum = 0;
    assert(0);
    
    for (GBIndex i = 0 ; i< GBCollectionGetSize(collection ); i++)
    {
        GBRef val = GBCollectionGetValueAtIndex(collection, i);
        
        const uint8_t ret = GBXMLNodeAddGBObjectForKey( newNode, val, keyVal);

        accum += ret;
    }
    */
    //GBRelease(keyVal);
    
    //return GBContainerGetSize(collection) == accum;
}

BOOLEAN_RETURN uint8_t GBXMLNodeAddGBListForKey(GBXMLNode* node , GBRef set , const GBString* key , uint8_t writeTypeAttr)
{
    return GBXMLNodeAddGBCollectionForKey(node , set , key,  writeTypeAttr);
}

BOOLEAN_RETURN uint8_t GBXMLNodeAddGBStringForKey(GBXMLNode* node , GBRef string , const GBString* key, uint8_t writeTypeAttr)
{
    if( node == NULL)
        return 0;
    
    if( xmlNodeIsText(((xmlNodePtr)node->node)->children))
        return 0;
    
    //GBXMLNode* newNode = GBXMLNodeAddNewChild(node, GBStringGetCStr(key));
    
    GBXMLNode* newNode =  GBXMLNodeAddNewChild(node, GBStringGetCStr(key) ,writeTypeAttr? GBObjectGetClassNameC(string) : NULL);
    
    if( newNode)
    {
        //GBXMLNodeAddProperty(newNode , TYPE_ATTRIBUTE_NAME , GBStringClassName);
        return GBXMLNodeSetContent(newNode , GBStringGetCStr(string));
    }
    
    return 0;
    
}



