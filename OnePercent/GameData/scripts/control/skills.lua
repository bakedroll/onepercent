core.control.on_skill_action("interest_1", function(branch_name, country_state)

  country_state.values["interest"] = country_state.values["interest"] + 0.1

end)

core.control.on_skill_action("propagation_banks", function(branch_name, country_state)

  country_state.branch_values[branch_name]["propagation"] = 1.0

end)

core.control.on_skill_action("propagation_control", function(branch_name, country_state)

  country_state.branch_values[branch_name]["propagation"] = 1.0

end)

core.control.on_skill_action("propagation_media", function(branch_name, country_state)

  country_state.branch_values[branch_name]["propagation"] = 1.0

end)

core.control.on_skill_action("propagation_concerns", function(branch_name, country_state)

  country_state.branch_values[branch_name]["propagation"] = 1.0

end)

core.control.on_skill_action("propagation_politics", function(branch_name, country_state)

  country_state.branch_values[branch_name]["propagation"] = 1.0

end)

core.control.on_branch_action(function(branch_name, country_state)

  if (country_state.branches_activated[branch_name] == true) then return end

  local propagated = country_state.branch_values[branch_name]["propagated"]
  for _, neighbour_state in pairs(country_state.neighbour_states) do
    propagated = propagated + neighbour_state.branch_values[branch_name]["propagation"]
  end

  country_state.branch_values[branch_name]["propagated"] = propagated

  if (propagated >= 20) then
    country_state.branches_activated[branch_name] = true
  end

end)