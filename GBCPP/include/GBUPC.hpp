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
 * \file GBUPC.hpp
 * \brief GB::UPC::Message C++ definitions. See GBUPC.h for C API
 */

#ifndef GBUPC_hpp
#define GBUPC_hpp

#include <GBUPC.h>

namespace GB
{
    /*!
     * @discussion UPC (Unified Process Communication)  Namespace for Client/Service/Message classes.
     */
    namespace UPC
    {
        /*!
         * @discussion Represents an instance that wraps a message passed between GBUPCServices and GBUPCClients.
         * This wrapper is here to provide the `decodeRootObject` method.
         */
        struct Message
        {
            /*!
             * @discussion The underlying C message.
             */
            const GBUPCMessage* data;
            
            /*!
             * @discussion Decodes the message and tries to construct a valid GB::Variant.
             * @param toVariant the variant to create.
             * @return true if the message contains a valid Variant, false otherwise. See `GBUPCMessageReadObject` and ` GBBinCoderDecodeRoot`.
             */
            bool decodeRootObject( GB::Variant& toVariant) const
            {
                GBBinCoder* coder = GBUPCMessageReadObject( data );
                if( coder)
                {
                    GBObject* root = GBBinCoderDecodeRoot(coder);
                    
                    toVariant = GB::Variant( root);
                    GBRelease(root);
                    GBRelease(coder);
                    return true;
                }
                
                return false;
            }
        };
        
        
        
    } // namespace UPC
    
} // namespace GB


#endif /* GBUPC_hpp */
