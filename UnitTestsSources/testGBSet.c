//
//  testGBSet.c
//  GroundBase
//
//  Created by Manuel Deneu on 24/09/2017.
//  Copyright © 2017 Manuel Deneu. All rights reserved.
//

#include <stdio.h>
#include <GBSet.h>
#include <GBArray.h>
#include <GBContainer.h>
#include <GBList.h>
#include <GBString.h>
#include "testGBSet.h"


static int iteration(const GBContainer* container , GBRef value , void*context)
{
    assert(container);
    assert(value);
    assert(container);
    assert(container == context);
    
    
    const GBString* str = GBStringInitWithCStr("test string");
    
    assert(GBObjectEquals(value, str));
    GBRelease(str);
    
    return 1;
}
static void testContainer( const GBContainer* container)
{
    assert(container);
    assert(isGBContainter(container));
    assert(GBContainerGetSize(container) > 0);
    
    
    GBContainerIterate(container, iteration, (void*)container);
    
    const GBString* str = GBStringInitWithCStr("test string");
    assert(GBContainerContainsValue(container, str));
    
    GBRelease(str);
}

void testGBSet()
{
    printf("--------Test GBSet --------\n");
    
    GBSet* set = GBSetInit();
    assert(set);
    assert(GBSetGetSize(set) == 0);
    
    assert(GBSetContainsValue(set, set) == 0);
    
    const GBString* str = GBStringInitWithCStr("test string");
    
    assert(GBSetAddValue(set, str));
    assert(GBSetAddValue(set, str) == 0);
    
    GBRelease(str);
    assert(GBObjectIsValid(str));
 
    assert(GBSetContainsValue(set, str));
    
    testContainer(set);
    
    GBRelease(set);
}

void testGBArray()
{
    printf("--------Test GBArray --------\n");
    
    GBArray* array = GBArrayInit();
    assert(array);
    assert(GBArrayGetSize(array) == 0);
    
    assert(GBArrayContainsValue(array, array) == 0);
    
    const GBString* str = GBStringInitWithCStr("test string");
    
    assert(GBArrayAddValue(array, str));
    assert(GBArrayAddValue(array, str) );
    
    GBRelease(str);
    assert(GBObjectIsValid(str));
    
    assert(GBArrayContainsValue(array, str));
    
    testContainer(array);
    
    GBRelease(array);
    
}

void testGBList()
{
    printf("--------Test GBList --------\n");
    
    GBList* array = GBListInit();
    assert(array);
    assert(GBListGetSize(array) == 0);
    
    assert(GBListContainsValue(array, array) == 0);
    
    const GBString* str = GBStringInitWithCStr("test string");
    
    assert(GBListAddValue(array, str));
    assert(GBListAddValue(array, str) );
    
    GBRelease(str);
    assert(GBObjectIsValid(str));
    
    assert(GBListContainsValue(array, str));
    
    testContainer(array);
    
    GBRelease(array);
    
}
