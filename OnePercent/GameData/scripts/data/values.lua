local control = lua.control

control:create_values({
  -- Individual values per country
  {
    name = "population",
    init = 0.0
  },
  {
    name = "wealth",
    init = 0.0
  },
  {
    name = "trust",
    init = 0.0,
    visible = true
  },

  -- Politics
  {
    name = "political_influence",
    init = 0.0
  },
  {
    name = "ideology",
    init = 0.0,
    visible = true
  },
  {
    name = "divisiveness",
    init = 0.0
  },
  {
    name = "education",
    init = 0.0
  },
  {
    name = "satisfaction",
    init = 0.0
  },

  -- banks
  {
    name = "dept",
    init = 0.0,
    visible = true
  },
  {
    name = "dept_p",
    init = 0.0
  },


  {
    name = "anger",
    init = 0.0
  }
})

for _, branch in pairs(model.branches) do
  control:create_values({
    {
      name = "propagation",
      init = 0.0,
      group = branch.name
    },
    {
      name = "propagated",
      init = 0.0,
      group = branch.name
    }
  })
end
