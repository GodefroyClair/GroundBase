/*
 * Copyright (c) 2016-2018 FlyLab
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
//  GBObject.c
//  GroundBase
//
//  Created by Manuel Deneu on 27/08/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//



#include <string.h> // temp debug strcmp
#include <stdarg.h> // va_start/end
#include <pthread.h>

#include <GBObject.h>
#include "GBHash.h"
#include <GBString.h>
#include <GBNumber.h>
#include <GBArray.h>

#include "GBObject_Private.h"
#include "Private/Dictionary.h"

#include "GBAllocator.h"


#define USE_COMPILER_CONSTRUCTOR // See below for begin/end functions.
#define USE_CUSTOM_ALLOCATOR

static uint8_t debugInvalidRelease = 0;


static List* _rootObjects = NULL;
static GBSize _totalGBObjects = 0;
static pthread_mutex_t _rootMutex;

static Dictionary* _classesCount = NULL;

//#define USE_GBNUMBER_CACHE

#ifdef USE_GBNUMBER_CACHE

#ifndef SIZE_numbersCache
#define SIZE_numbersCache (size_t) 32
#endif // #ifndef SIZE_numbersCache
static void* numbersCache[SIZE_numbersCache] = {0};

#endif // USE_GBNUMBER_CACHE

static void Internal_InitRuntimeStack(void);
static void Internal_DeInitRuntimeStack(void);


#ifdef DEBUG
static int ClassesIterator( const Dictionary* dict, const char* key , void* value , void* context);
#endif


#ifdef USE_COMPILER_CONSTRUCTOR

static void begin (void) __attribute__((constructor( GBRUNTIME_CTOR_PRIORITY )));
static void end (void) __attribute__((destructor( GBRUNTIME_CTOR_PRIORITY )));
static void begin (void)
{
    Internal_InitRuntimeStack();
}
static void end (void)
{
    Internal_DeInitRuntimeStack();
}
#endif

static void Internal_InitRuntimeStack()
{

    pthread_mutex_init(& _rootMutex, NULL);
    
    _rootObjects = ListInit();
    
    DEBUG_ASSERT(_rootObjects);
    
    _classesCount = DictionaryInit();
    
    DEBUG_ASSERT( _classesCount);
    
}

// Note : Locked by GBRelease's call. This call is the only invokation of this method.
static void Internal_RemoveClassInstance( const GBObjectClass* class )
{
    DEBUG_ASSERT( class);
    
    if( DictionaryContainsKey(_classesCount, class->name))
    {
        GBSize *numInstances = DictionaryGetValueForKey( _classesCount, class->name);
        DEBUG_ASSERT(numInstances);
        
        (*numInstances)--;

        if( *numInstances == 0)
        {
            if( class->deInitialize)
            {
                class->deInitialize();
            }
            
            DictionaryRemoveKey( _classesCount, class->name);
            GBFree(numInstances);
        }
        
    }
}

// Note : Locked by GBRelease's call. This call is the only invokation of this method.
static void Internal_AddClassInstance( const GBObjectClass* class )
{
    //DEBUG_ASSERT( _classesCount);
    if( DictionaryContainsKey(_classesCount, class->name))
    {
        GBSize *numInstances = DictionaryGetValueForKey( _classesCount, class->name);
        DEBUG_ASSERT(numInstances);
        (*numInstances)++;
    }
    else // create entry
    {   
        GBSize* numInstances = GBMalloc(sizeof(GBSize));
        *numInstances = 1;
        
        DictionaryAddValueForKey(_classesCount, class->name, numInstances);
        
        if( class->initialize)
        {
            class->initialize();
        }
    }
}

#ifdef DEBUG
static int ClassesIterator( const Dictionary* dict, const char* key , void* value , void* context)
{
    UNUSED_PARAMETER(dict);
    UNUSED_PARAMETER(value);
    UNUSED_PARAMETER(context);
    DEBUG_ERR("Remains : %s \n" , key);
    
    return 1;
}

#endif
// Note : Locked by GBRelease's call. This call is the only invokation of this method during runtime.
// Also called from end function ( witch is a runtime destructor), With *NO LOCK* this time.
static void Internal_DeInitRuntimeStack()
{

#ifdef DEBUG
    if( DictionaryGetSize( _classesCount) != 0)
    {
        DEBUG_ERR("[Internal_DeInitRuntimeStack] some Objects are still in _classesCount (size %zi)  \n" , DictionaryGetSize( _classesCount));

        
        DictionaryIterateValues(_classesCount, ClassesIterator, NULL);
    }
#endif
//#warning assert is failing here...
    //DEBUG_ASSERT( DictionaryGetSize( _classesCount) == 0);
    
    ListFree(_rootObjects);
    _rootObjects = NULL;
    
    DictionaryFree( _classesCount );

    _classesCount = NULL;
    
    pthread_mutex_destroy(&_rootMutex);
    
#ifdef USE_GBNUMBER_CACHE
    for(size_t i=0;i < SIZE_numbersCache ; ++i)
    {
        if( numbersCache[i] != NULL)
            GBFree(numbersCache[i]);
    }
#endif
}


#ifdef USE_CUSTOM_ALLOCATOR
GB_HOT void* GBObjectAlloc( GBAllocator allocator , const void* _class, ...)
{
#ifndef USE_COMPILER_CONSTRUCTOR
    if( _totalGBObjects == 0)
    {
        Internal_InitRuntimeStack();
        
    }
#endif
    if(   allocator.Calloc == NULL
       || allocator.Free   == NULL
       || allocator.Malloc == NULL
       || allocator.Realloc == NULL
       )
    {
        return NULL;
    }
    
    const GBObjectClass * class = (const GBObjectClass * ) _class;
    
    DEBUG_ASSERT(class);
    
    if( class == NULL)
        return NULL;
    
    pthread_mutex_lock( &_rootMutex);
    Internal_AddClassInstance( class );
    pthread_mutex_unlock( &_rootMutex);
    
    void * p = NULL;
#ifdef USE_GBNUMBER_CACHE
    if( class == GBNumberClass)
    {
        for(size_t i=0;i < SIZE_numbersCache ; ++i)
        {
            if( numbersCache[i] != NULL)
            {
                p = numbersCache[i];
                numbersCache[i] = NULL;
                break;
            }
            //DEBUG_ASSERT( numbersCache[i] == NULL);
        }
    }
    
    if(p == NULL)
    {
#endif
        p = allocator.Calloc(1, class->size , &allocator);
#ifdef USE_GBNUMBER_CACHE
    }
#endif
    DEBUG_ASSERT( p );
    
    if(p == NULL)
    {
        return NULL;
    }
    
    GBObjectBase* base = (GBObjectBase*)p;
    memcpy(&base->_allocator, &allocator, sizeof(GBAllocator) );

    base->refCount = 1;
    base->state = GBObjectUninitialized;
    
    base->strongReferences = NULL;
    
    
    
    //pthread_mutex_init(&base->_lock , NULL);

    * (const GBObjectClass **) p = class;
    
    if (class->constructor)
    {
        va_list ap;
        va_start(ap, _class);
        void* pTemp = class->constructor(p, &ap );
        va_end(ap);
        
        
        if( pTemp )
        {

            DEBUG_ASSERT(IsKindOfClass(p, _class) && "[GBObjectAlloc] returned object from constructor is invalid, perhaps its not derived from GBObject. ");
            // just to be sure someone is not messing things up in ctor
            p = pTemp;
            
            base->state = GBObjectValid;
            
            if (   ( base->_allocator.Calloc != StaticStringAllocator.Calloc)
                && ( base->_allocator.Free   != StaticStringAllocator.Free)
                && ( base->_allocator.usrPtr != StaticStringAllocator.usrPtr)
               )
            {
                _totalGBObjects++;
                /* NEEDS LOCK */
                pthread_mutex_lock( &_rootMutex);
                ListAddValue(_rootObjects, p);
                pthread_mutex_unlock( &_rootMutex);
            }
            
            DEBUG_ASSERT(GBObjectIsValid(p));
        }
        else
        {
            allocator.Free(p , &allocator);
            p = NULL;
            
         //   _totalGBObjects--;
        }
    }
    
    
    
    return p;
}

