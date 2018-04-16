core.control.on_skill_action("found_party", function(branch_name, country_state)

  local influence = country_state.values["political_influence"]

  influence = influence + 0.1
  if influence > 20 then influence = 20 end

  country_state.values["political_influence"] = influence

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

  if (country_state.branches_activated[branch_name] == true) then

    if branch_name == "politics" then

      if core.model.data == nil then
        core.model.data = {}
        core.model.data.first_skill_points = false
      end
      local data = core.model.data

      if data.first_skill_points == false then
        if country_state.values["political_influence"] >= 5 then
          simulation:add_skill_points(30)
          data.first_skill_points = true
        end
      end

    end

  else

    local propagated = country_state.branch_values[branch_name]["propagated"]
    for _, neighbour_state in pairs(country_state.neighbour_states) do
      propagated = propagated + neighbour_state.branch_values[branch_name]["propagation"]
    end

    country_state.branch_values[branch_name]["propagated"] = propagated

    if (propagated >= 20) then
      country_state.branches_activated[branch_name] = true
    end
  end

end)