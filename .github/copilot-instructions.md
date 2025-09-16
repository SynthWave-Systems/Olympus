# Olympus Blockchain - GitHub Copilot Instructions

Olympus is a C++ blockchain implementation with integrated smart contract testing framework. It includes the MCP (Miner Consensus Protocol) blockchain node and multiple smart contract test suites written in JavaScript/TypeScript.

**Always reference these instructions first and fallback to search or bash commands only when you encounter unexpected information that does not match the info here.**

## Working Effectively

### Prerequisites and Dependencies
- Install basic build tools:
  ```bash
  sudo apt-get update && sudo apt-get install -y git cmake wget unzip g++ 
  ```
- Install required libraries for RocksDB:
  ```bash
  sudo apt-get install -y liblz4-dev libzstd-dev zlib1g-dev
  ```
- Install Boost libraries:
  ```bash
  sudo apt-get install -y libboost-all-dev
  ```
  - This installs Boost v1.83.0, which is compatible with the required v1.81.0+

### Build RocksDB (Required Dependency)
**CRITICAL: Build RocksDB v8.3.3 before attempting to build MCP**
```bash
wget https://github.com/facebook/rocksdb/archive/v8.3.3.zip
unzip v8.3.3.zip
cd rocksdb-8.3.3
PORTABLE=1 make -j$(nproc) USE_RTTI=1 static_lib  # NEVER CANCEL: Takes 6-8 minutes. Set timeout to 15+ minutes.
sudo make install
cd .. && rm -rf rocksdb-8.3.3 v8.3.3.zip
```

### Initialize Git Submodules
**MUST be done before building MCP - the submodules contain required components**
```bash
git submodule update --init --recursive  # Takes ~5 seconds
```

### Build MCP Blockchain Node
**CRITICAL BUILD ISSUE**: The main MCP build currently FAILS due to external dependency unavailability.
```bash
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ../  # Takes <1 minute
make -j$(nproc)  # FAILS - cannot download mpir-cmake.tar.gz from AWS S3 bucket
```
**Known Issue**: Build fails when downloading "mpir-cmake.tar.gz" from `https://pekka-network.s3.ap-east-1.amazonaws.com/build-dependency/mpir-cmake.tar.gz`. The external dependency is currently unavailable.

**Workaround**: Until the dependency issue is resolved, focus development and testing on smart contract components.

### Smart Contract Testing Framework

#### Node.js Project Structure
The repository contains multiple smart contract test projects in `test/contracts/`:
- `mcp-dydx/` - DyDx protocol contracts (Node.js v20 compatible)
- `mcp-compound/` - Compound protocol contracts (requires Node.js v12 - incompatible)
- `mcp-diamond-test/` - Diamond pattern test contracts
- `mcp-multisig/` - Multi-signature wallet contracts
- `mcp-pancakeswap-test/` - PancakeSwap protocol contracts
- `mcp-uniswapv2-core/` - Uniswap V2 core contracts
- `mcp-uniswapv2-periphery/` - Uniswap V2 periphery contracts
- `mcp-upgradable-test/` - Upgradable contract patterns

#### Working with Smart Contract Projects

**mcp-dydx (Node.js v20 Compatible - Recommended)**
```bash
cd test/contracts/mcp-dydx
npm install --save-dev  # NEVER CANCEL: Takes 3-4 minutes. Set timeout to 10+ minutes.

# Create .env file with required configuration before deployment:
cp .env.example .env  # Edit with appropriate values
npm run deploy_ccnbeta  # Requires network configuration in .env
```

**mcp-diamond-test (Node.js v20 Compatible)**
```bash
cd test/contracts/mcp-diamond-test
npm install  # NEVER CANCEL: Takes ~1 minute. Set timeout to 5+ minutes.
# Contains Diamond pattern smart contract implementations for testing
```

**mcp-multisig (Multi-Signature Wallet)**
```bash
cd test/contracts/mcp-multisig
npm install  # Follow package instructions for dapp subdirectory
cd dapp && npm install
npm start  # Start development server
```

#### Working with mcp-compound
**WARNING**: mcp-compound requires Node.js v12 but the system has v20. Use nvm to switch versions if needed:
```bash
cd test/contracts/mcp-compound
# This will fail with current Node.js v20:
yarn install  # ERROR: Engine "node" incompatible, requires v12
```

### Running Individual Components

#### Node Information (when MCP build is working)
```bash
./mcp --help
```

