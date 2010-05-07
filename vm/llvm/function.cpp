#include "function.h"

#include <algorithm>
#include <iostream>

extern "C" {
	#include "../opcodes.h"
}

namespace {
	std::vector<unsigned int> get_block_addresses(NekoCodeChunk const & code_chunk) {
		std::vector<unsigned int> block_addresses;

		for (NekoCodeChunk::const_iterator it = code_chunk.begin(); it != code_chunk.end(); ++it) {
			switch (it->second.first) {
				case Jump:
				case JumpIf:
				case JumpIfNot:
					block_addresses.push_back(it->first);
					break;
				default:
					//do nothing
					;
			}
		}

		return block_addresses;
	}

	std::pair<const unsigned int, BasicBlock> make_block(NekoCodeChunk const & chunk) {
		return std::make_pair(chunk.getFromAddress(), BasicBlock(chunk));
	}

	Function::blocks_container get_blocks(NekoCodeChunk const & chunk) {
		std::vector<unsigned int> block_addresses = get_block_addresses(chunk);
		std::vector<NekoCodeChunk> chunks = chunk.splitByAddresses(block_addresses);

		Function::blocks_container result;

		std::transform(chunks.begin(), chunks.end(),
					   std::inserter(result, result.begin()),
					   std::ptr_fun(make_block));

		return result;
	}
}

Function::Function(NekoCodeChunk const & code_chunk) : blocks(get_blocks(code_chunk))
{}

void Function::neko_dump(std::string const & indent) const {
	std::cout << "{" << std::endl;
	std::cout << indent << "//Number of blocks: " << blocks.size() << std::endl;
	for (const_iterator it = begin();
		 it != end();
		 ++it) {
		std::cout << indent << it->first << " : ";
		it->second.neko_dump(indent);
	}
	std::cout << "}" << std::endl;
}
