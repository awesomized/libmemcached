#!/bin/sh
git config merge.touch.name = "Keep this file"
git config merge.touch.driver = "touch %A"
