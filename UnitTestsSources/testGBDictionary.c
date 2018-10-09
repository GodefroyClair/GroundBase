//
//  testGBDictionary.c
//  GroundBase
//
//  Created by Manuel Deneu on 23/09/2017.
//  Copyright © 2017 Manuel Deneu. All rights reserved.
//

#include "testGBDictionary.h"
#include <GBDictionary.h>
#include <GBContainer.h>
#include <GBNumber.h>

void testGBDictionary()
{
    {
        printf("--------Test GBDictionary --------\n");
        
        
        GBDictionary* dict = GBDictionaryInit();
        assert(dict);
        assert(GBDictionaryGetSize(dict) == 0);
        
        assert(isGBSequence(dict));
        assert(GBSequenceGetSize(dict) == 0);
        
        
        const GBString* key = GBStringInitWithCStr("Key1");
        const GBString* val = GBStringInitWithCStr("val1");
        
        assert(GBSequenceAddValueForKey(dict, val, key));
        assert(GBDictionaryAddValueForKey(dict, val, key) == 0);// 2nd time MUST fail
        
        
        GBRelease(val);
        
        assert(GBSequenceGetSize(dict) == 1);
        
        assert(GSequenceContainsKey(dict , key));
        assert(GBDictionaryContains(dict, key));
        
        GBRelease(key);
        
        
        GBDictionaryClear(dict);
        
        assert(GBSequenceGetSize(dict) == 0);
        assert(GBDictionaryGetSize(dict) == 0);
        GBRelease(dict);
    }
    {
        GBDictionary* dict = GBDictionaryInit();
        
        const GBContainer* keys = GBDictionaryGetKeyList(dict);
        assert(keys );
        
        assert(GBContainerGetSize(keys) == 0);
        GBRelease(keys);
        GBRelease(dict);
    }
}


static int iterNever(const GBSequence* sequence, const GBString* key , GBRef value , void* context)
{
    UNUSED_PARAMETER(sequence);
    UNUSED_PARAMETER(key);
    UNUSED_PARAMETER(value);
    UNUSED_PARAMETER(context);
    
    assert(0);
    return 1;
}


static int iterEarlyReturn(const GBSequence* sequence, const GBString* key , GBRef value , void* context)
{
    
    UNUSED_PARAMETER(key);
    UNUSED_PARAMETER(value);
    
    GBSize *accum = context;
    assert(accum);
    
    if( ++(*accum) == GBSequenceGetSize(sequence) - 1)
        return 0;
    
    return 1;
}

void testGBDictionary2()
{
    printf("--------Test GBDictionary2 --------\n");
    
    GBDictionary* dict = GBDictionaryInit();
    
    GBDictionaryIterateValues(dict, NULL, NULL);
    
    // iterNever MUST NEVER BE CALLED since dict is empty
    GBDictionaryIterateValues(dict, iterNever, NULL);
    
    for( GBIndex i= 0; i< 10;i++)
    {
        const GBString* k = GBStringInitWithFormat("Key%zi" , i);
        const GBNumber* v = GBNumberInitWithInt((int) i);
        
        assert(GBDictionaryAddValueForKey(dict, v, k));
        GBRelease(v);
        GBRelease(k);
    }
    
    GBSize accum = 0;
    // will early return before the last value
    GBDictionaryIterateValues(dict, iterEarlyReturn, &accum);
    assert(accum == GBDictionaryGetSize(dict) - 1);
    
    
    GBRelease(dict);
    
    
}
