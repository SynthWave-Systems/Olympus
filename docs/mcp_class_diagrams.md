# MCP Class Diagram Overview

This document lists every class defined under `mcp/` and visualizes the inheritance relationships per module.

## Application module

| Class | Defined in | Inherits from |
| --- | --- | --- |
| `vm_instance` | `application/cmdline.hpp` | `` |
| `daemon` | `application/daemon.hpp` | `` |
| `daemon_config` | `application/daemon.hpp` | `` |
| `thread_config` | `application/daemon.hpp` | `` |
| `thread_runner` | `application/daemon.hpp` | `` |

```mermaid
classDiagram
    class application_application_cmdline_hpp_vm_instance as "vm_instance"
    class application_application_daemon_hpp_daemon as "daemon"
    class application_application_daemon_hpp_daemon_config as "daemon_config"
    class application_application_daemon_hpp_thread_config as "thread_config"
    class application_application_daemon_hpp_thread_runner as "thread_runner"
```

## Common module

| Class | Defined in | Inherits from |
| --- | --- | --- |
| `CodeSizeCache` | `common/CodeSizeCache.h` | `` |
| `alarm` | `common/alarm.hpp` | `` |
| `operation` | `common/alarm.hpp` | `` |
| `async_task` | `common/async_task.hpp` | `` |
| `Signal` | `common/common.hpp` | `` |
| `error_message` | `common/common.hpp` | `` |
| `fast_steady_clock` | `common/common.hpp` | `` |
| `mru_list` | `common/common.hpp` | `` |
| `null_lock` | `common/common.hpp` | `` |
| `fifo_map_compare` | `common/fifo_map.hpp` | `` |
| `key_content` | `common/key_store.hpp` | `` |
| `key_store` | `common/key_store.hpp` | `` |
| `value_previous_work` | `common/key_store.hpp` | `` |
| `log` | `common/log.hpp` | `` |
| `logging` | `common/log.hpp` | `` |
| `Cache` | `common/lruc_cache.hpp` | `` |
| `KeyNotFound` | `common/lruc_cache.hpp` | `std::invalid_argument` |
| `NullLock` | `common/lruc_cache.hpp` | `` |
| `stopwatch` | `common/stopwatch.hpp` | `` |
| `stopwatch_guard` | `common/stopwatch.hpp` | `` |
| `stopwatch_manager` | `common/stopwatch.hpp` | `` |

```mermaid
classDiagram
    class common_common_CodeSizeCache_h_CodeSizeCache as "CodeSizeCache"
    class common_common_alarm_hpp_alarm as "alarm"
    class common_common_alarm_hpp_operation as "operation"
    class common_common_async_task_hpp_async_task as "async_task"
    class common_common_common_hpp_Signal as "Signal"
    class common_common_common_hpp_error_message as "error_message"
    class common_common_common_hpp_fast_steady_clock as "fast_steady_clock"
    class common_common_common_hpp_mru_list as "mru_list"
    class common_common_common_hpp_null_lock as "null_lock"
    class common_common_fifo_map_hpp_fifo_map_compare as "fifo_map_compare"
    class common_common_key_store_hpp_key_content as "key_content"
    class common_common_key_store_hpp_key_store as "key_store"
    class common_common_key_store_hpp_value_previous_work as "value_previous_work"
    class common_common_log_hpp_log as "log"
    class common_common_log_hpp_logging as "logging"
    class common_common_lruc_cache_hpp_Cache as "Cache"
    class common_common_lruc_cache_hpp_KeyNotFound as "KeyNotFound"
    class common_common_lruc_cache_hpp_NullLock as "NullLock"
    class common_common_stopwatch_hpp_stopwatch as "stopwatch"
    class common_common_stopwatch_hpp_stopwatch_guard as "stopwatch_guard"
    class common_common_stopwatch_hpp_stopwatch_manager as "stopwatch_manager"
    class external_std__invalid_argument as "std::invalid_argument"
    external_std__invalid_argument <|-- common_common_lruc_cache_hpp_KeyNotFound
```

## Consensus module

