/*
 * Copyright (c) 2016-2017 FlyLab
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
//  GBBinCoder.h
//  GroundBase
//
//  Created by Manuel Deneu on 19/09/2016.
//  Copyright © 2016 Manuel Deneu. All rights reserved.
//

/**
 * \file GBBinCoder.h
 * \brief  GBBinCoder is a serialization/deserialization primarily class designed for networking communications.
 */

#ifndef GBBinCoder_h
#define GBBinCoder_h

#ifdef __cplusplus
extern "C" {
#endif


#include <GBObject.h>
#include <GBString.h>
#include <GBNumber.h>
#include <GBSequence.h>

extern GBObjectClassRef GBBinCoderClass;
#define GBBinCoderClassName (const char*) "GBBinCoder"


/*!
 * @discussion An opaque GBBinCoder instance.
 */
typedef struct _GBBinCoder GBBinCoder;

/*!
 * @discussion Initialize an empty GBBinCoder instance. You own the returned object. See GBObject ownership notes.
 * @return an empty GBBinCoder instance
 */
GBBinCoder* GBBinCoderInit(void);
/*
 Will deep copy the buffer so the storage will stick around
 */
GBBinCoder* GBBinCoderInitByCopyingContent(const void* buf , GBSize bufLength);
/*
 Will _STILL_ use the underlying buffer passed( buf). To perform a copy of the buffer, use GBBinCoderInitByCopyingContent.
 */
GBBinCoder* GBBinCoderInitWithContent(const void* buf );

GBBinCoder* GBBinCoderInitWithRootObject( GBRef object);

// needs release
// You should test the GBObject returned type, to prevent any arbitrary code execution!
GBObject* GBBinCoderDecodeRoot( const GBBinCoder* decoder);
    
GBObject* GBBinCoderDecodeRootWithType( const GBBinCoder* decoder, GBObjectClassRef classType);

/*
     Encode methods
 Will return 0 if :
     - val and/or key is Null or empty
     - key is already added
 
Values & keys passed to Encode* methods are not retained !
 */

BOOLEAN_RETURN uint8_t GBBinCoderEncodeString( GBBinCoder* coder, const GBString* value ,const GBString* key );
BOOLEAN_RETURN uint8_t GBBinCoderEncodeNumber( GBBinCoder* coder, const GBNumber* value ,const GBString* key );
BOOLEAN_RETURN uint8_t GBBinCoderEncodeSequence( GBBinCoder* coder, const GBSequence* value ,const GBString* key );

/*
     Decode methods
 returned values must be released!
 */

GBString* GBBinCoderDecodeString( const GBBinCoder* coder ,const GBString* key );
GBNumber* GBBinCoderDecodeNumber( const GBBinCoder* coder ,const GBString* key );
GBSequence* GBBinCoderDecodeSequence( const GBBinCoder* coder ,const GBString* key );

// default implementation might return a GBArray. But it's likely to change in the future. To be sure about the returned container type, use 'GBBinCoderDecodeContainerWithOuputType' bellow.
GBContainer* GBBinCoderDecodeContainer( const GBBinCoder* coder ,const GBString* key );

// See  `GBContainerInitWithType` for informations about supported container classes.
GBContainer* GBBinCoderDecodeContainerWithOuputType( const GBBinCoder* coder ,const GBString* key , GBObjectClassRef classType );


GB_NO_NULL_POINTERS GBSize GBBinCoderGetBufferSize( const GBBinCoder* coder);

BOOLEAN_RETURN uint8_t GBBinCoderWriteBuffer( const GBBinCoder* coder , char*bufferOut);
const uint8_t* GBBinCoderGetBuffer( const GBBinCoder* coder);

    
#ifdef __cplusplus
}
#endif

    
#endif /* GBBinCoder_h */
