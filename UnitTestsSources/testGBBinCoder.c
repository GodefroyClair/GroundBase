//
//  testGBBinnCoder.c
//  TestGroundBase
//
//  Created by Manuel Deneu on 28/09/2017.
//  Copyright © 2017 Manuel Deneu. All rights reserved.
//
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <GBBinCoder.h>
#include <GBDictionary.h>

#include "testGBBinCoder.h"
#include "../src/Coders/Binn/binn.h"

void testGBBinCoder()
{
    printf("--------Test GBBinCoder --------\n");
    
    GBBinCoder* coder = GBBinCoderInit();
    assert(coder);
    assert(GBObjectIsValid(coder));
    assert(IsKindOfClass(coder,GBBinCoderClass));
    
    // invalid encode/decode
    assert(GBBinCoderEncodeString(coder, NULL, NULL) == 0);
    assert(GBBinCoderDecodeString(coder, NULL) == NULL);
    
    assert(GBBinCoderEncodeNumber(coder, NULL, NULL) == 0);
    assert(GBBinCoderDecodeNumber(coder, NULL) == NULL);
    
    GBString* v = GBStringInitWithCStr("valazejamzelknazemlkanzelmkanzemlazkenalmkzenazlemk");
    GBString* emptyKey = GBStringInit();
    
    assert(GBBinCoderEncodeString(coder, v, emptyKey) == 0);
    
    // valid
    GBString* k = GBStringInitWithCStr("key");
    
    assert(GBBinCoderEncodeString(coder, v, k));
    assert(GBBinCoderEncodeString(coder, v, k) == 0);
    
    GBNumber* vNum  = GBNumberInitWithFloat(3.14f);
    GBString* kNum = GBStringInitWithCStr("key2");
    
    assert(GBBinCoderEncodeNumber(coder,vNum, kNum));
    assert(GBBinCoderEncodeNumber(coder,vNum, kNum) == 0);
    
    GBRelease(vNum);
    
    assert(GBBinCoderDecodeString(coder, v) == NULL); //invalid key
    assert(GBBinCoderDecodeString(coder, emptyKey) == NULL); //empty key
    assert(GBBinCoderDecodeString(coder, NULL) == NULL); //still NULL
    
    GBString* readVal =  GBBinCoderDecodeString(coder, k);
    assert(readVal);
    assert(GBObjectGetRefCount(readVal) == 1);
    
    assert(GBStringEquals(v, readVal));
    
    GBNumber* readNum = GBBinCoderDecodeNumber(coder, kNum);
    assert(GBBinCoderDecodeNumber(coder, k) == NULL);
    assert(GBBinCoderDecodeString(coder, kNum) == NULL);
    assert(readNum);
    GBRelease(readNum);
    
    GBRelease(coder);
    GBRelease(k);
    GBRelease(kNum);
    assert(GBStringEquals(v, readVal));
    
    GBRelease(v);
    GBRelease(readVal);
    GBRelease(emptyKey);
    
    
}

void testGBBinCoder2()
{
    printf("--------Test GBBinCoder2 --------\n");
    
    GBBinCoder* encoder = GBBinCoderInit();
    
    const int numToAdd = 100;
    for(int i= 0; i< numToAdd;i++)
    {
        GBString* key = GBStringInitWithFormat("Key%i" , i);
        GBNumber* val = GBNumberInitWithFloat(i*100.f);
        assert(GBBinCoderEncodeNumber(encoder, val, key));
        GBRelease(key);
        GBRelease(val);
    }
    
    for(int i= 0; i< numToAdd;i++)
    {
        GBString* key = GBStringInitWithFormat("Key%i" , i);
        GBNumber* val = GBBinCoderDecodeNumber(encoder, key);
        assert(val);
        GBRelease(key);
        GBRelease(val);
    }

    const GBSize encodedSize = GBBinCoderGetBufferSize(encoder);
    assert(encodedSize > 0);
    
    char buffer[encodedSize];
    memset(buffer,0, encodedSize);
    
    assert(GBBinCoderWriteBuffer(encoder, buffer));
    
    printf("Buffer  size %zi '%s'\n" , encodedSize, buffer);
    
    GBBinCoder* decoder = GBBinCoderInitWithContent(buffer);
    assert(decoder);
    GBRelease(encoder);
    
    for(int i= 0; i< numToAdd;i++)
    {
        GBString* key = GBStringInitWithFormat("Key%i" , i);
        GBNumber* val = GBBinCoderDecodeNumber(decoder, key);
        assert(val);
        GBRelease(key);
        GBRelease(val);
    }
    
    GBRelease(decoder);

}

