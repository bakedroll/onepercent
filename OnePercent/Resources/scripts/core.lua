-- core function definitions
core = {}

-- calls the skill_actions functions for each country
-- and skill that is activated in an active branch
function core.update_skills_func()

  local branches = model.branches
  local countries = model.countries
  local state = model.state
  local skill_actions = actions.on_skill_update
  local country_state
  local a

  for cid, country in pairs(countries) do

    for _, branch in pairs(branches) do
      for _, skill in pairs(branch.skills) do
        country_state = state[cid]

        if (skill.activated == true and country_state.branches_activated[skill.branch] == true) then
          a = skill_actions[skill.name]

          if (a ~= nil) then
            for _, func in ipairs(a) do
              func(branch.name, country_state)
            end
          end
        end
      end
    end

  end

end

-- calls the branch_actions functions for each country
-- and each branch
function core.update_branches_func()

  local branches = model.branches
  local countries = model.countries
  local state = model.state
  local branch_actions = actions.on_branch_update

  for cid, country in pairs(countries) do
    for _, branch in pairs(branches) do
      for _, func in ipairs(branch_actions) do
        func(branch.name, state[cid])
      end
    end
  end
  
end