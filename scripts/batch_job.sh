#! /bin/sh

LOSEFACE=../build/loseface

echo "MLP global (25 inputs)..."
$LOSEFACE mlp_global.lua 25 25 mse > mlp_global_25_25.output
$LOSEFACE mlp_global.lua 25 50 mse > mlp_global_25_50.output
$LOSEFACE mlp_global.lua 25 75 mse > mlp_global_25_75.output

echo "MLP global (50 inputs)..."
$LOSEFACE mlp_global.lua 50 25 mse > mlp_global_50_25.output
$LOSEFACE mlp_global.lua 50 50 mse > mlp_global_50_50.output
$LOSEFACE mlp_global.lua 50 75 mse > mlp_global_50_75.output

echo "MLP global (75 inputs)..."
$LOSEFACE mlp_global.lua 75 25 mse > mlp_global_75_25.output
$LOSEFACE mlp_global.lua 75 50 mse > mlp_global_75_50.output
$LOSEFACE mlp_global.lua 75 75 mse > mlp_global_75_75.output

echo "aMLP (25 inputs - basic training)..."
$LOSEFACE mlp_array.lua 25 25 0 mse > mlp_array_25_25_0neg.output
$LOSEFACE mlp_array.lua 25 50 0 mse > mlp_array_25_50_0neg.output
$LOSEFACE mlp_array.lua 25 75 0 mse > mlp_array_25_75_0neg.output

echo "aMLP (50 inputs - basic training)..."
$LOSEFACE mlp_array.lua 50 25 0 mse > mlp_array_50_25_0neg.output
$LOSEFACE mlp_array.lua 50 50 0 mse > mlp_array_50_50_0neg.output
$LOSEFACE mlp_array.lua 50 75 0 mse > mlp_array_50_75_0neg.output

echo "aMLP (75 inputs - basic training)..."
$LOSEFACE mlp_array.lua 75 25 0 mse > mlp_array_75_25_0neg.output
$LOSEFACE mlp_array.lua 75 50 0 mse > mlp_array_75_50_0neg.output
$LOSEFACE mlp_array.lua 75 75 0 mse > mlp_array_75_75_0neg.output

echo "aMLP (25 inputs - special training)..."
$LOSEFACE mlp_array.lua 25 25 3 mse > mlp_array_25_25_3neg.output
$LOSEFACE mlp_array.lua 25 50 3 mse > mlp_array_25_50_3neg.output
$LOSEFACE mlp_array.lua 25 75 3 mse > mlp_array_25_75_3neg.output

echo "aMLP (50 inputs - special training)..."
$LOSEFACE mlp_array.lua 50 25 3 mse > mlp_array_50_25_3neg.output
$LOSEFACE mlp_array.lua 50 50 3 mse > mlp_array_50_50_3neg.output
$LOSEFACE mlp_array.lua 50 75 3 mse > mlp_array_50_75_3neg.output

echo "aMLP (75 inputs - special training)..."
$LOSEFACE mlp_array.lua 75 25 3 mse > mlp_array_75_25_3neg.output
$LOSEFACE mlp_array.lua 75 50 3 mse > mlp_array_75_50_3neg.output
$LOSEFACE mlp_array.lua 75 75 3 mse > mlp_array_75_75_3neg.output

echo "Testing with different number of negatives..."
#$LOSEFACE mlp_array.lua 50 50 0 mse > mlp_array_50_50_0neg.output
 $LOSEFACE mlp_array.lua 50 50 1 mse > mlp_array_50_50_1neg.output
 $LOSEFACE mlp_array.lua 50 50 2 mse > mlp_array_50_50_2neg.output
#$LOSEFACE mlp_array.lua 50 50 3 mse > mlp_array_50_50_3neg.output
 $LOSEFACE mlp_array.lua 50 50 4 mse > mlp_array_50_50_4neg.output
 $LOSEFACE mlp_array.lua 50 50 5 mse > mlp_array_50_50_5neg.output
 $LOSEFACE mlp_array.lua 50 50 6 mse > mlp_array_50_50_6neg.output
 $LOSEFACE mlp_array.lua 50 50 7 mse > mlp_array_50_50_7neg.output
 $LOSEFACE mlp_array.lua 50 50 8 mse > mlp_array_50_50_8neg.output
 $LOSEFACE mlp_array.lua 50 50 9 mse > mlp_array_50_50_9neg.output
 $LOSEFACE mlp_array.lua 50 50 10 mse > mlp_array_50_50_10neg.output
 $LOSEFACE mlp_array.lua 50 50 11 mse > mlp_array_50_50_11neg.output
 $LOSEFACE mlp_array.lua 50 50 12 mse > mlp_array_50_50_12neg.output
 $LOSEFACE mlp_array.lua 50 50 13 mse > mlp_array_50_50_13neg.output
 $LOSEFACE mlp_array.lua 50 50 14 mse > mlp_array_50_50_14neg.output
