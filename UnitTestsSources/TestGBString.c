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
//  TestGBString.c
//  GroundBase
//
//  Created by Manuel Deneu on 24/08/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//

#include <string.h>
#include <GBNumber.h>
#include <GBDictionary.h>
#include <GroundBase.h>
#include <GBArray.h>
#include <GBString.h>
#include "TestGBString.h"

#include "../src/Private/StringImpl.h"



void testGBString4()
{
    printf("--------Test GBString4 --------\n");
    
    {
        GBString* str = GBStringInitWithCStr("Hello");
        
        
        assert(GBStringClear(str));
        
        assert(GBStringIsEmpty(str));
        assert(GBStringGetCStr(str) == NULL);
        
        assert(GBStringSetContent(str, NULL));
        
        assert(GBStringSetContent(str, "Hello again"));
        assert( strcmp(GBStringGetCStr(str), "Hello again") == 0);
        assert(GBStringIsEmpty(str) == 0);
        assert(GBStringGetCStr(str) != NULL);
        
        GBRelease(str);
    }
    {
        GBString* str = GBStringInitWithCStr("Hello");
        
        assert(GBStringSetContentWithFormat(str, "%s" , "Hello again") );
        
        assert( strcmp(GBStringGetCStr(str), "Hello again") == 0);
        GBRelease(str);
    }
    {
        GBString* str2 = GBStringInitWithCStr("1-2-3.4");
        
        const void* ptrStr2 = str2;
        
        assert(GBObjectIsValid(str2));
        assert(ptrStr2 == str2);
        int a = -1;
        int b = -1;
        float c = -1.f;
        
        
        assert(GBObjectIsValid(str2));
        assert(ptrStr2 == str2);
        assert(GBStringScan(str2, "%i-%i-%f" , &a ,&b ,&c) == 3);
        assert(a == 1 && b == 2 && c == 3.4f);
        
        assert(GBObjectIsValid(str2));
        assert(ptrStr2 == str2);
        char nonExistingStr[2] = {0};
        assert(GBStringScan(str2, "%i-%i-%f-%s" , &a ,&b ,&c , nonExistingStr) == 3);
        assert(a == 1 && b == 2 && c == 3.4f);
        
        assert(GBObjectIsValid(str2));

        const GBSize ret = GBStringScan(str2, "%s;%f" , nonExistingStr , &c);
        assert(ret < 2);
        assert(ptrStr2 == str2);
        assert(GBObjectIsValid(str2));
        
        GBRelease(str2);
    }
}
void testGBString3()
{
    printf("--------Test GBString3 --------\n");
    
    
    GBString *str = GBStringInit();
    
    assert(GBStringIsEmpty(str));
    
    GBStringSetContent(str, "Hello");
    
    assert(GBStringIsEmpty(str) == 0);
    assert(strcmp(GBStringGetCStr(str) ,"Hello") == 0);
    
    GBRelease(str);
}

void testGBString2()
{
    printf("--------Test GBString2 --------\n");
    
    const int a = 12;
    const float b = 42.3f;
    const char *add = "This string's gonna be added to another one";

    GBString* str = GBStringInitWithFormat("%i -- %f -> '%s' \n" , a , b ,add);
    
    assert( str);
    
    printf("Total string '%s' \n" , GBStringGetCStr( str));
    
    int outA = 0;
    float outB = 0.0f;
    char outC[256];
    memset(outC , 0 , 256);
    
    if(GBStringScan(str, "%i -- %f -> '%s' \n" , &outA , &outB , outC ))
    {
        assert( a == outA);
        assert( b == outB);
        printf("A : %i \n" , outA);
        printf("B : %f \n" , outB);
        printf("C : %s \n" , outC);
    }
    
    GBRelease(str);
    
    
}

