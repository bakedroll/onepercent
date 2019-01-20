control:on_skill_update_action("found_party", function(branch_name, country_state)

  local influence = country_state.values["political_influence"]
  influence = gameplay.raise_value(influence, 0.4, 0.02, 0.0001, 5)
  country_state.values["political_influence"] = influence

end)

control:on_skill_update_action("canvassing", function(branch_name, country_state)

  local ideology = country_state.values["ideology"]
  local influence = country_state.values["political_influence"]

  ideology = gameplay.raise_value(ideology, influence * 0.1, 0.002, 0.0, 3)
  country_state.values["ideology"] = ideology

end)

control:on_skill_update_action("campaign_pledges", function(branch_name, country_state)

  local ideology = country_state.values["ideology"]
  local divisiveness = country_state.values["divisiveness"]
  local influence = country_state.values["political_influence"]

  ideology = gameplay.raise_value(ideology, influence * 0.1, 0.003, 0.0, 1)
  country_state.values["ideology"] = ideology

  divisiveness = gameplay.raise_value(divisiveness, 0.5, 0.01, 0.002, 12)
  country_state.values["divisiveness"] = divisiveness

end)

control:on_skill_update_action("propagation_banks", function(branch_name, country_state)

  country_state.branch_values[branch_name]["propagation"] = 1.0

end)

control:on_skill_update_action("propagation_control", function(branch_name, country_state)

  country_state.branch_values[branch_name]["propagation"] = 1.0

end)

control:on_skill_update_action("propagation_media", function(branch_name, country_state)

  country_state.branch_values[branch_name]["propagation"] = 1.0

end)

control:on_skill_update_action("propagation_concerns", function(branch_name, country_state)

  country_state.branch_values[branch_name]["propagation"] = 1.0

end)

control:on_skill_update_action("propagation_politics", function(branch_name, country_state)

  country_state.branch_values[branch_name]["propagation"] = 1.0

end)

control:on_event(defines.callback.on_branch_update, function(branch_name, country_state)

  if (country_state.branches_activated[branch_name] == true) then

    if branch_name == "politics" then
      if country_state.values["first_skill_points_politics"] == 0 and country_state.values["political_influence"] >= 5 then
        simulation:add_skill_points(30)
        country_state.values["first_skill_points_politics"] = 1
      end
    end

  else

    local propagated = country_state.branch_values[branch_name]["propagated"]
    for _, neighbour_state in pairs(country_state.neighbour_states) do
      propagated = propagated + neighbour_state.branch_values[branch_name]["propagation"] * 0.05
    end

    country_state.branch_values[branch_name]["propagated"] = propagated

    if (propagated >= 1.0) then
      country_state.branches_activated[branch_name] = true
    end
  end

end)
