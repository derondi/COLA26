#ifndef PAGE_H
#define PAGE_H

#include "ECC_Parameter_Set.h"
#include "FlashTypes.h"
#include <cstdint>

namespace NVM
{
namespace FlashMemory
{

struct PageMetadata
{
    // page_status_type Status;
    LPA_type LPA;
};

class Page
{
  public:
    Page()
    {
        // Metadata.Status = FREE_PAGE;
        Metadata.LPA = NO_LPA;
        // Metadata.SourceStreamID = NO_STREAM;

        LastWrittenTime = 0;
        RetentionTime = 0;
        Reads = ECC_Parameter_Set::ssd_init_read_times;
        
        // initial codeword type as 1kb, 2kb, 4kb, 8kb, 16kb
        SetCodewordType(static_cast<CodeWordType>(ECC_Parameter_Set::ssd_init_codeword_type_in_kb));
    };

    void SetCodewordType(CodeWordType type)
    {
        //CodewordType = type;
        switch (type) {
            case CodeWordType::_1kb:{
                //this->CodewordType = CodeWordType::_1kb;
                this->CodewordNumber = 16;
                break;
            }
            case CodeWordType::_2kb:{
                //this->CodewordType = CodeWordType::_2kb;
                this->CodewordNumber = 8;
                break;
            }  
            case CodeWordType::_4kb:{
                //this->CodewordType = CodeWordType::_4kb;
                this->CodewordNumber = 4;
                break;
            }
            case CodeWordType::_8kb:{
                //this->CodewordType = CodeWordType::_8kb;
                this->CodewordNumber = 2;
                break;
            }
            case CodeWordType::_16kb:{
                //this->CodewordType = CodeWordType::_16kb;
                this->CodewordNumber = 1;
                break;
            }
            default:
                assert(false && "Invalid CodeWordType");
                PRINT_ERROR("Invalid CodeWordType");
        }
        this->CodewordType = type;
    }

    // Last writtent time, the unit is nanosecond
    uint64_t LastWrittenTime;
    // Retention time, interval between this read command and the last write command, 
    // the unit is **days**
    double RetentionTime;
    // Read times 
    uint64_t Reads;
    // Codewords Type
    CodeWordType CodewordType;
    // Codewords Number
    uint64_t CodewordNumber;

    PageMetadata Metadata;

    void Write_metadata(const PageMetadata &metadata)
    {
        this->Metadata.LPA = metadata.LPA;
    }

    void Read_metadata(PageMetadata &metadata)
    {
        metadata.LPA = this->Metadata.LPA;
    }
};
} // namespace FlashMemory
} // namespace NVM

#endif // !PAGE_H