| Class | Defined in | Inherits from |
| --- | --- | --- |
| `ledger` | `consensus/ledger.hpp` | `` |
| `base_validate_result` | `consensus/validation.hpp` | `` |
| `validate_result` | `consensus/validation.hpp` | `` |
| `validation` | `consensus/validation.hpp` | `` |

```mermaid
classDiagram
    class consensus_consensus_ledger_hpp_ledger as "ledger"
    class consensus_consensus_validation_hpp_base_validate_result as "base_validate_result"
    class consensus_consensus_validation_hpp_validate_result as "validate_result"
    class consensus_consensus_validation_hpp_validation as "validation"
```

## Core module

| Class | Defined in | Inherits from |
| --- | --- | --- |
| `LogFilter` | `core/LogFilter.hpp` | `` |
| `PrecompiledContract` | `core/Precompiled.h` | `` |
| `PrecompiledRegistrar` | `core/Precompiled.h` | `` |
| `SealEngineFace` | `core/SealEngine.h` | `` |
| `approve` | `core/approve.hpp` | `` |
| `ApproveReceipt` | `core/approve_receipt.hpp` | `` |
| `block_cache` | `core/block_cache.hpp` | `mcp::iblock_cache` |
| `iblock_cache` | `core/block_cache.hpp` | `` |
| `block_store` | `core/block_store.hpp` | `` |
| `LocalisedBlock` | `core/blocks.hpp` | `block` |
| `block` | `core/blocks.hpp` | `` |
| `account_state` | `core/common.hpp` | `` |
| `advance_info` | `core/common.hpp` | `` |
| `block_child_key` | `core/common.hpp` | `` |
| `block_state` | `core/common.hpp` | `` |
| `dag_account_info` | `core/common.hpp` | `` |
| `epoch_approves_key` | `core/common.hpp` | `` |
| `free_key` | `core/common.hpp` | `` |
| `hash_tree_info` | `core/common.hpp` | `` |
| `min_wl_result` | `core/common.hpp` | `` |
| `skiplist_info` | `core/common.hpp` | `` |
| `summary` | `core/common.hpp` | `` |
| `block_param` | `core/config.hpp` | `` |
| `witness_param` | `core/config.hpp` | `` |
| `MainContractCaller` | `core/contract.hpp` | `` |
| `genesis` | `core/genesis.hpp` | `` |
| `graph` | `core/graph.hpp` | `` |
| `iApproveQueue` | `core/iapprove_queue.hpp` | `` |
| `param` | `core/param.hpp` | `ChainOperationParams` |
| `timeout_db_transaction` | `core/timeout_db_transaction.hpp` | `` |
| `LocalisedTransaction` | `core/transaction.hpp` | `Transaction` |
| `Transaction` | `core/transaction.hpp` | `` |
| `iTransactionQueue` | `core/transaction_queue.hpp` | `` |
| `LocalisedTransactionReceipt` | `core/transaction_receipt.hpp` | `TransactionReceipt` |
| `TransactionReceipt` | `core/transaction_receipt.hpp` | `` |

