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
//  GBProcessLock.h
//  GroundBase
//
//  Created by Manuel Deneu on 30/07/2016.
//

/**
 * \file GBProcessLock.h
 * \brief Process locking operations, usefull to prevent multiple instances of your program to run.
 * This API is deprecated and needs to be redesigned.
 */

#ifndef GBProcessLock_h
#define GBProcessLock_h

#ifdef __cplusplus
extern "C" {
#endif
    


#include <GBString.h>

/*!
 * @discussion Creates a named file lock that will hold until GBProcessUnLock is invoked.
 * @param pidFileHandler a pointer to a file handler that you should keep in order to call GBProcessUnLock. Returns 0 if NULL
 * @param name the name of the lock. Undefined behaviour if NULL
 * @return 1 on success, 0 otherwise.
 */
BOOLEAN_RETURN uint8_t GBProcessLock( int *pidFileHandler , const GBString* name ) GB_NO_NULL_ARGS(2,1) GB_DEPRECATED("5.5.2");
    
/*!
 * @discussion Unlocks a file, given its file handler.
 * @param pidFileHandler a pointer to a valid file handler.
 * @return 1 on success, 0 otherwise. In any case, the pointer value will be set to -1.
 */
BOOLEAN_RETURN uint8_t GBProcessUnLock( int *pidFileHandler ) GB_DEPRECATED("5.5.2");
    
#ifdef __cplusplus
}
#endif

#endif /* GBProcessLock_h */
