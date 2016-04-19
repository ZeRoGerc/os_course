#!/bin/bash

find -L "$1" -type l -mmin +10080

