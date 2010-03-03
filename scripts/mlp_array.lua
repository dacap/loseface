-- Lose Face - An open source face recognition project
-- Copyright (C) 2008-2010 David Capello
-- All rights reserved.
--
-- Description:
--   Uses a MLP array to classify a set of patterns.
--
-- Usage:
--   You can use this script directly running the following command:
--
--     loseface mlp_global.lua PATTERNS_DIR INPUTS HIDDENS SUBJECTS [NUMBER_OF_NEGATIVES] [STOP_GOAL]
--
-- Parameters:
--   PATTERNS_DIR: Directory where patterns are located
--                 ("orl_patterns" or "cidisinc_patterns")
--   INPUTS: Number of inputs for the each MLP
--   HIDDENS: Number of hidden neurons for each the MLP
--   SUBJECTS: Number of MLPs in the array (each MLP in the array has one output)
--   NUMBER_OF_NEGATIVES: Number of negative subjects to use in each training-iteration
--                        mixed with positive samples.
--                        If NUMBER_OF_NEGATIVES == 0 the default training is used.
--   STOP_GOAL: "fixed" to train 400 epochs or
--              "mse" to try to reach MSE < 1e-4
--              (default is "fixed" is you do not specify it)

-- MSE_GOAL = 1.5e-5
MSE_GOAL = 1e-4

PATTERNS_DIR = arg[1]
INPUTS = tonumber(arg[2])
HIDDENS = tonumber(arg[3])
SUBJECTS = tonumber(arg[4])
NUMBER_OF_NEGATIVES = tonumber(arg[5])
STOP_GOAL = arg[6]
if NUMBER_OF_NEGATIVES == nil then NUMBER_OF_NEGATIVES = 0 end
if STOP_GOAL == nil then STOP_GOAL = "fixed" end

print("----------------------------------------------------------------------")
print("INPUTS="..INPUTS)
print("HIDDENS="..HIDDENS)
print("NUMBER_OF_NEGATIVES = "..NUMBER_OF_NEGATIVES)
print("STOP_GOAL = "..STOP_GOAL)

LEARNING_RATE = 0.6
MOMENTUM = 0.1
INIT_WEIGHTS_MIN = -1.0
INIT_WEIGHTS_MAX =  1.0
FOLDS = 5 -- Number of folds
FIXED_EPOCHS = 400 -- Number of epochs for STOP_GOAL="fixed"
MAX_EPOCHS_FOR_MSE_GOAL = 2000 -- Maximum number of epochs when STOP_GOAL="mse"
REPEAT_EACH_FOLD = 10 -- Repeat each fold 10 times

SPECIAL_TRAINING_WHOLE_PROCESS_TIMES = 10 -- Repeat 10 times the whole special training
SPECIAL_TRAINING_SUBSET_EPOCHS = 10 -- Repeat 10 epochs each (positive+negatives) subset
                                    -- in the special training process

----------------------------------------------------------------------

function reset_mlp(mlp, seed)
  ann.init_random(seed)
  mlp:init({ min=INIT_WEIGHTS_MIN, max=INIT_WEIGHTS_MAX })
  return mlp
end

function create_mlp(seed)
  local mlp = ann.Mlp({ inputs=INPUTS, hiddens=HIDDENS, outputs=1 })
  return reset_mlp(mlp, seed)
end

----------------------------------------------------------------------

function do_train_fixed(mlp, train_set, epochs)
  return
  mlp:train({ learning_rate=LEARNING_RATE,
	      momentum=MOMENTUM,
	      set=train_set,
	      epochs=epochs,
	      shuffle=1,
	      goal=ann.BESTMSE })
end

function do_train_mse(mlp, train_set, mse, max_epochs)
  return
  mlp:train({ learning_rate=LEARNING_RATE,
	      momentum=MOMENTUM,
	      set=train_set,
	      epochs=max_epochs,
	      shuffle=1,
	      goal_mse=mse })
end

----------------------------------------------------------------------

function get_fold(k)
  local training_set = ann.PatternSet({ file=string.format("%s/%d_fold%d_training.txt", PATTERNS_DIR, INPUTS, k), inputs=INPUTS, outputs=SUBJECTS })
  local testing_set = ann.PatternSet({ file=string.format("%s/%d_fold%d_testing.txt", PATTERNS_DIR, INPUTS, k), inputs=INPUTS, outputs=SUBJECTS })
  return training_set, testing_set
end

----------------------------------------------------------------------

function prepare_positive_patterns(subject_nth, set)
  local positive_set = set:split_by_output({ subject_nth })[1]
  positive_set:set_output({ 1 })
  return positive_set
end

function prepare_negative_patterns(subject_nth, set)
  local all_others = {}
  for i=1,SUBJECTS do
    if i ~= subject_nth then
      table.insert(all_others, i)
    end
  end

  local negative_sets = set:split_by_output(all_others)

  for i=1,#negative_sets do
    negative_sets[i]:set_output({ 0 })
  end

  return negative_sets
end

----------------------------------------------------------------------

