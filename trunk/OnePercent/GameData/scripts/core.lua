core = {

  -- the underlying model
  model = 
  {
    branches = {},
    skills = {},
    values = {},
    countries = {},
    
    skills_name_map = {}
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
      for k,v in pairs(branches) do global_branches[k] = v end

      skillsContainer:add_branches(branches)

    end),

    -- same here with the skills
    create_skills = (function(skills)

      local global_skills = core.model.skills
      local map = core.model.skills_name_map
      for k,v in pairs(skills) do
        global_skills[k] = v
        map[v.name] = k

        v.activated = false
      end

      skillsContainer:add_skills(skills)

    end),

    -- ..and the countries
    create_countries = (function(countries)

      local global_countries = core.model.countries
      local branches = core.model.branches
      for k,v in pairs(countries) do
        global_countries[v.id] = v
        v.branch_activated = {}

        for _, branch in pairs(branches) do
          v.branch_activated[branch.name] = false
        end
      end

      simulation:add_countries(countries)

    end),

    -- ..and last but not least the values
    create_values = (function(values)

      local global_values = core.model.values
      for k,v in pairs(values) do global_values[k] = v end

      valuesContainer:add_values(values)

    end),

    set_skill_activated = (function(skill_name, activated)

      core.model.skills[core.model.skills_name_map[skill_name]].activated = activated

    end),

    set_branch_activated = (function(country_id, branch_name, activated)

      core.model.countries[country_id].branch_activated[branch_name] = activated

    end),

    -- calls the skill_actions functions for each country
    -- and skill that is activated in an active branch
    update_skills_func = (function(state, new_state)

      local countries = core.model.countries
      local skills = core.model.skills
      local skill_actions = core.control.skill_actions
      local country_state, new_country_state, neighbourship, actions

      for id, country in pairs(countries) do

        country_state = state:get_country_state(id)
        new_country_state = new_state:get_country_state(id)
        neighbourship = neighbourshipsContainer:get_neighbourship(id)

        for _, skill in ipairs(skills) do
          if (skill.activated == true and country.branch_activated[skill.branch] == true) then
            actions = skill_actions[skill.name]

            if (actions ~= nil) then
              country_state:set_current_branch(skill.branch)
              new_country_state:set_current_branch(skill.branch)
              neighbourship:set_current_branch(skill.branch)

              for _, func in ipairs(actions) do
                func(country_state, neighbourship, new_country_state)
              end
            end
          end
        end
      end

    end),

    -- calls the branch_actions functions for each country
    -- and each branch
    update_branches_func = (function(state, new_state)

      local countries = core.model.countries
      local branches = core.model.branches
      local actions = core.control.branch_actions
      local country_state, new_country_state, neighbourship

      for id, country in pairs(countries) do

        country_state = state:get_country_state(id)
        new_country_state = new_state:get_country_state(id)
        neighbourship = neighbourshipsContainer:get_neighbourship(id)

        for _, branch in ipairs(branches) do

          country_state:set_current_branch(branch.name)
          new_country_state:set_current_branch(branch.name)
          neighbourship:set_current_branch(branch.name)

          for _, func in ipairs(actions) do
            func(country_state, neighbourship, new_country_state)
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
