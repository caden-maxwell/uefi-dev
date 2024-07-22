#!/usr/bin/env bash

git submodule init
git submodule update
cd gnu-efi
make
