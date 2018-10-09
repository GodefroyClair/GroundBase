//
//  testSharedMemory.cpp
//  UnitTestsCPP
//
//  Created by Manuel Deneu on 23/11/2017.
//  Copyright © 2017 Manuel Deneu. All rights reserved.
//

#include <stdio.h>


#include <string.h>
#include "TestBase.hpp"

#include <GBSharedMemory.hpp>


bool testSharedMemory()
{
    {
        GB::SharedMemory::Reader reader("");
        assert(reader.isValid() == false);
        assert(reader.getReadHandle() == nullptr);
        
        GB::SharedMemory::Writer writer("" , 0);
        assert(writer.isValid() == false);
        assert(writer.getWriteHandle() == nullptr);
        
        
        GB::SharedMemory::Writer writer2("" , 120);
        assert(writer2.isValid() == false);
    }
    {
        const std::string validID = "UnitTestsCPP";
        const std::size_t numBytes = 512;
        
        GB::SharedMemory::Writer writer( validID , 0);
        assert(writer.isValid() == false);
        
        writer = GB::SharedMemory::Writer(validID , numBytes);
        assert(writer.isValid());
        assert(writer.getSize() == numBytes);
        
        assert(writer.getIdentifier() == validID );
        
        void* writeHandle = writer.getWriteHandle();
        assert(writeHandle);
        
        GB::SharedMemory::Reader reader(validID);
        GB::SharedMemory::Reader reader2(validID);
        
        assert(reader.isValid());
        assert(reader2.isValid());
        
        assert(reader.getIdentifier() == validID);
        assert(reader2.getIdentifier() == validID);
        
        const void* readHandle = reader.getReadHandle();
        const void* readHandle2 = reader.getReadHandle();
        assert( readHandle );
        assert( readHandle2 );
        
        
        for( int i =0;i<255;++i)
        {
            memset(writeHandle, i, numBytes);
        
            assert( bcmp(writeHandle, readHandle, numBytes) == 0);
            assert( bcmp(writeHandle, readHandle2, numBytes) == 0);
        }
        
    }
    return true;
}
