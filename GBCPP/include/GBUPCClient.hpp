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

/**
 * \file GBUPCClient.hpp
 * \brief GB::UPC::Client C++ API. See GBUPCClient.h for C API
 */

#ifndef GBUPCClient_hpp
#define GBUPCClient_hpp

#include <GBString.hpp>
#include <GBObject.hpp>
#include <GBUPCClient.h>
#include <GBRunLoop.hpp>
#include <GBVariant.hpp>
#include <GBUPC.hpp>

namespace GB
{
    namespace UPC
    {
        /* **** **** **** **** **** **** **** **** **** **** **** **** **** */
        
        class Client; // forward def
        
        class ClientDelegate
        {
        public:
            
            virtual void didReceiveData( GB::UPC::Client & , const GB::UPC::Message &message) = 0;
            
            virtual void receivedNotification( GB::UPC::Client & , GBUPCNotification) = 0;
            
            virtual ~ClientDelegate(){}
            
        protected:
            
            ClientDelegate(){}

        };
        
        /* **** **** **** **** **** **** **** **** **** **** **** **** **** */
        
        class Client : public GB::Object<GBUPCClient>
        {
        public:
            
            Client();
            
            void setDelegate( ClientDelegate* delegate)
            {
                _delegate = delegate;
            }
            
            bool connect( const std::string &domainName)
            {
                
                return GBUPCClientConnectToLocalEndPoint(  getAs<GBUPCClient >(), domainName.c_str() );
                /*
                GBUPCEndPoint endPoint;
                GBUPCEndPointInit( &endPoint);
                endPoint.connectionType = GBUPCTypeLocal;
                GB::Object<GBString> str( GBStringInitWithCStr(domainName.c_str()) );
                endPoint.client.local.name = str.getAs<GBString>() ;
                return GBUPCClientConnectToEndPoint( getAs<GBUPCClient >(), &endPoint);
                 */
            }
            
            bool connect( const std::string &ip , int port)
            {
                
                return GBUPCClientConnectToTCPEndPoint( getAs<GBUPCClient >() , ip.c_str() , port);
                /*
                GBUPCEndPoint endPoint;
                GBUPCEndPointInit( &endPoint);
                endPoint.connectionType = GBUPCTypeDistant;
                GB::Object<GBString> str( GBStringInitWithCStr(ip.c_str()) );
                endPoint.client.distant.address = str.getAs<GBString>() ;
                endPoint.client.distant.port = port;
                return GBUPCClientConnectToEndPoint( getAs<GBUPCClient >(), &endPoint);
                 */
            }
            
            bool disconnect()
            {
                return GBUPCClientDisconnect( getAs<GBUPCClient >() );
            }
            
            bool run()
            {
                return GBUPCClientRun( getAs<GBUPCClient >() );
            }
            
            bool isConnected() const GB_NO_EXCEPT
            {
                return GBUPCClientIsConnected( getAs<const GBUPCClient >() );
            }
            
            bool sendMessage(const GBUPCMessage* message)
            {
                return GBUPCClientSendMessage( getAs<GBUPCClient >()  , message );
            }
            
            bool sendObject( const GB::Variant &object , MsgType messageType)
            {
                return sendObject(object.getCObject(), messageType);
            }
            
            bool sendObject( GBRef object , MsgType messageType)
            {
                return GBUPCClientSendObject( getAs<GBUPCClient >(), object, messageType);
            }
            
            GB::RunLoop getRunLoop()
            {
                return GB::RunLoop( GBUPCClientGetRunLoop( getAs<GBUPCClient >() ));
            }
            
        private:
            ClientDelegate *_delegate;
            
        };
    } // end namespace UPC
    
} // end namespace GB


GB::UPC::Client::Client():
Object( GBUPCClientInit(
{
    [] ( GBUPCClient* client,  const GBUPCMessage* data  )
    {
        GB::UPC::Client* self = reinterpret_cast<GB::UPC::Client*>( GBUPCClientGetUserContext(client) );
        DEBUG_ASSERT(self);
        
        if( self && self->_delegate)
        {
            const GB::UPC::Message msg = {data};
            self->_delegate->didReceiveData( *self, msg);
        }
        else
        {
            DEBUG_LOG("Warning : no delegate set on GB::UPCClient %p\n" , (const void*) self);
        }
    },
    [] ( GBUPCClient* client , GBUPCNotification notification )
    {
        GB::UPC::Client* self = reinterpret_cast<GB::UPC::Client*>( GBUPCClientGetUserContext(client) );
        DEBUG_ASSERT(self);
        if( self && self->_delegate)
        {
            self->_delegate->receivedNotification(*self, notification);
        }
        else
        {
            DEBUG_LOG("Warning : no delegate set on GB::UPCClient %p\n" , (const void*) self);
        }
    }
})),

_delegate(nullptr)
{
    GBUPCClientSetUserContext( getAs<GBUPCClient >(), this);
}

#endif /* GBUPCClient_hpp */
