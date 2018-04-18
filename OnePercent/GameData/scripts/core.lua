core = {

  -- the underlying model
  model = 
  {
    branches = {},
    values = {},
    countries = {},
    state = {}
  },

  -- core function definitions
  control =
  {
    skill_actions = {},
    branch_actions = {},
    on_initialize_actions = {}
  }
}

function core.control.on_initialize_action(func)

  table.insert(core.control.on_initialize_actions, func)

end

function core.control.on_skill_action(skill_name, func)

  local actions = core.control.skill_actions
  if (actions[skill_name] == nil) then actions[skill_name] = {} end
  table.insert(actions[skill_name], func)

end

function core.control.on_branch_action(func)

  table.insert(core.control.branch_actions, func)

end

-- adds branches to the model and passes them to the c++ code
function core.control.create_branches(branches)

  local b = core.model.branches
  for _ , v in ipairs(branches) do
    v.skills = {}
    b[v.name] = v
  end

end

-- same here with the skills
function core.control.create_skills(skills)

  local branches = core.model.branches
  local branch

  for _, v in ipairs(skills) do
    branch = branches[v.branch]

    if branch ~= nil then
      v.activated = false
      branch.skills[v.name] = v
    else
      log:warn("Could not add skill '" .. v.name .. "'. Branch '" .. v.branch .. "' not found")
    end
  end

end

-- ..and the countries
function core.control.create_countries(countries)

  local c = core.model.countries
  local branches = core.model.branches
  for _, v in ipairs(countries) do
    v.neighbours = {}
    c[v.id] = v
  end

end

-- ..and last but not least the values
function core.control.create_values(values)

  local val = core.model.values
  for _, v in ipairs(values) do val[v.name] = v end

end

-- initializes the state for all countries with its values and branch_activated
function core.control.initialize_state()

  local branches = core.model.branches
  local countries = core.model.countries
  local values = core.model.values
  local state = core.model.state

  for cid, country in pairs(countries) do
    state[cid] = { values = {}, branch_values = {}, branches_activated = {} }

    -- branch_values and branch_activated
    for _, branch in pairs(branches) do
      state[cid].branch_values[branch.name] = {}
      state[cid].branches_activated[branch.name] = false
    end

    -- values
    for _, value in pairs(values) do
      if value.type == "default" then
        state[cid].values[value.name] = value.init
      elseif value.type == "branch" then
        for _, branch in pairs(branches) do
          state[cid].branch_values[branch.name][value.name] = value.init
        end
      else
        log:warn("Unknown value type '" .. value.type .. "'")
      end
    end

  end

end

-- update neighbours data
function core.control.initialize_neighbour_states()

  local countries = core.model.countries
  local state = core.model.state

  for cid, country_state in pairs(state) do 
    country_state.neighbour_states = {}
    for _, neighbour_id in ipairs(countries[cid].neighbours) do
      country_state.neighbour_states[neighbour_id] = state[neighbour_id]
    end
  end

end

function core.control.perform_on_initialize_actions()

  for _, func in ipairs(core.control.on_initialize_actions) do func() end

end

-- calls the skill_actions functions for each country
-- and skill that is activated in an active branch
function core.control.update_skills_func()

  local branches = core.model.branches
  local countries = core.model.countries
  local state = core.model.state
  local skill_actions = core.control.skill_actions
  local country_state

  for cid, country in pairs(countries) do

    for _, branch in pairs(branches) do
      for _, skill in pairs(branch.skills) do
        country_state = state[cid]

        if (skill.activated == true and country_state.branches_activated[skill.branch] == true) then
          actions = skill_actions[skill.name]

          if (actions ~= nil) then
            for _, func in ipairs(actions) do
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
function core.control.update_branches_func()

  local branches = core.model.branches
  local countries = core.model.countries
  local state = core.model.state
  local branch_actions = core.control.branch_actions

  for cid, country in pairs(countries) do
    for _, branch in pairs(branches) do
      for _, func in ipairs(branch_actions) do
        func(branch.name, state[cid])
      end
    end
  end
  
end