```mermaid
classDiagram
    class core_core_LogFilter_hpp_LogFilter as "LogFilter"
    class core_core_Precompiled_h_PrecompiledContract as "PrecompiledContract"
    class core_core_Precompiled_h_PrecompiledRegistrar as "PrecompiledRegistrar"
    class core_core_SealEngine_h_SealEngineFace as "SealEngineFace"
    class core_core_approve_hpp_approve as "approve"
    class core_core_approve_receipt_hpp_ApproveReceipt as "ApproveReceipt"
    class core_core_block_cache_hpp_block_cache as "block_cache"
    class core_core_block_cache_hpp_iblock_cache as "iblock_cache"
    class core_core_block_store_hpp_block_store as "block_store"
    class core_core_blocks_hpp_LocalisedBlock as "LocalisedBlock"
    class core_core_blocks_hpp_block as "block"
    class core_core_common_hpp_account_state as "account_state"
    class core_core_common_hpp_advance_info as "advance_info"
    class core_core_common_hpp_block_child_key as "block_child_key"
    class core_core_common_hpp_block_state as "block_state"
    class core_core_common_hpp_dag_account_info as "dag_account_info"
    class core_core_common_hpp_epoch_approves_key as "epoch_approves_key"
    class core_core_common_hpp_free_key as "free_key"
    class core_core_common_hpp_hash_tree_info as "hash_tree_info"
    class core_core_common_hpp_min_wl_result as "min_wl_result"
    class core_core_common_hpp_skiplist_info as "skiplist_info"
    class core_core_common_hpp_summary as "summary"
    class core_core_config_hpp_block_param as "block_param"
    class core_core_config_hpp_witness_param as "witness_param"
    class core_core_contract_hpp_MainContractCaller as "MainContractCaller"
    class core_core_genesis_hpp_genesis as "genesis"
    class core_core_graph_hpp_graph as "graph"
    class core_core_iapprove_queue_hpp_iApproveQueue as "iApproveQueue"
    class core_core_param_hpp_param as "param"
    class core_core_timeout_db_transaction_hpp_timeout_db_transaction as "timeout_db_transaction"
    class core_core_transaction_hpp_LocalisedTransaction as "LocalisedTransaction"
    class core_core_transaction_hpp_Transaction as "Transaction"
    class core_core_transaction_queue_hpp_iTransactionQueue as "iTransactionQueue"
    class core_core_transaction_receipt_hpp_LocalisedTransactionReceipt as "LocalisedTransactionReceipt"
    class core_core_transaction_receipt_hpp_TransactionReceipt as "TransactionReceipt"
    core_core_block_cache_hpp_iblock_cache <|-- core_core_block_cache_hpp_block_cache
    core_core_blocks_hpp_block <|-- core_core_blocks_hpp_LocalisedBlock
    class external_ChainOperationParams as "ChainOperationParams"
    external_ChainOperationParams <|-- core_core_param_hpp_param
    core_core_transaction_hpp_Transaction <|-- core_core_transaction_hpp_LocalisedTransaction
    core_core_transaction_receipt_hpp_TransactionReceipt <|-- core_core_transaction_receipt_hpp_LocalisedTransactionReceipt
```

## Db module

| Class | Defined in | Inherits from |
| --- | --- | --- |
| `database_config` | `db/common.hpp` | `` |
| `UInt64SafeOperator` | `db/counter.hpp` | `rocksdb::AssociativeMergeOperator` |
| `RocksDBWriteBatch` | `db/database.cpp` | `WriteBatchFace` |
| `database` | `db/database.hpp` | `DatabaseFace` |
| `backward_iterator` | `db/db_iterator.hpp` | `db_iterator` |
| `db_iterator` | `db/db_iterator.hpp` | `` |
| `forward_iterator` | `db/db_iterator.hpp` | `db_iterator` |
| `db_transaction` | `db/db_transaction.hpp` | `` |

```mermaid
classDiagram
    class db_db_common_hpp_database_config as "database_config"
    class db_db_counter_hpp_UInt64SafeOperator as "UInt64SafeOperator"
    class db_db_database_cpp_RocksDBWriteBatch as "RocksDBWriteBatch"
    class db_db_database_hpp_database as "database"
    class db_db_db_iterator_hpp_backward_iterator as "backward_iterator"
    class db_db_db_iterator_hpp_db_iterator as "db_iterator"
    class db_db_db_iterator_hpp_forward_iterator as "forward_iterator"
    class db_db_db_transaction_hpp_db_transaction as "db_transaction"
    class external_rocksdb__AssociativeMergeOperator as "rocksdb::AssociativeMergeOperator"
    external_rocksdb__AssociativeMergeOperator <|-- db_db_counter_hpp_UInt64SafeOperator
    class external_WriteBatchFace as "WriteBatchFace"
    external_WriteBatchFace <|-- db_db_database_cpp_RocksDBWriteBatch
    class external_DatabaseFace as "DatabaseFace"
    external_DatabaseFace <|-- db_db_database_hpp_database
    db_db_db_iterator_hpp_db_iterator <|-- db_db_db_iterator_hpp_backward_iterator
    db_db_db_iterator_hpp_db_iterator <|-- db_db_db_iterator_hpp_forward_iterator
```

## Node module

