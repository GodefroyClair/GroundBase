//
//  GBSequenceType_Private.h
//  GroundBase
//
//  Created by Manuel Deneu on 23/09/2017.
//  Copyright © 2017 Manuel Deneu. All rights reserved.
//

#ifndef GBSequence_Private_h
#define GBSequence_Private_h

#include <GBObject_Private.h>
#include <GBSequence.h>


typedef GBSize                 (*_SequenceGetSize)(const GBSequence* container);
typedef BOOLEAN_RETURN uint8_t (*_SequenceAddValueForKey)(GBSequence* container , GBRef value , const GBString* key);
typedef BOOLEAN_RETURN uint8_t (*_SequenceContainsKey)( const GBSequence* container , const GBString* key);
typedef GBRef (*_SequenceGetValue)( const GBSequence* container , const GBString* key);

typedef void (*_SequenceIterate)(const GBSequence* container , GBSequenceIterator method , void* context);

//typedef BOOLEAN_RETURN uint8_t (*_GBContainerReleaseContent)(GBContainer* container );
//typedef BOOLEAN_RETURN uint8_t (*_SequenceRemoveValue)(GBSequence* container , GBRef value);

typedef struct
{
    _SequenceGetSize         _getSize;
    _SequenceAddValueForKey  _addValueForKey;
    _SequenceContainsKey     _containsKey;
    _SequenceGetValue       _getValue;
    _SequenceIterate        _iterate;
    
}GBSequenceCallbacks;


typedef struct
{
    GBObjectBase base;
    GBSequenceCallbacks _methods;
    //void* _impl;
    
} GBSequenceBase;


BOOLEAN_RETURN uint8_t GBSequenceBaseInit( GBSequenceBase*self, GBSequenceCallbacks methods );


#endif /* GBSequence_Private_h */
