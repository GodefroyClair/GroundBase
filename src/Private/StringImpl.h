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

#ifndef StringImpl_h
#define StringImpl_h

#include <GBTypes.h>

#include "GBCommons.h"
#include "Array.h"

struct StringImpl
{
    char * text;
    int count;
};

//extern Array* atoms;

Array* StrImpl_getStringAtomsList( void );

BOOLEAN_RETURN uint8_t StrImpl_Initialize(void);
BOOLEAN_RETURN uint8_t StrImpl_Deinitialize(void);

BOOLEAN_RETURN GBIndex AtomExists(const char* text);
BOOLEAN_RETURN uint8_t AtomRemove( struct StringImpl* atom);

void Internal_removeAllCharOccurences(char* str, char c);

GBSize StrImpl_GetNumString(void);


#endif /* StringImpl_h */