| Class | Defined in | Inherits from |
| --- | --- | --- |
| `Block` | `node/Block.hpp` | `` |
| `TransactionReceiptPlaceholder` | `node/Block.hpp` | `dev::eth::TransactionReceipt` |
| `Client` | `node/Client.hpp` | `` |
| `ApproveQueue` | `node/approve_queue.hpp` | `iApproveQueue` |
| `block_arrival` | `node/arrival.hpp` | `` |
| `block_arrival_info` | `node/arrival.hpp` | `` |
| `block_pool` | `node/block_pool.hpp` | `` |
| `light_queue_item` | `node/block_pool.hpp` | `` |
| `missing_item` | `node/block_pool.hpp` | `` |
| `block_processor` | `node/block_processor.hpp` | `` |
| `late_message_cache` | `node/block_processor.hpp` | `` |
| `late_message_info` | `node/block_processor.hpp` | `` |
| `Statistics` | `node/chain.hpp` | `` |
| `chain` | `node/chain.hpp` | `std::enable_shared_from_this<mcp::chain>` |
| `chain_state` | `node/chain_state.hpp` | `` |
| `capability_metrics` | `node/common.hpp` | `` |
| `requesting_item` | `node/common.hpp` | `` |
| `composer` | `node/composer.hpp` | `` |
| `StandardTrace` | `node/debug.hpp` | `` |
| `Executive` | `node/evm/Executive.hpp` | `` |
| `ExtVM` | `node/evm/ExtVM.h` | `ExtVMFace` |
| `log` | `node/log.hpp` | `` |
| `approve_request_message` | `node/message.hpp` | `` |
| `block_processor_item` | `node/message.hpp` | `` |
| `catchup_request_message` | `node/message.hpp` | `` |
| `catchup_response_message` | `node/message.hpp` | `` |
| `hash_tree_request_message` | `node/message.hpp` | `` |
| `hash_tree_response_message` | `node/message.hpp` | `` |
| `joint_message` | `node/message.hpp` | `` |
| `joint_request_message` | `node/message.hpp` | `` |
| `peer_info_message` | `node/message.hpp` | `` |
| `peer_info_request_message` | `node/message.hpp` | `` |
| `summary_items` | `node/message.hpp` | `` |
| `transaction_request_message` | `node/message.hpp` | `` |
| `validate_status` | `node/message.hpp` | `` |
| `local_remote_ack_hello` | `node/node_capability.hpp` | `` |
| `node_capability` | `node/node_capability.hpp` | `p2p::icapability` |
| `peer_info` | `node/node_capability.hpp` | `` |
| `process_block_cache` | `node/process_block_cache.hpp` | `mcp::iblock_cache` |
| `put_item` | `node/process_block_cache.hpp` | `` |
| `requesting_mageger` | `node/requesting.hpp` | `` |
| `node_sync` | `node/sync.hpp` | `` |
| `sync_info` | `node/sync.hpp` | `` |
| `sync_request_status` | `node/sync.hpp` | `` |
| `FourByteTracer` | `node/tracers/4byte.hpp` | `Tracer` |
| `CallTracer` | `node/tracers/Call.hpp` | `Tracer` |
| `OpCode` | `node/tracers/OpCode.hpp` | `Tracer` |
| `PreStateTracer` | `node/tracers/PreState.hpp` | `Tracer` |
| `Tracer` | `node/tracers/Tracer.hpp` | `dev::eth::EVMLogger` |
| `TransactionQueue` | `node/transaction_queue.hpp` | `iTransactionQueue` |
| `unhandle_cache` | `node/unhandle.hpp` | `` |
| `unhandle_item` | `node/unhandle.hpp` | `` |
| `witness` | `node/witness.hpp` | `std::enable_shared_from_this<mcp::witness>` |
| `witness_config` | `node/witness.hpp` | `` |

