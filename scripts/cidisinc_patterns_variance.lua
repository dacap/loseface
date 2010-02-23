-- Lose Face - An open source face recognition project
-- Copyright (C) 2008-2010 David Capello
-- All rights reserved.
--
-- Description:
--   This script show you how many eigenfaces you need for 75%, 80%,
--   85% and 90% of variance using the CIDISINC database.
--
-- Usage:
--   You can use this script directly running the following command:
--
--     loseface cidisinc_patterns_variance.lua
--


dofile("calculate_from_variance.lua")
dofile("cidisinc_images_matrix.lua")

images_matrix = load_cidisinc_images_matrix()
partitions = { 100, 0, 0 }
percentages = { 75, 80, 85, 90 }
results = calculate_eigenfaces_for_given_variance(images_matrix, partitions, percentages)
for i = 1,#results do
  print(results[i].." components for "..percentages[i].."% of variance")
end
