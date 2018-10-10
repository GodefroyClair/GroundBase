/*
 * Copyright (c) 2017 FlyLab
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
//  GBUPCEndPoint.h
//  GroundBase
//
//  Created by Manuel Deneu on 01/10/2017.
//  Copyright © 2017 Manuel Deneu. All rights reserved.
//

/**
 * \file GBUPCEndPoint.h
 * \brief End point descriptions for GBUPCService/Client.
 */

#ifndef GBUPCEndPoint_h
#define GBUPCEndPoint_h



#include <GBString.h>
#include <string.h> //memset

GB_BEGIN_DCL

typedef enum
{
    GBUPCTypeDistant = 1,
    GBUPCTypeLocal   = 2
}GBUPCType;

struct GBUPCDistant
{
    const GBString* address;
    int   port;
};

struct GBUPCLocal
{
    const GBString* name;
};

/*!
 * @typedef GBUPCEndPoint
 * @brief Represents a connectable end point.
 * You should call `GBUPCEndPointRelease` on custom created instances
 */
typedef struct
{
    GBUPCType       connectionType;
    union
    {
        struct GBUPCLocal local;
        struct GBUPCDistant distant;
    } client;
    
}GBUPCEndPoint;

GB_ALWAYS_INLINE void GBUPCEndPointInit( GBUPCEndPoint* endpoint)
{
    memset(endpoint, 0, sizeof(GBUPCEndPoint));
}
GB_ALWAYS_INLINE void GBUPCEndPointRelease( GBUPCEndPoint* endpoint)
{    
    if(endpoint->connectionType == GBUPCTypeDistant )
    {
        GBRelease(endpoint->client.distant.address);
    }
    if( endpoint->connectionType == GBUPCTypeLocal)
    {
        GBRelease(endpoint->client.local.name);
    }
}

GB_ALWAYS_INLINE BOOLEAN_RETURN uint8_t GBUPCEndPointIsValid( const GBUPCEndPoint* endpoint)
{
    return    (   endpoint->connectionType == GBUPCTypeDistant
               && endpoint->client.distant.address
               && endpoint->client.distant.port > 0
               && !GBStringIsEmpty(endpoint->client.distant.address)
               )
           ||
              (endpoint->connectionType == GBUPCTypeLocal
               && endpoint->client.local.name
               && !GBStringIsEmpty( endpoint->client.local.name )
               );
}
    
GB_END_DCL

#endif /* GBUPCEndPoint_h */
