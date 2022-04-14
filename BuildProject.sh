#!/bin/sh
# sudo chmod 755 './BuildProject.sh'

OUTPUT=$($1/Bin/TMNEBuild $2 $3 $4)
source $1/Source/UnixRunner.sh "${OUTPUT}"