void testGBBinCoder3()
{
    printf("--------Test GBBinCoder3 --------\n");
    

    const char* strContent = "TEst String value";
    
    {
    
        assert(GBBinCoderInitWithRootObject(NULL) == NULL);
        
        GBBinCoder* encoder = NULL;
        
        const GBString* strVal = GBStringInitWithCStr(strContent);
        encoder = GBBinCoderInitWithRootObject(strVal);
        assert(encoder == NULL);
    
        
        GBRelease(strVal);

    }
}

void testGBBinCoder4()
{
    printf("--------Test GBBinCoder4 --------\n");
    
    GBDictionary* dict = GBDictionaryInit();
    
    const int numKeys = 10;
    
    for (int i=0;i <numKeys ;i ++)
    {
        GBObject* val = NULL;
        if (i % 2 == 0)
        {
         val = GBStringInitWithFormat("Lapin #%i value %f" , i , (float)i*3.4);
        }
        else
        {
            val = GBNumberInitWithFloat(i*100.f);
        }
        const GBString* k = GBStringInitWithFormat("key-%i" , i);
        GBDictionaryAddValueForKey(dict, val, k);
        GBRelease(val);
        GBRelease(k);
    }
    
    assert(GBDictionaryGetSize(dict) == numKeys);
    
    GBBinCoder* encoder = GBBinCoderInitWithRootObject(dict);
    assert(encoder);
    GBRelease(dict);
    
    const GBSize bufferOutSize = GBBinCoderGetBufferSize(encoder);
    assert(bufferOutSize);
    char buffOut[bufferOutSize];
    memset(buffOut, 0, bufferOutSize);
    assert(GBBinCoderWriteBuffer(encoder, buffOut));
    
    GBRelease(encoder);
    
    GBBinCoder* decoder = GBBinCoderInitWithContent(buffOut);
    assert(decoder);
    
    
    
    for (int i=0;i <numKeys ;i ++)
    {
        const GBString* val = GBStringInitWithFormat("Lapin #%i value %f" , i , (float)i*3.4);
        const GBString* k = GBStringInitWithFormat("key-%i" , i);
        
        GBObject* valOut = NULL;
        if (i % 2 == 0)
        {
            valOut = GBBinCoderDecodeString(decoder, k);
            assert(valOut);
            assert(GBStringEquals(val, valOut));
        }
        else
        {
            valOut = GBBinCoderDecodeNumber(decoder, k);
            assert(valOut);
            
        }
        GBRelease(val);
        GBRelease(valOut);
        GBRelease(k);
    }
    
    GBRelease(decoder);
    
    
}

