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
//  GBUPC.h
//  GroundBase
//
//  Created by Manuel Deneu on 26/09/2017.
//  Copyright © 2017 Manuel Deneu. All rights reserved.
//


/**
 * \file GBUPC.h
 * \brief Descriptions shared between GBUPCService & GBUPCClient
 *
 * GBUPCMessage C definitions. See GBUPC.hpp for C++ API.
 */


#ifndef GBUPC_h
#define GBUPC_h

GB_BEGIN_DCL

#include <GBBinCoder.h>
#include <GBUPCEndPoint.h>
#include <GBCommons.h>

/*!
 * @discussion An opaque type representing a client, on GBUPCService's side. GBUPCClientProxy are lighweight, non permanent objects. The only thing guarranted to be persistant is the 'user context' accessed via GBUPCClientProxy[Set/Set]UserContext
 */
typedef struct _UPCClientProxy GBUPCClientProxy;

/* Represents a service, client side*/
/*!
 * @discussion An opaque type representing a service, on GBUPCClient's side.
 */
typedef struct _UPCServiceProxy GBUPCServiceProxy;

BOOLEAN_RETURN uint8_t GBUPCServiceProxyGetEndPoint( const GBUPCClientProxy* proxy, GBUPCEndPoint* endPoint);

#define PACKED_AND_ALIGNED __attribute__((packed)) __attribute__((aligned(4)))

//#define UPCMessageMaxDataSize (size_t) 512
#define UPCMessageMaxDataSize (size_t) 2048

typedef  int32_t MsgType;

struct _UPCMessage
{
    MsgType    mtype;
    uint32_t dataSize;
    
    const void* data;
    
} PACKED_AND_ALIGNED;

#define GBUPCMessageHeaderSize offsetof(GBUPCMessage , data)

/*!
 * @discussion Represents an instance that wraps a message passed between GBUPCServices and GBUPCClients.
 */
typedef struct _UPCMessage GBUPCMessage;

GB_ALWAYS_INLINE void GBUPCMessageInit( GBUPCMessage* msg)
{
    msg->dataSize = 0;
    msg->mtype = 0;
    msg->data = NULL;
}

// needs release!
GBBinCoder* GBUPCMessageReadObject( const GBUPCMessage* message);

GB_END_DCL
    
#endif /* GBUPC_h */
