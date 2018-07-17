-- the underlying model
model = 
{
  branches = {},
  values = {},
  countries = {},
  state = {}
}

-- core function definitions
control =
{
  skill_actions = {},
  branch_actions = {},
  tick_actions = {},
  on_initialize_actions = {}
}

config = {}


function config.extend(table)

  helper.merge(config, table)

end

function control.on_initialize_action(func)

  table.insert(control.on_initialize_actions, func)

end

function control.on_skill_action(skill_name, func)

  local actions = control.skill_actions
  if (actions[skill_name] == nil) then actions[skill_name] = {} end
  table.insert(actions[skill_name], func)

end

function control.on_branch_action(func)

  table.insert(control.branch_actions, func)

end

function control.on_tick_action(func)

  table.insert(control.tick_actions, func)

end

-- adds branches to the model and passes them to the c++ code
function control.create_branches(branches)

  local b = model.branches
  for _, v in ipairs(branches) do
    v.skills = {}
    b[v.name] = v
  end

end

-- same here with the skills
function control.create_skills(skills)

  local branches = model.branches
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
function control.create_countries(countries)

  local c = model.countries
  local branches = model.branches
  for _, v in ipairs(countries) do
    v.neighbours = {}
    c[v.id] = v
  end

end

-- ..and last but not least the values
function control.create_values(values)

  local val = model.values
  for _, v in ipairs(values) do val[v.name] = v end

end

-- initializes the state for all countries with its values and branch_activated
function control.initialize_state()

  local branches = model.branches
  local countries = model.countries
  local values = model.values
  local state = model.state
  local init

  for cid, country in pairs(countries) do
    state[cid] = { values = {}, branch_values = {}, branches_activated = {} }
    if country.init_values == nil then country.init_values = {} end

    -- branch_values and branch_activated
    for _, branch in pairs(branches) do
      state[cid].branch_values[branch.name] = {}
      state[cid].branches_activated[branch.name] = false
    end

    -- values
    for _, value in pairs(values) do
      init = country.init_values[value.name] or value.init

      if value.type == "default" then
        state[cid].values[value.name] = init
      elseif value.type == "branch" then
        for _, branch in pairs(branches) do
          state[cid].branch_values[branch.name][value.name] = init
        end
      else
        log:warn("Unknown value type '" .. value.type .. "'")
      end
    end

    -- free memory
    country.init_values = nil

  end

end

-- update neighbours data
function control.initialize_neighbour_states()

  local countries = model.countries
  local state = model.state

  for cid, country_state in pairs(state) do 
    country_state.neighbour_states = {}
    for _, neighbour_id in ipairs(countries[cid].neighbours) do
      country_state.neighbour_states[neighbour_id] = state[neighbour_id]
    end
  end

end

function control.perform_on_initialize_actions()

  for _, func in ipairs(control.on_initialize_actions) do func() end

end

function control.update_tick_func()

  for _, func in ipairs(control.tick_actions) do func() end

end

-- calls the skill_actions functions for each country
-- and skill that is activated in an active branch
function control.update_skills_func()

  local branches = model.branches
  local countries = model.countries
  local state = model.state
  local skill_actions = control.skill_actions
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
function control.update_branches_func()

  local branches = model.branches
  local countries = model.countries
  local state = model.state
  local branch_actions = control.branch_actions

  for cid, country in pairs(countries) do
    for _, branch in pairs(branches) do
      for _, func in ipairs(branch_actions) do
        func(branch.name, state[cid])
      end
    end
  end
  
end