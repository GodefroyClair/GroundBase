//
//  testGBJSON.c
//  GroundBase
//
//  Created by Manuel Deneu on 23/09/2017.
//  Copyright © 2017 Manuel Deneu. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "testGBJSON.h"
#include <GBJSON.h>
#include <string.h>
#include <GBNumber.h>
#include <GBDictionary.h>
#include <GBArray.h>

void testGBJSONParse()
{
    printf("--------Test GBJSON Parse --------\n");
    
    
    /* Invalid tests */
    GBObject* objInvalid = NULL;
    
    objInvalid = GBJSONParse(NULL , 0);
    assert(objInvalid == NULL);
    
    objInvalid = GBJSONParse(NULL , GBSizeInvalid);
    assert(objInvalid == NULL);
    
    {
        char lol = '1';
    
        const GBNumber* num = GBJSONParse(&lol , 1);
        assert( num && IsKindOfClass(num, GBNumberClass));
        assert(GBNumberGetInt(num) == 1);
        
        GBRelease(num);
    }
    
    {
        char lol[] = "1.123";
        
        const GBNumber* num = GBJSONParse( lol , 5);
        assert( num && IsKindOfClass(num, GBNumberClass));
        
        assert(GBNumberGetDouble(num) == 1.123);
        
        GBRelease(num);
    }
    {
        const char buff1[] = "{ \"test\":\"Hello\" }";
        
        const GBObject* obj1 = GBJSONParse(buff1, strlen(buff1));
        assert(obj1);
        assert(IsKindOfClass(obj1, GBDictionaryClass));
        assert(GBDictionaryGetSize(obj1) == 1);
        
        const GBString* key = GBStringInitWithCStr("test");
        const GBString* val = GBStringInitWithCStr("Hello");
        
        assert(GBDictionaryContains(obj1, key));
        assert( GBStringEquals( GBDictionaryGetValueForKey(obj1, key) , val ));
        
        GBRelease(key);
        GBRelease(val);
        GBRelease(obj1);
    }
    {
        const char buff1[] = "{ \"test\":1234 }";
        
        const GBObject* obj1 = GBJSONParse(buff1, strlen(buff1));
        assert(obj1);
        assert(IsKindOfClass(obj1, GBDictionaryClass));
        assert(GBDictionaryGetSize(obj1) == 1);
        const GBString* key = GBStringInitWithCStr("test");
        assert(GBDictionaryContains(obj1, key));
        
        assert(IsKindOfClass(GBDictionaryGetValueForKey(obj1, key), GBNumberClass));
        
        GBRelease(key);
        GBRelease(obj1);
    }
    {
        const char buff1[] = "{ \"test\":-100.123 }";
        
        const GBObject* obj1 = GBJSONParse(buff1, strlen(buff1));
        assert(obj1);
        assert(IsKindOfClass(obj1, GBDictionaryClass));
        assert(GBDictionaryGetSize(obj1) == 1);
        const GBString* key = GBStringInitWithCStr("test");
        assert(GBDictionaryContains(obj1, key));
        assert(IsKindOfClass(GBDictionaryGetValueForKey(obj1, key), GBNumberClass));
        
        GBRelease(key);
        GBRelease(obj1);
    }
    {
        const char buff1[] = "[ 1 , 2 , 3 , 4 , 5]";
        
        const GBObject* obj1 = GBJSONParse(buff1, strlen(buff1));
        assert(obj1);
        assert(IsKindOfClass(obj1, GBArrayClass));
        assert(GBArrayGetSize(obj1) == 5);
        
        GBRef v = NULL;
        int acc = 1;
        GBArrayForEach(obj1, v)
        {
            assert(IsKindOfClass(v, GBNumberClass));
            assert(GBNumberGetInt(v) ==acc++ );
        }
        
        GBRelease(obj1);
    }
    {
        const char buff1[] = "{\"array\" : [ 1 , 2 , 3 , 4 , 5] , \"string\":\"value test 1 2 3 4\"}";
        
        const GBObject* obj1 = GBJSONParse(buff1, strlen(buff1));
        assert(obj1);
        assert(isGBSequence(obj1));
        
     
        const GBString* keyArray = GBStringInitWithCStr("array");
        assert(GBDictionaryContains(obj1, keyArray));
        GBRef array = GBDictionaryGetValueForKey(obj1, keyArray);
        assert(array);
        GBRef v = NULL;
        int acc = 1;
        GBArrayForEach(array, v)
        {
            assert(IsKindOfClass(v, GBNumberClass));
            assert(GBNumberGetInt(v) ==acc++ );
        }
        GBRelease(keyArray);
        
     
        const GBString* keyString = GBStringInitWithCStr("string");
        assert(GBDictionaryContains(obj1, keyString));
        GBRef str = GBDictionaryGetValueForKey(obj1, keyString);
        assert(IsKindOfClass(str, GBStringClass));
        assert(GBStringEqualsCStr(str, "value test 1 2 3 4"));
        
        
        GBRelease(keyString);
        GBRelease(obj1);
    }
    {
        const char buff1[]  = "{ \"val0\": 0.0, \"val1\": 10.0, \"val2\": 20.0, \"val3\": 30.0, \"val4\": 40.0, \"val5\": 50.0, \"val6\": 60.0, \"val7\": 70.0, \"val8\": 80.0, \"val9\": 90.0 }";
        
        const GBObject* obj1 = GBJSONParse(buff1, strlen(buff1));
        assert(obj1);
        assert(isGBSequence(obj1));
        
        
        for(int i = 0;i<10;i++)
        {
            const GBString* key = GBStringInitWithFormat("val%i" , i);
            //const GBNumber* v = GBNumberInitWithFloat(i*10.f);
            
            assert(GBDictionaryContains(obj1, key));
            
            
            GBRelease(key);
            //GBRelease(v);
        }
        
        
        GBRelease(obj1);
    }
    
    
    
}

