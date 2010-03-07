# Lose Face - An open source face recognition project
# Copyright (C) 2008-2009 David Capello
# All rights reserved.

function(add_loseface_test name)
  add_executable(${name} ${name}.cpp)
  target_link_libraries(${name} loseface-lib ${libs})
endfunction(add_loseface_test)

add_loseface_test(test_dist)
add_loseface_test(test_mat)
add_loseface_test(test_mean)
add_loseface_test(test_mlp)
add_loseface_test(test_perf)
