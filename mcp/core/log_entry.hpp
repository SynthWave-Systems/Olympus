#pragma once

#include <libdevcore/FixedHash.h>
#include <libdevcore/RLP.h>
#include <libdevcore/Address.h>

using log_bloom = dev::h2048;
using log_blooms = std::vector<log_bloom>;
using namespace dev;

namespace mcp
{
struct log_entry
{
	log_entry() = default;
	explicit log_entry(RLP const& _r);
	log_entry(Address const& _address, h256s _topics, bytes _data):
		address(_address), topics(std::move(_topics)), data(std::move(_data))
	{}

	void streamRLP(RLPStream& _s) const;

	log_bloom bloom() const;

	Address address;
	h256s topics;
	bytes data;
};

using log_entries = std::vector<log_entry>;

struct localised_log_entry: public log_entry
{
	localised_log_entry() = default;
	explicit localised_log_entry(log_entry const& _le): log_entry(_le) {}

	localised_log_entry(
		log_entry const& _le,
		mcp::block_hash const& _blockHash,
		unsigned _blockNumber,
		h256 const& _transactionHash,
		unsigned _transactionIndex,
		unsigned _logIndex
	):
		log_entry(_le),
		blockHash(_blockHash),
		blockNumber(_blockNumber),
		transactionHash(_transactionHash),
		transactionIndex(_transactionIndex),
		logIndex(_logIndex)
	{}

	mcp::block_hash blockHash;
	unsigned blockNumber = 0;
	h256 transactionHash;
	unsigned transactionIndex = 0;
	unsigned logIndex = 0;
	bool Removed = false;
};

using localised_log_entries = std::vector<localised_log_entry>;

inline log_bloom bloom(log_entries const& _logs)
{
	log_bloom ret;
	for (auto const& l: _logs)
		ret |= l.bloom();
	return ret;
}

}
