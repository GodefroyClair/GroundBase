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
 * \file GBJSON.h
 * \brief  Json parse/write C API. See GBJSON.hpp for C++ API
 */

#ifndef GBJSON_h
#define GBJSON_h

#ifdef __cplusplus
extern "C" {
#endif

#include <GBObject.h>

    
/*!
 * @discussion Tries to parse a JSON formatted string and create an appropriate GBObject in return.
 * @param buffer A JSON formatted string buffer. Will return NULL if buffer is NULL.
 * @param size the string size of the buffer parameter. Will return NULL if size is 0 or GBSizeInvalid.
 * @return a GBObject instance on success, NULL otherwise. Note : You must Release the created object.
 */
GBObject* GBJSONParse( const char* buffer , GBSize size);
    
/*!
 * @discussion Tries to generate a JSON formatted string from a GBObject instance.
 * @param objectToWrite The object to serialize. This is safe to pass a NULL pointer (NULL will be returned). Note that not all objects are serializable, only basic types and containers :  GBString GBNumber GBArray GBList GBSet GBDictionary. If you pass another GBObject type, an assertion will be triggered.
 * @return a head allocated string, or NULL on error. You must call `free` on the returned pointer to avoid memory leak.
 */
char* GBJSONWriteBuffer( GBRef objectToWrite );

#ifdef __cplusplus
}
#endif
#endif /* GBJSON_h */
