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
 * \file GBUPCService.hpp
 * \brief GB::UPC::Service C++ API. See GBUPCService.h for C API
 */

#ifndef GBUPCService_hpp
#define GBUPCService_hpp

#include <GBString.hpp>
#include <GBObject.hpp>
#include <GBUPCService.h>
#include <GBRunLoop.hpp>
#include <GBVariant.hpp>
#include <GBUPC.hpp>

namespace GB
{
    namespace UPC
    {
        class ClientProxy
        {
        public:
            friend class Service;
            
            bool getEndPoint( GBUPCEndPoint &endPoint) const
            {
                return GBUPCServiceProxyGetEndPoint(_proxy, &endPoint );
            }
            
        private:
            ClientProxy( const GBUPCClientProxy* cProxy):
            _proxy(cProxy)
            {}
            
           const GBUPCClientProxy *_proxy;
        };
        
        class Service;
        
        
        class ServiceDelegate
        {
        public:
            
            virtual void didReceiveData( GB::UPC::Service & , const GB::UPC::ClientProxy & , const GB::UPC::Message &  ) = 0;
            
            virtual bool connectionRequest( GB::UPC::Service & , const GB::UPC::ClientProxy &) = 0;
            
            virtual void clientDisconnected( GB::UPC::Service & , const GB::UPC::ClientProxy & , GBUPCDisconnectionReason) = 0;

            virtual ~ServiceDelegate(){}
            
        protected:
            ServiceDelegate()
            {}
            
        };
        
        /* **** **** **** **** **** **** **** **** **** **** **** **** **** */
        
        class Service : public GB::Object<GBUPCService>
        {
        public:

            Service( const std::string &name);

            Service& operator=(const Service &) GB_NO_EXCEPT = delete;
            
            void setDelegate( GB::UPC::ServiceDelegate* delegate)
            {
                _delegate = delegate;
            }
            
            bool setListeningPort( int port)
            {
                return GBUPCServiceSetListeningPort(getAs<GBUPCService>(), port);
            }
            
            bool isValid() const GB_NO_EXCEPT
            {
                return _ptr;
            }
            
            bool isRunning() const GB_NO_EXCEPT
            {
                return GBUPCServiceIsRunning( getAs<const GBUPCService>());
            }
            
            std::string getName() const GB_NO_EXCEPT 
            {
                return GBStringGetCStr( GBUPCServiceGetName( getAs<const GBUPCService>()) );
            }
            
            std::size_t getNumClients() const GB_NO_EXCEPT
            {
                return GBUPCServiceGetNumClients( getAs<const GBUPCService>() ) ;
            }
            
            // delegate must be set!
            // if you plan on using an external runloop, it should be set using `GBUPCServiceSetRunLoop` prior of calling Start.
            bool start()
            {
                if( _delegate == nullptr)
                    return false;
                return GBUPCServiceStart( getAs< GBUPCService >() );
            }
            
            bool closeAndRemoveClient( const GB::UPC::ClientProxy &proxy)
            {
                return GBUPCServiceCloseAndRemoveClient( getAs< GBUPCService >(), proxy._proxy);
            }
            
            bool sendMessage( const GB::UPC::ClientProxy &proxy , const GBUPCMessage* message)
            {
                return GBUPCServiceSendMessage( getAs< GBUPCService >(), proxy._proxy, message);
            }
            
            bool sendObject( const GB::UPC::ClientProxy &client , const GB::Variant& variant , MsgType messageType )
            {
                return sendObject(client, variant.getCObject(), messageType);
            }
            
            bool sendObject( const GB::UPC::ClientProxy &client, GBRef object , MsgType messageType)
            {
                return GBUPCServiceSendObject( getAs< GBUPCService >(), client._proxy, object, messageType);
            }
            
            bool broadcastMessage( const GBUPCMessage* message )
            {
                return GBUPCServiceBroadcastMessage( getAs< GBUPCService >(),  message);
            }
            
            bool broadcastObject( GBRef object , MsgType messageType)
            {
                return GBUPCServiceBroadcastObject( getAs< GBUPCService >(), object, messageType);
            }
            
            GB::RunLoop getRunLoop()
            {
                return GB::RunLoop( GBUPCServiceGetRunLoop( getAs< GBUPCService >() ) );
            }
            
        private:
            ServiceDelegate* _delegate;
        };
        
    } // end namespace UPC
    
} // end namespace GB


GB::UPC::Service::Service( const std::string &name):
Object( GBUPCServiceInitWithName(  GBStrMake( name) )),
_delegate(nullptr)
{
    
    // for now we must ensure GBUPCServiceInitWithName returned a valid instance.
    if( getAs<const GBUPCService>() == nullptr)
    {
        return ;
    }
    
    GBUPCServiceSetCallBacks(getAs<GBUPCService>(),
    {
        []( GBUPCService* service, const GBUPCClientProxy* client, const GBUPCMessage* data  )
        {
            GB::UPC::Service* self = reinterpret_cast<GB::UPC::Service*>( GBUPCServiceGetUserContext(service));
            
            if( self->_delegate)
            {
                GB::UPC::ClientProxy prox(client);
                
                const GB::UPC::Message msg{ data};
                self->_delegate->didReceiveData(*self, prox, msg );
            }
        },
        []( GBUPCService* service ,GBUPCClientProxy* client ) -> uint8_t
        {
            GB::UPC::Service* self = reinterpret_cast<GB::UPC::Service*>( GBUPCServiceGetUserContext(service));
            
            if( self->_delegate)
            {
                GB::UPC::ClientProxy prox(client);
                
                return self->_delegate->connectionRequest(*self, prox);
            }
            return 0;
        },
        [] ( GBUPCService* service , const GBUPCClientProxy* client , GBUPCDisconnectionReason reason )
        {
            GB::UPC::Service* self = reinterpret_cast<GB::UPC::Service*>( GBUPCServiceGetUserContext(service));
            
            if( self->_delegate)
            {
                GB::UPC::ClientProxy prox(client);
                
                self->_delegate->clientDisconnected(*self, prox, reason);
            }
        }
    });
    
    GBUPCServiceSetUserContext( getAs<GBUPCService>(), this);
    
}


#endif /* GBUPCService_hpp */