#else // #ifdef USE_CUSTOM_ALLOCATOR

GB_HOT void *GBObjectAlloc(GBAllocator allocator ,const void * _class, ...)
{
    UNUSED_PARAMETER(allocator);
    
#ifndef Internal_InitRuntimeStack
    if( _totalGBObjects == 0)
    {
        Internal_InitRuntimeStack();
        
    }
#endif
    
    const GBObjectClass * class = (const GBObjectClass * ) _class;
    
    DEBUG_ASSERT(class);
    
    if( class == NULL)
        return NULL;
    
    pthread_mutex_lock( &_rootMutex);
    Internal_AddClassInstance( class );
    pthread_mutex_unlock( &_rootMutex);
    
    
    
    void * p = GBDefaultAllocator.Calloc(1, class->size);
    

    DEBUG_ASSERT( p );
    
    if(p == NULL)
    {
        return NULL;
    }
    
    GBObjectBase* base = (GBObjectBase*)p;
    memcpy(&base->_allocator, &GBDefaultAllocator, sizeof(GBAllocator) );
    
    base->refCount = 1;
    base->state = GBObjectUninitialized;

    base->strongReferences = NULL;
    
    base->_lock =(pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
    //pthread_mutex_init(&base->_lock , NULL);
    

    * (const GBObjectClass **) p = class;
    
    if (class->constructor)
    {
        _totalGBObjects++;
        
        va_list ap;
        va_start(ap, _class);
        void* pTemp = class->constructor(p, & ap);
        va_end(ap);
        
        
        if( pTemp )
        {
            DEBUG_ASSERT(IsKindOfClass(p, _class));
            // just to be sure someone is not messing things up in ctor
            p = pTemp;
            
            base->state = GBObjectValid;
            
            
            // NEEDS LOCK
            pthread_mutex_lock( &_rootMutex);
            ListAddValue(_rootObjects, p);
            pthread_mutex_unlock( &_rootMutex);
         
        }
        else
        {
            GBDefaultAllocator.Free( p );
            p = NULL;
            
            _totalGBObjects--;
        }
    }

    
    DEBUG_ASSERT(GBObjectIsValid(p));
    return p;

}
#endif
BOOLEAN_RETURN uint8_t GBObjectIsCloneable(GBRef object)
{
    const GBObjectClass * const * cp = object;
    
    return (*cp)->clone != NULL;
}

GBObject * GBObjectClone (GBRef object)
{
    if( object == NULL)
        return NULL;
    
    const GBObjectClass * const * cp = object;
    
    DEBUG_ASSERT(object && * cp && (* cp)->clone);
    
    return (*cp)->clone( object );
}

int GBObjectGetRefCount( GBRef object)
{
    const GBObjectBase *cp = object;
    if( cp)
    {
        return cp->refCount;
    }
    return -1;
}

int GBRetain( GBRef object)
{

    if( GBObjectIsValid(object))
    {

        GBObjectBase *cp = CONST_CAST(GBObjectBase *) object;
        ++cp->refCount;
        
        if(cp->class->retain)
            cp->class->retain(object);
        
        return cp->refCount;
    }
    
    return -1;
    
}

BOOLEAN_RETURN uint8_t GBRelease (GBRef object)
{   
    if( object == NULL)
    {
        DEBUG_ERR("[GBRelease] NULL GBObject!\n");
        
        DEBUG_ASSERT( debugInvalidRelease == 0);
        return 0;
    }

    if(GBObjectIsValid(object) == 0)
    {
        DEBUG_ERR("[GBRelease] Invalid GBObject!\n");
        DEBUG_ASSERT( debugInvalidRelease == 0);
        return 0;
    }

    const GBObjectClass ** cp = (const GBObjectClass **) object;
    
    GBObjectBase *objBase = CONST_CAST(GBObjectBase *)object;
    
    /*
        Static allocated GBStrings can't be released.
     */
    if( objBase->_allocator.Calloc == StaticStringAllocator.Calloc)
    {
        return 1;
    }
    
    const GBObjectClass *class = objBase->class;
    
    if (object && (*cp) && (*cp)->name && (*cp)->destructor)
    {
        --objBase->refCount;

        if( (*cp)->release)
        {
           (*cp)->release(object);
        }
        
        if( objBase->refCount < 0 )
        {
            fprintf(stderr,"[GBRelease] over released!\n");
            DEBUG_ASSERT( debugInvalidRelease == 0);
            return 0;
        }
        if( objBase->refCount == 0)
        {
            GBRef objectRet = (*cp)->destructor((void*) object );
            
            if( !objectRet)
            {
                DEBUG_ERR("Warning : Destructor for class %s returned NULL\n" , class->name);
            }
            
            if( objBase->strongReferences)
            {
                while (ArrayGetSize(objBase->strongReferences))
                {
                    GBRef c = ArrayGetValueAtIndex(objBase->strongReferences, 0);

                    if( GBObjectIsValid(c))
                    {
                        GBRelease(c);
                    }
                    
                    ArrayRemoveValueAtIndex(objBase->strongReferences, 0);

                }
                DEBUG_ASSERT(ArrayGetSize(objBase->strongReferences) == 0);
                ArrayFree(objBase->strongReferences);
                objBase->strongReferences = NULL;
                
                
            }

            /* LOCK */
            pthread_mutex_lock( &_rootMutex);
            
            if( ListRemove(_rootObjects, object) )
            {
                
                void* ptr = (void*) object;
                objBase->state = GBObjectFreed;
#ifdef USE_GBNUMBER_CACHE
                uint8_t cached = 0;
                if( IsKindOfClass(object, GBNumberClass))
                {
                    for( size_t i = 0 ; i< SIZE_numbersCache ; ++i)
                    {
                        if( numbersCache[i] == ptr)
                        {
                            cached = 1;
                            break;
                        }
                        if( numbersCache[i] == NULL)
                        {
                            numbersCache[i] = ptr;
                            memset(ptr, 0, class->size);
                            cached = 1;
                            break;
                        }
                    }
                }
                if( !cached)
                {
#endif
                    objBase->_allocator.Free( ptr , &objBase->_allocator );
#ifdef USE_GBNUMBER_CACHE
                }
#endif
                _totalGBObjects--;
                
                Internal_RemoveClassInstance( class );
                
#ifndef USE_COMPILER_CONSTRUCTOR
                if( _totalGBObjects == 0)
                {
                    Internal_DeInitRuntimeStack();
                }
#endif
            }
            pthread_mutex_unlock( &_rootMutex);
            /* ENDOF LOCK */
        }
        
        return 1;
    }
    
    return 0;
}

GBSize GBObjectGetSize( GBRef object )
{
    const GBObjectClass * const * cp = object;
    DEBUG_ASSERT(object && * cp);
    
    return (* cp)->size;
}

BOOLEAN_RETURN uint8_t GBObjectEquals( GBRef obj1 ,GBRef obj2 )
{
    if( obj1 == NULL || obj2 == NULL)
        return 0;
    
    if( obj1 == obj2)
        return 1;
    
    const GBObjectClass * const * cp = obj1;
    const GBObjectClass * const * cp2 = obj2;
    
    if(strcmp( (*cp)->name , (*cp2)->name ) != 0)
    {
        return 0;
    }
    
    DEBUG_ASSERT(obj1 && * cp && (* cp)->equals);
    
    return (* cp)->equals( obj1, obj2);
}

BOOLEAN_RETURN uint8_t IsKindOfClass( GBRef obj  , GBObjectClassRef _class)
{
    if( _class == NULL)
        return 0;
    
    const GBObjectClass * const * cp = obj;

    if( obj && cp)
    {
        return (*cp) == _class;
    }
    
    return 0;
}

GBRef GBObjectGetClassName( GBRef object)
{
    if( object == NULL)
        return NULL;
    
    const GBObjectClass * const * cp = object;
    
    if( object && cp )
    {
        return GBStringInitWithCStr( (*cp)->name );
    }
    
    return NULL;
}

const char* GBObjectGetClassNameC( GBRef object)
{
    if( object == NULL)
        return NULL;
    
    const GBObjectClass * const * cp = object;
    
    if( object && cp )
    {
        return (*cp)->name;
    }
    
    return NULL;
}

GBObjectClassRef GBObjectGetClass( GBRef object)
{
    if( object == NULL)
        return NULL;
    
    const GBObjectClass * const * cp = object;
    
    if( object && cp )
    {
        return (*cp);
    }
    
    return NULL;
}

BOOLEAN_RETURN uint8_t GBObjectIsValid(GBRef obj)
{
    const GBObjectBase* base = (const GBObjectBase* )obj;
    if( !base)
        return 0;
    
    if( !base->class)
    {
        return 0;
    }
    /*
    if( base->class->constructor == NULL)
        return 0;
    */
    return GBObjectGetState(obj) == GBObjectValid;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/*
    Log
*/
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GBObjectLog( GBRef object )
{
    if( object == NULL)
        return;
    
    const GBObjectState state = GBObjectGetState(object);
    if( state != GBObjectValid)
    {
        DEBUG_LOG("GBObject  %p INVALID OBJ  %i \n", object , state);
        return ;
    }
    const GBString *desc = GBObjectGetDescription(object);
    
    if( desc)
    {
        const GBObjectClass * const * cp = object;
        const GBObjectBase* objBase = object;
        
        printf("GBObject (class %s) %p RefCount %i: '%s'  \n",
               (*cp)->name,
               object,
               objBase->refCount ,
               GBStringGetCStr( desc )
               );
        GBRelease(desc);
    }
}

GBRef /* is GBString*/ GBObjectGetDescription( GBRef object)
{
    const GBObjectClass * const * cp = object;
    
    if( *cp && (* cp)->description)
    {
        return (* cp)->description(cp) ;
        
    }
    return NULL;
}

size_t GBObjectIntrospection( uint8_t log)
{
    if( log)
    {
        
        printf("######## GBObjectIntrospection #########\n");
        printf("Objects : %zu \n" , ListGetSize(_rootObjects));
        
        void* value = NULL;
        ListForEach( _rootObjects , value)
        //for ( GBIndex i = 0; i<  ListGetSize(_rootObjects);i++)
        {
            const GBObject* obj = value;
            
            if( obj != _rootObjects )
                GBObjectLog( obj );
        }

        printf("--- Bytes Allocated %zi / Freed %zi \n" , GBAllocatorGetTotalAllocatedCount() , GBAllocatorGetTotalFreedCount() );
        printf("########################################\n");
    }
    return ListGetSize(_rootObjects);
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

BOOLEAN_RETURN uint8_t GBObjectAddStrongReference( GBRef object ,  GBRef  child )
{
    DEBUG_ASSERT(GBObjectIsValid(object));
    DEBUG_ASSERT(GBObjectIsValid(child));
    
    GBObjectBase *objectBase = CONST_CAST(GBObjectBase *) object;
    
    const GBObjectBase *childBase = child;
    if( objectBase == NULL)
        return 0;

    if( childBase == NULL)
        return 0;
    
    if( objectBase->strongReferences == NULL)
    {
        objectBase->strongReferences = ArrayInit();
    }
    DEBUG_ASSERT(objectBase->strongReferences);
    
    if( ArrayContainsValue(objectBase->strongReferences, child))
    {
        return 0;
    }
    
    ArrayAddValue(objectBase->strongReferences, child);
    // prout prout prout 
    GBRetain(child);
    
    return 1;
}

GBObjectState GBObjectGetState(GBRef object)
{
    const GBObjectBase *objectBase =(const GBObjectBase *) object;
    
    return objectBase->state;
}
/*
pthread_mutex_t* GBObjectGetMutex( GBRef object)
{
    GBObjectBase *objectBase =( GBObjectBase *) object;
    return &(objectBase->_lock);
}
 */

GBHashCode GBHash(GBRef object)
{
    const GBRef t = GBObjectGetDescription(object);
    const GBHashCode c =  GBHashFunction((const char*) GBStringGetCStr( t ), GBStringGetLength(t));
    GBRelease(t);
    return c;
}
/*
BOOLEAN_RETURN uint8_t GBObjectCanBeSerialized(GBRef object)
{
    GBObjectBase *objectBase =(GBObjectBase *) object;
    
    if( objectBase->class->serialize)
    {
        return 1;
    }
    return 0;
    
}
*/

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/*
BOOLEAN_RETURN uint8_t  GBObjectLock( GBObject* object)
{
    if (object == NULL)
        return 0;
    
    GBObjectBase *objectBase = object;
    
    return pthread_mutex_lock( &objectBase->_lock ) == 0;
}
BOOLEAN_RETURN uint8_t  GBObjectTryLock( GBObject*  object)
{
    if (object == NULL)
        return 0;
    
    GBObjectBase *objectBase = object;
    
    return pthread_mutex_trylock( &objectBase->_lock ) == 0;
}
BOOLEAN_RETURN uint8_t  GBObjectUnlock( GBObject*  object)
{
    if (object == NULL)
        return 0;
    
    GBObjectBase *objectBase = object;
    
    return pthread_mutex_unlock( &objectBase->_lock ) == 0;
}
 */

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/*
    Debug
 */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

GBSize GBObjectGetObjectsCount()
{
    return _totalGBObjects;
}

void GBRuntimeEnableInvalidReleaseDebug( uint8_t state)
{
    debugInvalidRelease = state;
}