```mermaid
classDiagram
    class node_node_Block_hpp_Block as "Block"
    class node_node_Block_hpp_TransactionReceiptPlaceholder as "TransactionReceiptPlaceholder"
    class node_node_Client_hpp_Client as "Client"
    class node_node_approve_queue_hpp_ApproveQueue as "ApproveQueue"
    class node_node_arrival_hpp_block_arrival as "block_arrival"
    class node_node_arrival_hpp_block_arrival_info as "block_arrival_info"
    class node_node_block_pool_hpp_block_pool as "block_pool"
    class node_node_block_pool_hpp_light_queue_item as "light_queue_item"
    class node_node_block_pool_hpp_missing_item as "missing_item"
    class node_node_block_processor_hpp_block_processor as "block_processor"
    class node_node_block_processor_hpp_late_message_cache as "late_message_cache"
    class node_node_block_processor_hpp_late_message_info as "late_message_info"
    class node_node_chain_hpp_Statistics as "Statistics"
    class node_node_chain_hpp_chain as "chain"
    class node_node_chain_state_hpp_chain_state as "chain_state"
    class node_node_common_hpp_capability_metrics as "capability_metrics"
    class node_node_common_hpp_requesting_item as "requesting_item"
    class node_node_composer_hpp_composer as "composer"
    class node_node_debug_hpp_StandardTrace as "StandardTrace"
    class node_node_evm_Executive_hpp_Executive as "Executive"
    class node_node_evm_ExtVM_h_ExtVM as "ExtVM"
    class node_node_log_hpp_log as "log"
    class node_node_message_hpp_approve_request_message as "approve_request_message"
    class node_node_message_hpp_block_processor_item as "block_processor_item"
    class node_node_message_hpp_catchup_request_message as "catchup_request_message"
    class node_node_message_hpp_catchup_response_message as "catchup_response_message"
    class node_node_message_hpp_hash_tree_request_message as "hash_tree_request_message"
    class node_node_message_hpp_hash_tree_response_message as "hash_tree_response_message"
    class node_node_message_hpp_joint_message as "joint_message"
    class node_node_message_hpp_joint_request_message as "joint_request_message"
    class node_node_message_hpp_peer_info_message as "peer_info_message"
    class node_node_message_hpp_peer_info_request_message as "peer_info_request_message"
    class node_node_message_hpp_summary_items as "summary_items"
    class node_node_message_hpp_transaction_request_message as "transaction_request_message"
    class node_node_message_hpp_validate_status as "validate_status"
    class node_node_node_capability_hpp_local_remote_ack_hello as "local_remote_ack_hello"
    class node_node_node_capability_hpp_node_capability as "node_capability"
    class node_node_node_capability_hpp_peer_info as "peer_info"
    class node_node_process_block_cache_hpp_process_block_cache as "process_block_cache"
    class node_node_process_block_cache_hpp_put_item as "put_item"
    class node_node_requesting_hpp_requesting_mageger as "requesting_mageger"
    class node_node_sync_hpp_node_sync as "node_sync"
    class node_node_sync_hpp_sync_info as "sync_info"
    class node_node_sync_hpp_sync_request_status as "sync_request_status"
    class node_node_tracers_4byte_hpp_FourByteTracer as "FourByteTracer"
    class node_node_tracers_Call_hpp_CallTracer as "CallTracer"
    class node_node_tracers_OpCode_hpp_OpCode as "OpCode"
    class node_node_tracers_PreState_hpp_PreStateTracer as "PreStateTracer"
    class node_node_tracers_Tracer_hpp_Tracer as "Tracer"
    class node_node_transaction_queue_hpp_TransactionQueue as "TransactionQueue"
    class node_node_unhandle_hpp_unhandle_cache as "unhandle_cache"
    class node_node_unhandle_hpp_unhandle_item as "unhandle_item"
    class node_node_witness_hpp_witness as "witness"
    class node_node_witness_hpp_witness_config as "witness_config"
    class external_dev__eth__TransactionReceipt as "dev::eth::TransactionReceipt"
    external_dev__eth__TransactionReceipt <|-- node_node_Block_hpp_TransactionReceiptPlaceholder
    class external_iApproveQueue as "iApproveQueue"
    external_iApproveQueue <|-- node_node_approve_queue_hpp_ApproveQueue
    class external_std__enable_shared_from_this_mcp__chain_ as "std::enable_shared_from_this<mcp::chain>"
    external_std__enable_shared_from_this_mcp__chain_ <|-- node_node_chain_hpp_chain
    class external_ExtVMFace as "ExtVMFace"
    external_ExtVMFace <|-- node_node_evm_ExtVM_h_ExtVM
    class external_p2p__icapability as "p2p::icapability"
    external_p2p__icapability <|-- node_node_node_capability_hpp_node_capability
    class external_mcp__iblock_cache as "mcp::iblock_cache"
    external_mcp__iblock_cache <|-- node_node_process_block_cache_hpp_process_block_cache
    node_node_tracers_Tracer_hpp_Tracer <|-- node_node_tracers_4byte_hpp_FourByteTracer
    node_node_tracers_Tracer_hpp_Tracer <|-- node_node_tracers_Call_hpp_CallTracer
    node_node_tracers_Tracer_hpp_Tracer <|-- node_node_tracers_OpCode_hpp_OpCode
    node_node_tracers_Tracer_hpp_Tracer <|-- node_node_tracers_PreState_hpp_PreStateTracer
    class external_dev__eth__EVMLogger as "dev::eth::EVMLogger"
    external_dev__eth__EVMLogger <|-- node_node_tracers_Tracer_hpp_Tracer
    class external_iTransactionQueue as "iTransactionQueue"
    external_iTransactionQueue <|-- node_node_transaction_queue_hpp_TransactionQueue
    class external_std__enable_shared_from_this_mcp__witness_ as "std::enable_shared_from_this<mcp::witness>"
    external_std__enable_shared_from_this_mcp__witness_ <|-- node_node_witness_hpp_witness
```

