#!/bin/bash
curl -sSLO $(curl -sSI https://github.com/catchorg/Catch2/releases/latest | sed -ne '\,^[Ll]ocation: *, !d
s,[Ll]ocation: *\(.*\)/tag/,\1/download/,
s,.$,/catch.hpp,
p')
