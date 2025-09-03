#pragma once

#include <memory>
#include <string>
#include <mcp/core/common.hpp>
#include <mcp/node/chain_state.hpp>
#include <mcp/core/block.hpp>
#include <mcp/core/transaction.hpp>

namespace mcp
{
    class Client;

    /**
     * @brief Intermediate interface for gathering blockchain data needed for transaction tracing
     * 
     * This class abstracts the blockchain data access and provides a clean interface
     * for debug_traceTransaction, decoupling it from direct database access.
     */
    class TraceDataProvider
    {
    public:
        /**
         * @brief Constructor that gathers all necessary data for tracing
         * @param client Shared pointer to the blockchain client
         * @param txHash Hash of the transaction to trace
         */
        TraceDataProvider(std::shared_ptr<Client> client, h256 const& txHash);

        /**
         * @brief Get the localised transaction data
         * @return Const reference to the localised transaction
         */
        LocalisedTransaction const& getTransaction() const { return m_transaction; }

        /**
         * @brief Get the block containing the transaction
         * @return Const reference to the block
         */
        Block const& getBlock() const { return m_block; }

        /**
         * @brief Create a properly initialized chain_state for tracing
         * @return A chain_state configured for tracing the transaction
         */
        chain_state createTraceState() const;

        /**
         * @brief Check if the data gathering was successful
         * @return True if all data is valid and ready for tracing
         */
        bool isValid() const { return m_isValid; }

        /**
         * @brief Get the error message if data gathering failed
         * @return String describing the error, empty if no error
         */
        std::string const& getErrorMessage() const { return m_errorMessage; }

        /**
         * @brief Get the client instance
         * @return Shared pointer to the client
         */
        std::shared_ptr<Client> getClient() const { return m_client; }

    private:
        std::shared_ptr<Client> m_client;
        LocalisedTransaction m_transaction;
        Block m_block;
        bool m_isValid;
        std::string m_errorMessage;

        /**
         * @brief Gather transaction and block data from the blockchain
         * @param txHash Hash of the transaction to gather data for
         */
        void gatherData(h256 const& txHash);

        /**
         * @brief Validate that all gathered data is consistent and complete
         */
        void validateData();

        /**
         * @brief Set error state with message
         * @param message Error message to set
         */
        void setError(std::string const& message);
    };
}