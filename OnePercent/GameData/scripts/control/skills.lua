lua.control:skill("found_party"):on_event(defines.callback.on_skill_update, function(branch_name, country_state)

  local influence = country_state.values["political_influence"]
  influence = gameplay.raise_value(influence, 0.4, 0.02, 0.0001, 5)
  country_state.values["political_influence"] = influence

end)

lua.control:skill("canvassing"):on_event(defines.callback.on_skill_update, function(branch_name, country_state)

  local ideology = country_state.values["ideology"]
  local influence = country_state.values["political_influence"]

  ideology = gameplay.raise_value(ideology, influence * 0.1, 0.002, 0.0, 3)
  country_state.values["ideology"] = ideology

end)

lua.control:skill("campaign_pledges"):on_event(defines.callback.on_skill_update, function(branch_name, country_state)

  local ideology = country_state.values["ideology"]
  local divisiveness = country_state.values["divisiveness"]
  local influence = country_state.values["political_influence"]

  ideology = gameplay.raise_value(ideology, influence * 0.1, 0.003, 0.0, 1)
  country_state.values["ideology"] = ideology

  divisiveness = gameplay.raise_value(divisiveness, 0.5, 0.01, 0.002, 12)
  country_state.values["divisiveness"] = divisiveness

end)

lua.control:skill("propagation_banks"):on_event(defines.callback.on_skill_update, function(branch_name, country_state)

  country_state.values["propagation_" .. branch_name] = 1.0

end)

lua.control:skill("propagation_control"):on_event(defines.callback.on_skill_update, function(branch_name, country_state)

  country_state.values["propagation_" .. branch_name] = 1.0

end)

lua.control:skill("propagation_media"):on_event(defines.callback.on_skill_update, function(branch_name, country_state)

  country_state.values["propagation_" .. branch_name] = 1.0

end)

lua.control:skill("propagation_concerns"):on_event(defines.callback.on_skill_update, function(branch_name, country_state)

  country_state.values["propagation_" .. branch_name] = 1.0

end)

lua.control:skill("propagation_politics"):on_event(defines.callback.on_skill_update, function(branch_name, country_state)

  country_state.values["propagation_" .. branch_name] = 1.0

end)

lua.control:on_event(defines.callback.on_branch_update, function(branch_name, country_state)

  if (country_state.branches_activated[branch_name] == true) then return end

  local propagated = country_state.values["propagated_" .. branch_name]
  for _, neighbour_state in pairs(country_state.neighbour_states) do
    propagated = propagated + neighbour_state.values["propagation_" .. branch_name] * 0.005
  end

  country_state.values["propagated_" .. branch_name] = propagated

  if (propagated >= 1.0) then
    country_state.branches_activated[branch_name] = true
  end

  print(helper.dump_object(country_state.values))

end)

lua.control:on_event(defines.callback.on_branch_activated, function(branch_name, country_state)

  if (branch_name == "politics") then
    lua.simulation:add_skill_points(30)
  end

end)

lua.control:on_event(defines.callback.on_branch_purchased, function(branch_name, country_state)

  country_state.values["propagated_" .. branch_name] = 1.0
  lua.countries:set_current_overlay_branch_name(branch_name)

end)

lua.control:on_event(defines.callback.on_branch_resigned, function(branch_name, country_state)

  country_state.values[branch_name]["propagated_" .. branch_name] = 0.0

end)