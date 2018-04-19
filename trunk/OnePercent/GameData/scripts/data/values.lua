local values = {
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
    name = "anger",
    type = "default",
    init = 0.0
  },
  {
    name = "political_influence",
    type = "default",
    init = 0.0
  },
  {
    name = "first_skill_points_politics",
    type = "default",
    init = 0.0
  },
  {
    name = "ideology",
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
}

core.control.create_values(values)