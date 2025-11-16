#include "Block.h"
#include "ECC_Parameter_Set.h"

namespace NVM
{
	namespace FlashMemory
	{
		Block::Block(unsigned int PagesNoPerBlock, flash_block_ID_type BlockID)
		{
			ID = BlockID;
			Pages = new Page[PagesNoPerBlock];
      EraseCount = ECC_Parameter_Set::ssd_init_pe_cycles;
		}

		Block::~Block()
		{
		delete[] Pages;
		}
	}
}