void testGBJSONSave(void)
{
    printf("--------Test GBJSON Save --------\n");
    
    assert( GBJSONWriteBuffer(NULL ) == NULL);
    

    {
        const GBNumber* num = GBNumberInitWithInt(1234);
        
        char* buff = GBJSONWriteBuffer(num);
        assert(buff);
        printf("RET int : '%s'\n" , buff);
        
        free(buff);
        GBRelease(num);
    }
    
    {
        const GBNumber* num = GBNumberInitWithFloat(-1234.1f);
        
        char* buff = GBJSONWriteBuffer(num);
        assert(buff);
        printf("RET  float : '%s'\n" , buff);
        free(buff);
        GBRelease(num);
    }
    {
        const GBNumber* num = GBNumberInitWithLong(4567890987654);
        
        char* buff = GBJSONWriteBuffer(num);
        assert(buff);
        printf("RET  long : '%s'\n" , buff);
        free(buff);
        GBRelease(num);
    }
    {
        const GBString* val = GBStringInitWithCStr("TEST String");
        
        char* buff = GBJSONWriteBuffer(val);
        assert(buff);
        printf("RET  string : '%s'\n" , buff);
        free(buff);
        GBRelease(val);
    }
     
    {
        GBArray* val = GBArrayInit();
        
        for(int i = 0;i<10;i++)
        {
            const GBString* s = GBStringInitWithFormat("val%i" , i);
            GBArrayAddValue(val, s);
            GBRelease(s);
        }
        
        
        char* buff = GBJSONWriteBuffer(val);
        assert(buff);
        printf("RET  array : '%s'\n" , buff);
        free(buff);
        GBRelease(val);
    }
    {
        GBDictionary* val = GBDictionaryInit();
        
        for(int i = 0;i<10;i++)
        {
            const GBString* key = GBStringInitWithFormat("val%i" , i);
            const GBNumber* v = GBNumberInitWithFloat(i*10.f);
            
            GBDictionaryAddValueForKey(val, v, key);
            GBRelease(key);
            GBRelease(v);
        }
        
        
        char* buff = GBJSONWriteBuffer(val);
        assert(buff);
        printf("RET  dict : '%s'\n" , buff);
        
        const GBObject* out = GBJSONParse(buff, strlen(buff));
        free(buff);
        
        assert(out);
        GBRef keyList1 = GBDictionaryGetKeyList(val);
        GBRef keyList2 = GBDictionaryGetKeyList(out);
        
        assert(GBObjectEquals( keyList1, keyList2));
        
        GBRelease(keyList1);
        GBRelease(keyList2);
        GBRelease(val);
        GBRelease(out);
    }

    
}
