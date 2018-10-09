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
//  testClone.c
//  GroundBase
//
//  Created by Manuel Deneu on 17/09/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <GBString.h>
#include <GBNumber.h>
#include <GBArray.h>
#include <GBList.h>
#include <GBDictionary.h>
#include <GBContainer.h>
#include <GroundBase.h>
#include "testClone.h"

static void testCloneDictionary(void);
//static void testCloneCollectionType(GBObjectClassRef collectionType);

void testClone()
{
    printf("----- Test Clone ----\n");
    
    /*  String */
    const char* ctnt = "Hello world";
    GBString *sourceStr = GBStringInitWithCStr( ctnt );
    GBString *destStr = GBObjectClone(sourceStr);
    
    assert(GBObjectEquals(sourceStr, destStr));
    assert(GBObjectGetRefCount(sourceStr) == 1);
    assert(GBObjectGetRefCount(destStr) == 1);
    GBRelease(sourceStr);
    assert( strcmp( GBStringGetCStr(destStr) , ctnt) ==0 );
    printf("String clone '%s' \n",GBStringGetCStr(destStr) );
    
    GBRelease(destStr);

    /*  GBNumber */
    
    const float val =678.78f;
    
    GBNumber* sourceNum = GBNumberInitWithFloat(val);
    GBNumber* destNum = GBObjectClone(sourceNum);
    
    assert(GBObjectEquals(sourceNum, destNum));
    assert(GBObjectGetRefCount(sourceNum) == 1);
    assert(GBObjectGetRefCount(destNum) == 1);
    
    GBRelease(sourceNum);
    assert(GBNumberGetFloat(destNum) == val);
    printf("Num clone %f \n" , GBNumberGetFloat(destNum));
    GBRelease(destNum);
    
    /*  GBArray / GBList */
    
    //testCloneCollectionType(GBArrayClass);
    //testCloneCollectionType(GBListClass);

    testCloneDictionary();
    
}
static void testCloneDictionary()
{
    
    GBDictionary* dict1 =  GBDictionaryInit();
    
    
    const GBString * key1 = GBStringInitWithCStr("Key1");
    const GBString * key2 = GBStringInitWithCStr("Key2");
    
    GBNumber* val1 = GBNumberInitWithInt(132);
    GBNumber* val2 = GBNumberInitWithInt(575);
    
    GBDictionaryAddValueForKey(dict1, val1, key1);
    GBDictionaryAddValueForKey(dict1, val2, key2);
    
    GBRelease(val1);
    GBRelease(val2);
    
    assert(GBObjectGetRefCount(val1) == 1);
    assert(GBObjectGetRefCount(val2) == 1);
    
    const GBDictionary* dict2 = GBObjectClone(dict1);
    assert(dict2);
    assert(GBDictionaryGetSize(dict1) == GBDictionaryGetSize(dict2));
    
    assert(GBObjectGetRefCount(val1) == 2);
    assert(GBObjectGetRefCount(val2) == 2);
    GBRelease(key1);
    GBRelease(key2);
    
    GBRelease(dict1);
    GBRelease(dict2);
    
}
/*
static void testCloneCollectionType(GBObjectClassRef collectionType)
{
    
    GBContainer* set = collectionType == GBArrayClass? GBArrayInit() : GBListInit();//  GBCollectionInitWithType(collectionType);
    const GBSize count = 10;
    for(GBIndex i = 0; i<count ; i++)
    {
        const GBString* v = GBStringInitWithFormat("Value %zi" , i);
        GBContainerAddValue(set, v);
        GBRelease(v);
    }
    
    assert(GBContainerGetSize(set) == count);
    
    const GBContainer* clone = GBObjectClone(set);
    
    
    for(GBIndex i = 0; i<GBContainerGetSize(clone) ; i++)
    {
        assert( GBObjectGetRefCount( GBContainerGetValueAtIndex(clone, i)) == 2);
    }
    
    GBRelease(set);
    assert(GBContainerGetSize(clone) == count);
    for(GBIndex i = 0; i<GBContainerGetSize(clone) ; i++)
    {
        assert( GBObjectGetRefCount( GBContainerGetValueAtIndex(clone, i)) == 1);
    }
    GBRelease(clone);
}
 */
