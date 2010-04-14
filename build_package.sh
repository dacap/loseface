#! /bin/sh

version=0.2
package=LoseFace-$version

mkdir $package
mkdir $package/docs
mkdir $package/scripts

cd $package

# Copy LoseFace main files
cp ../build/loseface.exe \
   ../start_cmd.bat \
   ../README.txt \
   ../ORL_TESTS.txt \
   .

# Copy manual
cp ../docs/manual/manual.html \
   ../docs/manual/manual.css \
   docs

# Copy scripts
cp ../scripts/batch_job.bat \
   ../scripts/calculate_from_variance.lua \
   ../scripts/create_patterns.lua \
   ../scripts/divide_images_matrix.lua \
   ../scripts/mlp_array.lua \
   ../scripts/mlp_array_extras.lua \
   ../scripts/mlp_global.lua \
   ../scripts/orl_images_matrix.lua \
   ../scripts/orl_patterns.lua \
   ../scripts/orl_patterns_variance.lua \
   scripts

# Create a script to run cmd with loseface.exe available everywhere
cat >start_cmd.bat <<EOF
@echo off
set PATH=%CD%;%PATH%
cmd
EOF

mkdir scripts/orl_faces
cp ../scripts/orl_faces/download_faces.bat \
   ../scripts/orl_faces/wget.exe \
   ../scripts/orl_faces/wget.txt \
   ../scripts/orl_faces/unzip.exe \
   ../scripts/orl_faces/unzip.txt scripts/orl_faces

mkdir scripts/orl_patterns
cp ../scripts/orl_patterns/tmpfile.txt scripts/orl_patterns

cd ..

zip -r -9 $package.zip $package
