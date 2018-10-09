//
//  DemoUPC.h
//  UPCDemoCPP
//
//  Created by Manuel Deneu on 08/12/2017.
//  Copyright © 2017 Manuel Deneu. All rights reserved.
//

#ifndef DemoUPC_h
#define DemoUPC_h

#define DEMO_UPC_SERVICE_NAME (const char*) "com.service.example"
#define DEMO_UPC_SERVICE_PORT (int) 12000


typedef enum
{
    DemoUPCCompute1 = 1,
    DemoUPCCompute1_Response = 2,
} DemoUPCMessageType;


#define VALUE_1_KEY (const char*) "Val1"
#define VALUE_2_KEY (const char*) "Val2"

#define RESPONSE_KEY    (const char*) "Response"
#define DATA_KEY        (const char*) "Data"

#define RESPONSE_OK        (const char*) "Ok"
#define RESPONSE_ERROR        (const char*) "Error"

#endif /* DemoUPC_h */
