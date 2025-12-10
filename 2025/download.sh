#!/bin/bash

SESSION_COOKIE=$(cat ./session-cookie)
DAY=$(printf "%02d" "$1")
FOLDER="./resources/$DAY"
mkdir $FOLDER
curl -b session=${SESSION_COOKIE} https://adventofcode.com/2025/day/$1/input -o $FOLDER/input
