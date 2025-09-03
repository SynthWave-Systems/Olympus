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
    // Basic validation - if we got a LocalisedTransaction from the client,
    // it means the transaction exists and is valid
    if (m_transaction.blockHash().isZero())
    {
        setError("Invalid transaction: no block hash");
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
    chain_state state(0, m_block.db(), BaseState::PreExisting);
    
    // Set the state root to the block's current state root
    // This provides the proper blockchain context for tracing
    state.setRoot(m_block.rootHash());
    
    return state;
}

void TraceDataProvider::setError(std::string const& message)
{
    m_isValid = false;
    m_errorMessage = message;
}