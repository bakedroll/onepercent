control.on_initialize_action(function()

  log:debug("on_initialize_action")

end)

control.on_tick_action(function()

  log:debug("on_tick_action")

end)


control.bind_branch_value_to_visuals("propagation", "takeover")