## P2p module

| Class | Defined in | Inherits from |
| --- | --- | --- |
| `capability_desc` | `p2p/capability.hpp` | `` |
| `icapability` | `p2p/capability.hpp` | `` |
| `peer_capability` | `p2p/capability.hpp` | `` |
| `node_endpoint` | `p2p/common.hpp` | `` |
| `node_info` | `p2p/common.hpp` | `` |
| `p2p_config` | `p2p/common.hpp` | `` |
| `discover_packet` | `p2p/discover_packet.hpp` | `` |
| `find_node_packet` | `p2p/discover_packet.hpp` | `discover_packet` |
| `neighbour` | `p2p/discover_packet.hpp` | `` |
| `neighbours_packet` | `p2p/discover_packet.hpp` | `discover_packet` |
| `ping_packet` | `p2p/discover_packet.hpp` | `discover_packet` |
| `pong_packet` | `p2p/discover_packet.hpp` | `discover_packet` |
| `RLPXFrameCoderImpl` | `p2p/frame_coder.cpp` | `` |
| `RLPXFrameCoder` | `p2p/frame_coder.hpp` | `` |
| `hankshake` | `p2p/handshake.hpp` | `std::enable_shared_from_this<hankshake>` |
| `hankshake_msg` | `p2p/handshake.hpp` | `` |
| `host` | `p2p/host.hpp` | `std::enable_shared_from_this<host>` |
| `host_node_table_event_handler` | `p2p/host.hpp` | `node_table_event_handler` |
| `node_bucket` | `p2p/node_table.hpp` | `` |
| `node_table` | `p2p/node_table.hpp` | `std::enable_shared_from_this<node_table>` |
| `node_table_event_handler` | `p2p/node_table.hpp` | `` |
| `send_udp_datagram` | `p2p/node_table.hpp` | `` |
| `peer` | `p2p/peer.hpp` | `std::enable_shared_from_this<peer>` |
| `peer_metrics` | `p2p/peer.hpp` | `` |
| `peer_manager` | `p2p/peer_manager.hpp` | `` |
| `peer_store` | `p2p/peer_store.hpp` | `` |
| `peers_content` | `p2p/peer_store.hpp` | `` |
| `upnp` | `p2p/upnp.hpp` | `` |

