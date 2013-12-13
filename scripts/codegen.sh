#!/bin/sh

LIBPATH='lib/'
GEN_COMMON='lib/gen/'

function run_codegen_precheck {
  if [ ! -f "tools/code/gen/codegen_auto" ]; then
    cd tools/code/gen/
    make
    cd ../../../
  fi
}

function run_codegen {
  echo ./tools/code/gen/codegen_auto $1 $2 $3 $4
  eval tools/code/gen/codegen_auto $1 $2 $3 $4
}

path=$(realpath $LIBPATH)
pre="''"
suff="''"
gencommon=$(realpath $GEN_COMMON)

run_codegen_precheck

for dir in $(ls -d $path/krad_*/); do
  prefix=$(basename $dir)
  localpath="${path}/${prefix}"
  gendir="${localpath}/gen"

  mkdir -p $gendir

  rm -rf $gendir/*
done

rm -rf $gencommon/*
mkdir -p $gencommon

for fname in $(ls -1 lib/*/*gen/*.c); do
  echo -e '#include "${fname}"' >> lib/gen/gen.c
done

run_codegen $path $pre $suff $gencommon

echo 'All Done!'