void testGBBinCoder5()
{
    printf("--------Test GBBinCoder5 --------\n");
    
    
    const GBSize numModules = 10;
    
    GBSize bufferSize = GBSizeInvalid;
    char* buff = NULL;
    
    
    {
        GBDictionary* root = GBDictionaryInit();
        
        GBDictionary* datas = GBDictionaryInit();
        const GBString* kDatas = GBStringInitWithCStr("Datas");
        assert(GBDictionaryAddValueForKey(root, datas,kDatas ));
        GBRelease(datas);
        
        GBArray* list = GBArrayInit();
        const GBString* kList = GBStringInitWithCStr("Modules");
        assert(GBDictionaryAddValueForKey(datas,list, kList));
        GBRelease(list);
        
        for( GBIndex i= 0;i <numModules ;i++)
        {
            const GBString* label = GBStringInitWithFormat("com.identifier.Module%zi" , i);
            assert(label);
            assert(GBArrayAddValue(list, label));
            GBRelease(label);
        }
        
        
        
        
        GBBinCoder* encoder = GBBinCoderInitWithRootObject(root);
        assert(encoder);
        bufferSize = GBBinCoderGetBufferSize(encoder);
        
        assert(bufferSize != GBSizeInvalid && bufferSize > 0);
        
        assert(buff == NULL);
        
        buff = malloc(bufferSize);
        assert(buff);
        
        assert(GBBinCoderWriteBuffer(encoder, buff));
        
        GBRelease(encoder);
        GBRelease(kList);
        GBRelease(kDatas);
        GBRelease(root);
        
    }
    
    assert(buff && bufferSize > 0);
    
    {
        binn * root =binn_open( buff );
        assert(root);
        
        binn* datas = NULL;
        assert(binn_object_get_object(root, "Datas",(void**) &datas));
        assert(datas && binn_type(datas) == BINN_OBJECT);
        
        binn* list = NULL;
        assert(binn_object_get_list(datas, "Modules",(void**) &list));
        
        
        assert(list && binn_type(list) == BINN_LIST);
        
        
        binn_iter iter;
        
        GBSize c=0;
        binn value;
        binn_list_foreach(list, value)
        {
            assert(binn_get_str(&value));
            
            c++;
        }
        assert(c == numModules);
        
        binn_free(root);
    }
    
    {

        
        GBBinCoder* decoder =  GBBinCoderInitWithContent(buff);
        
        assert(decoder);
        
        const GBString* kDatas = GBStringInitWithCStr("Datas");
        const GBString* kList = GBStringInitWithCStr("Modules");
        
        GBSequence* datas = GBBinCoderDecodeSequence(decoder, kDatas);
        assert( datas);
        
        const GBObject* list=  GBSequenceGetValueForKey(datas, kList);
        assert( list);
        
        assert(IsKindOfClass(list, GBArrayClass));
        

        assert(GBArrayGetSize(list) == numModules);
        
        const GBString *val = NULL;
        GBArrayForEach(list, val)
        {
            assert(IsKindOfClass(val, GBStringClass));
        }
        GBRelease(datas);
        
        GBRelease(kDatas);
        GBRelease(kList);
        GBRelease(decoder);
        
    }
    
    free(buff);
}

void testGBBinCoder6()
{
    printf("--------Test GBBinCoder6 --------\n");
    
    assert( GBBinCoderDecodeRootWithType(NULL, NULL) == NULL);
    assert( GBBinCoderDecodeRootWithType(NULL, GBStringClass) == NULL);
    assert( GBBinCoderDecodeRootWithType(NULL, GBDictionaryClass) == NULL);
    
    {
        GBString* str = GBStringInitWithFormat("Root string object");
        assert(str);
        
        GBDictionary* rootDict = GBDictionaryInit();
        GBString* kstr = GBStringInitWithFormat("key1");
        GBDictionaryAddValueForKey(rootDict, str, kstr);
        GBRelease(kstr);
        
        
        GBBinCoder* coder = GBBinCoderInitWithRootObject(rootDict);
        
        assert(coder);
        
        assert( GBBinCoderDecodeRootWithType(coder, GBArrayClass) == NULL);
        assert( GBBinCoderDecodeRootWithType(coder, GBStringClass) == NULL);
        assert( GBBinCoderDecodeRootWithType(coder, GBNumberClass) == NULL);
        
        GBObject* decodedRoot = GBBinCoderDecodeRootWithType(coder, GBDictionaryClass);
        assert(decodedRoot);
        assert(IsKindOfClass(decodedRoot, GBDictionaryClass));
        GBRelease(decodedRoot);
        
        GBRelease(str);
        GBRelease(rootDict);
        GBRelease(coder);
    }
}




