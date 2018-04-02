core.control.on_skill_action("interest_1", function(state, neighbourship, new_state)

  local interest = state:get_value("interest")
  local new_interest = interest:get() + 0.1

  new_state:get_value("interest"):set(new_interest)

end)

core.control.on_skill_action("propagation_banks", function(state, neighbourship, new_state)

  new_state:get_branch_value("propagation"):set(1.0)

end)

core.control.on_skill_action("propagation_control", function(state, neighbourship, new_state)

  new_state:get_branch_value("propagation"):set(1.0)

end)

core.control.on_skill_action("propagation_media", function(state, neighbourship, new_state)

  new_state:get_branch_value("propagation"):set(1.0)

end)

core.control.on_skill_action("propagation_concerns", function(state, neighbourship, new_state)

  new_state:get_branch_value("propagation"):set(1.0)

end)

core.control.on_skill_action("propagation_politics", function(state, neighbourship, new_state)

  new_state:get_branch_value("propagation"):set(1.0)

end)

core.control.on_branch_action(function(state, neighbourship, new_state)

  if (state:get_branch_activated()) then return end

  local nneighbours = neighbourship:get_num_neighbours()
  local propagated = state:get_branch_value("propagated"):get()
  local neighbourstate

  for i=0,nneighbours-1 do
    neighbourstate = neighbourship:get_neighbour_state(i)
    propagated = propagated + neighbourstate:get_branch_value("propagation"):get()
  end

  new_state:get_branch_value("propagated"):set(propagated)

  if (propagated >= 20) then
    new_state:set_branch_activated(true)
  end

end)