#### Run as ordinary node
```bash
./mcp --daemon --console --data_path=<Your data path>
```

#### Run as witness node
```bash
./mcp --daemon --console --data_path=<Your data path> --witness --witness_account=<Witness account keystore file> --password=<Password for keystore file>
```

## Validation and Testing

### Always Validate Changes With These Steps
1. **Smart Contract Development**: Focus on projects that work with current Node.js v20:
   ```bash
   # Recommended test projects:
   cd test/contracts/mcp-dydx
   npm install --save-dev  # Takes 3-4 minutes
   
   cd test/contracts/mcp-diamond-test
   npm install  # Takes ~1 minute
   
   # Test compilation/deployment (requires .env setup):
   npm run deploy_ccnbeta  # For projects that support it
   ```

2. **C++ Core Development**: Until the mpir dependency is fixed, you can:
   - Verify submodules are initialized: `git submodule status`
   - Test CMake configuration: `cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ../`
   - Partial build succeeds: `make -j$(nproc)` builds some libraries (libp2p.a, etc.)
   - Review and modify C++ source code in `mcp/` directory

3. **Manual Functional Testing**: When the main build is working:
   - Start a local node and verify it initializes without errors
   - Test RPC endpoints if available
   - Verify blockchain synchronization with test networks

### Build Timing Expectations
- **RocksDB build**: 6-8 minutes (NEVER CANCEL - set timeout to 15+ minutes)
- **mcp-dydx npm install**: 3-4 minutes (NEVER CANCEL - set timeout to 10+ minutes) 
- **mcp-diamond-test npm install**: ~1 minute (NEVER CANCEL - set timeout to 5+ minutes)
- **Git submodule initialization**: ~5 seconds
- **CMake configuration**: <1 minute
- **Full MCP build**: CURRENTLY FAILS due to dependency issues (partial build succeeds - some libraries built)

### Common Issues and Workarounds

1. **Main Build Failure**: 
   - **Issue**: Cannot download mpir-cmake.tar.gz
   - **Status**: Known external dependency issue
   - **Workaround**: Focus on smart contract components until resolved

2. **Smart Contract Node Version Conflicts**:
   - **mcp-compound requires Node.js v12** - incompatible with system v20
   - **Solution**: Use mcp-dydx for testing (compatible with v20)

3. **Missing Submodules**:
   - **Issue**: CMake fails with "does not contain a CMakeLists.txt file"
   - **Solution**: Always run `git submodule update --init --recursive` first

## Common Development Tasks

### Repository Structure
```
/home/runner/work/Olympus/Olympus/
├── mcp/                    # Main C++ blockchain implementation
├── test/contracts/         # Smart contract test projects
├── CMakeLists.txt         # Main build configuration
├── README.md              # Build instructions
└── build/                 # Build output directory
```

### Key Configuration Files
- `CMakeLists.txt` - Main build configuration
- `test/contracts/*/package.json` - Node.js project dependencies
- `test/contracts/*/hardhat.config.js` - Hardhat deployment configuration
- `test/contracts/*/.env` - Network and account configuration (create from .env.example)

### Frequently Used Commands (Validated)
```bash
# Verify dependencies
g++ -v                     # Should be >= 9
cmake --version            # Verify cmake is available
node --version             # v20.19.5 available

# Prepare build environment  
git submodule update --init --recursive
mkdir -p build && cd build

# Smart contract development
cd test/contracts/mcp-dydx
npm install --save-dev
```

### When Making Changes
1. **Always initialize submodules first** if working on C++ components
2. **Always test with mcp-dydx project** for smart contract changes
3. **Never cancel long-running builds** - set appropriate timeouts
4. **Focus on working components** until dependency issues are resolved

## Project Status Summary
- ✅ **C++ Dependencies**: RocksDB, Boost, build tools working
- ✅ **Git Submodules**: All submodules can be initialized
- ✅ **CMake Configuration**: Succeeds with proper setup
- ⚠️ **Main C++ Build**: Blocked by external dependency (mpir-cmake.tar.gz) - partial build succeeds
- ✅ **Smart Contract Framework**: mcp-dydx, mcp-diamond-test projects work with Node.js v20
- ✅ **Alternative Projects**: mcp-multisig and others available for development
- ❌ **mcp-compound**: Requires Node.js v12 (version conflict)
- ⚠️ **Node Execution**: Cannot test until main build is fixed