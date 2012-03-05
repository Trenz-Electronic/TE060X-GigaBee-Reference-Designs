#!/bin/sh
# Projects to release
REPOSITORY=TE060X-GigaBee-Reference-Designs
# Numbers of projects to release (list like '1 2 3 4')
PROJECT_LIST='1 2 3 4 5'
# Project 1 description
# List of folders requred to release (First will be used as name)
NAME1='GigaBee_XC6LX-Axi'
# Tool used to build
TOOL1='EDK-13.2'
# Project Version
VER1='1.0'

# Project 2 description
NAME2='GigaBee_XC6LX-Axi_lite'
TOOL2='EDK-13.1'
VER2='1.0'

# Project 3 description
NAME3='GigaBee_XC6LX-Blinkin'
TOOL3='ISE-13.2'
VER3='1.0'

# Project 4 description
NAME4='GigaBee_XC6SLX-MIG'
TOOL4='ISE-12.4'
VER4='1.0'

# Project 5 description
NAME5='GigaBee_UDP-Datalogger'
TOOL5='ISE-13.1'
VER5='1.0'
############# Do not edit below this line !!!
SUBVER=`git rev-parse --short HEAD`

if [ -z "$PROJECT_LIST" ]; then exit ; fi
cd ..
for l in $PROJECT_LIST
do
	namelist=NAME$l
	tool=TOOL$l
	ver=VER$l
	name=${!namelist}
	set $name
	for proj in ${!namelist}
	do
		zip -rq $1-${!tool}-v${!ver}.${SUBVER}.zip ${REPOSITORY}/${proj}
	done
done
cd ${REPOSITORY}

