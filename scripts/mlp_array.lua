-- Lose Face - An open source face recognition project
-- Copyright (C) 2008-2009 David Capello
-- All rights reserved.

function main(INPUTS, HIDDENS)
  print("----------------------------------------------------------------------")
  print("INPUTS="..INPUTS..", HIDDENS="..HIDDENS)

  LEARNING_RATE = 0.6
  MOMENTUM = 0.1
  -- INPUTS = 50
  -- HIDDENS = 50
  SUBJECTS = 40
  INIT_WEIGHTS_MIN = -1.0
  INIT_WEIGHTS_MAX =  1.0

  CROSS_VALIDATION = { { 80, 20,  0 },
		       { 60, 20, 20 },
		       { 40, 20, 40 },
		       { 20, 20, 60 },
		       {  0, 20, 80 } }

  TRAINING_BASIC = 1
  TRAINING_MIX_1POS_1NEG = 2
  TRAINING_MIX_1POS_2NEG = 3
  TRAINING_MIX_1POS_3NEG = 4
  TRAINING_TYPE = TRAINING_BASIC
  -- TRAINING_TYPE = TRAINING_MIX_1POS_1NEG

  ----------------------------------------------------------------------

  function create_mlp(seed)
    ann.init_random({ seed=seed })

    local mlp = ann.Mlp({ inputs=INPUTS, hiddens=HIDDENS, outputs=1 })
    mlp:init({ min=INIT_WEIGHTS_MIN, max=INIT_WEIGHTS_MAX })

    return mlp
  end

  ----------------------------------------------------------------------

  function do_train(mlp, train_set, epochs)
    mlp:train({ learning_rate=LEARNING_RATE,
		momentum=MOMENTUM,
		set=train_set,
		epochs=epochs,
		shuffle=1,
		goal=ann.BESTMSE })
  end

  ----------------------------------------------------------------------

  function get_partition(partition_number)
    local training_set = ann.PatternSet({ file="orl_patterns/"..INPUTS.."_cross"..partition_number.."_training.txt", inputs=INPUTS, outputs=40 })
    local testing_set = ann.PatternSet({ file="orl_patterns/"..INPUTS.."_cross"..partition_number.."_testing.txt", inputs=INPUTS, outputs=40 })
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

    local hits_train = {}
    local hits_test = {}
    for seed=1,10 do
      -- We create 40 MLP networks (one MLP to identify one subject)
      local mlps = {}
      for subject_nth = 1,SUBJECTS do
	-- Create a new MLP for subject 'subject_nth'
	local mlp = create_mlp(seed)
	table.insert(mlps, mlp)

	-- Get the positive/negative patterns for this particular network
	local positive_set = prepare_positive_patterns(subject_nth, train_set)
	local negative_sets = prepare_negative_patterns(subject_nth, train_set)

	-- positive_set:save(string.format("orl_patterns_array/subject%02d_positive.txt", subject_nth))
	-- for i=1,#negative_sets do
	-- 	negative_sets[i]:save(string.format("orl_patterns_array/subject%02d_negative%02d.txt", subject_nth, i))
	-- end

	local fullmix = ann.PatternSet()
	fullmix:merge({ positive_set })
	fullmix:merge(negative_sets)

	----------------------------------------------------------------------
	-- basic training
	if TRAINING_TYPE == TRAINING_BASIC then
	  do_train(mlp, fullmix, 400)
	  ----------------------------------------------------------------------
	  -- mixing patterns 1 positive + 1 negative
	elseif TRAINING_TYPE == TRAINING_MIX_1POS_1NEG then
	  for i=1,10 do
	    for j=1,#negative_sets do                -- rotate negative patterns
	      local mix = ann.PatternSet()
	      mix:merge({ positive_set, negative_sets[j] })
	      mix:shuffle()
	      do_train(mlp, mix, 10)
	    end
	  end
	  ----------------------------------------------------------------------
	  -- mixing patterns 1 positive + 2 negative
	elseif TRAINING_TYPE == TRAINING_MIX_1POS_2NEG then
	  for i=1,10 do
	    local j = 0
	    for j=1,#negative_sets,2 do                -- rotate negative patterns
	      local mix = ann.PatternSet()
	      mix:merge({ positive_set, negative_sets[j], negative_sets[j+1] })
	      mix:shuffle()
	      do_train(mlp, mix, 10)
	    end
	  end
	  ----------------------------------------------------------------------
	  -- mixing patterns 1 positive + 3 negative
	elseif TRAINING_TYPE == TRAINING_MIX_1POS_3NEG then
	  for i=1,10 do
	    for j=1,#negative_sets,3 do                -- rotate negative patterns
	      local mix = ann.PatternSet()
	      mix:merge({ positive_set, negative_sets[j], negative_sets[j+1], negative_sets[j+2] })
	      mix:shuffle()
	      do_train(mlp, mix, 10)
	    end
	  end
	end
	----------------------------------------------------------------------

	print(string.format("    MLP#%02d MSE=%.16g", subject_nth, mlp:mse({ set=fullmix })))
      end

      -- create the array of networks
      local array = ann.MlpArray(mlps)
      table.insert(hits_train, test_array(array, train_set))
      table.insert(hits_test, test_array(array, test_set))

      print("  RUN#"..seed.." Hits TRAIN="..hits_train[#hits_train].." TEST="..hits_test[#hits_test])
    end

    local accum_train = 0
    local accum_test = 0
    for j=1,#hits_train do accum_train = accum_train + hits_train[j] end
    for j=1,#hits_test do accum_test = accum_test + hits_test[j] end
    print("Hits TRAIN="..(accum_train / #hits_train).." TEST="..(accum_test / #hits_test).." ("..#hits_test.." runs) "..os.date())

    total_train = total_train + (accum_train / #hits_train)
    total_test = total_test + (accum_test / #hits_test)
  end

  print("AVG TRAIN="..(total_train/#CROSS_VALIDATION).." TEST="..(total_test/#CROSS_VALIDATION))
end

main(25, 25)
main(25, 50)
main(25, 75)

main(50, 25)
main(50, 50)
main(50, 75)

main(75, 25)
main(75, 50)
main(75, 75)
