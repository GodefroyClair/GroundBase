//
//  testGBSocket.c
//  UnitTests
//
//  Created by Manuel Deneu on 06/12/2017.
//  Copyright © 2017 Manuel Deneu. All rights reserved.
//

#include <stdio.h>
#include <GBSocket.h>
#include "testGBSocket.h"


void testGBSocket()
{
    printf("--------Test GBSocket --------\n");
    
    
    /* Internet tests */
    
    assert( GBTCPSocketCreateListener( -1, 1, NULL) == NULL);
    
    GBFDSource* sock = GBTCPSocketCreateListener( 0, 1, NULL);
    assert(sock);
    GBRelease(sock);
    
    sock = GBTCPSocketCreateListener( 0, 0, NULL);
    assert(sock);
    GBRelease(sock);
    
    
    assert(GBTCPSocketConnectTo(NULL, 0, NULL)==NULL);
    assert(GBTCPSocketConnectTo("lol", 0, NULL)==NULL);
    assert(GBTCPSocketConnectTo("127.0.0.1", 0, NULL)==NULL);
    assert(GBTCPSocketConnectTo("127.0.0.1", -1, NULL)==NULL);
    
    
    
    assert(GBTCPSocketAccept( NULL , NULL , NULL , NULL) == NULL);
    
    /* NEEDS A RUN LOOP
    sock = GBTCPSocketCreateListener(0, 1, NULL);
    assert(sock);
    GBFDSource* inSock = GBTCPSocketAccept(sock, NULL, NULL, NULL);
    
    GBRelease(inSock);
    */
    
    
    /* Domain tests */
    

    assert(GBDomainCreateListener(NULL, 0, NULL ) == NULL);
    
    assert(GBDomainSocketConnectTo(NULL, NULL) == NULL);
    assert(GBDomainSocketConnectTo("lol", NULL) == NULL);
    
    
    assert(GBSocketWriteBlock(NULL, NULL, 0) == 0);
    const char b[2];
    assert(GBSocketWriteBlock(NULL, b, 0) == 0);
    assert(GBSocketWriteBlock(NULL, b, 2) == 0);
    
    assert(GBSocketReadBlock(NULL, NULL, 0) == 0);
    assert(GBSocketReadBlock(NULL, NULL, GBSizeInvalid) == 0);
    
}
