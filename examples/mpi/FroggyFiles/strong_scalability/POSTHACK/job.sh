#!/bin/bash

source /applis/site/nix.sh
#nix-env --switch-profile /nix/var/nix/profiles/per-user/robertcaulk/yade_mpi
nix-env --switch-profile $NIX_USER_PROFILE_DIR/yade
#mpiexec -np `cat $OAR_FILE_NODES|wc -l` -x LD_LIBRARY_PATH --prefix $openmpi_DIR --machinefile $OAR_NODE_FILE -mca plm_rsh_agent "oarsh" yadempi-daily -n -x testMPI_weakScalability.py
__YADE_COMMAND__
