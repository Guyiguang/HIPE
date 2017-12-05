#!/bin/bash
set -eu

# Get the absolute path to this script with all symlinks resolved.
THIS_SCRIPT="$(readlink -f "$0")"
# Get the directory name of this script.
DIR_SCRIPT="${THIS_SCRIPT%/*}"
# Change into this script's directory.
cd -- "$DIR_SCRIPT"

# Check if HIPE_EXTERNAL is set and use default paths if not.
# FIXME
# Remove hard-coded system paths that lie outside of the HIPE source directory.
if [[ -z ${HIPE_EXTERNAL+x} ]]
then
  # This was uncommented in the original script but serves no purpose as it is
  # immediately overridden by the following line.
  #export HIPE_EXTERNAL=/mnt/ssd/hipeExternal
  export HIPE_EXTERNAL=/work/external/
fi

# Create the build directory and change into it.
mkdir -p build
cd build

# FIXME
# This also contains a hard-coded path which should presumably agree with the
# path given for HIPE_EXTERNAL above (i.e. both should point to /mnt/ssd or
# /work.
cmake \
  -DCMAKE_BUILD_TYPE=Release \
  -DUSE_DLIB=ON \
  -DUSE_GPERFTOOLS=OFF \
  -DGPERFTOOLS_DIR=/mnt/ssd/gperftools/install/ ..
make VERBOSE=1 -j$(nproc)
