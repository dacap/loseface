-- Lose Face - An open source face recognition project
-- Copyright (C) 2008-2010 David Capello
-- All rights reserved.
--
-- Description:
--   This script converts the ORL images to points in an eigenspace.
--   These points (patterns) can be used as input for a MLP neural net.
--   The faces are loaded from orl_faces/ directory, and patterns are
--   saved in orl_patterns/ directory.
--
-- Usage:
--   You can use this script directly running the following command:
--
--     loseface orl_patterns.lua
--

dofile("create_patterns.lua")
dofile("orl_images_matrix.lua")

-- create_patterns2:
--   Creates patterns using different partitions (folds) for training/test images,
--   this is necessary to do k-fold cross-validation later
function create_patterns2(images_matrix, inputs)
  create_patterns(images_matrix, inputs, {80,20, 0}, "orl_patterns/"..inputs.."_fold1")
  create_patterns(images_matrix, inputs, {60,20,20}, "orl_patterns/"..inputs.."_fold2")
  create_patterns(images_matrix, inputs, {40,20,40}, "orl_patterns/"..inputs.."_fold3")
  create_patterns(images_matrix, inputs, {20,20,60}, "orl_patterns/"..inputs.."_fold4")
  create_patterns(images_matrix, inputs, { 0,20,80}, "orl_patterns/"..inputs.."_fold5")
end

images_matrix = load_orl_images_matrix()
create_patterns2(images_matrix, 25)
create_patterns2(images_matrix, 50)
create_patterns2(images_matrix, 75)
