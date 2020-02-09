lua.control:create_values({
  -- Individual values per country
  {
    name = "population",
    type = "default",
    init = 0.0
  },
  {
    name = "wealth",
    type = "default",
    init = 0.0
  },
  {
    name = "trust",
    type = "default",
    init = 0.0,
    visible = true
  },

  -- Politics
  {
    name = "political_influence",
    type = "default",
    init = 0.0
  },
  {
    name = "ideology",
    type = "default",
    init = 0.0,
    visible = true
  },
  {
    name = "divisiveness",
    type = "default",
    init = 0.0
  },
  {
    name = "education",
    type = "default",
    init = 0.0
  },
  {
    name = "satisfaction",
    type = "default",
    init = 0.0
  },

  -- banks
  {
    name = "dept",
    type = "default",
    init = 0.0,
    visible = true
  },
  {
    name = "dept_p",
    type = "default",
    init = 0.0
  },


  {
    name = "anger",
    type = "default",
    init = 0.0
  },
  {
    name = "propagated",
    type = "branch",
    init = 0.0
  },
  {
    name = "propagation",
    type = "branch",
    init = 0.0
  }
})