#!/bin/sh
# sudo chmod 755 './BuildProject.sh'

OUTPUT=$(./Tools/TMNEBuild/Bin/TMNEBuild $1 $2 $3)
source ./Tools/TMNEBuild/Source/UnixRunner.sh "${OUTPUT}"