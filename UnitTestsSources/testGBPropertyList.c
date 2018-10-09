//
//  testGBPropertyList.c
//  TestGroundBase
//
//  Created by Manuel Deneu on 01/10/2017.
//  Copyright © 2017 Manuel Deneu. All rights reserved.
//

#include "testGBPropertyList.h"


#include <stdio.h>
#include <GBPropertyList.h>
#include <GBDictionary.h>
#include <GBNumber.h>

static void printSeq( const GBDictionary* dict ,int indent)
{
    const GBArray* keys = GBDictionaryGetKeyList(dict);
    const GBString* k = NULL;
    GBArrayForEach(keys, k)
    {
        for(int i=0;i<indent;i++)
            printf("\t");
        
        printf("Key '%s'" , GBStringGetCStr(k));
        
        GBRef v = GBDictionaryGetValueForKey(dict,k);
        assert(v);
        if( IsKindOfClass(v, GBDictionaryClass))
        {
            printSeq(v, indent+1);
        }
        else
        {
            
            GBObjectLog(v);
            
        }
        
        printf("\n");
    }
    
    GBRelease(keys);
}

void testGBPropertyList()
{
    printf("----- Test GBPropertyList ----\n");
    
    {
        GBPropertyList* plist = GBPropertyListInit();
        assert(plist);
        
        
        assert(GBPropertyListWriteRootObject(plist,NULL) == 0);
        assert(GBPropertyListWriteRootObject(NULL,NULL) == 0);
        GBRelease(plist);
    }
    
    {
        const GBString* file = GBStringInitWithCStr("text.plist");
        
        const GBPropertyList* plist =  GBPropertyListInitWithFile(file);
        
        GBRelease(file);
        
        assert(GBPropertyListGetRoot(plist));
        
        
        
        printSeq(GBPropertyListGetRoot(plist), 0);
        GBRelease(plist);
        
    }
}

static void compareValues( GBRef originalVal , GBRef readVal);

