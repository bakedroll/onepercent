core = {

  -- the underlying model
  model = 
  {
    branches = {},
    values = {},
    countries = {},
    state = {},

    branches_name_map = {}
  },

  -- core function definitions
  control =
  {
    skill_actions = {},
    branch_actions = {},

    on_skill_action = (function(skill_name, func)

      local actions = core.control.skill_actions
      if (actions[skill_name] == nil) then actions[skill_name] = {} end
      table.insert(actions[skill_name], func)

    end),

    on_branch_action = (function(func)

      table.insert(core.control.branch_actions, func)

    end),

    -- adds branches to the model and passes them to the c++ code
    create_branches = (function(branches)

      local global_branches = core.model.branches
      local map = core.model.branches_name_map
      for _ , v in ipairs(branches) do
        v.skills = {}
        table.insert(global_branches, v)
        map[v.name] = #global_branches
      end

    end),

    -- same here with the skills
    create_skills = (function(skills)

      local branches = core.model.branches
      local branches_name_map = core.model.branches_name_map
      local branch

      for _, v in ipairs(skills) do
        branch = branches[branches_name_map[v.branch]]

        if branch ~= nil then
          v.activated = false
          table.insert(branch.skills, v)
        else
          log:warn("Could not add skill '" .. v.name .. "'. Branch '" .. v.branch .. "' not found")
        end
      end

    end),

    -- ..and the countries
    create_countries = (function(countries)

      local global_countries = core.model.countries
      local branches = core.model.branches
      for k, v in pairs(countries) do
        v.neighbours = {}
        global_countries[v.id] = v
      end

    end),

    -- ..and last but not least the values
    create_values = (function(values)

      local global_values = core.model.values
      for _, v in ipairs(values) do table.insert(global_values, v) end

    end),

    -- initializes the state for all countries with its values and branch_activated
    initialize_state = (function()

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

    end),

    -- update neighbours data
    initialize_neighbour_states = (function()

      local countries = core.model.countries
      local state = core.model.state

      for cid, country_state in pairs(state) do 
        country_state.neighbour_states = {}
        for _, neighbour_id in ipairs(countries[cid].neighbours) do
          country_state.neighbour_states[neighbour_id] = state[neighbour_id]
        end
      end

    end),

    -- calls the skill_actions functions for each country
    -- and skill that is activated in an active branch
    update_skills_func = (function()

      local branches = core.model.branches
      local countries = core.model.countries
      local state = core.model.state
      local skill_actions = core.control.skill_actions
      local country_state

      for cid, country in pairs(countries) do

        for _, branch in ipairs(branches) do
          for _, skill in ipairs(branch.skills) do
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

    end),

    -- calls the branch_actions functions for each country
    -- and each branch
    update_branches_func = (function()

      local branches = core.model.branches
      local countries = core.model.countries
      local state = core.model.state
      local branch_actions = core.control.branch_actions

      for cid, country in pairs(countries) do
        for _, branch in ipairs(branches) do
          for _, func in ipairs(branch_actions) do
            func(branch.name, state[cid])
          end
        end
      end
      
    end)
  },

  -- contains functions for debug purposes e.g. printing values
  helper = 
  {
    -- dumping table to a string
    dump_object = (function(o)

      if type(o) == 'table' then
        local s = '{ '
        for k,v in pairs(o) do
          if type(k) ~= 'number' then k = '"'..k..'"' end
          s = s .. '['..k..'] = ' .. core.helper.dump_object(v) .. ','
        end
        return s .. '} '
      else
        return tostring(o)
      end

    end),

    -- copies a table value by value
    deepcopy = (function(orig)

      local orig_type = type(orig)
      local copy
      if orig_type == 'table' then
          copy = {}
          for orig_key, orig_value in next, orig, nil do
              copy[core.helper.deepcopy(orig_key)] = core.helper.deepcopy(orig_value)
          end
          setmetatable(copy, core.helper.deepcopy(getmetatable(orig)))
      else -- number, string, boolean, etc
          copy = orig
      end
      return copy

    end)
  }
}
