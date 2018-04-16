local values = {
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