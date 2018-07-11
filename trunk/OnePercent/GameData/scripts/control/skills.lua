max = 0.4
binc = 0.02
einc = 0.0001
-- einc = 0.0000001
decay = 5

core.control.on_skill_action("found_party", function(branch_name, country_state)

  local influence = country_state.values["political_influence"]
  local max, binc, einc, decay = max, binc, einc, decay

  if influence < max then
    local raise = einc + ((binc - einc) * (1.0 - (influence / max)) ^ decay)
    influence = influence + raise
  else
    influence = influence + einc
  end

  country_state.values["political_influence"] = influence

end)

core.control.on_skill_action("canvassing", function(branch_name, country_state)

end)

core.control.on_skill_action("campaign_pledges", function(branch_name, country_state)

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
      if country_state.values["first_skill_points_politics"] == 0 and country_state.values["political_influence"] >= 5 then
        simulation:add_skill_points(30)
        country_state.values["first_skill_points_politics"] = 1
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