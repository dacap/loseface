#! /bin/sh

LOSEFACE=../build/loseface
FACES=$1
SUBJECTS=$2
if [ x"$FACES" = x"" ] ; then FACES=orl_patterns ; fi
if [ x"$SUBJECTS" = x"" ] ; then SUBJECTS=40 ; fi

echo "Patterns=\"$FACES\", Subjects=$SUBJECTS"

echo "MLP global (25 inputs)..."
$LOSEFACE mlp_global.lua $FACES 25 25 $SUBJECTS mse > $FACES/mlp_global_25_25.output
$LOSEFACE mlp_global.lua $FACES 25 50 $SUBJECTS mse > $FACES/mlp_global_25_50.output
$LOSEFACE mlp_global.lua $FACES 25 75 $SUBJECTS mse > $FACES/mlp_global_25_75.output

echo "MLP global (50 inputs)..."
$LOSEFACE mlp_global.lua $FACES 50 25 $SUBJECTS mse > $FACES/mlp_global_50_25.output
$LOSEFACE mlp_global.lua $FACES 50 50 $SUBJECTS mse > $FACES/mlp_global_50_50.output
$LOSEFACE mlp_global.lua $FACES 50 75 $SUBJECTS mse > $FACES/mlp_global_50_75.output

echo "MLP global (75 inputs)..."
$LOSEFACE mlp_global.lua $FACES 75 25 $SUBJECTS mse > $FACES/mlp_global_75_25.output
$LOSEFACE mlp_global.lua $FACES 75 50 $SUBJECTS mse > $FACES/mlp_global_75_50.output
$LOSEFACE mlp_global.lua $FACES 75 75 $SUBJECTS mse > $FACES/mlp_global_75_75.output

echo "aMLP (25 inputs - basic training)..."
$LOSEFACE mlp_array.lua $FACES 25 25 $SUBJECTS 0 mse > $FACES/mlp_array_25_25_0neg.output
$LOSEFACE mlp_array.lua $FACES 25 50 $SUBJECTS 0 mse > $FACES/mlp_array_25_50_0neg.output
$LOSEFACE mlp_array.lua $FACES 25 75 $SUBJECTS 0 mse > $FACES/mlp_array_25_75_0neg.output

echo "aMLP (50 inputs - basic training)..."
$LOSEFACE mlp_array.lua $FACES 50 25 $SUBJECTS 0 mse > $FACES/mlp_array_50_25_0neg.output
$LOSEFACE mlp_array.lua $FACES 50 50 $SUBJECTS 0 mse > $FACES/mlp_array_50_50_0neg.output
$LOSEFACE mlp_array.lua $FACES 50 75 $SUBJECTS 0 mse > $FACES/mlp_array_50_75_0neg.output

echo "aMLP (75 inputs - basic training)..."
$LOSEFACE mlp_array.lua $FACES 75 25 $SUBJECTS 0 mse > $FACES/mlp_array_75_25_0neg.output
$LOSEFACE mlp_array.lua $FACES 75 50 $SUBJECTS 0 mse > $FACES/mlp_array_75_50_0neg.output
$LOSEFACE mlp_array.lua $FACES 75 75 $SUBJECTS 0 mse > $FACES/mlp_array_75_75_0neg.output

echo "aMLP (25 inputs - special training)..."
$LOSEFACE mlp_array.lua $FACES 25 25 $SUBJECTS 3 mse > $FACES/mlp_array_25_25_3neg.output
$LOSEFACE mlp_array.lua $FACES 25 50 $SUBJECTS 3 mse > $FACES/mlp_array_25_50_3neg.output
$LOSEFACE mlp_array.lua $FACES 25 75 $SUBJECTS 3 mse > $FACES/mlp_array_25_75_3neg.output

echo "aMLP (50 inputs - special training)..."
$LOSEFACE mlp_array.lua $FACES 50 25 $SUBJECTS 3 mse > $FACES/mlp_array_50_25_3neg.output
$LOSEFACE mlp_array.lua $FACES 50 50 $SUBJECTS 3 mse > $FACES/mlp_array_50_50_3neg.output
$LOSEFACE mlp_array.lua $FACES 50 75 $SUBJECTS 3 mse > $FACES/mlp_array_50_75_3neg.output

echo "aMLP (75 inputs - special training)..."
$LOSEFACE mlp_array.lua $FACES 75 25 $SUBJECTS 3 mse > $FACES/mlp_array_75_25_3neg.output
$LOSEFACE mlp_array.lua $FACES 75 50 $SUBJECTS 3 mse > $FACES/mlp_array_75_50_3neg.output
$LOSEFACE mlp_array.lua $FACES 75 75 $SUBJECTS 3 mse > $FACES/mlp_array_75_75_3neg.output

echo "Testing with different number of negatives..."
#$LOSEFACE mlp_array.lua $FACES 50 50 $SUBJECTS 0 mse > $FACES/mlp_array_50_50_0neg.output
 $LOSEFACE mlp_array.lua $FACES 50 50 $SUBJECTS 1 mse > $FACES/mlp_array_50_50_1neg.output
 $LOSEFACE mlp_array.lua $FACES 50 50 $SUBJECTS 2 mse > $FACES/mlp_array_50_50_2neg.output
#$LOSEFACE mlp_array.lua $FACES 50 50 $SUBJECTS 3 mse > $FACES/mlp_array_50_50_3neg.output
 $LOSEFACE mlp_array.lua $FACES 50 50 $SUBJECTS 4 mse > $FACES/mlp_array_50_50_4neg.output
 $LOSEFACE mlp_array.lua $FACES 50 50 $SUBJECTS 5 mse > $FACES/mlp_array_50_50_5neg.output
 $LOSEFACE mlp_array.lua $FACES 50 50 $SUBJECTS 6 mse > $FACES/mlp_array_50_50_6neg.output
 $LOSEFACE mlp_array.lua $FACES 50 50 $SUBJECTS 7 mse > $FACES/mlp_array_50_50_7neg.output
 $LOSEFACE mlp_array.lua $FACES 50 50 $SUBJECTS 8 mse > $FACES/mlp_array_50_50_8neg.output
 $LOSEFACE mlp_array.lua $FACES 50 50 $SUBJECTS 9 mse > $FACES/mlp_array_50_50_9neg.output
 $LOSEFACE mlp_array.lua $FACES 50 50 $SUBJECTS 10 mse > $FACES/mlp_array_50_50_10neg.output
 $LOSEFACE mlp_array.lua $FACES 50 50 $SUBJECTS 11 mse > $FACES/mlp_array_50_50_11neg.output
 $LOSEFACE mlp_array.lua $FACES 50 50 $SUBJECTS 12 mse > $FACES/mlp_array_50_50_12neg.output
 $LOSEFACE mlp_array.lua $FACES 50 50 $SUBJECTS 13 mse > $FACES/mlp_array_50_50_13neg.output
 $LOSEFACE mlp_array.lua $FACES 50 50 $SUBJECTS 14 mse > $FACES/mlp_array_50_50_14neg.output
