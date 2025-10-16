// Aleth: Ethereum C++ client, tools and libraries.
// Copyright 2018-2019 Aleth Authors.
// Copyright 2024-2025 The Olympus Developers.
// Licensed under the GNU General Public License, Version 3.
#pragma once

#include <evmc/evmc.h>
#include <evmc/utils.h>

#if __cplusplus
extern "C" {
#endif

EVMC_EXPORT struct evmc_vm* evmc_create_aleth_interpreter() EVMC_NOEXCEPT;

#if __cplusplus
}
#endif
