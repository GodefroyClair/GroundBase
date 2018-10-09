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
//  GBObject.h
//  GroundBase
//
//  Created by Manuel Deneu on 27/08/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//

/**
 * \file GBObject.h
 * \brief GBObject Public interface.
 *
 * Base class &  runtime system for the GroundBase Library. See GBObject.hpp for the C++ counterpart.
 *
 * General rules about memory management:
 *
 * GBObjects created must be released!
 *
 * GBRef number = GBNumberInitWithInt(1);
 *
 * GBRelease(number);
 
 * When you pass a value to a method, you must release it later.
 
 * Values returned from methods must be retained if you want to keep a strong reference to it.
 
 * Values passed as parameters must be released if created localy only for the purpose of calling a function
 
 ex 1
 GBString *key = GBStringInitWithCStr("Key1");
 GBRef valueForKey  = GBDictionaryGetValueForKey(dict , key);
 GBrelease(key);
 
 ex 2
 GBString *val = GBStringInitWithCStr("Foo");
 GBCollectionAddValue(collect  , val); // val is now owned by collect. The value pointed by val can be released
 GBrelease(val);
 
**/

#ifndef GBObject_h
#define GBObject_h

#ifdef __cplusplus
extern "C" {
#endif


#include <stdarg.h>
#include <stdint.h>

#include <GBCommons.h>
#include <GBTypes.h>




/*!
 * @typedef _GBObjectClass
 * @brief Base class for every GroundBase object.
 */
typedef struct _GBObjectClass GBObjectClass;
    
/*!
 * @typedef GBObjectClassRef
 * @brief A const reference to a GBObject instance.
 */
typedef const GBObjectClass* GBObjectClassRef;



/*!
 * @discussion Retains a GroundBase object.
 * @param object The object to retain.
 * @return the updated reference count.
 */
int GBRetain( GBRef object);

/*!
 * @discussion Releases a GroundBase object. Note : GBRelease will assert on invalid or NULL object if GBRuntimeEnableInvalidReleaseDebug has been set to 1.
 * @param object The object to release.
 * @return 0 in case any error occured during release, ie for invalid objects , NULL pointers, over-released objects, etc.
 */
BOOLEAN_RETURN uint8_t GBRelease (GBRef object);

/*!
 * @discussion Clones a GBObject instance.
 * @param object The object to clone.
 * @return A clone of the object. Note for containers : inside values will not be cloned, but their reference count will be incremented.
 */
GBObject * GBObjectClone (GBRef object );

BOOLEAN_RETURN uint8_t GBObjectIsCloneable(GBRef object);

GBSize GBObjectGetSize (GBRef object);

BOOLEAN_RETURN uint8_t GBObjectIsValid(GBRef obj);

/*
 Two objects are equal if their hash are the same. ie GBHash(object1) == GBHash(object2)
 */
BOOLEAN_RETURN uint8_t GBObjectEquals( GBRef obj1 ,GBRef obj2 );

/*!
 * @discussion Tests if a GBObject ref is an instance of a certain class.
 * @param obj the instance to test
 * @param _class the class to test (e.g GBStringClass, GBNumberClass)
 * @return true if obj inherits from _class. 
 */
BOOLEAN_RETURN uint8_t IsKindOfClass( GBRef obj, GBObjectClassRef _class);

GBRef GBObjectGetClassName( GBRef object);

GBObjectClassRef GBObjectGetClass( GBRef object);

/*!
 * @discussion Returns a code that can be used to identify an object in a hashing structure.
 * @param object a valid GBObject to get the hashed value from.
 * @return A number of type GBHashCode.
 */
GBHashCode GBHash(GBRef object);


/*!
 * @discussion Shows the object's description on the standard output.
 * @param object a valid GBObject.
 */
void GBObjectLog( GBRef object );

/*!
 * @discussion Returns the object's description on the standard output.
 * @param object a valid GBObject.
 * @return A GBString instance of the object's description.
 */
GBRef /* is GBString*/ GBObjectGetDescription( GBRef object);


/* Debug */
GBSize GBObjectGetObjectsCount(void);


/*!
 * @discussion Checks whether an GBObject's instance can be serialized.
 * @param object a valid GBObject.
 * @return 1 if the object can be serialized, 0 otherwise.
 */
//BOOLEAN_RETURN uint8_t GBObjectCanBeSerialized(GBRef object);

BOOLEAN_RETURN uint8_t  GBObjectLock( GBObject* object);
BOOLEAN_RETURN uint8_t  GBObjectTryLock( GBObject* object);
BOOLEAN_RETURN uint8_t  GBObjectUnlock( GBObject* object);


//#define GBObjectCast(object , class) ({ GBRef cast = (class*) object;  GBRef retval = GBObjectIsValid(cast)? object : NULL;  retval;})

size_t GBObjectIntrospection( uint8_t log);

int GBObjectGetRefCount( GBRef object);

/* **** **** **** **** **** **** **** **** **** */
/*
 Runtime settings
 */

/*
 if set to 1, invalid object releases from GBRelease will trigger an assertion.
 This only works in DEBUG mode.
 */
void GBRuntimeEnableInvalidReleaseDebug( uint8_t state);

#ifdef __cplusplus
}
#endif
#endif /* GBObject_h */
