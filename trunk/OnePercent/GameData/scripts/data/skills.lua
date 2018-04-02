local skills = {
  {
    name = "interest_1",
    displayName = "INTEREST I",
    type = "passive",
    branch = "banks",
    cost = 2
  },
  {
    name = "interest_2",
    displayName = "INTEREST II",
    type = "passive",
    branch = "banks",
    cost = 5
  },
  {
    name = "anger_test",
    displayName = "ANGER TEST",
    type = "passive",
    branch = "banks",
    cost = 5
  },
  {
    name = "propagation_banks",
    displayName = "propagation banks",
    type = "passive",
    branch = "banks",
    cost = 10
  },
  {
    name = "propagation_control",
    displayName = "propagation control",
    type = "passive",
    branch = "control",
    cost = 10
  },
  {
    name = "propagation_media",
    displayName = "propagation media",
    type = "passive",
    branch = "media",
    cost = 10
  },
  {
    name = "propagation_concerns",
    displayName = "propagation concerns",
    type = "passive",
    branch = "concerns",
    cost = 10
  },
  {
    name = "propagation_politics",
    displayName = "propagation politics",
    type = "passive",
    branch = "politics",
    cost = 10
  },
}

core.control.create_skills(skills)