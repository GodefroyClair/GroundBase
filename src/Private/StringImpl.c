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
#include <string.h>

#include "StringImpl.h"
#include "../GBAllocator.h"

Array* atoms = NULL;


Array* StrImpl_getStringAtomsList()
{
    return atoms;
}


GBSize StrImpl_GetNumString(void)
{
    return ArrayGetSize( atoms);
}

BOOLEAN_RETURN uint8_t StrImpl_Initialize()
{
    DEBUG_ASSERT(atoms == NULL);
    if( atoms == NULL)
    {
        atoms = ArrayInit();
    }
    return atoms != NULL;
}

BOOLEAN_RETURN uint8_t StrImpl_Deinitialize()
{
    DEBUG_ASSERT( atoms != NULL);
    
    if( atoms)
    {
        ArrayFree(atoms);
        atoms = NULL;
    }
    
    return atoms == NULL;
}

BOOLEAN_RETURN GBIndex AtomExists(const char* text)
{
    
    for (GBIndex i = 0; i<  ArrayGetSize(atoms)  ; i++)
    {
        const struct StringImpl* a = ArrayGetValueAtIndex(atoms, i);
        
        if( a == NULL)
        {
            //"[GBString AtomExists] Found NULL Atom at index %zi \n"
            continue;
        }
        if( a->text == NULL)
        {
            //("[GBString AtomExists] error a->text is NULL (text is '%s') !\n" , text);
            return GBIndexInvalid;
        }
        if( a && strcmp( a->text , text ) == 0)
        {
            return i;
            break;
        }
    }
    
    return GBIndexInvalid;
}

BOOLEAN_RETURN uint8_t AtomRemove( struct StringImpl* atom)
{
    if( atom == NULL)
        return 0;
    
    if( atom->text == NULL)
        return 0;
    
    const GBIndex index  = AtomExists(atom->text);
    
    if( index != GBIndexInvalid )
    {
        struct StringImpl* atm = CONST_CAST(struct StringImpl*) ArrayGetValueAtIndex(atoms, index);
        DEBUG_ASSERT(atm);
        --atm->count;
        
        if(atm->count <= 0)
        {
            ArrayRemoveValueAtIndex(atoms , index);
            
            GBFree( atm->text );
            
            GBFree( atm );
            
            atm = NULL;
            return  1;
        }
        else
        {
            return 0;
        }
    }
    
    
    
    return 0;
}

void Internal_removeAllCharOccurences(char* str, char c)
{
    char *pr = str;
    char *pw = str;
    
    while (*pr)
    {
        *pw = *pr++;
        pw += (*pw != c);
    }
    *pw = '\0';
}