void testGBPropertyWrite()
{
    printf("----- Test GBPropertyList Write ----\n");
    
    GBPropertyList* plist = GBPropertyListInit();
    assert(plist);
    
    GBDictionary* dict = GBDictionaryInit();
    
    GBDictionary* module = GBDictionaryInit();
    const GBString* mdlKey = GBStringInitWithCStr("Module");
    GBDictionaryAddValueForKey(dict, module,mdlKey );
    GBRelease(module);
    
    {
        const GBString* descKey = GBStringInitWithCStr("Description");
        const GBString* desc  = GBStringInitWithCStr("Test description");
    
        GBDictionaryAddValueForKey(module, desc, descKey);
        GBRelease(desc);
        GBRelease(descKey);
    }
    
    {
        const GBString* key = GBStringInitWithCStr("Dev");
        const GBString* val  = GBStringInitWithCStr("FlyLab");
        
        GBDictionaryAddValueForKey(module, val, key);
        GBRelease(val);
        GBRelease(key);
    }
    {
        const GBString* key = GBStringInitWithCStr("Version");
        const GBNumber* val  = GBNumberInitWithFloat(0.4f);
        
        GBDictionaryAddValueForKey(module, val, key);
        GBRelease(val);
        GBRelease(key);
    }
    {
        const GBString* key = GBStringInitWithCStr("Version2");
        const GBNumber* val  = GBNumberInitWithDouble(0.004f);
        
        GBDictionaryAddValueForKey(module, val, key);
        GBRelease(val);
        GBRelease(key);
    }
    {
        const GBString* key = GBStringInitWithCStr("Listened");
        const GBNumber* val  = GBNumberInitWithInt(0);
        
        GBDictionaryAddValueForKey(module, val, key);
        GBRelease(val);
        GBRelease(key);
    }
    {
        const GBString* key = GBStringInitWithCStr("Label");
        const GBString* val  = GBStringInitWithCStr("com.flylab.test");
        
        GBDictionaryAddValueForKey(module, val, key);
        GBRelease(val);
        GBRelease(key);
    }
    {
        const GBString* key = GBStringInitWithCStr("Launch");
        GBDictionary* labelDict  = GBDictionaryInit();
        
        {
            const GBString* k = GBStringInitWithCStr("Label");
            const GBString* v = GBStringInitWithCStr("com.FlyLab.FlyServer");
            GBDictionaryAddValueForKey(labelDict, v, k);
            GBRelease(k);
            GBRelease(v);
        }
        {
            const GBString* k = GBStringInitWithCStr("Command");
            const GBString* v = GBStringInitWithCStr("/opt/FlyLab/Bin/FlyServer");
            GBDictionaryAddValueForKey(labelDict, v, k);
            GBRelease(k);
            GBRelease(v);
        }
        {
            const GBString* k = GBStringInitWithCStr("RestartAtCrash");
            const GBNumber* v = GBNumberInitWithInt(1);
            GBDictionaryAddValueForKey(labelDict, v, k);
            GBRelease(k);
            GBRelease(v);
        }
        {
            const GBString* k = GBStringInitWithCStr("StartAtLaunch");
            const GBNumber* v = GBNumberInitWithInt(1);
            GBDictionaryAddValueForKey(labelDict, v, k);
            GBRelease(k);
            GBRelease(v);
        }
        {
            const GBString* k = GBStringInitWithCStr("DelayAtStart");
            const GBNumber* v = GBNumberInitWithInt(0);
            GBDictionaryAddValueForKey(labelDict, v, k);
            GBRelease(k);
            GBRelease(v);
        }
        
        
        GBDictionaryAddValueForKey(module, labelDict, key);
        GBRelease(labelDict);
        GBRelease(key);
    }
    
    GBRelease(mdlKey);
    
    assert(GBPropertyListWriteRootObject(plist , dict));
    
    assert(GBPropertyListSaveFile(plist , GBSTR("text.plist")));
    GBRelease(dict);
    
    printf("----- Test GBPropertyList Read ----\n");
    {
        GBPropertyList* readPList = GBPropertyListInitWithFile( GBSTR("text.plist"));
        assert(readPList);
        const GBDictionary* readRoot = GBPropertyListGetRoot(readPList);
        assert(readRoot);
        const GBDictionary* originalRoot = GBPropertyListGetRoot(plist);
        assert(originalRoot);
        
        
        assert(GBDictionaryGetSize(readRoot) == GBDictionaryGetSize(originalRoot));
        
        
        const GBArray* originalKeys = GBDictionaryGetKeyList(originalRoot);
        
        const GBString* key = NULL;
        GBArrayForEach(originalKeys, key)
        {
            
            
            printf("Test original / read = Key '%s'\n" , GBStringGetCStr(key));
            assert(GBDictionaryContains(readRoot, key));
            
            const GBRef originalVal = GBDictionaryGetValueForKey(originalRoot, key);
            const GBRef readVal = GBDictionaryGetValueForKey(readRoot, key);
            
            
            compareValues(originalVal,  readVal);
            
            
            
        }
        
        GBRelease(originalKeys);
        
        
        GBRelease(readPList);
    }
    
    GBRelease(plist);
    
}

static void compareValues( GBRef originalVal , GBRef readVal)
{
    assert(GBObjectGetClass(originalVal) == GBObjectGetClass(readVal));
    
    
    if( IsKindOfClass(originalVal, GBStringClass))
    {
        assert(GBStringEquals(originalVal, readVal));
    }
    else if( IsKindOfClass(originalVal, GBNumberClass))
    {
        if( GBNumberGetType(originalVal) == GBNumberTypeInt)
        {
            assert( GBObjectEquals(originalVal, readVal));
        }
        else if( GBNumberGetType(originalVal) == GBNumberTypeFloat || GBNumberGetType(originalVal) == GBNumberTypeDouble)
        {
            const double v1 = GBNumberToDouble(originalVal);
            const double v2 = GBNumberToDouble(readVal);
            printf("Compare %f with %f \n" , v1 , v2);
            
            const double epsilon = 1.e-07F;
            printf("Choosen epsilon %f" , epsilon);
            assert(v1 > v2 - 1.e-07F && v1 < v2 + 1.e-07F);
        }
    }
    else if( IsKindOfClass(originalVal, GBDictionaryClass))
    {
        const GBArray* originalKeys = GBDictionaryGetKeyList(originalVal);
        
        const GBString* key = NULL;
        GBArrayForEach(originalKeys, key)
        {
            printf("Test original / read = Key '%s'\n" , GBStringGetCStr(key));
            const GBRef originalVal2 = GBDictionaryGetValueForKey(originalVal, key);
            const GBRef readVal2 = GBDictionaryGetValueForKey(readVal, key);
            compareValues(originalVal2, readVal2);
        }
        GBRelease(originalKeys);
    }
}

