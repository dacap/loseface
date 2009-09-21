-- Lose Face - An open source face recognition project
-- Copyright (C) 2008-2009 David Capello
-- All rights reserved.

-- MSE_GOAL = 1.5e-5
MSE_GOAL = 1e-4

number_of_negatives = tonumber(arg[4])
if number_of_negatives == nil then number_of_negatives = 0 end

stop_goal = arg[5]
if stop_goal == nil then stop_goal = "fixed" end

print("number_of_negatives = "..number_of_negatives)
print("stop_goal = "..stop_goal)

function mlp_array(patterns_dir, INPUTS, HIDDENS)
  print("----------------------------------------------------------------------")
  print("INPUTS="..INPUTS..", HIDDENS="..HIDDENS)

  LEARNING_RATE = 0.6
  MOMENTUM = 0.1
  SUBJECTS = 40
  INIT_WEIGHTS_MIN = -1.0
  INIT_WEIGHTS_MAX =  1.0

  CROSS_VALIDATION = { { 80, 20,  0 },
		       { 60, 20, 20 },
		       { 40, 20, 40 },
		       { 20, 20, 60 },
		       {  0, 20, 80 } }

  ----------------------------------------------------------------------

  function create_mlp(seed)
    ann.init_random({ seed=seed })

    local mlp = ann.Mlp({ inputs=INPUTS, hiddens=HIDDENS, outputs=1 })
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

  function get_partition(partition_number)
    local training_set = ann.PatternSet({ file=string.format("%s/%d_cross%d_training.txt", patterns_dir, INPUTS, partition_number), inputs=INPUTS, outputs=SUBJECTS })
    local testing_set = ann.PatternSet({ file=string.format("%s/%d_cross%d_testing.txt", patterns_dir, INPUTS, partition_number), inputs=INPUTS, outputs=SUBJECTS })
    return training_set, testing_set
  end

  ----------------------------------------------------------------------

  function prepare_positive_patterns(subject_nth, set)
    local positive_set = set:split({ byoutput={ subject_nth } })[1]
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

    local negative_sets = set:split({ byoutput=all_others })

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
      local outputs = array:recall({ set=positive_set })
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

  for i=1,#CROSS_VALIDATION do
    local train_set, test_set = get_partition(i)

    -- Normalize patterns
    local n = ann.Normalizer({ type=ann.MINMAX, set=train_set })
    n:normalize({ train_set, test_set })

    print(string.format("    TRAINING BEGIN "..os.date()))

    local hits_train = {}
    local hits_test = {}
    for seed=1,10 do
      -- We create "SUBJECTS" MLP networks (one MLP to identify one subject)
      local mlps = {}
      for subject_nth = 1,SUBJECTS do
	t1 = os.date()

	-- Create a new MLP for subject 'subject_nth'
	local mlp = create_mlp(seed)
	table.insert(mlps, mlp)

	-- Get the positive/negative patterns for this particular network
	local positive_set = prepare_positive_patterns(subject_nth, train_set)
	local negative_sets = prepare_negative_patterns(subject_nth, train_set)

	local fullmix = ann.PatternSet()
	fullmix:merge({ positive_set })
	fullmix:merge(negative_sets)

	local epochs = 0
	local adjustements = 0

	----------------------------------------------------------------------
	-- basic training
	if number_of_negatives == 0 then
	  if stop_goal == "fixed" then
	    epochs = do_train_fixed(mlp, fullmix, 400)
	  elseif stop_goal == "mse" then
	    epochs = do_train_mse(mlp, fullmix, MSE_GOAL, 2000)
	  end
	  adjustements = epochs * #fullmix
	----------------------------------------------------------------------
	-- mixing patterns 1 positive + 'number_of_negatives' negatives
	else
	  if stop_goal == "fixed" then
	    for i=1,10 do
	      for j=1,#negative_sets,number_of_negatives do -- rotate negative patterns
		local mix = ann.PatternSet()
		mix:merge({ positive_set })
		for k=0,number_of_negatives-1 do
		  if negative_sets[j+k] == nil then break end
		  mix:merge({ negative_sets[j+k] })
		end
		mix:shuffle()

		local t = do_train_fixed(mlp, mix, 10)
		epochs = epochs + t
		adjustements = adjustements + t * #mix
	      end
	    end
	  elseif stop_goal == "mse" then
	    while mlp:mse({ set=fullmix }) > MSE_GOAL and epochs < 100000 do
	      for j=1,#negative_sets,number_of_negatives do -- rotate negative patterns
		local mix = ann.PatternSet()
		mix:merge({ positive_set })
		for k=0,number_of_negatives-1 do
		  if negative_sets[j+k] == nil then break end
		  mix:merge({ negative_sets[j+k] })
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
	print(string.format("    MLP#%02d MSE=%.16g\t(%s-%s) epochs=%d (adjustements=%d)",
			    subject_nth, mlp:mse({ set=fullmix }), t1, t2, epochs, adjustements))
      end
      print(string.format("    TRAINING END "..os.date()))

      -- create the array of networks
      local array = ann.MlpArray(mlps)
      table.insert(hits_train, test_array(array, train_set))
      table.insert(hits_test, test_array(array, test_set))

      print("  RUN#"..seed.." Hits TRAIN="..hits_train[#hits_train].." TEST="..hits_test[#hits_test])

      -- -- save the array of networks
      -- for j = 1,SUBJECTS do
      -- 	array:save({ file=string.format("_saved_models_%d/cross%d_run%02d_net%02d.txt", TRAINING_TYPE, i, seed, j) })
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

  print("AVG TRAIN="..(total_train/#CROSS_VALIDATION).." TEST="..(total_test/#CROSS_VALIDATION))
end

mlp_array(arg[1], tonumber(arg[2]), tonumber(arg[3]))
