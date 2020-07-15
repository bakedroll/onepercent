-- core function definitions
core = {}

-- calls the skill update functions for each country
-- and skill that is activated in an active branch
function core.update_skills_func()

  local branches = model.branches
  local countries = model.countries
  local state = model.state
  local country_state
  local a

  for cid, country in pairs(countries) do

    for _, branch in pairs(branches) do
      for _, skill in pairs(branch.skills) do
        country_state = state[cid]

        if (skill.activated == true and country_state.branches_activated[skill.branch] == true) then
          skill:update(branch.name, country_state)
        end
      end
    end

  end

end

-- calls the branch callback functions for each country
-- and each branch
function core.update_branches_func()

  local branches = model.branches
  local countries = model.countries
  local state = model.state

  for cid, country in pairs(countries) do
    for _, branch in pairs(branches) do
      control:update_branch(branch.name, state[cid])
    end
  end
  
end