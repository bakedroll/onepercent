local skills = {

  -- politics
  {
    name = "found_party",
    display_name = "Found party",
    description = "Found a party that follows your interests",
    type = "passive",
    branch = "politics",
    cost = 2
  },
  {
    name = "canvassing",
    display_name = "Canvassing",
    description = "Canvass for your party in rural areas",
    type = "passive",
    branch = "politics",
    cost = 10,
    dependencies = { "found_party" }
  },
  {
    name = "campaign_pledges",
    display_name = "Campaign pledges",
    description = "Campaign pledges can divide your population",
    type = "passive",
    branch = "politics",
    cost = 8,
    dependencies = { "found_party" }
  },


  {
    name = "propagation_banks",
    display_name = "propagation banks",
    type = "passive",
    branch = "banks",
    cost = 10,
    dependencies = { "canvassing" }
  },
  {
    name = "propagation_control",
    display_name = "propagation control",
    type = "passive",
    branch = "control",
    cost = 10
  },
  {
    name = "propagation_media",
    display_name = "propagation media",
    type = "passive",
    branch = "media",
    cost = 10
  },
  {
    name = "propagation_concerns",
    display_name = "propagation concerns",
    type = "passive",
    branch = "concerns",
    cost = 10
  },
  {
    name = "propagation_politics",
    display_name = "propagation politics",
    type = "passive",
    branch = "politics",
    cost = 10
  },
}

control.create_skills(skills)