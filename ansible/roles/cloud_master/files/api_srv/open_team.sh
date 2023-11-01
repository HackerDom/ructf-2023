#!/usr/bin/env bash
./open_network.py $1
./open_vm.py $1 3
./make_vm_available.py $1 3