```mermaid
classDiagram
    class p2p_p2p_capability_hpp_capability_desc as "capability_desc"
    class p2p_p2p_capability_hpp_icapability as "icapability"
    class p2p_p2p_capability_hpp_peer_capability as "peer_capability"
    class p2p_p2p_common_hpp_node_endpoint as "node_endpoint"
    class p2p_p2p_common_hpp_node_info as "node_info"
    class p2p_p2p_common_hpp_p2p_config as "p2p_config"
    class p2p_p2p_discover_packet_hpp_discover_packet as "discover_packet"
    class p2p_p2p_discover_packet_hpp_find_node_packet as "find_node_packet"
    class p2p_p2p_discover_packet_hpp_neighbour as "neighbour"
    class p2p_p2p_discover_packet_hpp_neighbours_packet as "neighbours_packet"
    class p2p_p2p_discover_packet_hpp_ping_packet as "ping_packet"
    class p2p_p2p_discover_packet_hpp_pong_packet as "pong_packet"
    class p2p_p2p_frame_coder_cpp_RLPXFrameCoderImpl as "RLPXFrameCoderImpl"
    class p2p_p2p_frame_coder_hpp_RLPXFrameCoder as "RLPXFrameCoder"
    class p2p_p2p_handshake_hpp_hankshake as "hankshake"
    class p2p_p2p_handshake_hpp_hankshake_msg as "hankshake_msg"
    class p2p_p2p_host_hpp_host as "host"
    class p2p_p2p_host_hpp_host_node_table_event_handler as "host_node_table_event_handler"
    class p2p_p2p_node_table_hpp_node_bucket as "node_bucket"
    class p2p_p2p_node_table_hpp_node_table as "node_table"
    class p2p_p2p_node_table_hpp_node_table_event_handler as "node_table_event_handler"
    class p2p_p2p_node_table_hpp_send_udp_datagram as "send_udp_datagram"
    class p2p_p2p_peer_hpp_peer as "peer"
    class p2p_p2p_peer_hpp_peer_metrics as "peer_metrics"
    class p2p_p2p_peer_manager_hpp_peer_manager as "peer_manager"
    class p2p_p2p_peer_store_hpp_peer_store as "peer_store"
    class p2p_p2p_peer_store_hpp_peers_content as "peers_content"
    class p2p_p2p_upnp_hpp_upnp as "upnp"
    p2p_p2p_discover_packet_hpp_discover_packet <|-- p2p_p2p_discover_packet_hpp_find_node_packet
    p2p_p2p_discover_packet_hpp_discover_packet <|-- p2p_p2p_discover_packet_hpp_neighbours_packet
    p2p_p2p_discover_packet_hpp_discover_packet <|-- p2p_p2p_discover_packet_hpp_ping_packet
    p2p_p2p_discover_packet_hpp_discover_packet <|-- p2p_p2p_discover_packet_hpp_pong_packet
    class external_std__enable_shared_from_this_hankshake_ as "std::enable_shared_from_this<hankshake>"
    external_std__enable_shared_from_this_hankshake_ <|-- p2p_p2p_handshake_hpp_hankshake
    class external_std__enable_shared_from_this_host_ as "std::enable_shared_from_this<host>"
    external_std__enable_shared_from_this_host_ <|-- p2p_p2p_host_hpp_host
    p2p_p2p_node_table_hpp_node_table_event_handler <|-- p2p_p2p_host_hpp_host_node_table_event_handler
    class external_std__enable_shared_from_this_node_table_ as "std::enable_shared_from_this<node_table>"
    external_std__enable_shared_from_this_node_table_ <|-- p2p_p2p_node_table_hpp_node_table
    class external_std__enable_shared_from_this_peer_ as "std::enable_shared_from_this<peer>"
    external_std__enable_shared_from_this_peer_ <|-- p2p_p2p_peer_hpp_peer
```

## Rpc module

| Class | Defined in | Inherits from |
| --- | --- | --- |
| `rpc_config` | `rpc/config.hpp` | `` |
| `rpc_connection` | `rpc/connection.hpp` | `std::enable_shared_from_this<mcp::rpc_connection>` |
| `rpc_handler` | `rpc/handler.hpp` | `std::enable_shared_from_this<mcp::rpc_handler>` |
| `rpc` | `rpc/rpc.hpp` | `` |
| `rpc_ws` | `rpc/rpc_ws.hpp` | `std::enable_shared_from_this<rpc_ws>` |
| `rpc_ws_config` | `rpc/rpc_ws.hpp` | `` |
| `rpc_ws_connection` | `rpc/rpc_ws.hpp` | `std::enable_shared_from_this<rpc_ws_connection>` |
| `rpc_ws_handler` | `rpc/rpc_ws.hpp` | `std::enable_shared_from_this<mcp::rpc_ws_handler>` |
| `subscribe` | `rpc/rpc_ws.hpp` | `std::enable_shared_from_this<subscribe>` |

