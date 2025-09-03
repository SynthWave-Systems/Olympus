#include "TraceDataProvider.hpp"
#include <mcp/node/Client.hpp>
#include <mcp/common/Exceptions.h>

using namespace mcp;

TraceDataProvider::TraceDataProvider(std::shared_ptr<Client> client, h256 const& txHash)
    : m_client(client), m_isValid(false)
{
    if (!m_client)
    {
        setError("Client is null");
        return;
    }

    try
    {
        gatherData(txHash);
        validateData();
    }
    catch (std::exception const& e)
    {
        setError(std::string("Error gathering trace data: ") + e.what());
    }
    catch (...)
    {
        setError("Unknown error gathering trace data");
    }
}

void TraceDataProvider::gatherData(h256 const& txHash)
{
    // Get the localised transaction first
    m_transaction = m_client->localisedTransaction(txHash);
    
    // Get the block containing the transaction
    m_block = m_client->blockByHash(m_transaction.blockHash(), true);
}

void TraceDataProvider::validateData()
{
    // Validate transaction index is within block bounds
    if (m_transaction.transactionIndex() >= m_block.transactions().size())
    {
        setError("Transaction index is out of bounds for the block");
        return;
    }

    // Validate the transaction hash matches
    auto const& blockTx = m_block.transactions()[m_transaction.transactionIndex()];
    if (blockTx.sha3() != m_transaction.sha3())
    {
        setError("Transaction hash mismatch in block");
        return;
    }

    // If we get here, everything is valid
    m_isValid = true;
    m_errorMessage.clear();
}

chain_state TraceDataProvider::createTraceState() const
{
    if (!m_isValid)
    {
        throw std::runtime_error("Cannot create trace state: " + m_errorMessage);
    }

    // Create state with the block's database, using PreExisting base state
    // This ensures the state has proper blockchain context for tracers
    chain_state state(0, m_block.state().db(), BaseState::PreExisting);
    
    // Set the state root to before any transactions in this block
    state.setRoot(m_block.stateRootBeforeTx());
    
    // Execute all transactions up to (but not including) the target transaction
    // This sets up the proper state context for tracing the target transaction
    state.executeBlockTransactions(m_block, m_transaction.transactionIndex(), *m_client->blockChain().sealEngine());
    
    return state;
}

void TraceDataProvider::setError(std::string const& message)
{
    m_isValid = false;
    m_errorMessage = message;
}