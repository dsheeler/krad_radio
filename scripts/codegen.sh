#!/bin/sh

GEN_FORMATS=(json)
LIBPATH='lib/'
CLEAN=0

function run_bootstrap {

  if [ ! -f "tools/code/gen/codegen_bootstrap" ]; then
    cd tools/code/gen/
    make 
    cd ../../../
  fi

  if [ -f "tools/code/gen/bootstrapped.c" ]; then
    echo rm -rf tools/code/gen/bootstrapped.c
    rm -rf tools/code/gen/bootstrapped.c
    echo rm -rf tools/code/gen/bootstrapped.h
    rm -rf tools/code/gen/bootstrapped.h
  fi
  
  if [ $CLEAN -eq 0 ]; then
    echo 'Bootstrapping...'
    cd tools/code/gen/
    echo ./codegen_bootstrap $1 $2 $3 $4
    eval ./codegen_bootstrap $1 $2 $3 $4
    cd ../../../
  fi 
}

function run_codegen_precheck {
  run_bootstrap $1 $2 $3 $4
  if [ ! -f "tools/code/gen/codegen" ]; then
    cd tools/code/gen/
    make cgen 
    cd ../../../
  fi
}

function run_codegen {
  if [ -f $5 ]; then
    if [ $CLEAN -eq 1 ]; then
      echo rm -rf $5
      rm -rf $5
    fi
  fi
  if [ -f $header ]; then
    if [ $CLEAN -eq 1 ]; then
      echo rm -rf $header
      rm -rf $header
    fi
  fi
  if [ -f $helperh ]; then
    if [ $CLEAN -eq 1 ]; then
      echo rm -rf $helperh
      rm -rf $helperh
    fi
  fi
  if [ $CLEAN -eq 0 ]; then
    eval tools/code/gen/codegen $1 $2 $3 $4 $5 $6
  fi
}

if [[ $1 = "clean" ]]; then
  CLEAN=1
  echo 'Cleaning up!'
fi

path=$(realpath $LIBPATH)
pre="''"
suff="_info"
common="${path}/krad_web/gen/common.h"

run_codegen_precheck $path $pre $suff $format

for format in "${GEN_FORMATS[@]}"; do
  type_common="${path}/krad_web/gen/to_${format}.c"
  for dir in $(ls -d $path/*/); do
    prefix=$(basename $dir)
    localpath="${path}/${prefix}"
    gendir="${localpath}/gen"
    if [ $CLEAN -eq 0 ]; then
        mkdir -p $gendir
    fi
    prefix="${gendir}/${prefix}"
    helper="${prefix}_helpers.c"
    helperh="${prefix}_helpers.h"
    header="${prefix}_to_${format}.h"
    main="${prefix}_to_${format}.c"

    if [ $CLEAN -eq 0 ]; then
      echo "Generating" $helper
      echo "Generating" $helperh
    fi

    run_codegen $localpath $pre $suff "helper" $helper
    
    if [ $CLEAN -eq 0 ]; then
      echo "Generating" $header
      echo "Generating" $main
    fi
    
    run_codegen $localpath $pre $suff $format $main
    
  done

  if [ $CLEAN -eq 0 ]; then
    echo "Generating" $type_common
  fi

  run_codegen $path $pre $suff "type_common" $type_common $format
  
done

if [ $CLEAN -eq 0 ]; then
  echo "Generating" $common
fi

run_codegen $path $pre $suff "common" $common

echo 'All Done!'



