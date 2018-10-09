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
//  testGBObject.c
//  GroundBase
//
//  Created by Manuel Deneu on 09/09/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//

#include <GroundBase.h>
#include <GBNumber.h>
#include <GBArray.h>
#include <GBList.h>
#include "testGBObject.h"


#include "../src/GBObject_Private.h"
#include "../src/Private/Array.h"

void testGBObject()
{
    printf("----- Test GBObject ----\n");
    
    void* invalid = NULL;
    
    assert(GBObjectIsValid(invalid) == 0);
    assert(GBRetain(invalid )== -1);
    
    /*
    int inval2 = 1;
    
    assert(GBObjectIsValid(&inval2) == 0);
    assert(GBRetain(&inval2 )== -1);
    */
    GBNumber* n1 = GBNumberInitWithLong(1);
    
    void* ptrV = n1;
    
    assert(GBObjectIsValid(n1));
    assert(n1 == ptrV);
    
    assert(GBObjectGetRefCount(n1 )== 1);
    
    const int retainNum  = 10;
    
    for (int i = 0; i < retainNum ; i++)
    {
        assert(GBObjectGetRefCount(n1) == i+1);
        GBRetain(n1);
        
    }
    
    printf("GBObject ref count %i \n " , GBObjectGetRefCount(n1));
    
    const int releaseCount = GBObjectGetRefCount(n1);
    
    for (int i = 0; i < releaseCount ; i++)
    {
        assert(GBObjectGetRefCount(n1) == releaseCount-i);
        
        printf("%i GBObject ref count %i \n " , i,GBObjectGetRefCount(n1));
        GBRelease(n1);
        
    }
    
    // n1 is null
    
    /*
    GBRef test = GBStringInit();
    GBRef testCast = GBObjectCast(test, GBString);
    assert(testCast && IsKindOfClass(testCast, GBStringClass));
    GBRelease(test);
    
    GBRef test2 = NULL;
    GBRef testCast2 = GBObjectCast(test2, GBString);
    assert(testCast2 == NULL);
    
    
    const int foo = 1;
    GBRef test3 = &foo;
    
    GBRef testCast3 = GBObjectCast(test3, GBString);
    assert(testCast3 == NULL);
    */
}



void testGBObjectOwnership()
{
    printf("----- Test GBObject Ownership ----\n");
    
    //set and array will share the same objects
    
    GBString * str = GBStringInitWithCStr("Hello");
    

    
    GBArray *array = GBArrayInit();
    GBList *set = GBListInit();

    GBListAddValue(set, str);
    GBArrayAddValue(array, str);
    
    const GBSize count = 10;
    for ( GBIndex i = 0; i < count; i++)
    {
        GBNumber *num = GBNumberInitWithFloat(i*1.f);
        GBArrayAddValue(array, num);
        GBListAddValue(set, num);
        GBRelease(num);
    }

    assert( GBArrayGetSize(array) == count +1 );
    assert( GBListGetSize(set) == count +1 );
    
    assert(GBObjectGetRefCount(str ) == 3);
    
    GBRelease(array);
    
    GBRef val = NULL;
    GBIndex i = 0;
    GBListForEach(set, val)
    {
        if( i==0)
        {
            assert( IsKindOfClass( val , GBStringClass));
        }
        else
            assert( IsKindOfClass( val , GBNumberClass));
        
        i++;
    }
    
    
    GBRelease(set);
    
    assert(GBObjectIsValid(str));
    printf("String value %s \n"  , GBStringGetCStr(str));
    GBRelease(str);
    
    
}

void testGBObjectInternals()
{
    printf("----- Test GBObject Internals ----\n");
    
    GBString* base = GBStringInit();
    assert(base);
    GBObjectBase* baseObj = (GBObjectBase*)base;
    
    assert(baseObj);

    GBNumber* childNum = GBNumberInitWithInt(1);
    
    
    GBSize size = ArrayGetSize( baseObj->strongReferences );
    assert( GBObjectAddStrongReference(base, childNum) );
    
    GBRelease( childNum );
    assert(GBObjectIsValid(childNum));
    
    assert( ArrayGetSize( baseObj->strongReferences ) == size +1);
    
    // add for the 2nd time must fail
    assert( GBObjectAddStrongReference(base, childNum) == 0 );
    assert( ArrayGetSize( baseObj->strongReferences ) == size +1);
    
    
    GBRelease( base );
    
}


void testGBObject2(void)
{
    GBNumber* num1 = GBNumberInitWithInt(1);
    
    printf("num1 %p\n" ,(void*) num1);
    GBRelease(num1);
    GBNumber* num2 = GBNumberInitWithInt(10);
    printf("num2 %p\n" ,(void*)  num2);
    
    const GBSize t = 128;
    
    for (size_t i = 0; i<t; ++i)
    {
        GBNumber* num = GBNumberInitWithInt(10);
        printf("num3 %p\n" ,(void*)  num);
        GBRelease(num);
    }
    
    GBRelease(num2);
}


