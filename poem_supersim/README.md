# SuperSim - A flexible network simulator

## Build Instructions
This is the recommended environment setup and build instructions for using
SuperSim. These instructions are for Ubuntu 14.xx, but are easily adaptable.

### Dependent Libraries
Install zlib and jsoncpp
```bash
sudo apt-get install zlib1g-dev
sudo apt-get install libjsoncpp-dev
```

### Build System Tools
Install cpplint and gtest
```bash
mkdir ~/.google
cd ~/.google
git clone https://github.com/nicmcd/cpplint.git
git clone https://github.com/nicmcd/gtest.git
cd gtest/make
make gtest_main.a
```

### Build SuperSim
```bash
make all
```

## Running SuperSim
SuperSim is configured from a JSON style settings file.
```bash
bin/supersim json/flattened_butterfly_simplemem.json
```

The configuration can be modified at the command line.
```bash
bin/supersim json/flattened_butterfly_simplemem.json network_system.network.dimensions[3]=uint=10
```