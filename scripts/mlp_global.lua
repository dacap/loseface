-- Lose Face - An open source face recognition project
-- Copyright (C) 2008-2010 David Capello
-- All rights reserved.
--
-- Description:
--   Uses a common MLP of 3 layers to classify a set of patterns.
--
-- Usage:
--   You can use this script directly running the following command:
--
--     loseface mlp_global.lua PATTERNS_DIR INPUTS HIDDENS SUBJECTS [STOP_GOAL]
--
-- Parameters:
--   PATTERNS_DIR: Directory where patterns are located
--                 ("orl_patterns" or "cidisinc_patterns")
--   INPUTS: Number of inputs for the MLP
--   HIDDENS: Number of hidden neurons for the MLP
--   SUBJECTS: Number of outputs for the MLP
--   STOP_GOAL: "fixed" to train 400 epochs or
--              "mse" to try to reach MSE < 2.5e-4
--              (default is "fixed" is you do not specify it)

MSE_GOAL = 2.5e-4
-- MSE_GOAL = 0.1

PATTERNS_DIR = arg[1]
INPUTS = tonumber(arg[2])
HIDDENS = tonumber(arg[3])
SUBJECTS = tonumber(arg[4])
STOP_GOAL = arg[5]
if STOP_GOAL == nil then STOP_GOAL = "fixed" end

print("----------------------------------------------------------------------")
print("INPUTS="..INPUTS)
print("HIDDENS="..HIDDENS)
print("STOP_GOAL = "..STOP_GOAL)

LEARNING_RATE = 0.6
MOMENTUM = 0.1
INIT_WEIGHTS_MIN = -1.0
INIT_WEIGHTS_MAX =  1.0
FOLDS = 5 -- Number of folds to use
FIXED_EPOCHS = 400 -- Number of epochs for STOP_GOAL="fixed"
MAX_EPOCHS_FOR_MSE_GOAL = 4000 -- Maximum number of epochs when STOP_GOAL="mse"
REPEAT_EACH_FOLD = 10 -- Repeat each fold 10 times

----------------------------------------------------------------------

function create_mlp(seed)
  ann.init_random({ seed=seed })

  local mlp = ann.Mlp({ inputs=INPUTS, hiddens=HIDDENS, outputs=SUBJECTS })
  mlp:init({ min=INIT_WEIGHTS_MIN, max=INIT_WEIGHTS_MAX })

  return mlp
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

----------------------------------------------------------------------

function test_mlp(mlp, set)
  local hits = 0
  local total = 0
  for subject_nth = 1,SUBJECTS do
    -- Here we call prepare_positive_patterns to get the pattern of
    -- 'subject_nth', but we do not use the 'target' for testing,
    -- so the outputs of 'positive_set' is irrelevant
    local positive_set = prepare_positive_patterns(subject_nth, set)
    local outputs = mlp:recall(positive_set)
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

  local hits_train = {}
  local hits_test = {}
  for seed=1,REPEAT_EACH_FOLD do
    t1 = os.date()

    mlp = create_mlp(seed)

    local epochs = 0
    if STOP_GOAL == "fixed" then
      epochs = do_train_fixed(mlp, train_set, FIXED_EPOCHS)
    elseif STOP_GOAL == "mse" then
      epochs = do_train_mse(mlp, train_set, MSE_GOAL, MAX_EPOCHS_FOR_MSE_GOAL)
    end

    t2 = os.date()

    table.insert(hits_train, test_mlp(mlp, train_set))
    table.insert(hits_test, test_mlp(mlp, test_set))

    print(string.format("  RUN#%02d MSE=%.16g Hits TRAIN=%.16g TEST=%.16g\t(%s-%s) epochs=%d",
			seed, mlp:mse(train_set),
			hits_train[#hits_train],
			hits_test[#hits_test], t1, t2, epochs))
  end

  local accum_train = 0
  local accum_test = 0
  for j=1,#hits_train do accum_train = accum_train + hits_train[j] end
  for j=1,#hits_test do accum_test = accum_test + hits_test[j] end
  print("Hits TRAIN="..(accum_train / #hits_train).." TEST="..(accum_test / #hits_test).." ("..#hits_test.." runs) "..os.date())

  total_train = total_train + (accum_train / #hits_train)
  total_test = total_test + (accum_test / #hits_test)
end

print("AVG TRAIN="..(total_train/FOLDS).." TEST="..(total_test/FOLDS))