function test_array(array, set)
  local hits = 0
  local total = 0
  for subject_nth = 1,SUBJECTS do
    -- Here we call prepare_positive_patterns to get the pattern of
    -- 'subject_nth', but we do not use the 'target' for testing,
    -- so the outputs of 'positive_set' is irrelevant
    local positive_set = prepare_positive_patterns(subject_nth, set)
    local outputs = array:recall(positive_set)
    for j=1,#outputs do
      local output = outputs[j]

      -- get the max output
      local max_pos = 0
      for i=1,#output do
	--if output[i] > 0 then
	if max_pos == 0 or output[max_pos] < output[i] then
	  max_pos = i
	end
	--end
      end

      if max_pos == subject_nth then
	hits = hits+1;
      end
      total = total+1;
    end
  end

  --print("   > hits/total = "..hits.."/"..total.." = "..hits/total)
  return hits/total
end

----------------------------------------------------------------------

local total_train = 0
local total_test = 0

for i=1,FOLDS do
  local train_set, test_set = get_fold(i)

  -- Normalize patterns
  local n = ann.Normalizer(train_set)
  n:normalize(train_set, test_set)

  print(string.format("	   TRAINING BEGIN "..os.date()))

  local hits_train = {}
  local hits_test = {}
  for seed=1,REPEAT_EACH_FOLD do
    -- We create "SUBJECTS" MLP networks (one MLP to identify one subject)
    local mlps = {}
    for subject_nth = 1,SUBJECTS do
      t1 = os.date()

      -- Create a new MLP for subject 'subject_nth'
      local mlp = create_mlp(seed)

      -- Get the positive/negative patterns for this particular network
      local positive_set = prepare_positive_patterns(subject_nth, train_set)
      local negative_sets = prepare_negative_patterns(subject_nth, train_set)

      local fullmix = ann.PatternSet()
      fullmix:merge(positive_set)
      for i=1,#negative_sets do
	fullmix:merge(negative_sets[i])
      end

      local epochs = 0
      local adjustements = 0

      ----------------------------------------------------------------------
      -- basic training
      if NUMBER_OF_NEGATIVES == 0 then
	if STOP_GOAL == "fixed" then
	  epochs = do_train_fixed(mlp, fullmix, FIXED_EPOCHS)
	elseif STOP_GOAL == "mse" then
	  epochs = do_train_mse(mlp, fullmix, MSE_GOAL, MAX_EPOCHS_FOR_MSE_GOAL)
	end
	adjustements = epochs * #fullmix
      ----------------------------------------------------------------------
      -- mixing patterns 1 positive + 'NUMBER_OF_NEGATIVES' negatives
      else
	if STOP_GOAL == "fixed" then
	  for i=1,SPECIAL_TRAINING_WHOLE_PROCESS_TIMES do
	    for j=1,#negative_sets,NUMBER_OF_NEGATIVES do -- rotate negative patterns
	      local mix = ann.PatternSet()
	      mix:merge(positive_set)
	      for k=0,NUMBER_OF_NEGATIVES-1 do
		if negative_sets[j+k] == nil then break end
		mix:merge(negative_sets[j+k])
	      end
	      mix:shuffle()

	      local t = do_train_fixed(mlp, mix, SPECIAL_TRAINING_SUBSET_EPOCHS)
	      epochs = epochs + t
	      adjustements = adjustements + t * #mix
	    end
	  end
	elseif STOP_GOAL == "mse" then
	  local seed2 = seed
	  while mlp:mse(fullmix) > MSE_GOAL do

	    -- if the model has not convergence, lets initialize the weights again
	    if epochs >= 100000 then
	      epochs = 0
	      seed2 = seed2 + 1
	      reset_mlp(mlp, seed2)
	    end

	    for j=1,#negative_sets,NUMBER_OF_NEGATIVES do -- rotate negative patterns
	      local mix = ann.PatternSet()
	      mix:merge(positive_set)
	      for k=0,NUMBER_OF_NEGATIVES-1 do
		if negative_sets[j+k] == nil then break end
		mix:merge(negative_sets[j+k])
	      end
	      mix:shuffle()

	      local t = do_train_fixed(mlp, mix, 1)
	      epochs = epochs + t
	      adjustements = adjustements + t * #mix
	    end
	  end
	end
      end

      ----------------------------------------------------------------------

      t2 = os.date()
      print(string.format("	 MLP#%02d MSE=%.16g\t(%s-%s) epochs=%d (adjustements=%d)",
			  subject_nth, mlp:mse(fullmix), t1, t2, epochs, adjustements))

      table.insert(mlps, mlp)
    end
    print(string.format("    TRAINING END "..os.date()))

    -- create the array of networks
    local array = ann.MlpArray(mlps)
    --print("  test_array(train_set)")
    table.insert(hits_train, test_array(array, train_set))
    --print("  test_array(test_set)")
    table.insert(hits_test, test_array(array, test_set))

    print("  RUN#"..seed.." Hits TRAIN="..hits_train[#hits_train].." TEST="..hits_test[#hits_test])

    -- -- save the array of networks
    -- for j = 1,SUBJECTS do
    --	array:save(string.format("_saved_models_%d/cross%d_run%02d_net%02d.txt", TRAINING_TYPE, i, seed, j))
    -- end
  end

  local accum_train = 0
  local accum_test = 0
  for j=1,#hits_train do accum_train = accum_train + hits_train[j] end
  for j=1,#hits_test do accum_test = accum_test + hits_test[j] end
  print("Hits TRAIN="..(accum_train / #hits_train).." TEST="..(accum_test / #hits_test).." ("..#hits_test.." runs)")

  total_train = total_train + (accum_train / #hits_train)
  total_test = total_test + (accum_test / #hits_test)
end

print("AVG TRAIN="..(total_train/FOLDS).." TEST="..(total_test/FOLDS))