```mermaid
classDiagram
    class rpc_rpc_config_hpp_rpc_config as "rpc_config"
    class rpc_rpc_connection_hpp_rpc_connection as "rpc_connection"
    class rpc_rpc_handler_hpp_rpc_handler as "rpc_handler"
    class rpc_rpc_rpc_hpp_rpc as "rpc"
    class rpc_rpc_rpc_ws_hpp_rpc_ws as "rpc_ws"
    class rpc_rpc_rpc_ws_hpp_rpc_ws_config as "rpc_ws_config"
    class rpc_rpc_rpc_ws_hpp_rpc_ws_connection as "rpc_ws_connection"
    class rpc_rpc_rpc_ws_hpp_rpc_ws_handler as "rpc_ws_handler"
    class rpc_rpc_rpc_ws_hpp_subscribe as "subscribe"
    class external_std__enable_shared_from_this_mcp__rpc_connection_ as "std::enable_shared_from_this<mcp::rpc_connection>"
    external_std__enable_shared_from_this_mcp__rpc_connection_ <|-- rpc_rpc_connection_hpp_rpc_connection
    class external_std__enable_shared_from_this_mcp__rpc_handler_ as "std::enable_shared_from_this<mcp::rpc_handler>"
    external_std__enable_shared_from_this_mcp__rpc_handler_ <|-- rpc_rpc_handler_hpp_rpc_handler
    class external_std__enable_shared_from_this_rpc_ws_ as "std::enable_shared_from_this<rpc_ws>"
    external_std__enable_shared_from_this_rpc_ws_ <|-- rpc_rpc_rpc_ws_hpp_rpc_ws
    class external_std__enable_shared_from_this_rpc_ws_connection_ as "std::enable_shared_from_this<rpc_ws_connection>"
    external_std__enable_shared_from_this_rpc_ws_connection_ <|-- rpc_rpc_rpc_ws_hpp_rpc_ws_connection
    class external_std__enable_shared_from_this_mcp__rpc_ws_handler_ as "std::enable_shared_from_this<mcp::rpc_ws_handler>"
    external_std__enable_shared_from_this_mcp__rpc_ws_handler_ <|-- rpc_rpc_rpc_ws_hpp_rpc_ws_handler
    class external_std__enable_shared_from_this_subscribe_ as "std::enable_shared_from_this<subscribe>"
    external_std__enable_shared_from_this_subscribe_ <|-- rpc_rpc_rpc_ws_hpp_subscribe
```

## Wallet module

| Class | Defined in | Inherits from |
| --- | --- | --- |
| `key_manager` | `wallet/key_manager.hpp` | `` |
| `CryptoJSON` | `wallet/key_store.hpp` | `` |
| `cipherparamsJSON` | `wallet/key_store.hpp` | `` |
| `key_content` | `wallet/key_store.hpp` | `` |
| `key_store` | `wallet/key_store.hpp` | `` |
| `scryptParamsJSON` | `wallet/key_store.hpp` | `` |
| `wallet` | `wallet/wallet.hpp` | `std::enable_shared_from_this<mcp::wallet>` |

```mermaid
classDiagram
    class wallet_wallet_key_manager_hpp_key_manager as "key_manager"
    class wallet_wallet_key_store_hpp_CryptoJSON as "CryptoJSON"
    class wallet_wallet_key_store_hpp_cipherparamsJSON as "cipherparamsJSON"
    class wallet_wallet_key_store_hpp_key_content as "key_content"
    class wallet_wallet_key_store_hpp_key_store as "key_store"
    class wallet_wallet_key_store_hpp_scryptParamsJSON as "scryptParamsJSON"
    class wallet_wallet_wallet_hpp_wallet as "wallet"
    class external_std__enable_shared_from_this_mcp__wallet_ as "std::enable_shared_from_this<mcp::wallet>"
    external_std__enable_shared_from_this_mcp__wallet_ <|-- wallet_wallet_wallet_hpp_wallet
```