void testGBString()
{
    printf("--------Test GBString --------\n");
 
    const GBSize numATomsAtStart = StrImpl_GetNumString();
    
    GBString* str0 = GBStringInitWithCStr("");
    GBString* str01 = GBStringInitWithCStr("HELLO");
    
    assert(GBObjectIsValid(str0) );
    assert(GBObjectIsValid(str01));
    assert(GBObjectIsValid(NULL) == 0);
    
    GBRelease(str0);
    GBRelease(str01);
    
    
    
    GBString* str1 = GBStringInitWithCStr("Hello");
    GBString* str2 = GBStringInit();
    
    assert(GBStringGetLength(str2) == 0);
    assert(GBStringIsValid(str1));
    assert(GBStringIsValid(str2) == 0 );
    GBStringSetContent(str2, "Hello2");
    
    assert(GBObjectEquals(str1, str2) == 0);
    
    printf("String str1 : '%s' len %lu \n" , GBStringGetCStr(str1) , GBStringGetLength(str1));
    printf("String str2-1 : '%s' len %lu \n" , GBStringGetCStr(str2) , GBStringGetLength(str2));
    
    
    GBStringSetContent(str2, "Hello2-Changed");
    
    printf("String str2-2 : '%s' len %lu \n" , GBStringGetCStr(str2) , GBStringGetLength(str2));
    
    GBStringClear(str2);
    assert(GBStringGetLength(str2) == 0);
    assert(GBStringGetCStr(str2) == NULL);
    
    GBRelease( str1);
    GBRelease( str2);
    
    
    
    GBString *appendCStr = GBStringInitWithCStr("Hello");
    GBStringAppendCStr(appendCStr, " World");
    
    printf("appendCStr : '%s' \n" , GBStringGetCStr(appendCStr));
    
    
    GBString *appendString = GBStringInitWithCStr("Hello");
    GBString *worldString = GBStringInitWithCStr(" World");
    
    GBStringAppend(appendString, worldString);
    
    printf("appendString : '%s' \n" , GBStringGetCStr(appendString));
    
    assert(  GBStringEquals(appendString, appendCStr));
    assert(GBStringGetLength(appendCStr ) == GBStringGetLength(appendString));
    
    
    assert( GBStringAppend(NULL , NULL)            == 0);
    assert( GBStringAppendCStr(appendString, NULL) == 0);
    assert( GBStringAppend(appendString, NULL)     == 0);
    assert( GBStringAppendCStr(NULL, "Hello")      == 0);
    assert( GBStringAppend(NULL, appendString)     == 0);
    
    GBRelease( appendCStr);
    GBRelease( appendString);
    GBRelease( worldString);
    
    const char strContent[] = "The quick brown fox jumps over the lazy dog";
    
    GBString* string1 = GBStringInitWithCStr(strContent);
    GBString* string2 = GBStringInitWithCStr(strContent);
    
    
    for (GBIndex i = 0; i<GBStringGetLength(string1);i++)
    {
        const char c1 = GBStringGetCharacterAt(string1, i);
        const char c2 = GBStringGetCharacterAt(string1, i);
        assert( c1 == c2);
        assert(c1 == strContent[i]);
    }
    
    GBString* string3 = NULL;
    GBString* string4 = NULL;
    
    assert(GBStringEquals(string1, string3) == 0);
    assert(GBStringEquals(string2, string4) == 0);
    assert(GBStringEquals(string3, string4) == 0);
    
    assert(GBStringEquals(string1, string1));
    assert(GBStringEquals(string2, string2));
    assert(GBStringEquals(string1, string2));
    assert(GBObjectEquals(string1, string2));
    
    GBRelease(string1);
    GBRelease(string2);
    
    assert( StrImpl_GetNumString() - numATomsAtStart == 0);
    
    
    {
        const char* t ="My funny Valentine...";
        
        
        GBSize count = 1000;
        GBArray* array = GBArrayInit();
        for (GBIndex i = 0;  i< count ; i++)
        {
            GBString* str = GBStringInitWithCStr(t);
            GBRef clone = GBObjectClone(str);
            GBArrayAddValue(array, clone);
            GBRelease(str);
            GBRelease(clone);
        }
        
        assert( StrImpl_GetNumString()- numATomsAtStart  == 1);
        
        GBRelease(array);
    }
    /* Test static GBStrings*/
    {
        
        const GBString* emptyStr = GBSTR("");
        assert(emptyStr);
        
        assert(GBStringIsValid(emptyStr));
        assert(GBStringIsEmpty(emptyStr));
        
        GBString* st = GBObjectClone(emptyStr);
        
        assert(st);
        const char *add = "this time we do real content";
        GBStringAppendCStr(st, add);
        assert(GBStringEqualsCStr(st, add));
        
        
        GBRelease(st);
        assert(GBObjectIsValid(emptyStr ));
        assert(GBStringIsValid(emptyStr ));
        assert(GBStringIsEmpty(emptyStr));
        
    }
    {
        GBString* s = GBStringInitWithCStr("Hello");
        
        assert( GBStringSetContentWithFormat(s , "") );
        assert(GBStringEqualsCStr(s, ""));
        
        GBRelease(s);
        
        s = GBStringInitWithFormat("");
        assert(s);
        assert(GBStringEqualsCStr(s, ""));
        assert( GBStringSetContentWithFormat(s , "") );
        assert(GBStringEqualsCStr(s, ""));
        
        GBRelease(s);
    }
    
}




void testGBStringStatic(  )
{
    printf("--------Test GBString Static --------\n");
    
    {
        
        const GBString* str1 = GBSTR("Hello");
        const GBString* str2 = GBSTR("Hello");
        assert(str1);
        assert(str2);
        
        assert(IsKindOfClass(str1, GBStringClass));
        assert(IsKindOfClass(str2, GBStringClass));
        
        assert( GBStringEquals(str1, str2));
        
        GBRelease(str1);
        assert(GBObjectIsValid(str1));
        assert(IsKindOfClass(str1, GBStringClass));
        
        GBString* newStr = GBObjectClone(str1);
        
        assert( GBStringEquals(str1, newStr));
        
        GBRelease(newStr);
        
        assert( GBStringEquals(str1, str2));
    }
    {
        //const GBString* str2 = GBSTR("Hello");
        
        GBDictionary* dict = GBDictionaryInit();
        
        const int size = 10;
        for (int i = 0; i < size ; i++)
        {
            const GBNumber* v1 = GBNumberInitWithInt( i);
            
            if( GBDictionaryAddValueForKey(dict, v1, GBSTR("k%i" , i)) == 0)
            {
                assert(0);
            }
            GBRelease(v1);
        }
        
        
        
        
        
        GBRelease(dict);
        
    }
    
}

