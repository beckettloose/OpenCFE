#ifndef __D2_BUILDER_H
#define __D2_BUILDER_H

namespace Volvo {

class D2_Builder {
public:
    typedef enum OperationIdentifier {
        Unknown = 0x00,
        StopDiagSession = 0xA0,
        NoOperation = 0xA1,
        SecurityAccess= 0xA3,
        ReadDataByAddr = 0xA5,
        ReadDataByIdent = 0xA6,
        ReadDataByAddr2 = 0xA7, // TODO: how is this different than #1?
        StopData = 0xA9,
        DynamicRecords = 0xAA,
        ReadFreezeFrame = 0xAC,
        ReadDTC = 0xAE,
        ClearDTC = 0xAF,

        IOCtlByOffset = 0xB0,
        IOCtlByIdent = 0xB1,
        CtlRoutineByOffset = 0xB2,
        WriteDataBlockByOffset = 0xB8,
        ReadDataBlockByOffset = 0xB9,
        WriteDataBlockByAddr = 0xBA,
        ReadDataBlockByAddr = 0xBB
    } OperationIdentifier;
};
}

#endif // !__D2_BUILDER_H
