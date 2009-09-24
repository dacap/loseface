-- Lose Face - An open source face recognition project
-- Copyright (C) 2008-2009 David Capello
-- All rights reserved.

function divide_images_matrix(images_matrix, partitions)
  -- Preparing partition
  do
    local str_partitions = ""
    for i = 1,#partitions do
      if i > 1 then
	str_partitions = str_partitions .. ", "
      end
      str_partitions = str_partitions .. partitions[i]
    end
  end

  local num_of_images = 0
  local images_for_training = {}
  local images_for_testing = {}
  local subject_for_training = {}
  local subject_for_testing = {}

  for i = 1,#images_matrix do 
    for j = 1,#images_matrix[i] do
      num_of_images = num_of_images + 1
    end
  end

  for i = 1,#images_matrix do
    local ibeg = 1
    local iend
    local images_in_parts

    for j = 1,#partitions do
      images_in_parts = #images_matrix[i] * partitions[j] / 100

      iend = ibeg + math.floor(images_in_parts)
      if images_in_parts - math.floor(images_in_parts) < 0.5 then
	iend = iend - 1
      end

      if (j % 2) == 1 then
	-- print(string.format("  Subject#%02d -- ibeg=%d iend=%d", i, ibeg, iend))

	for k = ibeg,iend do
	  -- print(images_matrix[i][k])
	  -- print(string.format("  Subject#%02d Image#%02d for Training", i, k))
	  table.insert(images_for_training, images_matrix[i][k])
	  table.insert(subject_for_training, i)
	end
      else
	-- print(string.format("  Subject#%02d -- ibeg=%d iend=%d   TEST", i, ibeg, iend))

	for k = ibeg,iend do
	  -- print("k="..k)
	  -- print(images_matrix[i][k])
	  -- print(string.format("  Subject#%02d Image#%02d for Testing", i, k))
	  table.insert(images_for_testing, images_matrix[i][k])
	  table.insert(subject_for_testing, i)
	end
      end
      ibeg = iend + 1
    end
  end

  -- Throw an error if some images were lost in the process (this is
  -- only a check as an assert, if this validation fails, it means
  -- that the above code is just wrong)
  if num_of_images ~= (#images_for_training + #images_for_testing) then
    print("divide_images_matrix failed!")
    print("  - num_of_images = " .. num_of_images)
    print("  - images_for_training = " .. #images_for_training)
    print("  - images_for_testing = " .. #images_for_testing)
    os.exit()
  end

  return num_of_images,
         images_for_training, images_for_testing,
         subject_for_training, subject_for_testing
end
