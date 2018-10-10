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
//  GBXMLNode.h
//  GroundBase
//
//  Created by Manuel Deneu on 14/07/2016.
//

/**
 * \file GBXMLNode.h
 * \brief A XML node from GBXMLNode.
 */

#ifndef GBXMLNode_h
#define GBXMLNode_h



#include <stdint.h>

#include <GBString.h>
#include <GBObject.h>
#include <GBDictionary.h>
#include <GBCommons.h>

GB_BEGIN_DCL

extern GBObjectClassRef GBXMLNodeClass;
#define GBXMLNodeClassName (const char*) "GBXMLNode"

/*!
 * @discussion An opaque type representing a XML node. GBXMLNode can be created/accessed from a GBXMLDocument.
 */
typedef struct _GBXMLNode GBXMLNode;


/* Conforms to xmlElementType */
typedef enum
{
    GBXMLNodeTypeElement   = 1,
    GBXMLNodeTypeAttribute = 2,
    GBXMLNodeTypeText      = 3,
    GBXMLNodeTypeDoc       = 9
    
} GBXMLNodeType;
    
    
#define TypeNameAttribute (const char*) "Type"



/* Getters */

const GBString* GBXMLNodeGetName(const GBXMLNode* node);

uint8_t GBXMLNodeNameEquals( const GBXMLNode* node , const char* name);
GBXMLNodeType GBXMLNodeGetType( const GBXMLNode* node);

// Only relevant on Elements nodes?
uint8_t GBXMLNodeHasProperty( const GBXMLNode* node , const char* propertyName);

// needs free()
char* GBXMLNodeGetProperty( const GBXMLNode* node , const char* propertyName);

BOOLEAN_RETURN uint8_t GBXMLNodeContentIsEqualTo( const GBXMLNode* node , const char* content);

// needs free()
char* GBXMLNodeGetContent( const GBXMLNode* node);

// GBObject getters
/*
GBRef GBXMLNodeGetGBObject( const GBXMLNode* node);
GBRef GBXMLNodeGetString( const GBXMLNode* node);
GBRef GBXMLNodeGetGBNumber( const GBXMLNode* node  );
GBRef GBXMLNodeGetSet(const GBXMLNode* node);
GBRef GBXMLNodeGetArray(const GBXMLNode* node);
GBDictionary* GBXMLNodeGetDictionary( const GBXMLNode* node);
*/
/* Children, tree */
BOOLEAN_RETURN uint8_t GBXMLNodeHasChildren(const GBXMLNode* node);

GBSize GBXMLNodeGetNumChildren(const GBXMLNode* node);

GBXMLNode* GBXMLNodeGetFirstChildren(const GBXMLNode* node);
GBXMLNode* GBXMLNodeGetChildrenByName( const GBXMLNode* node, const GBString* name);

BOOLEAN_RETURN uint8_t GBXMLNodeRemoveChildren(GBXMLNode* node ,  const GBString* key);

GBXMLNode* GBXMLNodeGetChildrenForIndex( const GBXMLNode* node, GBIndex index);

GBXMLNode* GBXMLNodeGetNext(const GBXMLNode* node);

BOOLEAN_RETURN uint8_t GBXMLNodeIsText( const GBXMLNode* node);

/* EDITION */

GBXMLNode* GBXMLNodeAddNewChild( GBXMLNode* node , const char* name , const char* typename);
//GBXMLNode* GBXMLNodeAddNewChildWithType( GBXMLNode* node , const char* name , const char* typename);

/*!
 * @discussion Sets a node's content (plain text)
 * @param node a valid XML node.
 * @return true if content is fully added, false  otherwise.
 */
BOOLEAN_RETURN uint8_t GBXMLNodeSetContent(GBXMLNode* node , const char* content);


BOOLEAN_RETURN uint8_t GBXMLNodeAddProperty(GBXMLNode* node , const char* propName , const char* value);
BOOLEAN_RETURN uint8_t GBXMLNodeSetName(GBXMLNode* node , const char* name);

/*!
 * @discussion Adds a child node to an other node
 * @param node a valid XML node.
 * @param object The value to add.
 * @return true if content is fully added, false  otherwise. Note : Will return false if some content was already added (via GBXMLNodeSetContent) to the node.
 */
BOOLEAN_RETURN uint8_t GBXMLNodeAddGBObjectForKey(GBXMLNode* node , GBRef object , const GBString* key);
    
BOOLEAN_RETURN uint8_t GBXMLNodeAddGBObjectForKeyWithAttribute(GBXMLNode* node , GBRef object , const GBString* key);

/*!
 * @discussion Adds a child node representing a GBNumber to an other node
 * @param node a valid XML node.
 * @param number The number value to add.
 * @return true if content is fully added, false  otherwise. Note : Will return false if some content was already added (via GBXMLNodeSetContent) to the node.
 */
BOOLEAN_RETURN uint8_t GBXMLNodeAddGBNumberForKey(GBXMLNode* node , GBRef number , const GBString* key , uint8_t writeTypeAttr);

/*!
 * @discussion Adds a child node representing a GBDictionary to an other node
 * @param node a valid XML node.
 * @param dict The dictionary value to add.
 * @return true if content is fully added, false  otherwise. Note : Will return false if some content was already added (via GBXMLNodeSetContent) to the node.
 */
BOOLEAN_RETURN uint8_t GBXMLNodeAddGBDictionaryForKey(GBXMLNode* node , GBRef dict , const GBString* key , uint8_t writeTypeAttr);

/*!
 * @discussion Adds a child node representing a GBList to an other node
 * @param node a valid XML node.
 * @param set The set value to add.
 * @return true if content is fully added, false  otherwise. Note : Will return false if some content was already added (via GBXMLNodeSetContent) to the node.
 */
BOOLEAN_RETURN uint8_t GBXMLNodeAddGBListForKey(GBXMLNode* node , GBRef set , const GBString* key , uint8_t writeTypeAttr);

/*!
 * @discussion Adds a child node representing a GBArray to an other node
 * @param node a valid XML node.
 * @param array The set value to add.
 * @return true if content is fully added, false  otherwise. Note : Will return false if some content was already added (via GBXMLNodeSetContent) to the node.
 */
BOOLEAN_RETURN uint8_t GBXMLNodeAddGBArrayForKey(GBXMLNode* node , GBRef array , const GBString* key, uint8_t writeTypeAttr);

/*!
 * @discussion Adds a child node representing a GBCollection to an other node
 * @param node a valid XML node.
 * @param collection A GBObject conforming to GBCollectionType.
 * @return true if content is fully added, false  otherwise. Note : Will return false if some content was already added (via GBXMLNodeSetContent) to the node.
 */
BOOLEAN_RETURN uint8_t GBXMLNodeAddGBCollectionForKey(GBXMLNode* node , GBRef collection , const GBString* key, uint8_t writeTypeAttr);

/*!
 * @discussion Adds a child node representing a GBString to an other node
 * @param node a valid XML node.
 * @param string The value to add.
 * @return true if content is fully added, false  otherwise. Note : Will return false if some content was already added (via GBXMLNodeSetContent) to the node.
 */
BOOLEAN_RETURN uint8_t GBXMLNodeAddGBStringForKey(GBXMLNode* node , GBRef string , const GBString* key , uint8_t writeTypeAttr);
/**/

GB_END_DCL


#endif /* GBXMLNode_h */
