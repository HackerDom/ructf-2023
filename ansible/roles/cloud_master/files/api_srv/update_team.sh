#!/usr/bin/env bash
./isolate_network.py $1
./delete_team_instance_vm.py $1 3
./create_team_instance.py $1 3
