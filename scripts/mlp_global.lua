-- Lose Face - An open source face recognition project
-- Copyright (C) 2008-2009 David Capello
-- All rights reserved.

MSE_GOAL = 2.5e-4

stop_goal = arg[3]
if stop_goal == nil then stop_goal = "fixed" end

print("stop_goal = "..stop_goal)

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

  ----------------------------------------------------------------------

  function create_mlp(seed)
    ann.init_random({ seed=seed })

    local mlp = ann.Mlp({ inputs=INPUTS, hiddens=HIDDENS, outputs=40 })
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
    local training_set = ann.PatternSet({ file="orl_patterns/"..INPUTS.."_cross"..partition_number.."_training.txt", inputs=INPUTS, outputs=40 })
    local testing_set = ann.PatternSet({ file="orl_patterns/"..INPUTS.."_cross"..partition_number.."_testing.txt", inputs=INPUTS, outputs=40 })
    return training_set, testing_set
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
      t1 = os.date()

      mlp = create_mlp(seed)

      local epochs = 0
      if stop_goal == "fixed" then
	epochs = do_train_fixed(mlp, train_set, 400)
      elseif stop_goal == "mse" then
	epochs = do_train_mse(mlp, train_set, MSE_GOAL, 4000)
      end

      t2 = os.date()

      table.insert(hits_train, mlp:test({ set=train_set }))
      table.insert(hits_test, mlp:test({ set=test_set }))

      print(string.format("  RUN#%02d MSE=%.16g Hits TRAIN=%.16g TEST=%.16g\t(%s-%s) epochs=%d",
			  seed, mlp:mse({ set=train_set }),
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

  print("AVG TRAIN="..(total_train/#CROSS_VALIDATION).." TEST="..(total_test/#CROSS_VALIDATION))
end

main(tonumber(arg[1]), tonumber(arg[2]))
